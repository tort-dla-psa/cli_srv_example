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
friend class Admin;
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
        if(client_socket){
            client_socket->disconnectFromHost();
        }
    }

    auto get_addr()const{
        return this->addr;
    }
public slots:
    void onWrite(){
        size_t mes_size;
        read_all(client_socket, &mes_size, sizeof(mes_size));
        std::string mes;
        mes.resize(mes_size);
        auto ch_ptr = const_cast<char*>(mes.data());
        read_all(client_socket, ch_ptr, mes_size);
        emit gotMessage(mes);
    }
private slots:
    void onDisconnected(){
        emit this->disconnected();
    }
signals:
    void disconnected();
    void gotMessage(std::string mes);
};

class Admin:public Player{
public:
    Admin(QTcpSocket* cli_sock)
        :Player(cli_sock)
    {}

    //constructor that moves and secures data from player
    Admin(Player* plr)
        :Player(plr->client_socket)
    {
        this->addr = plr->addr;
        plr->client_socket = nullptr;
    }
};