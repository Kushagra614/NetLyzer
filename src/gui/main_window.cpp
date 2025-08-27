#include "main_window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), 
      sniffer_(std::make_unique<PacketSniffer>()),
      packetCount_(0) {

    setWindowTitle("NetLyzer - Network Packet Analyzer");
    setupUI();
    populateInterfaces();

    // Setup statistics timer
    statsTimer_ = new QTimer(this);
    connect(statsTimer_, &QTimer::timeout, this, &MainWindow::updateStatistics);

    resize(1200, 600);

    // Initial status
    statusLabel_->setText("Ready - Select interface and click Start Capture");
}

MainWindow::~MainWindow() {
    if (sniffer_) {
        sniffer_->stop_capture();
    }
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Control panel
    QHBoxLayout *controlLayout = new QHBoxLayout();

    // Interface selection
    controlLayout->addWidget(new QLabel("Interface:"));
    interfaceSelector_ = new QComboBox();
    interfaceSelector_->setMinimumWidth(200);
    controlLayout->addWidget(interfaceSelector_);

    // Control buttons
    startButton_ = new QPushButton("Start Capture");
    startButton_->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; }");
    stopButton_ = new QPushButton("Stop Capture");
    stopButton_->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-weight: bold; padding: 8px; }");
    stopButton_->setEnabled(false);
    resetButton_ = new QPushButton("Clear Table");
    resetButton_->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font-weight: bold; padding: 8px; }");

    controlLayout->addWidget(startButton_);
    controlLayout->addWidget(stopButton_);
    controlLayout->addWidget(resetButton_);
    controlLayout->addStretch();

    // Packet table
    packetTable_ = new QTableWidget();
    packetTable_->setColumnCount(7);
    QStringList headers = {"No.", "Timestamp", "Source IP", "Dest IP", "Protocol", "Src Port", "Dest Port"};
    packetTable_->setHorizontalHeaderLabels(headers);
    packetTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    packetTable_->setAlternatingRowColors(true);
    packetTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    packetTable_->setSortingEnabled(true);

    // Status bar
    statusLabel_ = new QLabel("Ready");
    statusBar()->addWidget(statusLabel_);

    // Layout assembly
    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(packetTable_);

    setCentralWidget(centralWidget);

    // Connect signals
    connect(startButton_, &QPushButton::clicked, this, &MainWindow::startCapture);
    connect(stopButton_, &QPushButton::clicked, this, &MainWindow::stopCapture);
    connect(resetButton_, &QPushButton::clicked, this, &MainWindow::resetTable);
}

void MainWindow::populateInterfaces() {
    auto interfaces = PacketSniffer::get_available_interfaces();
    if (interfaces.empty()) {
        QMessageBox::warning(this, "No Interfaces", "No network interfaces found! You may need to run with administrator privileges.");
        statusLabel_->setText("Error - No network interfaces available");
        startButton_->setEnabled(false);
        return;
    }

    for (const auto &iface : interfaces) {
        interfaceSelector_->addItem(QString::fromStdString(iface));
    }

    statusLabel_->setText(QString("Found %1 network interface(s)").arg(interfaces.size()));
}

void MainWindow::startCapture() {
    auto selectedInterface = interfaceSelector_->currentText().toStdString();
    if (selectedInterface.empty()) {
        QMessageBox::warning(this, "Error", "Please select a network interface.");
        return;
    }

    if (!sniffer_->init(selectedInterface)) {
        QMessageBox::critical(this, "Error", "Failed to initialize packet capture. Make sure you have proper permissions.");
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

    // Update UI state
    startButton_->setEnabled(false);
    stopButton_->setEnabled(true);
    interfaceSelector_->setEnabled(false);

    statusLabel_->setText(QString("Capturing on %1...").arg(QString::fromStdString(selectedInterface)));
    statsTimer_->start(1000); // Update stats every second
    packetCount_ = 0;
}

void MainWindow::stopCapture() {
    sniffer_->stop_capture();
    statsTimer_->stop();

    // Update UI state
    startButton_->setEnabled(true);
    stopButton_->setEnabled(false);
    interfaceSelector_->setEnabled(true);

    auto stats = sniffer_->get_statistics();
    statusLabel_->setText(QString("Capture stopped. Packets captured: %1, Dropped: %2")
                         .arg(stats.packets_captured)
                         .arg(stats.packets_dropped));
}

void MainWindow::resetTable() {
    packetTable_->setRowCount(0);
    packetCount_ = 0;
    statusLabel_->setText("Table cleared");
}

void MainWindow::updateStatistics() {
    auto stats = sniffer_->get_statistics();
    statusLabel_->setText(QString("Capturing... Packets: %1, Dropped: %2, Display: %3")
                         .arg(stats.packets_captured)
                         .arg(stats.packets_dropped)
                         .arg(packetCount_));
}

void MainWindow::displayPacket(const PacketSniffer::PacketData &packet) {
    int row = packetTable_->rowCount();
    packetTable_->insertRow(row);
    packetCount_++;

    // Add packet data to table
    packetTable_->setItem(row, 0, new QTableWidgetItem(QString::number(packetCount_)));
    packetTable_->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(packet.timestamp)));
    packetTable_->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(packet.source_ip)));
    packetTable_->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(packet.dest_ip)));
    packetTable_->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(packet.protocol)));
    packetTable_->setItem(row, 5, new QTableWidgetItem(QString::number(packet.source_port)));
    packetTable_->setItem(row, 6, new QTableWidgetItem(QString::number(packet.dest_port)));

    // Color code by protocol
    QColor rowColor;
    if (packet.protocol == "TCP") {
        rowColor = QColor(220, 255, 220); // Light green
    } else if (packet.protocol == "UDP") {
        rowColor = QColor(220, 220, 255); // Light blue
    } else if (packet.protocol == "ICMP") {
        rowColor = QColor(255, 255, 220); // Light yellow
    } else {
        rowColor = QColor(255, 220, 220); // Light red
    }

    for (int col = 0; col < packetTable_->columnCount(); ++col) {
        packetTable_->item(row, col)->setBackground(rowColor);
    }

    // Auto-scroll to latest packet
    packetTable_->scrollToBottom();
}