#include <QApplication>

#include "Controller.h"

#include <gst/gst.h>

int main(int argc, char *argv[])
{
    gst_init(&argc, &argv);

    QApplication a(argc, argv);

    Controller controller;

    return a.exec();
}
