// #include "network/packet_sniffer.h"
// #include <iostream>

// int main() {
//     PacketSniffer sniffer;

//     auto interfaces = PacketSniffer::get_available_interfaces();
//     if (interfaces.empty()) {
//         std::cerr << "No network interfaces found!" << std::endl;
//         return 1;
//     }

//     std::cout << "Available Interfaces:" << std::endl;
//     for (size_t i = 0; i < interfaces.size(); ++i) {
//         std::cout << i << ": " << interfaces[i] << std::endl;
//     }

//     int choice;
//     std::cout << "Select an interface to capture packets: ";
//     std::cin >> choice;

//     if (choice < 0 || choice >= interfaces.size()) {
//         std::cerr << "Invalid selection!" << std::endl;
//         return 1;
//     }

//     if (!sniffer.init(interfaces[choice])) {
//         std::cerr << "Failed to initialize sniffer!" << std::endl;
//         return 1;
//     }

//     sniffer.set_packet_callback([](const PacketSniffer::PacketData& data) {
//         std::cout << "[" << data.timestamp << "] "
//                   << data.source_ip << ":" << data.source_port
//                   << " -> " << data.dest_ip << ":" << data.dest_port
//                   << " [" << data.protocol << "] "
//                   << "Length: " << data.length << " bytes" << std::endl;
//     });

//     if (!sniffer.start_capture()) {
//         std::cerr << "Failed to start packet capture!" << std::endl;
//         return 1;
//     }

//     std::cout << "Capturing packets... Press Enter to stop." << std::endl;
//     std::cin.ignore();
//     std::cin.get();

//     sniffer.stop_capture();
//     std::cout << "Capture stopped." << std::endl;

//     auto stats = sniffer.get_statistics();
//     std::cout << "Packets Captured: " << stats.packets_captured << std::endl;
//     std::cout << "Packets Dropped: " << stats.packets_dropped << std::endl;

//     return 0;
// }


#include <QApplication>
#include "gui/main_window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
