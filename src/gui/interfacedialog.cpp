#include "netlyzer/gui/interfacedialog.h"
#include <pcap.h>
#include <QMessageBox>

InterfaceDialog::InterfaceDialog(QWidget *parent)
    : QDialog(parent)
    , m_interfaceList(nullptr)
    , m_okButton(nullptr)
    , m_cancelButton(nullptr)
    , m_refreshButton(nullptr)
    , m_descriptionLabel(nullptr)
{
    setWindowTitle("Select Network Interface");
    setModal(true);
    resize(500, 400);
    
    setupUI();
    populateInterfaces();
}

InterfaceDialog::~InterfaceDialog() = default;

void InterfaceDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    
    // Description
    auto *titleLabel = new QLabel("Select a network interface to capture packets:", this);
    titleLabel->setStyleSheet("font-weight: bold; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);
    
    // Interface list
    m_interfaceList = new QListWidget(this);
    m_interfaceList->setAlternatingRowColors(true);
    mainLayout->addWidget(m_interfaceList);
    
    // Description label
    m_descriptionLabel = new QLabel("Select an interface to see its description.", this);
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setStyleSheet("color: #666; font-style: italic; margin: 10px 0;");
    mainLayout->addWidget(m_descriptionLabel);
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout();
    
    m_refreshButton = new QPushButton("Refresh", this);
    m_okButton = new QPushButton("OK", this);
    m_cancelButton = new QPushButton("Cancel", this);
    
    m_okButton->setDefault(true);
    m_okButton->setEnabled(false);
    
    buttonLayout->addWidget(m_refreshButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(m_interfaceList, &QListWidget::itemSelectionChanged,
            this, &InterfaceDialog::onSelectionChanged);
    connect(m_refreshButton, &QPushButton::clicked,
            this, &InterfaceDialog::refreshInterfaces);
    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void InterfaceDialog::populateInterfaces()
{
    m_interfaceList->clear();
    
    pcap_if_t *alldevs;
    char errbuf[PCAP_ERRBUF_SIZE];
    
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        QMessageBox::critical(this, "Error", QString("Error finding devices: %1").arg(errbuf));
        return;
    }
    
    for (pcap_if_t *d = alldevs; d != nullptr; d = d->next) {
        QString interfaceName = QString::fromUtf8(d->name);
        QString description = d->description ? QString::fromUtf8(d->description) : "No description available";
        
        auto *item = new QListWidgetItem(interfaceName, m_interfaceList);
        item->setData(Qt::UserRole, description);
        item->setToolTip(description);
        
        // Add icon based on interface type
        if (interfaceName.contains("eth") || interfaceName.contains("en")) {
            item->setIcon(QIcon(":/icons/ethernet.png"));
        } else if (interfaceName.contains("wlan") || interfaceName.contains("wifi")) {
            item->setIcon(QIcon(":/icons/wifi.png"));
        } else if (interfaceName.contains("lo")) {
            item->setIcon(QIcon(":/icons/loopback.png"));
        } else {
            item->setIcon(QIcon(":/icons/interface.png"));
        }
    }
    
    pcap_freealldevs(alldevs);
    
    if (m_interfaceList->count() == 0) {
        auto *item = new QListWidgetItem("No interfaces found", m_interfaceList);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        m_descriptionLabel->setText("No network interfaces were found. Make sure you have the necessary permissions.");
    }
}

void InterfaceDialog::refreshInterfaces()
{
    populateInterfaces();
    m_okButton->setEnabled(false);
    m_selectedInterface.clear();
    m_descriptionLabel->setText("Select an interface to see its description.");
}

void InterfaceDialog::onSelectionChanged()
{
    auto *currentItem = m_interfaceList->currentItem();
    if (currentItem && currentItem->flags() & Qt::ItemIsSelectable) {
        m_selectedInterface = currentItem->text();
        m_okButton->setEnabled(true);
        
        QString description = currentItem->data(Qt::UserRole).toString();
        m_descriptionLabel->setText(QString("Interface: %1\nDescription: %2")
                                   .arg(m_selectedInterface)
                                   .arg(description));
    } else {
        m_selectedInterface.clear();
        m_okButton->setEnabled(false);
        m_descriptionLabel->setText("Select an interface to see its description.");
    }
}

QString InterfaceDialog::selectedInterface() const
{
    return m_selectedInterface;
}
