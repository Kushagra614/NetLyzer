#pragma once

#include <pcap.h>
#include <string>
#include <functional>
#include <vector>
#include <atomic>
#include <thread>
#include <memory>

using namespace std;
class PacketSniffer
{
public:
    struct PacketData
    {
        vector<uint8_t> raw_data;
        string source_ip;
        string dest_ip;
        uint16_t source_port;
        uint16_t dest_port;
        string protocol;
        size_t length;
        string timestamp;
    };

    using PacketCallback = function<void(const PacketData &)>;

    PacketSniffer();
    ~PacketSniffer();

    bool init(const string &interface_name);
    bool start_capture(const string &filter = "");
    void stop_capture();
    void set_packet_callback(PacketCallback callback);

    static vector<string> get_available_interfaces();

    struct Statistics
    {
        unsigned long packets_captured;
        unsigned long packets_dropped;
        unsigned long packets_dropped_by_interface;
    };
    Statistics get_statistics();

private:
    pcap_t *handle_;
    atomic<bool> is_running_;
    thread capture_thread_;
    PacketCallback packet_callback_;
    string error_buffer_;
    Statistics statistics_;

    void capture_loop();
    static void packet_handler(u_char *user, const struct pcap_pkthdr *header, const u_char *packet);
    PacketData parse_packet(const struct pcap_pkthdr *header, const u_char *packet);
};
