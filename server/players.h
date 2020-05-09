#pragma once
#include <QTcpSocket>
#include <QHostAddress>
#include <QObject>
#include <QString>
#include "helpers.h"
#include <iostream>

class Player:public QObject{
    Q_OBJECT
protected:
    QTcpSocket* client_socket;
    QHostAddress addr;
public:
    Player(QTcpSocket* cli_sock)
        :QObject(),
        client_socket(cli_sock)
    {
        connect(client_socket, &QTcpSocket::readyRead,
            this, &Player::onWrite);
        //re-emit
        connect(client_socket, &QTcpSocket::disconnected,
            this, &Player::onDisconnected);
        addr = client_socket->peerAddress();
    }

    ~Player(){
        client_socket->disconnectFromHost();
    }

    auto get_addr()const{
        return this->addr;
    }
public slots:
    void onWrite(){
        size_t mes_size;
        read_all(client_socket, &mes_size, sizeof(mes_size));
        std::cout<<"client wrote bytes count:"<<mes_size<<"\n";
        std::string mes;
        mes.resize(mes_size);
        auto ch_ptr = const_cast<char*>(mes.data());
        read_all(client_socket, ch_ptr, mes_size);
        std::cout<<"client wrote something:"<<mes<<"\n";
    }
private slots:
    void onDisconnected(){
        emit this->disconnected();
    }
signals:
    void disconnected();
};

class Admin:public Player{
public:
    Admin(QTcpSocket* cli_sock)
        :Player(cli_sock)
    {}
};