#include "netlyzer/network/packet_capture.h"
#include "netlyzer/network/packet_parser.h"
#include <QDateTime>
#include <QDebug>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>

PacketCapture::PacketCapture(QObject *parent)
    : QObject(parent)
    , m_handle(nullptr)
    , m_captureThread(new QThread(this))
    , m_isCapturing(false)
    , m_packetCount(0)
{
}

PacketCapture::~PacketCapture()
{
    stopCapture();
    if (m_captureThread->isRunning()) {
        m_captureThread->quit();
        m_captureThread->wait();
    }
}

bool PacketCapture::startCapture(const QString &interface)
{
    if (m_isCapturing) {
        return false;
    }

    char errbuf[PCAP_ERRBUF_SIZE];
    m_handle = pcap_open_live(interface.toUtf8().constData(), BUFSIZ, 1, 1000, errbuf);
    
    if (!m_handle) {
        qDebug() << "Error opening interface:" << errbuf;
        return false;
    }

    m_interface = interface;
    m_isCapturing = true;
    m_packetCount = 0;

    // Start capture in separate thread
    auto *worker = new CaptureWorker(m_handle, this);
    worker->moveToThread(m_captureThread);
    
    connect(m_captureThread, &QThread::started, worker, &CaptureWorker::startLoop);
    connect(m_captureThread, &QThread::finished, worker, &QObject::deleteLater);
    
    m_captureThread->start();
    
    return true;
}

void PacketCapture::stopCapture()
{
    if (!m_isCapturing) {
        return;
    }

    m_isCapturing = false;
    
    if (m_handle) {
        pcap_breakloop(m_handle);
    }
    
    if (m_captureThread->isRunning()) {
        m_captureThread->quit();
        m_captureThread->wait();
    }
    
    if (m_handle) {
        pcap_close(m_handle);
        m_handle = nullptr;
    }
}

void PacketCapture::packetHandler(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    auto *capture = reinterpret_cast<PacketCapture*>(userData);
    capture->processPacket(pkthdr, packet);
}

void PacketCapture::processPacket(const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    if (!m_isCapturing) {
        return;
    }

    m_packetCount++;
    
    QString timestamp = formatTimestamp(pkthdr->ts);
    QString source = "Unknown";
    QString destination = "Unknown";
    QString protocol = "Unknown";
    QString info = "";
    
    // Parse Ethernet header
    if (pkthdr->len >= sizeof(struct ether_header)) {
        const struct ether_header *eth = reinterpret_cast<const struct ether_header*>(packet);
        
        if (ntohs(eth->ether_type) == ETHERTYPE_IP && pkthdr->len >= sizeof(struct ether_header) + sizeof(struct ip)) {
            const struct ip *ip_hdr = reinterpret_cast<const struct ip*>(packet + sizeof(struct ether_header));
            
            source = QString(inet_ntoa(ip_hdr->ip_src));
            destination = QString(inet_ntoa(ip_hdr->ip_dst));
            
            switch (ip_hdr->ip_p) {
                case IPPROTO_TCP:
                    protocol = "TCP";
                    break;
                case IPPROTO_UDP:
                    protocol = "UDP";
                    break;
                case IPPROTO_ICMP:
                    protocol = "ICMP";
                    break;
                default:
                    protocol = QString("IP (%1)").arg(ip_hdr->ip_p);
                    break;
            }
            
            info = parsePacketInfo(packet, pkthdr->len);
        }
    }
    
    QByteArray packetData(reinterpret_cast<const char*>(packet), pkthdr->len);
    
    emit packetCaptured(m_packetCount, timestamp, source, destination, 
                       protocol, pkthdr->len, info, packetData);
}

QString PacketCapture::formatTimestamp(const struct timeval &tv)
{
    QDateTime dateTime = QDateTime::fromSecsSinceEpoch(tv.tv_sec);
    return dateTime.toString("hh:mm:ss.zzz");
}

QString PacketCapture::parsePacketInfo(const u_char *packet, int length)
{
    Q_UNUSED(packet)
    Q_UNUSED(length)
    
    // TODO: Implement detailed packet info parsing
    return "Packet data";
}

// CaptureWorker implementation
CaptureWorker::CaptureWorker(pcap_t *handle, PacketCapture *capture)
    : m_handle(handle)
    , m_capture(capture)
{
}

void CaptureWorker::startLoop()
{
    if (m_handle) {
        pcap_loop(m_handle, -1, PacketCapture::packetHandler, 
                 reinterpret_cast<u_char*>(m_capture));
    }
}
