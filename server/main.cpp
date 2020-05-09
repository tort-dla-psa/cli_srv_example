#include <QVector>
#include <QTcpSocket>
#include <QTcpServer>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QCoreApplication>
#include <QtCore>
#include <iostream>
#include "server.h"

int main(int argc, char* argv[]){
    QCoreApplication a(argc, argv);
    Server s(1337);
    QObject::connect(&s, &Server::finished,
        &a, &QCoreApplication::quit);
    QTimer::singleShot(0, &s, &Server::run);

    return a.exec();
}