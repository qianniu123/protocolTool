#include "protocol.h"
#include <QDebug>

Protocol::Protocol(QObject *parent) : QObject(parent)
{

}


void Protocol::slot_data_recv(char *data,int len)
{
    qDebug() << QString("Protocol::slot_data_recv").arg(data).arg(len);

}

int Protocol::parse_pack(char *data, int len)
{
    qDebug() << QString("Protocol::parse_pack(%1,%2)").arg(data).arg(len);
    return 0;
}
