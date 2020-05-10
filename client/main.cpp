#include <QVector>
#include <QTcpSocket>
#include <QTcpServer>
#include <QObject>
#include <iostream>
#include "helpers.h"

class Connection:public QTcpSocket{
public:
};

void write(QTcpSocket* sock, const std::string &data){
    auto size = data.size();
    write_all(sock, &size, sizeof(size));//1000.0000.0000.0000
    write_all(sock, data.data(), size);
    sock->waitForBytesWritten();
}

int main(int argc, char* argv[]){
    std::string addr;
    quint16 port;
    if(argc == 3){
        addr = std::string(argv[1]);
        port = std::atoi(argv[2]);
    }else{
        std::cout << "enter ip:port\n";
        std::string tmp;
        std::cin>>tmp; //127.0.0.1:1337
        auto place = tmp.find(':');
        addr = tmp.substr(0, place);//127.0.0.1
        port = std::stoi(tmp.substr(place+1)); //1337
    }

    auto sock = new QTcpSocket();
    sock->connectToHost(QString::fromStdString(addr), port);
    sock->waitForConnected();
    while(true){
        std::string mes;
        std::getline(std::cin, mes);
        if(mes == "\\quit"){
            break;
        }
        try{
            write(sock, mes);
        }catch(...){
            break;
        }
    }
}