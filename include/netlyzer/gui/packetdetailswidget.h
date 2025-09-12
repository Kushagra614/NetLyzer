#ifndef PACKETDETAILSWIDGET_H
#define PACKETDETAILSWIDGET_H

#include <QWidget>
#include <QTreeView>
#include <QStandardItemModel>
#include <QVBoxLayout>

class PacketDetailsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PacketDetailsWidget(QWidget *parent = nullptr);
    ~PacketDetailsWidget();

    void showPacketDetails(int packetNumber);
    void clearDetails();

private:
    void setupUI();
    void addProtocolLayer(const QString &name, const QStringList &fields);

    QTreeView *m_treeView;
    QStandardItemModel *m_model;
    QVBoxLayout *m_layout;
};

#endif // PACKETDETAILSWIDGET_H
