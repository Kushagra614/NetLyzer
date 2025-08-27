
#include "packet_sniffer.h"
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>

PacketSniffer::PacketSniffer() : handle_(nullptr),
                                 is_running_(false),
                                 error_buffer_(PCAP_ERRBUF_SIZE, '\0')
{
    statistics_ = {0, 0, 0};
}

PacketSniffer::~PacketSniffer()
{
    stop_capture();
    if (handle_)
    {
        pcap_close(handle_);
    }
}

bool PacketSniffer::init(const string &interface_name)
{
    handle_ = pcap_open_live(interface_name.c_str(), BUFSIZ, 1, 1000, error_buffer_.data());
    if (!handle_) {
        std::cerr << "Error opening interface: " << error_buffer_ << std::endl;
        return false;
    }

    if (pcap_setnonblock(handle_, 1, error_buffer_.data()) != 0)
    {
        std::cerr << "Error setting non-blocking mode: " << error_buffer_ << std::endl;
        pcap_close(handle_);
        handle_ = nullptr;
        return false;
    }

    return true;
}

bool PacketSniffer::start_capture(const string &filter)
{
    if (!handle_ || is_running_)
        return false;

    if (!filter.empty())
    {
        bpf_program fp;
        if (pcap_compile(handle_, &fp, filter.c_str(), 0, PCAP_NETMASK_UNKNOWN) != 0) {
            std::cerr << "Error compiling filter: " << pcap_geterr(handle_) << std::endl;
            return false;
        }
        if (pcap_setfilter(handle_, &fp) != 0)
        {
            std::cerr << "Error setting filter: " << pcap_geterr(handle_) << std::endl;
            pcap_freecode(&fp);
            return false;
        }
        pcap_freecode(&fp);
    }

    is_running_ = true;
    capture_thread_ = thread(&PacketSniffer::capture_loop, this);
    return true;
}

void PacketSniffer::stop_capture()
{
    if (is_running_)
    {
        is_running_ = false;
        if (handle_)
            pcap_breakloop(handle_);
        if (capture_thread_.joinable())
            capture_thread_.join();
    }
}

void PacketSniffer::set_packet_callback(PacketCallback callback)
{
    packet_callback_ = move(callback);
}

vector<string> PacketSniffer::get_available_interfaces()
{
    vector<string> interfaces;
    pcap_if_t *alldevs;
    char errbuf[PCAP_ERRBUF_SIZE];

    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        std::cerr << "Error finding interfaces: " << errbuf << std::endl;
        return interfaces;
    }

    for (pcap_if_t *dev = alldevs; dev != nullptr; dev = dev->next)
    {
        if (dev->name && strlen(dev->name) > 0) {
            string iface_name = dev->name;
            if (dev->description) {
                iface_name += " (" + string(dev->description) + ")";
            }
            interfaces.push_back(iface_name);
        }
    }

    pcap_freealldevs(alldevs);
    return interfaces;
}

PacketSniffer::Statistics PacketSniffer::get_statistics()
{
    if (handle_)
    {
        struct pcap_stat stats;
        if (pcap_stats(handle_, &stats) == 0)
        {
            statistics_.packets_captured = stats.ps_recv;
            statistics_.packets_dropped = stats.ps_drop;
            statistics_.packets_dropped_by_interface = stats.ps_ifdrop;
        }
    }
    return statistics_;
}

void PacketSniffer::capture_loop()
{
    while (is_running_)
    {
        int result = pcap_dispatch(handle_, 10, packet_handler, reinterpret_cast<u_char *>(this));
        if (result < 0) {
            std::cerr << "Error in packet capture: " << pcap_geterr(handle_) << std::endl;
            break;
        }
        // Small delay to prevent CPU spinning
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void PacketSniffer::packet_handler(u_char *user, const struct pcap_pkthdr *header, const u_char *packet)
{
    auto *sniffer = reinterpret_cast<PacketSniffer *>(user);
    if (sniffer->packet_callback_)
    {
        try {
            auto packet_data = sniffer->parse_packet(header, packet);
            sniffer->packet_callback_(packet_data);
        } catch (const std::exception& e) {
            std::cerr << "Error parsing packet: " << e.what() << std::endl;
        }
    }
}

PacketSniffer::PacketData PacketSniffer::parse_packet(const struct pcap_pkthdr *header, const u_char *packet)
{
    PacketData data;
    data.raw_data.assign(packet, packet + header->len);
    data.length = header->len;

    // Format timestamp
    stringstream ss;
    struct tm* timeinfo = localtime(&header->ts.tv_sec);
    ss << put_time(timeinfo, "%H:%M:%S");
    ss << "." << setfill('0') << setw(6) << header->ts.tv_usec;
    data.timestamp = ss.str();

    // Check if we have enough data for ethernet header
    if (header->len < sizeof(struct ether_header)) {
        data.protocol = "Invalid";
        return data;
    }

    const struct ether_header *eth_header = reinterpret_cast<const struct ether_header *>(packet);

    if (ntohs(eth_header->ether_type) == ETHERTYPE_IP)
    {
        // Check if we have enough data for IP header
        if (header->len < sizeof(struct ether_header) + sizeof(struct ip)) {
            data.protocol = "Truncated IP";
            return data;
        }

        const struct ip *ip_header = reinterpret_cast<const struct ip *>(packet + sizeof(struct ether_header));

        // Validate IP header length
        if (ip_header->ip_hl < 5) {
            data.protocol = "Invalid IP";
            return data;
        }

        char src_ip[INET_ADDRSTRLEN], dst_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);

        data.source_ip = src_ip;
        data.dest_ip = dst_ip;

        int ip_header_len = ip_header->ip_hl << 2;

        switch (ip_header->ip_p)
        {
        case IPPROTO_TCP:
        {
            if (header->len >= sizeof(struct ether_header) + ip_header_len + sizeof(struct tcphdr)) {
                const struct tcphdr *tcp_header = reinterpret_cast<const struct tcphdr *>(
                    packet + sizeof(struct ether_header) + ip_header_len);
                data.protocol = "TCP";
                data.source_port = ntohs(tcp_header->th_sport);
                data.dest_port = ntohs(tcp_header->th_dport);
            } else {
                data.protocol = "TCP (truncated)";
            }
            break;
        }
        case IPPROTO_UDP:
        {
            if (header->len >= sizeof(struct ether_header) + ip_header_len + sizeof(struct udphdr)) {
                const struct udphdr *udp_header = reinterpret_cast<const struct udphdr *>(
                    packet + sizeof(struct ether_header) + ip_header_len);
                data.protocol = "UDP";
                data.source_port = ntohs(udp_header->uh_sport);
                data.dest_port = ntohs(udp_header->uh_dport);
            } else {
                data.protocol = "UDP (truncated)";
            }
            break;
        }
        case IPPROTO_ICMP:
            data.protocol = "ICMP";
            break;
        default:
            data.protocol = "IP (" + to_string(ip_header->ip_p) + ")";
        }
    }
    else {
        data.protocol = "Non-IP (" + to_string(ntohs(eth_header->ether_type)) + ")";
    }

    return data;
}
