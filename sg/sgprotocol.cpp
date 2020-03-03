#include "sgprotocol.h"
#include <QDebug>

SGProtocol::SGProtocol(QObject *parent) : QObject(parent)
{

    m_cmdList.clear();
    m_cmdList << "DEVICE_HELLO" << "DEVICE_BIND"<< "DEVICE_HEARTBEAT" << "DEVICE_SOFTWARE_VERSION_Upload";
    //SGProtocol::slot_device_hello_send();
    m_strToCmd["DEVICE_HELLO"]      = &SGProtocol::slot_device_hello_send;
    m_strToCmd["DEVICE_BIND"]       = &SGProtocol::slot_device_bind_send;
    m_strToCmd["DEVICE_HEARTBEAT"]  = &SGProtocol::slot_device_heartbeat_send;
    m_strToCmd["DEVICE_SOFTWARE_VERSION_Upload"]=&SGProtocol::slot_device_software_version_upload;


    //----------
    packInit();
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



void SGProtocol::slot_device_hello_send()
{
    qDebug() << QString("hello send");


}

void SGProtocol::slot_device_heartbeat_send()
{ 
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

    qDebug() << QString("heartbeat send: pack_len = %1").arg(pack_len);
}

void SGProtocol::slot_device_bind_send()
{
    qDebug() << QString("device_bind send");

}

void SGProtocol::slot_device_software_version_upload()
{
    qDebug() << QString("software_version_upload send");

}
