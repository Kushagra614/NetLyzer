#include "packet_sniffer.h"
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <ctime>
#include <sstream>
#include <iomanip>

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
    if (!handle_)
        return false;

    if (pcap_setnonblock(handle_, 1, error_buffer_.data()) != 0)
    {
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
        if (pcap_compile(handle_, &fp, filter.c_str(), 0, PCAP_NETMASK_UNKNOWN) != 0)
            return false;
        if (pcap_setfilter(handle_, &fp) != 0)
        {
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

    if (pcap_findalldevs(&alldevs, errbuf) == -1)
        return interfaces;

    for (pcap_if_t *dev = alldevs; dev != nullptr; dev = dev->next)
    {
        interfaces.push_back(dev->name);
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
        pcap_dispatch(handle_, -1, packet_handler, reinterpret_cast<u_char *>(this));
    }
}

void PacketSniffer::packet_handler(u_char *user, const struct pcap_pkthdr *header, const u_char *packet)
{
    auto *sniffer = reinterpret_cast<PacketSniffer *>(user);
    if (sniffer->packet_callback_)
    {
        auto packet_data = sniffer->parse_packet(header, packet);
        sniffer->packet_callback_(packet_data);
    }
}

PacketSniffer::PacketData PacketSniffer::parse_packet(const struct pcap_pkthdr *header, const u_char *packet)
{
    PacketData data;
    data.raw_data.assign(packet, packet + header->len);
    data.length = header->len;

    stringstream ss;
    ss << put_time(localtime(&header->ts.tv_sec), "%Y-%m-%d %H:%M:%S");
    ss << "." << setfill('0') << setw(6) << header->ts.tv_usec;
    data.timestamp = ss.str();

    const struct ether_header *eth_header = reinterpret_cast<const struct ether_header *>(packet);

    if (ntohs(eth_header->ether_type) == ETHERTYPE_IP)
    {
        const struct ip *ip_header = reinterpret_cast<const struct ip *>(packet + sizeof(struct ether_header));

        char src_ip[INET_ADDRSTRLEN], dst_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);

        data.source_ip = src_ip;
        data.dest_ip = dst_ip;

        switch (ip_header->ip_p)
        {
        case IPPROTO_TCP:
        {
            const struct tcphdr *tcp_header = reinterpret_cast<const struct tcphdr *>(packet + sizeof(struct ether_header) + (ip_header->ip_hl << 2));
            data.protocol = "TCP";
            data.source_port = ntohs(tcp_header->th_sport);
            data.dest_port = ntohs(tcp_header->th_dport);
            break;
        }
        case IPPROTO_UDP:
        {
            const struct udphdr *udp_header = reinterpret_cast<const struct udphdr *>(packet + sizeof(struct ether_header) + (ip_header->ip_hl << 2));
            data.protocol = "UDP";
            data.source_port = ntohs(udp_header->uh_sport);
            data.dest_port = ntohs(udp_header->uh_dport);
            break;
        }
        case IPPROTO_ICMP:
            data.protocol = "ICMP";
            break;
        default:
            data.protocol = "Unknown";
        }
    }

    return data;
}
