// packet_parser.h
#pragma once

#include <string>
#include <vector>
#include <cstdint>

class PacketParser {
public:
    struct ParsedPacket {
        std::string source_ip;
        std::string dest_ip;
        uint16_t source_port;
        uint16_t dest_port;
        std::string protocol;
        size_t length;
        std::string payload;
    };

    static ParsedPacket parse(const std::vector<uint8_t>& raw_data);
};
#ifndef PACKET_PARSER_H
#define PACKET_PARSER_H

#include <string>
#include <vector>
#include <cstdint>

class PacketParser {
public:
    struct EthernetHeader {
        std::string dest_mac;
        std::string source_mac;
        uint16_t ethertype;
    };

    struct IPHeader {
        uint8_t version;
        uint8_t header_length;
        uint8_t type_of_service;
        uint16_t total_length;
        uint16_t identification;
        uint16_t flags;
        uint8_t ttl;
        uint8_t protocol;
        uint16_t checksum;
        std::string source_ip;
        std::string dest_ip;
    };

    struct TCPHeader {
        uint16_t source_port;
        uint16_t dest_port;
        uint32_t sequence_number;
        uint32_t acknowledgment_number;
        uint8_t header_length;
        uint8_t flags;
        uint16_t window_size;
        uint16_t checksum;
        uint16_t urgent_pointer;
    };

    struct UDPHeader {
        uint16_t source_port;
        uint16_t dest_port;
        uint16_t length;
        uint16_t checksum;
    };

    static EthernetHeader parse_ethernet(const uint8_t* data);
    static IPHeader parse_ip(const uint8_t* data);
    static TCPHeader parse_tcp(const uint8_t* data);
    static UDPHeader parse_udp(const uint8_t* data);
    
    static std::string mac_to_string(const uint8_t* mac);
    static std::string protocol_to_string(uint8_t protocol);
};

#endif // PACKET_PARSER_H
