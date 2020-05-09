#pragma once
#include <QVector>
#include <QTcpSocket>
#include <QTcpServer>
#include <QObject>
#include <QString>
#include "players.h"
#include <iostream>

class Server:public QTcpServer{
    Q_OBJECT

    QVector<Player*> clients;
    Admin* adm;
    quint16 port;
public:
    Server(quint16 port, QObject* parent=nullptr):
        QTcpServer(parent),
        port(port)
    {
        connect(this, &QTcpServer::newConnection,
            this, &Server::onNewConnection);
    }
    ~Server(){
        for(auto &cl:clients){
            delete cl;
        }
    }
public slots:
    void onPlayerDisconnect(){
        auto pl = dynamic_cast<Player*>(sender());
        auto addr = pl->get_addr();
        auto addr_str = addr.toString().toStdString();
        std::cout<<"someone disconnected:"<<addr_str<<"\n";
        clients.removeOne(pl);

        auto adm_cast = dynamic_cast<Admin*>(pl);
        if(adm_cast){
            std::cout<<"admin disconnected!\n";
            clients.clear();
            emit finished();
        }
    }

    void onNewConnection(){
        //[virtual]QTcpSocket *QTcpServer::nextPendingConnection()
        auto client_socket =  nextPendingConnection();
        auto addr = client_socket->peerAddress().toString();
        std::cout<<"new connection:"<<addr.toStdString()<<"\n";
        if(clients.size() == 0){
            auto adm = new Admin(client_socket);
            this->adm = adm;
            clients.push_back(adm);
        }else{
            auto plr = new Player(client_socket);
            clients.push_back(plr);
        }
        Player* new_connection = clients.back();
        connect(new_connection, &Player::disconnected,
            this, &Server::onPlayerDisconnect);
    }
    void run(){
        this->listen(QHostAddress::Any, port);
    }
signals:
    void finished();
};