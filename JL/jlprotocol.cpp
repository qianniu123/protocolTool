#include "jlprotocol.h"
#include <QDebug>

JLProtocol::JLProtocol(Protocol *parent) : Protocol(parent)//: QObject(parent)
{
    //m_protocolList << "JL_Proto";
    //---------------------------------------
    m_cmdList.clear();
    m_cmdList << "HEART_BEAT" << "TEST";

    m_strToCmd.clear();
    m_strToCmd["HEART_BEAT"] = (slot_function)(&JLProtocol::slot_heartbeat_send);
    m_strToCmd["TEST"] = (slot_function)&JLProtocol::slot_test_send;
}

void JLProtocol::slot_heartbeat_send()
{
    qDebug() << QString("JLProtocol::slot_heartbeat_send");
}

void JLProtocol::slot_test_send()
{
    qDebug() << QString("jl slot_test_send");
}
