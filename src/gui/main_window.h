#pragma once

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <memory>
#include "../network/packet_sniffer.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startCapture();
    void stopCapture();
    void resetTable();
    void displayPacket(const PacketSniffer::PacketData &packet);

private:
    void setupUI();
    void populateInterfaces();

    QTableWidget *packetTable_;
    QPushButton *startButton_;
    QPushButton *stopButton_;
    QPushButton *resetButton_;
    QComboBox *interfaceSelector_;

    std::unique_ptr<PacketSniffer> sniffer_;
};