#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initialize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::
updatePortList(
        const QStringList &portList)
{
    ui->comboBoxSerialPortNames->clear();

    for (auto port : portList)
    {
        ui->comboBoxSerialPortNames->addItem(port);
    }
}

void MainWindow::
updateSerialState(
        const bool &isConnected)
{
    if (isConnected == true)
    {
        ui->pushButtonConnect->setText("Disconnected");
    }
    else
    {
        ui->pushButtonConnect->setText("Connected");
    }

    ui->pushButtonConnect->setChecked(isConnected);
    ui->comboBoxSerialPortNames->setEnabled(!isConnected);
}

QString MainWindow::
portName() const
{
    return ui->comboBoxSerialPortNames->currentText();
}

void MainWindow::
setTrackState(const bool &isTracking)
{
    QString trackingStateString = "";

    if (isTracking == true)
    {
        trackingStateString = "Tracking";
    }
    else
    {
        trackingStateString = "Not Tracking";
    }

    ui->labelTrackState->
            setText(trackingStateString);
}

WId MainWindow::
getWindowID() const
{
    return ui->widgetRenderer->winId();
}

void MainWindow::
initialize()
{
    ui->widgetRenderer->setMouseTracking(true);
    ui->widgetRenderer->installEventFilter(this);
}

bool MainWindow::
eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->widgetRenderer)
    {
        if (event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent =
                    reinterpret_cast<QMouseEvent*>(event);

            if (mouseEvent->button() == Qt::LeftButton)
            {
                const QPoint mousePos =
                        mouseEvent->pos();

                const QSize widgetSize =
                        ui->widgetRenderer->size();

                const QPoint realValue(
                            mousePos.x() -
                            (widgetSize.width() / 2),
                            -1 * (mousePos.y() -
                            (widgetSize.height() / 2)));

                ui->spinBoxX_Error->setValue(realValue.x());
                ui->spinBoxY_Error->setValue(realValue.y());

                Q_EMIT sigStartTrackRequested(
                            realValue.x(), realValue.y());
            }
            else
            {
                ui->spinBoxX_Error->setValue(0);
                ui->spinBoxY_Error->setValue(0);

                Q_EMIT sigStopTrackRequested();
            }
        }
    }

    return false;
}

void MainWindow::
on_pushButtonConnect_clicked(bool checked)
{
    Q_EMIT sigChangeSerialConnectionRequseted(checked);
}

void MainWindow::
on_pushButtonRefresh_clicked()
{
    if (ui->pushButtonConnect->
            isChecked() == false)
    {
        Q_EMIT sigRefreshSerialPortListRequested();
    }
}

void MainWindow::on_pushButtonOSD_On_clicked()
{
    Q_EMIT sigChangeOSD_Requested(true);
}

void MainWindow::on_pushButtonOSD_Off_clicked()
{
    Q_EMIT sigChangeOSD_Requested(false);
}
