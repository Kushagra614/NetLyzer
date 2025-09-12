#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTextEdit>
#include <QSplitter>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QHeaderView>
#include <QStyleFactory>

class SimpleNetLyzer : public QMainWindow
{
    Q_OBJECT

public:
    SimpleNetLyzer(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("NetLyzer - Network Packet Analyzer");
        resize(1200, 800);
        
        setupUI();
        setupMenus();
        setupStatusBar();
    }

private:
    void setupUI()
    {
        auto *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        auto *mainLayout = new QVBoxLayout(centralWidget);
        
        // Create splitter for three-pane layout
        auto *splitter = new QSplitter(Qt::Vertical, this);
        
        // Packet list table
        m_packetTable = new QTableWidget(this);
        m_packetTable->setColumnCount(6);
        QStringList headers = {"No.", "Time", "Source", "Destination", "Protocol", "Length"};
        m_packetTable->setHorizontalHeaderLabels(headers);
        m_packetTable->setAlternatingRowColors(true);
        m_packetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        
        // Packet details
        m_detailsText = new QTextEdit(this);
        m_detailsText->setReadOnly(true);
        m_detailsText->setMaximumHeight(200);
        
        // Hex dump
        m_hexDump = new QTextEdit(this);
        m_hexDump->setReadOnly(true);
        m_hexDump->setMaximumHeight(200);
        m_hexDump->setFont(QFont("Courier", 9));
        
        splitter->addWidget(m_packetTable);
        splitter->addWidget(m_detailsText);
        splitter->addWidget(m_hexDump);
        splitter->setSizes({400, 200, 200});
        
        mainLayout->addWidget(splitter);
        
        // Add sample data
        addSamplePackets();
    }
    
    void setupMenus()
    {
        auto *fileMenu = menuBar()->addMenu("&File");
        fileMenu->addAction("&Open", this, &SimpleNetLyzer::openFile);
        fileMenu->addAction("&Save", this, &SimpleNetLyzer::saveFile);
        fileMenu->addSeparator();
        fileMenu->addAction("E&xit", this, &QWidget::close);
        
        auto *captureMenu = menuBar()->addMenu("&Capture");
        captureMenu->addAction("&Start", this, &SimpleNetLyzer::startCapture);
        captureMenu->addAction("S&top", this, &SimpleNetLyzer::stopCapture);
        
        auto *helpMenu = menuBar()->addMenu("&Help");
        helpMenu->addAction("&About", this, &SimpleNetLyzer::showAbout);
    }
    
    void setupStatusBar()
    {
        statusBar()->showMessage("Ready - NetLyzer v1.0");
        auto *packetLabel = new QLabel("Packets: 3", this);
        statusBar()->addPermanentWidget(packetLabel);
    }
    
    void addSamplePackets()
    {
        // Add sample packet data
        QStringList packet1 = {"1", "12:34:56.123", "192.168.1.100", "192.168.1.1", "TCP", "74"};
        QStringList packet2 = {"2", "12:34:56.456", "192.168.1.1", "192.168.1.100", "TCP", "60"};
        QStringList packet3 = {"3", "12:34:57.789", "192.168.1.100", "8.8.8.8", "UDP", "64"};
        
        addPacketRow(packet1);
        addPacketRow(packet2);
        addPacketRow(packet3);
        
        m_packetTable->resizeColumnsToContents();
    }
    
    void addPacketRow(const QStringList &data)
    {
        int row = m_packetTable->rowCount();
        m_packetTable->insertRow(row);
        
        for (int col = 0; col < data.size(); ++col) {
            auto *item = new QTableWidgetItem(data[col]);
            if (col == 4) { // Protocol column
                if (data[col] == "TCP") {
                    item->setBackground(QColor(220, 255, 220));
                } else if (data[col] == "UDP") {
                    item->setBackground(QColor(255, 255, 220));
                }
            }
            m_packetTable->setItem(row, col, item);
        }
    }
    
private slots:
    void openFile() {
        QMessageBox::information(this, "Open File", "Open file functionality will be implemented.");
    }
    
    void saveFile() {
        QMessageBox::information(this, "Save File", "Save file functionality will be implemented.");
    }
    
    void startCapture() {
        QMessageBox::information(this, "Start Capture", "Packet capture will be implemented with libpcap.");
    }
    
    void stopCapture() {
        QMessageBox::information(this, "Stop Capture", "Stop capture functionality will be implemented.");
    }
    
    void showAbout() {
        QMessageBox::about(this, "About NetLyzer",
            "<h2>NetLyzer v1.0</h2>"
            "<p>A professional network packet analyzer built with Qt6 and C++.</p>"
            "<p><b>Features:</b></p>"
            "<ul>"
            "<li>Real-time packet capture</li>"
            "<li>Protocol analysis</li>"
            "<li>Hex dump viewer</li>"
            "<li>Modern Qt interface</li>"
            "</ul>"
            "<p>Built with ❤️ for network analysis</p>");
    }

private:
    QTableWidget *m_packetTable;
    QTextEdit *m_detailsText;
    QTextEdit *m_hexDump;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("NetLyzer");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("NetLyzer Team");
    
    // Set modern style
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Apply dark theme
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(darkPalette);
    
    // Create and show main window
    SimpleNetLyzer window;
    window.show();
    
    return app.exec();
}

#include "main.moc"
