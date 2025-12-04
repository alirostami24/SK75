#pragma once

#include <QDebug>
#include <QMouseEvent>
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

    void updatePortList(
            const QStringList &portList);

    void updateSerialState(
            const bool &isConnected);

    QString portName() const;

    void setTrackState(
            const bool &isTracking);

    WId getWindowID() const;

private:
    Ui::MainWindow *ui;

    void initialize();

    bool eventFilter(QObject *object, QEvent *event);

Q_SIGNALS:
    void sigChangeSerialConnectionRequseted(
            const bool &shouldConnect);

    void sigRefreshSerialPortListRequested();

    void sigStartTrackRequested(const int16_t &xPos,
                                const int16_t &pos);

    void sigStopTrackRequested();

    void sigChangeOSD_Requested(
            const bool &state);

private Q_SLOTS:
    void on_pushButtonConnect_clicked(bool checked);

    void on_pushButtonRefresh_clicked();
    void on_pushButtonOSD_On_clicked();
    void on_pushButtonOSD_Off_clicked();
};
