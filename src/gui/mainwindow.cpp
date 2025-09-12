#include "netlyzer/gui/mainwindow.h"
#include "netlyzer/gui/packetlistwidget.h"
#include "netlyzer/gui/packetdetailswidget.h"
#include "netlyzer/gui/hexdumpwidget.h"
#include "netlyzer/gui/interfacedialog.h"
#include "netlyzer/network/packet_capture.h"

#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QToolButton>
#include <QProgressBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainSplitter(nullptr)
    , m_rightSplitter(nullptr)
    , m_packetListWidget(nullptr)
    , m_packetDetailsWidget(nullptr)
    , m_hexDumpWidget(nullptr)
    , m_statusLabel(nullptr)
    , m_packetCountLabel(nullptr)
    , m_interfaceLabel(nullptr)
    , m_packetCapture(nullptr)
    , m_statusTimer(new QTimer(this))
    , m_isCapturing(false)
    , m_packetCount(0)
{
    setWindowTitle("NetLyzer - Network Packet Analyzer");
    setWindowIcon(QIcon(":/icons/netlyzer.png"));
    resize(1200, 800);
    
    setupUI();
    setupMenus();
    setupToolBar();
    setupStatusBar();
    connectSignals();
    
    // Initialize packet capture
    m_packetCapture = std::make_unique<PacketCapture>();
    
    // Setup status timer
    m_statusTimer->setInterval(1000); // Update every second
    connect(m_statusTimer, &QTimer::timeout, this, &MainWindow::updateStatus);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    // Create main layout
    auto *mainLayout = new QVBoxLayout(m_centralWidget);
    
    // Create filter bar
    auto *filterLayout = new QHBoxLayout();
    auto *filterLabel = new QLabel("Filter:", this);
    auto *filterEdit = new QLineEdit(this);
    filterEdit->setPlaceholderText("Enter display filter (e.g., tcp.port == 80)");
    auto *applyFilterBtn = new QToolButton(this);
    applyFilterBtn->setText("Apply");
    auto *clearFilterBtn = new QToolButton(this);
    clearFilterBtn->setText("Clear");
    
    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(filterEdit, 1);
    filterLayout->addWidget(applyFilterBtn);
    filterLayout->addWidget(clearFilterBtn);
    
    mainLayout->addLayout(filterLayout);
    
    // Create splitters for layout
    m_mainSplitter = new QSplitter(Qt::Vertical, this);
    m_rightSplitter = new QSplitter(Qt::Vertical, this);
    
    // Create widgets
    m_packetListWidget = new PacketListWidget(this);
    m_packetDetailsWidget = new PacketDetailsWidget(this);
    m_hexDumpWidget = new HexDumpWidget(this);
    
    // Setup splitter layout
    m_rightSplitter->addWidget(m_packetDetailsWidget);
    m_rightSplitter->addWidget(m_hexDumpWidget);
    m_rightSplitter->setSizes({300, 200});
    
    m_mainSplitter->addWidget(m_packetListWidget);
    m_mainSplitter->addWidget(m_rightSplitter);
    m_mainSplitter->setSizes({400, 500});
    
    mainLayout->addWidget(m_mainSplitter, 1);
}

void MainWindow::setupMenus()
{
    // File menu
    auto *fileMenu = menuBar()->addMenu("&File");
    
    m_openFileAction = new QAction("&Open...", this);
    m_openFileAction->setShortcut(QKeySequence::Open);
    m_openFileAction->setIcon(QIcon(":/icons/open.png"));
    
    m_saveFileAction = new QAction("&Save As...", this);
    m_saveFileAction->setShortcut(QKeySequence::SaveAs);
    m_saveFileAction->setIcon(QIcon(":/icons/save.png"));
    
    m_exitAction = new QAction("E&xit", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    
    fileMenu->addAction(m_openFileAction);
    fileMenu->addAction(m_saveFileAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_exitAction);
    
    // Capture menu
    auto *captureMenu = menuBar()->addMenu("&Capture");
    
    m_selectInterfaceAction = new QAction("&Interfaces...", this);
    m_selectInterfaceAction->setIcon(QIcon(":/icons/interface.png"));
    
    m_startCaptureAction = new QAction("&Start", this);
    m_startCaptureAction->setShortcut(QKeySequence("Ctrl+E"));
    m_startCaptureAction->setIcon(QIcon(":/icons/start.png"));
    
    m_stopCaptureAction = new QAction("S&top", this);
    m_stopCaptureAction->setShortcut(QKeySequence("Ctrl+E"));
    m_stopCaptureAction->setIcon(QIcon(":/icons/stop.png"));
    m_stopCaptureAction->setEnabled(false);
    
    captureMenu->addAction(m_selectInterfaceAction);
    captureMenu->addSeparator();
    captureMenu->addAction(m_startCaptureAction);
    captureMenu->addAction(m_stopCaptureAction);
    
    // View menu
    auto *viewMenu = menuBar()->addMenu("&View");
    
    m_clearPacketsAction = new QAction("&Clear Packets", this);
    m_clearPacketsAction->setShortcut(QKeySequence("Ctrl+L"));
    m_clearPacketsAction->setIcon(QIcon(":/icons/clear.png"));
    
    viewMenu->addAction(m_clearPacketsAction);
    
    // Statistics menu
    auto *statisticsMenu = menuBar()->addMenu("&Statistics");
    
    m_statisticsAction = new QAction("&Summary", this);
    m_statisticsAction->setIcon(QIcon(":/icons/statistics.png"));
    
    statisticsMenu->addAction(m_statisticsAction);
    
    // Help menu
    auto *helpMenu = menuBar()->addMenu("&Help");
    
    m_aboutAction = new QAction("&About NetLyzer", this);
    helpMenu->addAction(m_aboutAction);
}

void MainWindow::setupToolBar()
{
    auto *toolBar = addToolBar("Main");
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    
    toolBar->addAction(m_selectInterfaceAction);
    toolBar->addSeparator();
    toolBar->addAction(m_startCaptureAction);
    toolBar->addAction(m_stopCaptureAction);
    toolBar->addSeparator();
    toolBar->addAction(m_clearPacketsAction);
    toolBar->addSeparator();
    toolBar->addAction(m_openFileAction);
    toolBar->addAction(m_saveFileAction);
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel("Ready", this);
    m_packetCountLabel = new QLabel("Packets: 0", this);
    m_interfaceLabel = new QLabel("Interface: None", this);
    
    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(m_packetCountLabel);
    statusBar()->addPermanentWidget(m_interfaceLabel);
}

void MainWindow::connectSignals()
{
    // Menu actions
    connect(m_startCaptureAction, &QAction::triggered, this, &MainWindow::startCapture);
    connect(m_stopCaptureAction, &QAction::triggered, this, &MainWindow::stopCapture);
    connect(m_selectInterfaceAction, &QAction::triggered, this, &MainWindow::selectInterface);
    connect(m_openFileAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(m_saveFileAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(m_clearPacketsAction, &QAction::triggered, this, &MainWindow::clearPackets);
    connect(m_statisticsAction, &QAction::triggered, this, &MainWindow::showStatistics);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);
}

void MainWindow::startCapture()
{
    if (m_currentInterface.isEmpty()) {
        selectInterface();
        return;
    }
    
    if (m_packetCapture && m_packetCapture->startCapture(m_currentInterface)) {
        m_isCapturing = true;
        m_startCaptureAction->setEnabled(false);
        m_stopCaptureAction->setEnabled(true);
        m_statusLabel->setText("Capturing packets...");
        m_statusTimer->start();
    } else {
        QMessageBox::critical(this, "Error", "Failed to start packet capture!");
    }
}

void MainWindow::stopCapture()
{
    if (m_packetCapture) {
        m_packetCapture->stopCapture();
    }
    
    m_isCapturing = false;
    m_startCaptureAction->setEnabled(true);
    m_stopCaptureAction->setEnabled(false);
    m_statusLabel->setText("Capture stopped");
    m_statusTimer->stop();
}

void MainWindow::selectInterface()
{
    InterfaceDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        m_currentInterface = dialog.selectedInterface();
        m_interfaceLabel->setText(QString("Interface: %1").arg(m_currentInterface));
    }
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Capture File", "", "PCAP Files (*.pcap *.pcapng);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        // TODO: Implement file opening
        m_statusLabel->setText(QString("Opened: %1").arg(fileName));
    }
}

void MainWindow::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Capture File", "", "PCAP Files (*.pcap);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        // TODO: Implement file saving
        m_statusLabel->setText(QString("Saved: %1").arg(fileName));
    }
}

void MainWindow::clearPackets()
{
    m_packetListWidget->clearPackets();
    m_packetDetailsWidget->clearDetails();
    m_hexDumpWidget->clearData();
    m_packetCount = 0;
    updateStatus();
}

void MainWindow::showStatistics()
{
    // TODO: Implement statistics dialog
    QMessageBox::information(this, "Statistics", "Statistics feature coming soon!");
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About NetLyzer",
        "<h2>NetLyzer v1.0</h2>"
        "<p>A professional network packet analyzer built with Qt6 and C++.</p>"
        "<p>Features:</p>"
        "<ul>"
        "<li>Real-time packet capture</li>"
        "<li>Protocol analysis</li>"
        "<li>Hex dump viewer</li>"
        "<li>Filtering and search</li>"
        "</ul>"
        "<p>Built with ❤️ for network analysis</p>");
}

void MainWindow::applyFilter()
{
    // TODO: Implement filtering
}

void MainWindow::updateStatus()
{
    if (m_packetCapture) {
        m_packetCount = m_packetCapture->getPacketCount();
        m_packetCountLabel->setText(QString("Packets: %1").arg(m_packetCount));
    }
}
