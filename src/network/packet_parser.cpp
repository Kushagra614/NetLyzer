// packet_parser.cpp
#include "packet_parser.h"
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <sstream>
#include <iomanip>

PacketParser::ParsedPacket PacketParser::parse(const std::vector<uint8_t>& raw_data) {
    ParsedPacket parsed;
    parsed.length = raw_data.size();

    // Parse Ethernet header
    const struct ether_header* eth_header = reinterpret_cast<const struct ether_header*>(raw_data.data());
    if (ntohs(eth_header->ether_type) == ETHERTYPE_IP) {
        const struct ip* ip_header = reinterpret_cast<const struct ip*>(raw_data.data() + sizeof(struct ether_header));

        char src_ip[INET_ADDRSTRLEN];
        char dst_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);

        parsed.source_ip = src_ip;
        parsed.dest_ip = dst_ip;

        switch (ip_header->ip_p) {
            case IPPROTO_TCP: {
                const struct tcphdr* tcp_header = reinterpret_cast<const struct tcphdr*>(
                    raw_data.data() + sizeof(struct ether_header) + (ip_header->ip_hl << 2)
                );
                parsed.protocol = "TCP";
                parsed.source_port = ntohs(tcp_header->th_sport);
                parsed.dest_port = ntohs(tcp_header->th_dport);
                break;
            }
            case IPPROTO_UDP: {
                const struct udphdr* udp_header = reinterpret_cast<const struct udphdr*>(
                    raw_data.data() + sizeof(struct ether_header) + (ip_header->ip_hl << 2)
                );
                parsed.protocol = "UDP";
                parsed.source_port = ntohs(udp_header->uh_sport);
                parsed.dest_port = ntohs(udp_header->uh_dport);
                break;
            }
            case IPPROTO_ICMP:
                parsed.protocol = "ICMP";
                parsed.source_port = 0;
                parsed.dest_port = 0;
                break;
            default:
                parsed.protocol = "Unknown";
                parsed.source_port = 0;
                parsed.dest_port = 0;
                break;
        }

        // Extract payload (for demonstration, limited to first 50 bytes)
        const u_char* payload_ptr = raw_data.data() + sizeof(struct ether_header) + (ip_header->ip_hl << 2);
        size_t payload_length = raw_data.size() - (payload_ptr - raw_data.data());
        payload_length = payload_length > 50 ? 50 : payload_length; // Limit payload size

        std::ostringstream payload_stream;
        for (size_t i = 0; i < payload_length; ++i) {
            payload_stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(payload_ptr[i]) << " ";
        }
        parsed.payload = payload_stream.str();
    }

    return parsed;
}
