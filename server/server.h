#pragma once
#include <QVector>
#include <QTcpSocket>
#include <QTcpServer>
#include <QObject>
#include <QString>
#include "players.h"
#include "id_gen.h"
#include "rooms.h"
#include <iostream>

class Server:public QTcpServer{
    Q_OBJECT

    quint16 port;
    WaitingRoom* wait_room;
    QVector<GameRoom*> rooms;

    IdGenerator gen;
public:
    Server(quint16 port, QObject* parent=nullptr):
        QTcpServer(parent),
        port(port)
    {
        wait_room = new WaitingRoom();
        connect(wait_room, &WaitingRoom::roomJoinRequested,
            this, &Server::onRoomJoinRequested);
        connect(wait_room, &WaitingRoom::roomCreateRequested,
            this, &Server::onRoomCreateRequested);
        connect(this, &QTcpServer::newConnection,
            this, &Server::onNewConnection);
    }
    ~Server(){
        delete wait_room;
    }
public slots:
    void onNewConnection(){
        //[virtual]QTcpSocket *QTcpServer::nextPendingConnection()
        auto client_socket =  nextPendingConnection();
        auto addr = client_socket->peerAddress().toString();
        std::cout<<"new connection:"<<addr.toStdString()<<"\n";
        auto plr = new Player(client_socket);
        connect(plr, &Player::gotMessage,
            wait_room, &WaitingRoom::onPlayerMessage);
        wait_room->addPlayer(plr);
    }
    void run(){
        this->listen(QHostAddress::Any, port);
    }
    void onRoomJoinRequested(Player* plr, std::string id, std::string pass){
        auto predicate = [&id](GameRoom* room){
            if(room->get_id() == id){
                return true;
            }else{
                return false;
            }
        };
        auto room_it = std::find_if(rooms.begin(), rooms.end(), predicate);
        if(room_it == rooms.end()){
        }else{
            auto room = *room_it;
            if(room->get_pass() == pass){
                wait_room->removePlayer(plr);
                disconnect(plr, &Player::gotMessage,
                    wait_room, &WaitingRoom::onPlayerMessage);
                disconnect(plr, &Player::disconnected,
                    wait_room, &WaitingRoom::onPlayerDisconnect);
                disconnect(plr, &Player::disconnected,
                    wait_room, &WaitingRoom::onPlayerDisconnect);
                connect(plr, &Player::gotMessage,
                    room, &GameRoom::onPlayerMessage);
                room->addPlayer(plr);
            }
        }
    }
    void onRoomCreateRequested(Player* plr, std::string pass){
        auto id = gen.get_id();
        wait_room->removePlayer(plr);
        disconnect(plr, &Player::gotMessage,
            wait_room, &WaitingRoom::onPlayerMessage);
        disconnect(plr, &Player::disconnected,
            wait_room, &WaitingRoom::onPlayerDisconnect);
        auto adm = new Admin(plr);
        delete plr;

        auto room = new GameRoom(adm, pass);
        room->id = gen.get_id();
        std::cout<<"new room, id:"<<room->get_id()
            <<" pass:"<<room->get_pass()<<"\n";
        connect(adm, &Player::gotMessage,
            room, &GameRoom::onPlayerMessage);
        connect(adm, &Player::disconnected,
            room, &GameRoom::onPlayerDisconnect);
        connect(room, &Room::empty,
            this, &Server::onRoomEmpty);
        rooms.push_back(room);
    }
    void onRoomEmpty(){
        auto room = (GameRoom*)sender();
        rooms.removeOne(room);
        std::cout<<"room id:"<<room->get_id()<<" is empty now\n";
        delete room;
    }
signals:
    void finished();
};