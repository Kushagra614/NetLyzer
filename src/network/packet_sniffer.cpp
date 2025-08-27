
#include "packet_sniffer.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <net/ethernet.h>

PacketSniffer::PacketSniffer() 
    : handle_(nullptr), is_running_(false) {
    statistics_.packets_captured = 0;
    statistics_.packets_dropped = 0;
    statistics_.packets_dropped_by_interface = 0;
}

PacketSniffer::~PacketSniffer() {
    stop_capture();
    if (handle_) {
        pcap_close(handle_);
    }
}

bool PacketSniffer::init(const string& interface_name) {
    char errbuf[PCAP_ERRBUF_SIZE];
    
    handle_ = pcap_open_live(interface_name.c_str(), BUFSIZ, 1, 1000, errbuf);
    if (handle_ == nullptr) {
        error_buffer_ = string(errbuf);
        return false;
    }
    
    return true;
}

bool PacketSniffer::start_capture(const string& filter) {
    if (!handle_) {
        return false;
    }
    
    // Set filter if provided
    if (!filter.empty()) {
        struct bpf_program fp;
        if (pcap_compile(handle_, &fp, filter.c_str(), 0, PCAP_NETMASK_UNKNOWN) == -1) {
            error_buffer_ = "Failed to compile filter";
            return false;
        }
        if (pcap_setfilter(handle_, &fp) == -1) {
            error_buffer_ = "Failed to set filter";
            return false;
        }
    }
    
    is_running_ = true;
    capture_thread_ = thread(&PacketSniffer::capture_loop, this);
    
    return true;
}

void PacketSniffer::stop_capture() {
    is_running_ = false;
    if (capture_thread_.joinable()) {
        capture_thread_.join();
    }
}

void PacketSniffer::set_packet_callback(PacketCallback callback) {
    packet_callback_ = callback;
}

vector<string> PacketSniffer::get_available_interfaces() {
    vector<string> interfaces;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevs, *device;
    
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        return interfaces;
    }
    
    for (device = alldevs; device != nullptr; device = device->next) {
        interfaces.push_back(string(device->name));
    }
    
    pcap_freealldevs(alldevs);
    return interfaces;
}

PacketSniffer::Statistics PacketSniffer::get_statistics() {
    if (handle_) {
        struct pcap_stat stat;
        if (pcap_stats(handle_, &stat) == 0) {
            statistics_.packets_captured = stat.ps_recv;
            statistics_.packets_dropped = stat.ps_drop;
            statistics_.packets_dropped_by_interface = stat.ps_ifdrop;
        }
    }
    return statistics_;
}

void PacketSniffer::capture_loop() {
    while (is_running_) {
        int result = pcap_dispatch(handle_, 1, packet_handler, reinterpret_cast<u_char*>(this));
        if (result == -1) {
            break;
        }
    }
}

void PacketSniffer::packet_handler(u_char* user, const struct pcap_pkthdr* header, const u_char* packet) {
    PacketSniffer* sniffer = reinterpret_cast<PacketSniffer*>(user);
    
    if (sniffer->packet_callback_) {
        PacketData data = sniffer->parse_packet(header, packet);
        sniffer->packet_callback_(data);
    }
}

PacketSniffer::PacketData PacketSniffer::parse_packet(const struct pcap_pkthdr* header, const u_char* packet) {
    PacketData data;
    
    // Store raw data
    data.raw_data.assign(packet, packet + header->caplen);
    data.length = header->len;
    
    // Generate timestamp
    auto now = chrono::system_clock::now();
    auto time_t = chrono::system_clock::to_time_t(now);
    auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    stringstream ss;
    ss << put_time(localtime(&time_t), "%H:%M:%S");
    ss << "." << setfill('0') << setw(3) << ms.count();
    data.timestamp = ss.str();
    
    // Initialize default values
    data.source_port = 0;
    data.dest_port = 0;
    data.protocol = "Unknown";
    
    // Parse Ethernet header
    const struct ether_header* eth_header = reinterpret_cast<const struct ether_header*>(packet);
    
    if (ntohs(eth_header->ether_type) == ETHERTYPE_IP) {
        // Parse IP header
        const struct ip* ip_header = reinterpret_cast<const struct ip*>(packet + sizeof(struct ether_header));
        
        // Extract IP addresses
        data.source_ip = inet_ntoa(ip_header->ip_src);
        data.dest_ip = inet_ntoa(ip_header->ip_dst);
        
        // Parse protocol-specific headers
        switch (ip_header->ip_p) {
            case IPPROTO_TCP: {
                const struct tcphdr* tcp_header = reinterpret_cast<const struct tcphdr*>(
                    packet + sizeof(struct ether_header) + (ip_header->ip_hl << 2)
                );
                data.protocol = "TCP";
                data.source_port = ntohs(tcp_header->th_sport);
                data.dest_port = ntohs(tcp_header->th_dport);
                break;
            }
            case IPPROTO_UDP: {
                const struct udphdr* udp_header = reinterpret_cast<const struct udphdr*>(
                    packet + sizeof(struct ether_header) + (ip_header->ip_hl << 2)
                );
                data.protocol = "UDP";
                data.source_port = ntohs(udp_header->uh_sport);
                data.dest_port = ntohs(udp_header->uh_dport);
                break;
            }
            case IPPROTO_ICMP:
                data.protocol = "ICMP";
                data.source_port = 0;
                data.dest_port = 0;
                break;
            default:
                data.protocol = "Other IP";
                break;
        }
    } else {
        data.source_ip = "N/A";
        data.dest_ip = "N/A";
        data.protocol = "Non-IP";
    }
    
    return data;
}
