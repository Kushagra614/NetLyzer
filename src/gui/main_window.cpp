#include "main_window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), sniffer_(std::make_unique<PacketSniffer>()) {
    setupUI();
    populateInterfaces();
}

MainWindow::~MainWindow() {
    sniffer_->stop_capture();
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Interface selection and buttons
    QHBoxLayout *controlLayout = new QHBoxLayout();
    interfaceSelector_ = new QComboBox();
    startButton_ = new QPushButton("Start Capture");
    stopButton_ = new QPushButton("Stop Capture");
    resetButton_ = new QPushButton("Reset Table");

    controlLayout->addWidget(interfaceSelector_);
    controlLayout->addWidget(startButton_);
    controlLayout->addWidget(stopButton_);
    controlLayout->addWidget(resetButton_);

    // Packet table setup
    packetTable_ = new QTableWidget();
    packetTable_->setColumnCount(6);
    packetTable_->setHorizontalHeaderLabels({"Timestamp", "Source IP", "Destination IP", "Protocol", "Source Port", "Destination Port"});
    packetTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Add widgets to main layout
    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(packetTable_);

    setCentralWidget(centralWidget);

    // Connect buttons to slots
    connect(startButton_, &QPushButton::clicked, this, &MainWindow::startCapture);
    connect(stopButton_, &QPushButton::clicked, this, &MainWindow::stopCapture);
    connect(resetButton_, &QPushButton::clicked, this, &MainWindow::resetTable);
}

void MainWindow::populateInterfaces() {
    auto interfaces = PacketSniffer::get_available_interfaces();
    for (const auto &iface : interfaces) {
        interfaceSelector_->addItem(QString::fromStdString(iface));
    }
}

void MainWindow::startCapture() {
    auto selectedInterface = interfaceSelector_->currentText().toStdString();
    if (selectedInterface.empty()) {
        QMessageBox::warning(this, "Error", "Please select a network interface.");
        return;
    }

    if (!sniffer_->init(selectedInterface)) {
        QMessageBox::critical(this, "Error", "Failed to initialize packet capture.");
        return;
    }

    sniffer_->set_packet_callback([this](const PacketSniffer::PacketData &packet) {
        QMetaObject::invokeMethod(this, [this, packet]() {
            displayPacket(packet);
        });
    });

    if (!sniffer_->start_capture()) {
        QMessageBox::critical(this, "Error", "Failed to start packet capture.");
    }
}

void MainWindow::stopCapture() {
    sniffer_->stop_capture();
}

void MainWindow::resetTable() {
    packetTable_->setRowCount(0);
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
}