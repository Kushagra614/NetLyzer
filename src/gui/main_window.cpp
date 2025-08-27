
#include "main_window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QTextEdit>
#include <QSplitter>
#include <QGroupBox>
#include <QStatusBar>
#include <QTimer>
#include <QProgressBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), sniffer_(std::make_unique<PacketSniffer>()) {
    setupUI();
    populateInterfaces();
    resize(1200, 700);
    
    // Setup status bar
    statusBar()->showMessage("Ready");
    
    // Setup timer for statistics updates
    statsTimer_ = new QTimer(this);
    connect(statsTimer_, &QTimer::timeout, this, &MainWindow::updateStatistics);
}

MainWindow::~MainWindow() {
    sniffer_->stop_capture();
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Control panel
    QGroupBox *controlGroup = new QGroupBox("Network Interface Control");
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroup);
    
    QLabel *interfaceLabel = new QLabel("Interface:");
    interfaceSelector_ = new QComboBox();
    interfaceSelector_->setMinimumWidth(200);
    
    startButton_ = new QPushButton("Start Capture");
    stopButton_ = new QPushButton("Stop Capture");
    resetButton_ = new QPushButton("Reset Table");
    
    startButton_->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; }");
    stopButton_->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-weight: bold; padding: 8px; }");
    resetButton_->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font-weight: bold; padding: 8px; }");
    
    stopButton_->setEnabled(false);

    controlLayout->addWidget(interfaceLabel);
    controlLayout->addWidget(interfaceSelector_);
    controlLayout->addWidget(startButton_);
    controlLayout->addWidget(stopButton_);
    controlLayout->addWidget(resetButton_);
    controlLayout->addStretch();

    // Statistics panel
    QGroupBox *statsGroup = new QGroupBox("Statistics");
    QHBoxLayout *statsLayout = new QHBoxLayout(statsGroup);
    
    packetsLabel_ = new QLabel("Packets: 0");
    droppedLabel_ = new QLabel("Dropped: 0");
    rateLabel_ = new QLabel("Rate: 0 pps");
    
    packetsLabel_->setStyleSheet("QLabel { font-weight: bold; color: #2E7D32; }");
    droppedLabel_->setStyleSheet("QLabel { font-weight: bold; color: #D32F2F; }");
    rateLabel_->setStyleSheet("QLabel { font-weight: bold; color: #1976D2; }");
    
    statsLayout->addWidget(packetsLabel_);
    statsLayout->addWidget(droppedLabel_);
    statsLayout->addWidget(rateLabel_);
    statsLayout->addStretch();

    // Create splitter for main content
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal);
    
    // Left panel - Packet table
    QWidget *leftWidget = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    
    packetTable_ = new QTableWidget();
    packetTable_->setColumnCount(7);
    packetTable_->setHorizontalHeaderLabels({
        "Timestamp", "Source IP", "Destination IP", 
        "Protocol", "Source Port", "Destination Port", "Length"
    });
    packetTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    packetTable_->setAlternatingRowColors(true);
    packetTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    leftLayout->addWidget(packetTable_);
    
    // Right panel - Packet details
    QWidget *rightWidget = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    
    QLabel *detailsLabel = new QLabel("Packet Details");
    detailsLabel->setStyleSheet("QLabel { font-weight: bold; font-size: 14px; }");
    
    packetDetails_ = new QTextEdit();
    packetDetails_->setReadOnly(true);
    packetDetails_->setMaximumWidth(400);
    packetDetails_->setFont(QFont("Courier", 9));
    
    rightLayout->addWidget(detailsLabel);
    rightLayout->addWidget(packetDetails_);
    
    mainSplitter->addWidget(leftWidget);
    mainSplitter->addWidget(rightWidget);
    mainSplitter->setSizes({800, 400});

    // Add all to main layout
    mainLayout->addWidget(controlGroup);
    mainLayout->addWidget(statsGroup);
    mainLayout->addWidget(mainSplitter);

    setCentralWidget(centralWidget);

    // Connect signals
    connect(startButton_, &QPushButton::clicked, this, &MainWindow::startCapture);
    connect(stopButton_, &QPushButton::clicked, this, &MainWindow::stopCapture);
    connect(resetButton_, &QPushButton::clicked, this, &MainWindow::resetTable);
    connect(packetTable_, &QTableWidget::cellClicked, this, &MainWindow::showPacketDetails);
}

void MainWindow::populateInterfaces() {
    auto interfaces = PacketSniffer::get_available_interfaces();
    for (const auto &iface : interfaces) {
        interfaceSelector_->addItem(QString::fromStdString(iface));
    }
    
    if (interfaces.empty()) {
        QMessageBox::warning(this, "Warning", "No network interfaces found. Make sure you have proper permissions.");
    }
}

void MainWindow::startCapture() {
    auto selectedInterface = interfaceSelector_->currentText().toStdString();
    if (selectedInterface.empty()) {
        QMessageBox::warning(this, "Error", "Please select a network interface.");
        return;
    }

    if (!sniffer_->init(selectedInterface)) {
        QMessageBox::critical(this, "Error", "Failed to initialize packet capture. Check permissions.");
        return;
    }

    sniffer_->set_packet_callback([this](const PacketSniffer::PacketData &packet) {
        QMetaObject::invokeMethod(this, [this, packet]() {
            displayPacket(packet);
        });
    });

    if (!sniffer_->start_capture()) {
        QMessageBox::critical(this, "Error", "Failed to start packet capture.");
        return;
    }
    
    startButton_->setEnabled(false);
    stopButton_->setEnabled(true);
    interfaceSelector_->setEnabled(false);
    statusBar()->showMessage("Capturing packets on " + QString::fromStdString(selectedInterface));
    
    packetCount_ = 0;
    startTime_ = QDateTime::currentDateTime();
    statsTimer_->start(1000); // Update every second
}

void MainWindow::stopCapture() {
    sniffer_->stop_capture();
    startButton_->setEnabled(true);
    stopButton_->setEnabled(false);
    interfaceSelector_->setEnabled(true);
    statusBar()->showMessage("Capture stopped");
    statsTimer_->stop();
}

void MainWindow::resetTable() {
    packetTable_->setRowCount(0);
    packetDetails_->clear();
    packetCount_ = 0;
    packetsLabel_->setText("Packets: 0");
    droppedLabel_->setText("Dropped: 0");
    rateLabel_->setText("Rate: 0 pps");
}

void MainWindow::displayPacket(const PacketSniffer::PacketData &packet) {
    int row = packetTable_->rowCount();
    packetTable_->insertRow(row);

    packetTable_->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(packet.timestamp)));
    packetTable_->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(packet.source_ip)));
    packetTable_->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(packet.dest_ip)));
    packetTable_->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(packet.protocol)));
    packetTable_->setItem(row, 4, new QTableWidgetItem(QString::number(packet.source_port)));
    packetTable_->setItem(row, 5, new QTableWidgetItem(QString::number(packet.dest_port)));
    packetTable_->setItem(row, 6, new QTableWidgetItem(QString::number(packet.length)));

    // Color code by protocol
    QColor color;
    if (packet.protocol == "TCP") color = QColor(200, 255, 200);
    else if (packet.protocol == "UDP") color = QColor(255, 200, 200);
    else if (packet.protocol == "ICMP") color = QColor(200, 200, 255);
    else color = QColor(255, 255, 200);

    for (int i = 0; i < 7; ++i) {
        packetTable_->item(row, i)->setBackground(color);
    }

    // Auto-scroll to latest packet
    packetTable_->scrollToBottom();
    
    packetCount_++;
}

void MainWindow::showPacketDetails(int row, int column) {
    if (row < 0 || row >= packetTable_->rowCount()) return;
    
    QString details = "=== PACKET DETAILS ===\n\n";
    details += "Timestamp: " + packetTable_->item(row, 0)->text() + "\n";
    details += "Source IP: " + packetTable_->item(row, 1)->text() + "\n";
    details += "Destination IP: " + packetTable_->item(row, 2)->text() + "\n";
    details += "Protocol: " + packetTable_->item(row, 3)->text() + "\n";
    details += "Source Port: " + packetTable_->item(row, 4)->text() + "\n";
    details += "Destination Port: " + packetTable_->item(row, 5)->text() + "\n";
    details += "Packet Length: " + packetTable_->item(row, 6)->text() + " bytes\n\n";
    
    QString protocol = packetTable_->item(row, 3)->text();
    if (protocol == "TCP") {
        details += "=== TCP INFORMATION ===\n";
        details += "• Transmission Control Protocol\n";
        details += "• Connection-oriented, reliable\n";
        details += "• Used for web browsing, email, file transfer\n";
    } else if (protocol == "UDP") {
        details += "=== UDP INFORMATION ===\n";
        details += "• User Datagram Protocol\n";
        details += "• Connectionless, fast\n";
        details += "• Used for DNS, streaming, gaming\n";
    } else if (protocol == "ICMP") {
        details += "=== ICMP INFORMATION ===\n";
        details += "• Internet Control Message Protocol\n";
        details += "• Used for network diagnostics\n";
        details += "• ping, traceroute commands use ICMP\n";
    }
    
    packetDetails_->setText(details);
}

void MainWindow::updateStatistics() {
    auto stats = sniffer_->get_statistics();
    packetsLabel_->setText(QString("Packets: %1").arg(stats.packets_captured));
    droppedLabel_->setText(QString("Dropped: %1").arg(stats.packets_dropped));
    
    // Calculate packet rate
    if (startTime_.isValid()) {
        qint64 elapsed = startTime_.secsTo(QDateTime::currentDateTime());
        if (elapsed > 0) {
            double rate = static_cast<double>(packetCount_) / elapsed;
            rateLabel_->setText(QString("Rate: %1 pps").arg(rate, 0, 'f', 1));
        }
    }
}
