#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QStatusBar>
#include <QTimer>
#include <memory>
#include "../network/packet_sniffer.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startCapture();
    void stopCapture();
    void resetTable();
    void updateStatistics();

private:
    void setupUI();
    void populateInterfaces();
    void displayPacket(const PacketSniffer::PacketData &packet);

    QComboBox *interfaceSelector_;
    QPushButton *startButton_;
    QPushButton *stopButton_;
    QPushButton *resetButton_;
    QTableWidget *packetTable_;
    QLabel *statusLabel_;
    QTimer *statsTimer_;

    std::unique_ptr<PacketSniffer> sniffer_;
    int packetCount_;
};

#endif // MAIN_WINDOW_H