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