#ifndef NETLYZER_ERROR_H
#define NETLYZER_ERROR_H

#include <stdexcept>
#include <string>

namespace netlyzer {

class NetLyzerError : public std::runtime_error {
public:
    explicit NetLyzerError(const std::string& msg) : std::runtime_error(msg) {}
};

class PcapError : public NetLyzerError {
public:
    explicit PcapError(const std::string& msg) : NetLyzerError("PCAP Error: " + msg) {}
};

class SocketError : public NetLyzerError {
public:
    explicit SocketError(const std::string& msg) : NetLyzerError("Socket Error: " + msg) {}
};

class ProtocolError : public NetLyzerError {
public:
    explicit ProtocolError(const std::string& msg) : NetLyzerError("Protocol Error: " + msg) {}
};

// Helper function to check pcap return values
inline void check_pcap(int status, const std::string& error_msg) {
    if (status == -1) {
        throw PcapError(error_msg);
    }
}

} // namespace netlyzer

#endif // NETLYZER_ERROR_H
