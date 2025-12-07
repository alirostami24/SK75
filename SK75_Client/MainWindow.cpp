#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::
MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initialize();
}

MainWindow::
~MainWindow()
{
    delete ui;
}

void MainWindow::
updateSerialPortsList(
        const QStringList &list)
{
    ui->comboBoxSerialPortName->clear();

    for (auto item : list)
    {
        ui->comboBoxSerialPortName->
                addItem(item);
    }
}

void MainWindow::
updateSerialPortButton(
        const bool &state)
{
    const QString buttonText =
            (state == true ?
                 "Disconnect" :
                 "Connect");

    ui->pushButtonConnect->
            setText(buttonText);

    ui->pushButtonConnect->
            setChecked(state);
}

QString MainWindow::
serialPortName() const
{
    return ui->comboBoxSerialPortName->
            currentText();
}

int32_t MainWindow::
serialPortBaudrate() const
{
    return ui->spinBoxBaudrate->value();
}

void MainWindow::
initialize()
{
    const QPixmap pixmap(":/refreshIcon");

    ui->toolButtonRefresh->
            setIconSize(QSize(24, 40));

    ui->toolButtonRefresh->
            setIcon(QIcon(pixmap));
}

void MainWindow::
on_toolButtonRefresh_clicked()
{
    Q_EMIT sigRefreshSerialPortListRequested();
}

void MainWindow::
on_pushButtonConnect_clicked()
{
    const bool state =
            ui->pushButtonConnect->isChecked();

    updateSerialPortButton(!state);

    if (state == true)
    {
        Q_EMIT sigConnectToSerialPortRequested();
    }
    else
    {
        Q_EMIT sigDisconnectFromSerialPortRequested();
    }
}

void MainWindow::
on_pushButtonAutoLock_clicked()
{
    Q_EMIT sigAutoLockRequested();
}

void MainWindow::
on_pushButtonStopTrack_clicked()
{
    Q_EMIT sigStopTrackRequested();
}
