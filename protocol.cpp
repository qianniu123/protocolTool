#include "protocol.h"
#include <QDebug>

Protocol::Protocol(QObject *parent) : QObject(parent)
{

}


void Protocol::slot_data_recv(char *data,int len)
{
    qDebug() << QString("Protocol::slot_data_recv len=%1").arg(len);

}
