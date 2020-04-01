#include "protocol.h"
#include <QDebug>

Protocol::Protocol(QObject *parent) : QObject(parent)
{

}


void Protocol::slot_data_recv(char *data,int len)
{
    qDebug() << QString("Protocol::slot_data_recv len=%1").arg(len);

}

void Protocol::register_event_callback(int32_t mask, event_callback callback)
{
     qDebug() << QString("Protocol::register_event_callback");

}
