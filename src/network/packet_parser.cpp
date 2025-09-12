
#include "netlyzer/network/packet_parser.h"

PacketParser::EthernetHeader PacketParser::parse_ethernet(const uint8_t* data) {
    EthernetHeader header;
    const struct ether_header* eth = reinterpret_cast<const struct ether_header*>(data);
    
    header.source_mac = mac_to_string(eth->ether_shost);
    header.dest_mac = mac_to_string(eth->ether_dhost);
    header.ethertype = ntohs(eth->ether_type);
    
    return header;
}

PacketParser::IPHeader PacketParser::parse_ip(const uint8_t* data) {
    IPHeader header;
    const struct ip* ip_hdr = reinterpret_cast<const struct ip*>(data);
    
    header.version = ip_hdr->ip_v;
    header.header_length = ip_hdr->ip_hl * 4;
    header.type_of_service = ip_hdr->ip_tos;
    header.total_length = ntohs(ip_hdr->ip_len);
    header.identification = ntohs(ip_hdr->ip_id);
    header.flags = ntohs(ip_hdr->ip_off) >> 13;
    header.ttl = ip_hdr->ip_ttl;
    header.protocol = ip_hdr->ip_p;
    header.checksum = ntohs(ip_hdr->ip_sum);
    
    char src_ip[INET_ADDRSTRLEN], dst_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_hdr->ip_src), src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip_hdr->ip_dst), dst_ip, INET_ADDRSTRLEN);
    
    header.source_ip = src_ip;
    header.dest_ip = dst_ip;
    
    return header;
}

PacketParser::TCPHeader PacketParser::parse_tcp(const uint8_t* data) {
    TCPHeader header;
    const struct tcphdr* tcp_hdr = reinterpret_cast<const struct tcphdr*>(data);
    
    header.source_port = ntohs(tcp_hdr->th_sport);
    header.dest_port = ntohs(tcp_hdr->th_dport);
    header.sequence_number = ntohl(tcp_hdr->th_seq);
    header.acknowledgment_number = ntohl(tcp_hdr->th_ack);
    header.header_length = tcp_hdr->th_off * 4;
    header.flags = tcp_hdr->th_flags;
    header.window_size = ntohs(tcp_hdr->th_win);
    header.checksum = ntohs(tcp_hdr->th_sum);
    header.urgent_pointer = ntohs(tcp_hdr->th_urp);
    
    return header;
}

PacketParser::UDPHeader PacketParser::parse_udp(const uint8_t* data) {
    UDPHeader header;
    const struct udphdr* udp_hdr = reinterpret_cast<const struct udphdr*>(data);
    
    header.source_port = ntohs(udp_hdr->uh_sport);
    header.dest_port = ntohs(udp_hdr->uh_dport);
    header.length = ntohs(udp_hdr->uh_ulen);
    header.checksum = ntohs(udp_hdr->uh_sum);
    
    return header;
}

std::string PacketParser::mac_to_string(const uint8_t* mac) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < 6; ++i) {
        if (i > 0) ss << ":";
        ss << std::setw(2) << static_cast<unsigned>(mac[i]);
    }
    return ss.str();
}

std::string PacketParser::protocol_to_string(uint8_t protocol) {
    switch (protocol) {
        case IPPROTO_TCP: return "TCP";
        case IPPROTO_UDP: return "UDP";
        case IPPROTO_ICMP: return "ICMP";
        case IPPROTO_IGMP: return "IGMP";
        default: return "Unknown (" + std::to_string(protocol) + ")";
    }
}
