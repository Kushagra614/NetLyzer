#ifndef PACKET_CAPTURE_H
#define PACKET_CAPTURE_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QString>
#include <pcap.h>
#include <atomic>

class PacketCapture : public QObject
{
    Q_OBJECT

public:
    explicit PacketCapture(QObject *parent = nullptr);
    ~PacketCapture();

    bool startCapture(const QString &interface);
    void stopCapture();
    bool isCapturing() const { return m_isCapturing; }
    int getPacketCount() const { return m_packetCount; }

signals:
    void packetCaptured(int number, const QString &time, const QString &source,
                       const QString &destination, const QString &protocol,
                       int length, const QString &info, const QByteArray &data);

public:
    static void packetHandler(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet);

private:
    void processPacket(const struct pcap_pkthdr *pkthdr, const u_char *packet);
    QString formatTimestamp(const struct timeval &tv);
    QString parsePacketInfo(const u_char *packet, int length);

    pcap_t *m_handle;
    QThread *m_captureThread;
    std::atomic<bool> m_isCapturing;
    std::atomic<int> m_packetCount;
    QMutex m_mutex;
    QString m_interface;
};

class CaptureWorker : public QObject
{
    Q_OBJECT

public:
    CaptureWorker(pcap_t *handle, PacketCapture *capture);

public slots:
    void startLoop();

private:
    pcap_t *m_handle;
    PacketCapture *m_capture;
};

#endif // PACKET_CAPTURE_H
