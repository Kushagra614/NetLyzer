#ifndef PACKET_SNIFFER_H
#define PACKET_SNIFFER_H

#include <string>
#include <vector>
#include <thread>
#include <functional>
#include <pcap.h>

using namespace std;

class PacketSniffer {
public:
    struct PacketData {
        string timestamp;
        string source_ip;
        string dest_ip;
        string protocol;
        uint16_t source_port = 0;
        uint16_t dest_port = 0;
        uint32_t length = 0;
        vector<uint8_t> raw_data;
    };

    struct Statistics {
        uint32_t packets_captured = 0;
        uint32_t packets_dropped = 0;
        uint32_t packets_dropped_by_interface = 0;
    };

    using PacketCallback = function<void(const PacketData&)>;

    PacketSniffer();
    ~PacketSniffer();

    bool init(const string& interface_name);
    bool start_capture(const string& filter = "");
    void stop_capture();
    void set_packet_callback(PacketCallback callback);

    static vector<string> get_available_interfaces();
    Statistics get_statistics();

private:
    void capture_loop();
    static void packet_handler(u_char* user, const struct pcap_pkthdr* header, const u_char* packet);
    PacketData parse_packet(const struct pcap_pkthdr* header, const u_char* packet);

    pcap_t* handle_;
    bool is_running_;
    string error_buffer_;
    thread capture_thread_;
    PacketCallback packet_callback_;
    Statistics statistics_;
};

#endif // PACKET_SNIFFER_H