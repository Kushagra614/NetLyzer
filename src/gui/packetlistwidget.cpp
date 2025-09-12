#include "netlyzer/gui/packetlistwidget.h"
#include <QHeaderView>
#include <QFont>

PacketListWidget::PacketListWidget(QWidget *parent)
    : QWidget(parent)
    , m_tableView(nullptr)
    , m_model(nullptr)
    , m_proxyModel(nullptr)
    , m_layout(nullptr)
{
    setupUI();
    setupModel();
}

PacketListWidget::~PacketListWidget() = default;

void PacketListWidget::setupUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    m_tableView = new QTableView(this);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setSortingEnabled(true);
    
    // Set font for better readability
    QFont font = m_tableView->font();
    font.setFamily("Consolas, Monaco, monospace");
    font.setPointSize(9);
    m_tableView->setFont(font);
    
    m_layout->addWidget(m_tableView);
}

void PacketListWidget::setupModel()
{
    m_model = new QStandardItemModel(this);
    m_model->setHorizontalHeaderLabels({
        "No.", "Time", "Source", "Destination", "Protocol", "Length", "Info"
    });
    
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    
    m_tableView->setModel(m_proxyModel);
    
    // Set column widths
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->setColumnWidth(0, 60);   // No.
    m_tableView->setColumnWidth(1, 120);  // Time
    m_tableView->setColumnWidth(2, 120);  // Source
    m_tableView->setColumnWidth(3, 120);  // Destination
    m_tableView->setColumnWidth(4, 80);   // Protocol
    m_tableView->setColumnWidth(5, 80);   // Length
    
    // Connect selection signal
    connect(m_tableView->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &PacketListWidget::onSelectionChanged);
}

void PacketListWidget::addPacket(int number, const QString &time, const QString &source,
                                const QString &destination, const QString &protocol,
                                int length, const QString &info)
{
    QList<QStandardItem*> items;
    
    auto *numberItem = new QStandardItem(QString::number(number));
    numberItem->setTextAlignment(Qt::AlignCenter);
    items << numberItem;
    
    auto *timeItem = new QStandardItem(time);
    items << timeItem;
    
    auto *sourceItem = new QStandardItem(source);
    items << sourceItem;
    
    auto *destItem = new QStandardItem(destination);
    items << destItem;
    
    auto *protocolItem = new QStandardItem(protocol);
    protocolItem->setTextAlignment(Qt::AlignCenter);
    
    // Color code protocols
    if (protocol == "TCP") {
        protocolItem->setBackground(QColor(220, 255, 220));
    } else if (protocol == "UDP") {
        protocolItem->setBackground(QColor(255, 255, 220));
    } else if (protocol == "ICMP") {
        protocolItem->setBackground(QColor(255, 220, 220));
    }
    
    items << protocolItem;
    
    auto *lengthItem = new QStandardItem(QString::number(length));
    lengthItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    items << lengthItem;
    
    auto *infoItem = new QStandardItem(info);
    items << infoItem;
    
    m_model->appendRow(items);
    
    // Auto-scroll to bottom
    m_tableView->scrollToBottom();
}

void PacketListWidget::clearPackets()
{
    m_model->clear();
    m_model->setHorizontalHeaderLabels({
        "No.", "Time", "Source", "Destination", "Protocol", "Length", "Info"
    });
}

void PacketListWidget::applyFilter(const QString &filter)
{
    m_proxyModel->setFilterWildcard(filter);
}

void PacketListWidget::onSelectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)
    
    if (current.isValid()) {
        QModelIndex sourceIndex = m_proxyModel->mapToSource(current);
        int packetNumber = m_model->item(sourceIndex.row(), 0)->text().toInt();
        emit packetSelected(packetNumber);
    }
}
