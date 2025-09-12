#ifndef INTERFACEDIALOG_H
#define INTERFACEDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

class InterfaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InterfaceDialog(QWidget *parent = nullptr);
    ~InterfaceDialog();

    QString selectedInterface() const;

private slots:
    void refreshInterfaces();
    void onSelectionChanged();

private:
    void setupUI();
    void populateInterfaces();

    QListWidget *m_interfaceList;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QPushButton *m_refreshButton;
    QLabel *m_descriptionLabel;
    
    QString m_selectedInterface;
};

#endif // INTERFACEDIALOG_H
