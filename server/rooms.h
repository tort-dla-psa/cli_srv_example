#pragma once
#include "players.h"
#include <QObject>
#include <QVector>
#include <QString>
#include <string>
#include <iostream>

class Room:public QObject{
    Q_OBJECT
protected:
    QVector<Player*> clients;
public:
    Room():QObject(){}
    ~Room(){
        for(auto &cl:clients){
            delete cl;
        }
        clients.clear();
    }
    void addPlayer(Player* pl){
        clients.push_back(pl);
        connect(pl, &Player::disconnected,
            this, &Room::onPlayerDisconnect);
    }

    //Note:removes from container in Room
    void removePlayer(Player* plr){
        clients.removeOne(plr);
    }
public slots:
    //abstract method
    virtual void onPlayerMessage(std::string mes)=0;
    virtual void onPlayerDisconnect(){
        auto pl = dynamic_cast<Player*>(sender());
        auto addr = pl->get_addr();
        auto addr_str = addr.toString().toStdString();
        std::cout<<"someone disconnected:"<<addr_str<<"\n";
        clients.removeOne(pl);
    }
signals:
    void empty();
};
class GameRoom:public Room{
    Q_OBJECT
    friend class Server;
    Admin* adm;
    std::string pass, id;
public:
    GameRoom(Admin* adm, const std::string &pass = "")
        :Room()
    {
        this->adm = adm;
        Room::addPlayer(adm);
        this->pass = pass;
    }

    const std::string& get_pass()const{
        return pass;
    }
    const std::string& get_id()const{
        return id;
    }
public slots:
    void onPlayerMessage(std::string mes)override{
        auto plr = (Player*)sender();
        std::cout<<"GameRoom id "<<get_id()<<":"
            <<"player "<<plr->get_addr().toString().toStdString()
            <<" wrote this:"<<mes<<"\n";
    }
    void onPlayerDisconnect()override{
        Room::onPlayerDisconnect();
        auto pl = (Player*)sender();
        auto adm_cast = dynamic_cast<Admin*>(pl);
        if(adm_cast){
            std::cout<<"admin disconnected!\n";
            for(auto &cl:clients){
                delete cl;
            }
            clients.clear();
            emit empty();
        }
    }
};

class WaitingRoom:public Room{
    Q_OBJECT
    const std::string start_cmd, join_cmd;
public:
    WaitingRoom(const std::string start_cmd="/start",
        const std::string join_cmd="/join")
    :Room(),
        start_cmd(start_cmd),
        join_cmd(join_cmd)
    {}
public slots:
    void onPlayerMessage(std::string mes)override{
        auto plr = (Player*)sender();
        std::cout<<"Waiting room:"<<plr->get_addr().toString().toStdString()
            <<" wrote this:"<<mes<<"\n";
        QString str = QString::fromStdString(mes);
        auto list = str.split(' ');
        auto cmd_str = list.at(0).toStdString();
        if(cmd_str == start_cmd){
            std::string pass;
            if(mes == start_cmd){
                pass = "";
            }else{
                pass = list.at(1).toStdString();
            }
            emit roomCreateRequested(plr, pass);
        }else if(cmd_str == join_cmd){
            // /join abcdef pass
            auto id = list.at(1).toStdString();
            auto pass = list.at(2).toStdString();
            emit roomJoinRequested(plr, id, pass);
        }
    }
    void onPlayerDisconnect()override{
        Room::onPlayerDisconnect();
        auto pl = dynamic_cast<Player*>(sender());
        clients.removeOne(pl);
        delete pl;
    }
signals:
    void roomCreateRequested(Player* plr, std::string pass);
    void roomJoinRequested(Player* plr, std::string id, std::string pass);
};