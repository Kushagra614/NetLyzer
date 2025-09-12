#include "netlyzer/gui/packetdetailswidget.h"
#include <QHeaderView>
#include <QFont>

PacketDetailsWidget::PacketDetailsWidget(QWidget *parent)
    : QWidget(parent)
    , m_treeView(nullptr)
    , m_model(nullptr)
    , m_layout(nullptr)
{
    setupUI();
}

PacketDetailsWidget::~PacketDetailsWidget() = default;

void PacketDetailsWidget::setupUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    m_treeView = new QTreeView(this);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setRootIsDecorated(true);
    m_treeView->setExpandsOnDoubleClick(true);
    
    // Set font for better readability
    QFont font = m_treeView->font();
    font.setFamily("Consolas, Monaco, monospace");
    font.setPointSize(9);
    m_treeView->setFont(font);
    
    m_model = new QStandardItemModel(this);
    m_model->setHorizontalHeaderLabels({"Field", "Value"});
    
    m_treeView->setModel(m_model);
    m_treeView->header()->setStretchLastSection(true);
    m_treeView->setColumnWidth(0, 200);
    
    m_layout->addWidget(m_treeView);
}

void PacketDetailsWidget::showPacketDetails(int packetNumber)
{
    clearDetails();
    
    // TODO: Get actual packet data and parse it
    // For now, show sample data
    
    auto *frameItem = new QStandardItem(QString("Frame %1").arg(packetNumber));
    frameItem->appendRow({new QStandardItem("Frame Length"), new QStandardItem("64 bytes")});
    frameItem->appendRow({new QStandardItem("Capture Length"), new QStandardItem("64 bytes")});
    frameItem->appendRow({new QStandardItem("Arrival Time"), new QStandardItem("2024-01-01 12:00:00.123456")});
    m_model->appendRow(frameItem);
    
    auto *ethItem = new QStandardItem("Ethernet II");
    ethItem->appendRow({new QStandardItem("Destination"), new QStandardItem("aa:bb:cc:dd:ee:ff")});
    ethItem->appendRow({new QStandardItem("Source"), new QStandardItem("11:22:33:44:55:66")});
    ethItem->appendRow({new QStandardItem("Type"), new QStandardItem("IPv4 (0x0800)")});
    m_model->appendRow(ethItem);
    
    auto *ipItem = new QStandardItem("Internet Protocol Version 4");
    ipItem->appendRow({new QStandardItem("Version"), new QStandardItem("4")});
    ipItem->appendRow({new QStandardItem("Header Length"), new QStandardItem("20 bytes")});
    ipItem->appendRow({new QStandardItem("Total Length"), new QStandardItem("52")});
    ipItem->appendRow({new QStandardItem("Protocol"), new QStandardItem("TCP (6)")});
    ipItem->appendRow({new QStandardItem("Source Address"), new QStandardItem("192.168.1.100")});
    ipItem->appendRow({new QStandardItem("Destination Address"), new QStandardItem("192.168.1.1")});
    m_model->appendRow(ipItem);
    
    auto *tcpItem = new QStandardItem("Transmission Control Protocol");
    tcpItem->appendRow({new QStandardItem("Source Port"), new QStandardItem("12345")});
    tcpItem->appendRow({new QStandardItem("Destination Port"), new QStandardItem("80")});
    tcpItem->appendRow({new QStandardItem("Sequence Number"), new QStandardItem("1234567890")});
    tcpItem->appendRow({new QStandardItem("Flags"), new QStandardItem("0x018 (PSH, ACK)")});
    tcpItem->appendRow({new QStandardItem("Window Size"), new QStandardItem("65535")});
    m_model->appendRow(tcpItem);
    
    // Expand all items
    m_treeView->expandAll();
}

void PacketDetailsWidget::clearDetails()
{
    m_model->clear();
    m_model->setHorizontalHeaderLabels({"Field", "Value"});
}

void PacketDetailsWidget::addProtocolLayer(const QString &name, const QStringList &fields)
{
    auto *layerItem = new QStandardItem(name);
    
    for (int i = 0; i < fields.size(); i += 2) {
        if (i + 1 < fields.size()) {
            layerItem->appendRow({
                new QStandardItem(fields[i]),
                new QStandardItem(fields[i + 1])
            });
        }
    }
    
    m_model->appendRow(layerItem);
}
