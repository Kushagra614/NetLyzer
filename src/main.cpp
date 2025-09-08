#include <iostream>
#include <pcap.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <iomanip>

using namespace std;

// Function to print packet data in hex and ASCII
void print_payload(const u_char *payload, int len) {
    const u_char *ch = payload;
    int i, j;
    
    for(i=0; i < len; i++) {
        printf("%02x ", *ch);
        ch++;
        
        if (i % 16 == 15) {
            printf("         ");
            for(j=i-15; j <= i; j++) {
                if (payload[j] >= 32 && payload[j] <= 128)
                    printf("%c", (unsigned char)payload[j]);
                else
                    printf(".");
            }
            printf("\n");
        }
    }
    
    if (i % 16 != 0) {
        for(j=0; j < 15 - (i % 16); j++)
            printf("   ");
        printf("         ");
        for(j=(i - (i % 16)); j <= i; j++) {
            if (payload[j] >= 32 && payload[j] <= 128)
                printf("%c", (unsigned char)payload[j]);
            else
                printf(".");
        }
    }
    printf("\n");
}

// Callback function that will be called for each packet
void packet_handler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    static int count = 1;
    cout << "\n\nPacket #" << count++ << " (" << pkthdr->len << " bytes)" << endl;
    
    // Ethernet header
    struct ether_header *eth_header = (struct ether_header *)packet;
    cout << "\nEthernet Header:" << endl;
    cout << "   |-Source MAC      : " << hex << setw(2) << setfill('0')
         << (int)eth_header->ether_shost[0] << ":"
         << hex << setw(2) << setfill('0') << (int)eth_header->ether_shost[1] << ":"
         << hex << setw(2) << setfill('0') << (int)eth_header->ether_shost[2] << ":"
         << hex << setw(2) << setfill('0') << (int)eth_header->ether_shost[3] << ":"
         << hex << setw(2) << setfill('0') << (int)eth_header->ether_shost[4] << ":"
         << hex << setw(2) << setfill('0') << (int)eth_header->ether_shost[5] << endl;
         
    cout << "   |-Destination MAC : " << hex << setw(2) << setfill('0')
         << (int)eth_header->ether_dhost[0] << ":"
         << hex << setw(2) << setfill('0') << (int)eth_header->ether_dhost[1] << ":"
         << hex << setw(2) << setfill('0') << (int)eth_header->ether_dhost[2] << ":"
         << hex << setw(2) << setfill('0') << (int)eth_header->ether_dhost[3] << ":"
         << hex << setw(2) << setfill('0') << (int)eth_header->ether_dhost[4] << ":"
         << hex << setw(2) << setfill('0') << (int)eth_header->ether_dhost[5] << endl;
    
    // If it's an IP packet
    if (ntohs(eth_header->ether_type) == ETHERTYPE_IP) {
        struct ip *ip_header = (struct ip*)(packet + sizeof(struct ether_header));
        
        cout << "\nIP Header:" << endl;
        cout << "   |-Version        : " << (ip_header->ip_v) << endl;
        cout << "   |-Header Length  : " << (ip_header->ip_hl * 4) << " bytes" << endl;
        cout << "   |-TTL            : " << (int)ip_header->ip_ttl << endl;
        cout << "   |-Protocol       : " << (int)ip_header->ip_p << endl;
        cout << "   |-Source IP      : " << inet_ntoa(ip_header->ip_src) << endl;
        cout << "   |-Destination IP : " << inet_ntoa(ip_header->ip_dst) << endl;
        
        // TCP Packet
        if (ip_header->ip_p == IPPROTO_TCP) {
            struct tcphdr *tcp_header = (struct tcphdr*)(packet + sizeof(struct ether_header) + (ip_header->ip_hl * 4));
            
            cout << "\nTCP Header:" << endl;
            cout << "   |-Source Port      : " << ntohs(tcp_header->source) << endl;
            cout << "   |-Destination Port : " << ntohs(tcp_header->dest) << endl;
            
            int tcp_header_size = tcp_header->doff * 4;
            int payload_size = ntohs(ip_header->ip_len) - (ip_header->ip_hl * 4) - tcp_header_size;
            
            if (payload_size > 0) {
                cout << "\nPayload (" << payload_size << " bytes):" << endl;
                const u_char *payload = packet + sizeof(struct ether_header) + (ip_header->ip_hl * 4) + tcp_header_size;
                print_payload(payload, payload_size);
            }
        }
        // UDP Packet
        else if (ip_header->ip_p == IPPROTO_UDP) {
            struct udphdr *udp_header = (struct udphdr*)(packet + sizeof(struct ether_header) + (ip_header->ip_hl * 4));
            
            cout << "\nUDP Header:" << endl;
            cout << "   |-Source Port      : " << ntohs(udp_header->source) << endl;
            cout << "   |-Destination Port : " << ntohs(udp_header->dest) << endl;
            
            int payload_size = ntohs(udp_header->len) - 8; // 8 bytes UDP header
            
            if (payload_size > 0) {
                cout << "\nPayload (" << payload_size << " bytes):" << endl;
                const u_char *payload = packet + sizeof(struct ether_header) + (ip_header->ip_hl * 4) + 8;
                print_payload(payload, payload_size);
            }
        }
        // ICMP Packet
        else if (ip_header->ip_p == IPPROTO_ICMP) {
            struct icmphdr *icmp_header = (struct icmphdr*)(packet + sizeof(struct ether_header) + (ip_header->ip_hl * 4));
            
            cout << "\nICMP Header:" << endl;
            cout << "   |-Type : " << (int)icmp_header->type << endl;
            cout << "   |-Code : " << (int)icmp_header->code << endl;
        }
    }
    
    cout << "\n" << string(80, '=') << endl;
}

int main(int argc, char *argv[]) {
    pcap_if_t *alldevs;
    pcap_if_t *d;
    int inum;
    int i = 0;
    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;
    char filter_exp[] = "ip";
    bpf_u_int32 mask;
    bpf_u_int32 net;
    
    // Get the list of available devices
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        cerr << "Error finding devices: " << errbuf << endl;
        return 1;
    }
    
    // Print the list
    cout << "Available network interfaces:" << endl;
    for(d = alldevs, i = 0; d; d = d->next, i++) {
        cout << i+1 << ". " << d->name;
        if (d->description)
            cout << " (" << d->description << ")" << endl;
        else
            cout << " (No description available)" << endl;
    }
    
    if (i == 0) {
        cout << "No interfaces found! Make sure you have the necessary permissions." << endl;
        return 1;
    }
    
    cout << "\nEnter the interface number (1-" << i << "): ";
    cin >> inum;
    
    if (inum < 1 || inum > i) {
        cout << "Interface number out of range." << endl;
        pcap_freealldevs(alldevs);
        return 1;
    }
    
    // Jump to the selected adapter
    for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);
    
    // Open the device in promiscuous mode
    handle = pcap_open_live(d->name, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        cerr << "Couldn't open device " << d->name << ": " << errbuf << endl;
        pcap_freealldevs(alldevs);
        return 1;
    }
    
    // Get network address and mask
    if (pcap_lookupnet(d->name, &net, &mask, errbuf) == -1) {
        cerr << "Can't get netmask for device " << d->name << endl;
        net = 0;
        mask = 0;
    }
    
    // Compile and apply the filter
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        cerr << "Couldn't parse filter " << filter_exp << ": " << pcap_geterr(handle) << endl;
        pcap_freealldevs(alldevs);
        return 1;
    }
    
    if (pcap_setfilter(handle, &fp) == -1) {
        cerr << "Couldn't install filter " << filter_exp << ": " << pcap_geterr(handle) << endl;
        pcap_freealldevs(alldevs);
        return 1;
    }
    
    cout << "\nStarting packet capture on " << d->name << ". Press Ctrl+C to stop..." << endl;
    
    // Free the device list
    pcap_freealldevs(alldevs);
    
    // Start capturing packets
    pcap_loop(handle, 0, packet_handler, NULL);
    
    // Close the session
    pcap_close(handle);
    
    return 0;
}
