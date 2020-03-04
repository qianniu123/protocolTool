#include "jlprotocol.h"
#include <QDebug>

JLProtocol::JLProtocol(Protocol *parent) : Protocol(parent)//: QObject(parent)
{
    //m_protocolList << "JL_Proto";
    //---------------------------------------
    m_cmdList.clear();
    m_cmdList << "HEART_BEAT" << "TEST";

    //first method
    m_strToCmd_jl.clear();
    m_strToCmd_jl["HEART_BEAT"] = &JLProtocol::slot_heartbeat_send;

    //or another method
    m_strToCmd.clear();
    m_strToCmd["HEART_BEAT"] = (slot_function)(&JLProtocol::slot_heartbeat_send);
    m_strToCmd["TEST"] = (slot_function)&JLProtocol::slot_test_send;
}

void JLProtocol::slot_cmd_send(QString strCmd) //strCmd to cmd function
{
    qDebug() << QString("CMD=%1").arg(strCmd);
    slot_func_jl func = m_strToCmd_jl[strCmd];
    (this->*func)();//成员函数指针的使用
}

void JLProtocol::slot_heartbeat_send()
{
    qDebug() << QString("JLProtocol::slot_heartbeat_send");
}

void JLProtocol::slot_test_send()
{
    qDebug() << QString("jl slot_test_send");
}
