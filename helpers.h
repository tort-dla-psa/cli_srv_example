#pragma once
#include <QTcpSocket>

static void write_all(QTcpSocket* sock, const void* data, const size_t &size){
    int len=0;
    auto data_to_send = (const char*)data;
    while(len != size){
        int status = sock->write(data_to_send+len, size-len);
        if(status == -1){
            throw std::runtime_error("something went wrong");
        }
        len += status;
    }
}

static void read_all(QTcpSocket* sock, void* data, const size_t &size){
    int len=0;
    auto data_to_read = (char*)data;
    while(len != size){
        int status = sock->read(data_to_read+len, size-len);
        if(status == -1){
            throw std::runtime_error("something went wrong");
        }
        len += status;
    }
}