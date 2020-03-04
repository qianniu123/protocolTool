#include "sgprotocol.h"
#include <QDebug>
#include <QDateTime>

SGProtocol::SGProtocol(Protocol *parent) : Protocol(parent)//: QObject(parent)
{
    //m_protocolList << "SG_Proto";

    //-------------------------------------------------------------------------------
    m_cmdList.clear();
    m_cmdList << "DEVICE_HELLO" << "DEVICE_BIND"<< "DEVICE_HEARTBEAT" << "DEVICE_SOFTWARE_VERSION_Upload";    

    /*
    m_strToCmd_sg.clear();
    m_strToCmd_sg["DEVICE_HELLO"]   = &SGProtocol::slot_device_hello_send;
    m_strToCmd_sg["DEVICE_BIND"]    = &SGProtocol::slot_device_bind_send;
    *///or
    m_strToCmd.clear();
    m_strToCmd["DEVICE_HELLO"]      = (slot_function)(&SGProtocol::slot_device_hello_send);
    m_strToCmd["DEVICE_BIND"]       = (slot_function)&SGProtocol::slot_device_bind_send;
    m_strToCmd["DEVICE_HEARTBEAT"]  = (slot_function)&SGProtocol::slot_device_heartbeat_send;
    m_strToCmd["DEVICE_SOFTWARE_VERSION_Upload"]=(slot_function)&SGProtocol::slot_device_software_version_upload;

    //----------
    //packInit();
}

void SGProtocol::packInit()
{
    p_sg_pack = nullptr;
    memset(sendBuffer, 0, sizeof(sendBuffer));
    p_sg_pack = (sg_commut_data_t*)sendBuffer;
    p_sg_pack->protocol_ver     = SG_PROTOCOL_VER;
    p_sg_pack->enryption_type   = SG_ENCRYPTION_TYPE;
    p_sg_pack->status           = 0;

    packEnd[0] = SG_PACKET_END1;
    packEnd[1] = SG_PACKET_END2;
    packEnd[2] = SG_PACKET_END3;
    packEnd[3] = SG_PACKET_END4;
    packEnd[4] = SG_PACKET_END5;
}

void SGProtocol::slot_cmd_send(QString strCmd)
{
    qDebug() << QString("CMD=%1").arg(strCmd);
    /*
    slot_func_sg func = m_strToCmd_sg[strCmd];
    if(func) (this->*func)();//成员函数指针的使用
    */
}

void SGProtocol::slot_device_hello_send()
{
    qDebug() << QString("hello send");
    int pack_len = 0;
    uint8_t *p_pack_end;
    packInit();
    memcpy(p_sg_pack->token, token, sizeof(token));

    p_sg_pack->cmd = DEVICE_HELLO;
    p_sg_pack->data_len = 0;
    p_pack_end = p_sg_pack->data;
    memcpy(p_pack_end, packEnd, sizeof(packEnd));

    pack_len = sizeof(sg_commut_data_t) + sizeof(packEnd);

    //debug msg
    memset(debugBuffer, 0, sizeof(debugBuffer));
    sprintf(debugBuffer, "------>send: DEVICE_HELLO-->CMD:%04X", p_sg_pack->cmd);
    emit sig_send_debug_data(debugBuffer);

    //send
    emit sig_send_data(sendBuffer, pack_len);
}

void SGProtocol::slot_device_heartbeat_send()
{ 
    qDebug() << QString("SGProtocol::slot_device_heartbeat_send");

    int pack_len = 0;
    uint8_t *p_pack_end;
    packInit();
    memcpy(p_sg_pack->token, token, sizeof(token));

    p_sg_pack->cmd = DEVICE_HEARTBEAT;
    sg_walk_t walk = {1000 ,1428551739};
    p_sg_pack->data_len = sizeof(walk);
    memcpy(p_sg_pack->data, &walk, sizeof(walk));

    p_pack_end = p_sg_pack->data + sizeof(walk);
    memcpy(p_pack_end, packEnd, sizeof(packEnd));
    pack_len = sizeof(sg_commut_data_t) + sizeof(walk) + sizeof(packEnd);

    //debug msg
    memset(debugBuffer, 0, sizeof(debugBuffer));
    sprintf(debugBuffer, "------>send: DEVICE_HEARTBEAT-->CMD:%04X\n walk:count=%d, time=%d", p_sg_pack->cmd, walk.count, walk.time);
    emit sig_send_debug_data(debugBuffer);

    //send
    emit sig_send_data(sendBuffer, pack_len);
}

void SGProtocol::slot_device_bind_send()
{
    qDebug() << QString("SGProtocol::slot_device_bind_send");

    int pack_len = 0;
    uint8_t *p_pack_end;
    packInit();
    memcpy(p_sg_pack->token, token, sizeof(token));

    p_sg_pack->cmd = DEVICE_BIND;
    dev_bind_req_t bind;
    memcpy(bind.userId, userId, sizeof(userId));
    bind.status = 1;

    p_sg_pack->data_len = sizeof(dev_bind_req_t);
    memcpy(p_sg_pack->data, &bind, sizeof(dev_bind_req_t));

    p_pack_end = p_sg_pack->data + p_sg_pack->data_len;
    memcpy(p_pack_end, packEnd, sizeof(packEnd));
    pack_len = sizeof(sg_commut_data_t) + (unsigned int)p_sg_pack->data_len + sizeof(packEnd);

    //debug msg
    memset(debugBuffer, 0, sizeof(debugBuffer));
    sprintf(debugBuffer, "------>send: DEVICE_BIND-->CMD:%04X", p_sg_pack->cmd);
    emit sig_send_debug_data(debugBuffer);

    //send
    emit sig_send_data(sendBuffer, pack_len);
}

void SGProtocol::slot_device_software_version_upload()
{
    qDebug() << QString("SGProtocol::slot_device_software_version_upload");

    int pack_len = 0;
    uint8_t *p_pack_end;
    packInit();
    memcpy(p_sg_pack->token, token, sizeof(token));

    p_sg_pack->cmd = DEVICE_SOFTWARE_VERSION_UPLOAD;
    sg_version_t version;
    version.time = (int)QDateTime::currentSecsSinceEpoch();
    memcpy(version.version, "V0102V0103", strlen("V0102V0103"));

    p_sg_pack->data_len = sizeof(sg_version_t);
    memcpy(p_sg_pack->data, &version, sizeof(version));

    p_pack_end = p_sg_pack->data + p_sg_pack->data_len;
    memcpy(p_pack_end, packEnd, sizeof(packEnd));
    pack_len = sizeof(sg_commut_data_t) + sizeof(sg_version_t) + sizeof(packEnd);

    //debug msg
    memset(debugBuffer, 0, sizeof(debugBuffer));
    sprintf(debugBuffer, "------>send: DEVICE_SOFTWARE_VERSION_UPLOAD-->CMD:%04X", p_sg_pack->cmd);
    emit sig_send_debug_data(debugBuffer);

    //send
    emit sig_send_data(sendBuffer, pack_len);
}
