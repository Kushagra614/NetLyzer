#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QLabel>
#include <QTimer>
#include <memory>

class PacketListWidget;
class PacketDetailsWidget;
class HexDumpWidget;
class InterfaceDialog;
class PacketCapture;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startCapture();
    void stopCapture();
    void selectInterface();
    void openFile();
    void saveFile();
    void showAbout();
    void clearPackets();
    void showStatistics();
    void applyFilter();
    void updateStatus();

private:
    void setupUI();
    void setupMenus();
    void setupToolBar();
    void setupStatusBar();
    void connectSignals();

    // UI Components
    QWidget *m_centralWidget;
    QSplitter *m_mainSplitter;
    QSplitter *m_rightSplitter;
    
    PacketListWidget *m_packetListWidget;
    PacketDetailsWidget *m_packetDetailsWidget;
    HexDumpWidget *m_hexDumpWidget;
    
    // Menu and toolbar actions
    QAction *m_startCaptureAction;
    QAction *m_stopCaptureAction;
    QAction *m_selectInterfaceAction;
    QAction *m_openFileAction;
    QAction *m_saveFileAction;
    QAction *m_clearPacketsAction;
    QAction *m_exitAction;
    QAction *m_aboutAction;
    QAction *m_statisticsAction;
    
    // Status bar
    QLabel *m_statusLabel;
    QLabel *m_packetCountLabel;
    QLabel *m_interfaceLabel;
    
    // Core components
    std::unique_ptr<PacketCapture> m_packetCapture;
    QTimer *m_statusTimer;
    
    QString m_currentInterface;
    bool m_isCapturing;
    int m_packetCount;
};

#endif // MAINWINDOW_H
