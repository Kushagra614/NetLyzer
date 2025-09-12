#include "netlyzer/core/packet_model.h"

PacketModel::PacketModel(QObject *parent)
    : QObject(parent)
{
}

PacketModel::~PacketModel() = default;

void PacketModel::addPacket(const PacketData &packet)
{
    m_packets.append(packet);
    emit packetAdded(packet);
}

void PacketModel::clearPackets()
{
    m_packets.clear();
    emit packetsCleared();
}

PacketData PacketModel::getPacket(int index) const
{
    if (index >= 0 && index < m_packets.size()) {
        return m_packets[index];
    }
    return PacketData();
}

int PacketModel::getPacketCount() const
{
    return m_packets.size();
}
