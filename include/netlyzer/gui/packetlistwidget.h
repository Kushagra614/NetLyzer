#ifndef PACKETLISTWIDGET_H
#define PACKETLISTWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

class PacketListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PacketListWidget(QWidget *parent = nullptr);
    ~PacketListWidget();

    void addPacket(int number, const QString &time, const QString &source, 
                   const QString &destination, const QString &protocol, 
                   int length, const QString &info);
    void clearPackets();
    void applyFilter(const QString &filter);

signals:
    void packetSelected(int packetNumber);

private slots:
    void onSelectionChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    void setupUI();
    void setupModel();

    QTableView *m_tableView;
    QStandardItemModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
    QVBoxLayout *m_layout;
};

#endif // PACKETLISTWIDGET_H
