#ifndef HEXDUMPWIDGET_H
#define HEXDUMPWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>

class HexDumpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HexDumpWidget(QWidget *parent = nullptr);
    ~HexDumpWidget();

    void showHexData(const QByteArray &data);
    void clearData();

private:
    void setupUI();
    QString formatHexDump(const QByteArray &data);

    QTextEdit *m_textEdit;
    QVBoxLayout *m_layout;
};

#endif // HEXDUMPWIDGET_H
