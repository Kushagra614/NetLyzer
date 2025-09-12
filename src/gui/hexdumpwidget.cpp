#include "netlyzer/gui/hexdumpwidget.h"
#include <QFont>

HexDumpWidget::HexDumpWidget(QWidget *parent)
    : QWidget(parent)
    , m_textEdit(nullptr)
    , m_layout(nullptr)
{
    setupUI();
}

HexDumpWidget::~HexDumpWidget() = default;

void HexDumpWidget::setupUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    m_textEdit = new QTextEdit(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setLineWrapMode(QTextEdit::NoWrap);
    
    // Set monospace font for hex display
    QFont font("Consolas, Monaco, monospace");
    font.setPointSize(9);
    m_textEdit->setFont(font);
    
    // Set background color for better contrast
    m_textEdit->setStyleSheet("QTextEdit { background-color: #f8f8f8; }");
    
    m_layout->addWidget(m_textEdit);
}

void HexDumpWidget::showHexData(const QByteArray &data)
{
    QString hexDump = formatHexDump(data);
    m_textEdit->setPlainText(hexDump);
}

void HexDumpWidget::clearData()
{
    m_textEdit->clear();
}

QString HexDumpWidget::formatHexDump(const QByteArray &data)
{
    QString result;
    const int bytesPerLine = 16;
    
    for (int i = 0; i < data.size(); i += bytesPerLine) {
        // Offset
        result += QString("%1  ").arg(i, 4, 16, QChar('0')).toUpper();
        
        // Hex bytes
        QString hexPart;
        QString asciiPart;
        
        for (int j = 0; j < bytesPerLine; ++j) {
            if (i + j < data.size()) {
                unsigned char byte = static_cast<unsigned char>(data[i + j]);
                hexPart += QString("%1 ").arg(byte, 2, 16, QChar('0')).toUpper();
                
                // ASCII representation
                if (byte >= 32 && byte <= 126) {
                    asciiPart += QChar(byte);
                } else {
                    asciiPart += '.';
                }
            } else {
                hexPart += "   ";
                asciiPart += ' ';
            }
            
            // Add extra space after 8 bytes
            if (j == 7) {
                hexPart += " ";
            }
        }
        
        result += hexPart + " |" + asciiPart + "|\n";
    }
    
    return result;
}
