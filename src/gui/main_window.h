
#pragma once

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QTextEdit>
#include <QTimer>
#include <QDateTime>
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
    void showPacketDetails(int row, int column);
    void updateStatistics();

private:
    void setupUI();
    void populateInterfaces();

    // UI Components
    QTableWidget *packetTable_;
    QPushButton *startButton_;
    QPushButton *stopButton_;
    QPushButton *resetButton_;
    QComboBox *interfaceSelector_;
    QTextEdit *packetDetails_;
    
    // Statistics labels
    QLabel *packetsLabel_;
    QLabel *droppedLabel_;
    QLabel *rateLabel_;
    
    // Statistics tracking
    QTimer *statsTimer_;
    QDateTime startTime_;
    int packetCount_;

    std::unique_ptr<PacketSniffer> sniffer_;
};
