#pragma once

#include <QDebug>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateSerialPortsList(
            const QStringList &list);

    void updateSerialPortButton(
            const bool &state);

    QString serialPortName() const;
    int32_t serialPortBaudrate() const;

private:
    Ui::MainWindow *ui;

    void initialize();


Q_SIGNALS:
    void sigRefreshSerialPortListRequested();

    void sigConnectToSerialPortRequested();
    void sigDisconnectFromSerialPortRequested();

    void sigAutoLockRequested();
    void sigStopTrackRequested();

private Q_SLOTS:
    void on_toolButtonRefresh_clicked();

    void on_pushButtonConnect_clicked();
    void on_pushButtonAutoLock_clicked();
    void on_pushButtonStopTrack_clicked();
};
