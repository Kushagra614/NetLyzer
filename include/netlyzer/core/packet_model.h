#ifndef PACKET_MODEL_H
#define PACKET_MODEL_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QDateTime>

struct PacketData {
    int number;
    QDateTime timestamp;
    QString source;
    QString destination;
    QString protocol;
    int length;
    QString info;
    QByteArray rawData;
    
    PacketData() : number(0), length(0) {}
};

class PacketModel : public QObject
{
    Q_OBJECT

public:
    explicit PacketModel(QObject *parent = nullptr);
    ~PacketModel();

    void addPacket(const PacketData &packet);
    void clearPackets();
    PacketData getPacket(int index) const;
    int getPacketCount() const;

signals:
    void packetAdded(const PacketData &packet);
    void packetsCleared();

private:
    QList<PacketData> m_packets;
};

#endif // PACKET_MODEL_H
