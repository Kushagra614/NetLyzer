# NetLyzer

A lightweight network packet analyzer written in C++ that captures and analyzes network traffic in real-time, similar to Wireshark but with a terminal-based interface.

## Features

- Real-time packet capture from network interfaces
- Detailed packet analysis including:
  - Ethernet headers (MAC addresses)
  - IP headers (IPv4)
  - TCP/UDP/ICMP protocol support
  - Payload inspection in hex and ASCII
- BPF filter support
- Cross-platform (Linux/macOS)

## Prerequisites

- C++17 compatible compiler (g++/clang++)
- CMake (3.10 or higher)
- libpcap development libraries

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y cmake g++ libpcap-dev
```

### macOS (using Homebrew)
```bash
brew install cmake libpcap
```

## Building

```bash
# Clone the repository
git clone https://github.com/Kushagra614/NetLyzer.git
cd NetLyzer

# Build using the build script
chmod +x build.sh
./build.sh
```

## Usage

1. Run the application:
   ```bash
   sudo ./netlyzer
   ```
   Note: Superuser privileges are required for packet capture.

2. Select a network interface from the list
3. The application will start capturing and displaying packet information
4. Press Ctrl+C to stop the capture

## Example Output

```
Packet #1 (98 bytes)

Ethernet Header:
   |-Source MAC      : 00:11:22:33:44:55
   |-Destination MAC : 66:77:88:99:aa:bb

IP Header:
   |-Version        : 4
   |-Header Length  : 20 bytes
   |-TTL            : 64
   |-Protocol       : 6 (TCP)
   |-Source IP      : 192.168.1.100
   |-Destination IP : 142.250.190.78

TCP Header:
   |-Source Port      : 12345
   |-Destination Port : 443
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Author

Kushagra - [Kushagra614](https://github.com/Kushagra614)
