#include "sgprotocol.h"
#include <QDebug>
#include <QDateTime>

#include <stdlib.h>
#include <time.h>

SGProtocol::SGProtocol(Protocol *parent) : Protocol(parent)//: QObject(parent)
{
    //m_protocolList << "SG_Proto";

    //-------------------------------------------------------------------------------
    m_cmdList.clear();
    m_cmdList<< "CMD_NB_VERS" << "CMD_DEV_CCID" << "CMD_DEV_IMSI" << "CMD_NB_NET_STATE" \
             << "CMD_NB_GPS" << "CMD_NB_GSV" << "CMD_NB_RFCAL" << "CMD_NB_IMEI_SN"  \
             << "CMD_DEV_UPDATA_TIME" \
             \
             << "DEVICE_HELLO" << "DEVICE_BIND"<< "DEVICE_HEARTBEAT" << "DEVICE_SOFTWARE_VERSION_Upload" \
             << "HEART_UPLOAD" << "LOCATION_UPLOAD" << "WEATHER_QUERY" << "USER_BASE_INFO_UPLOAD" \
             << "SLEEP_UPLOAD" << "LUNAR_QUERY" << "HEALTH_UPLOAD";

    /*
    m_strToCmd_sg.clear();
    m_strToCmd_sg["DEVICE_HELLO"]   = &SGProtocol::slot_device_hello_send;
    m_strToCmd_sg["DEVICE_BIND"]    = &SGProtocol::slot_device_bind_send;
    *///or
    m_strToCmd.clear();
    //local cmd
    m_strToCmd["CMD_NB_VERS"]        = (slot_function)&SGProtocol::slot_get_nb_vers;
    m_strToCmd["CMD_DEV_CCID"]       = (slot_function)&SGProtocol::slot_get_nb_ccid;
    m_strToCmd["CMD_DEV_IMSI"]       = (slot_function)&SGProtocol::slot_get_nb_imsi;
    //m_strToCmd["CMD_NB_STATE"]       = (slot_function)&SGProtocol::slot_get_nb_state;
    m_strToCmd["CMD_NB_NET_STATE"]  = (slot_function)&SGProtocol::slot_get_net_state;
    m_strToCmd["CMD_NB_GPS"]        = (slot_function)&SGProtocol::slot_get_nb_gps;
    m_strToCmd["CMD_NB_GSV"]        = (slot_function)&SGProtocol::slot_get_nb_gsv;
    m_strToCmd["CMD_NB_RFCAL"]      = (slot_function)&SGProtocol::slot_get_nb_rfcal;
    m_strToCmd["CMD_NB_IMEI_SN"]    = (slot_function)&SGProtocol::slot_get_nb_imeisn;

    m_strToCmd["CMD_DEV_UPDATA_TIME"]=(slot_function)&SGProtocol::slot_get_nb_time;

    //net cmd
    m_strToCmd["DEVICE_HELLO"]      = (slot_function)(&SGProtocol::slot_device_hello_send);
    m_strToCmd["DEVICE_BIND"]       = (slot_function)&SGProtocol::slot_device_bind_send;
    m_strToCmd["DEVICE_HEARTBEAT"]  = (slot_function)&SGProtocol::slot_device_heartbeat_send;
    m_strToCmd["DEVICE_SOFTWARE_VERSION_Upload"]=(slot_function)&SGProtocol::slot_device_software_version_upload;

    m_strToCmd["HEART_UPLOAD"]      = (slot_function)&SGProtocol::slot_heart_upload;
    m_strToCmd["LOCATION_UPLOAD"]   = (slot_function)&SGProtocol::slot_location_upload;
    m_strToCmd["WEATHER_QUERY"]     = (slot_function)&SGProtocol::slot_weather_query;
    m_strToCmd["USER_BASE_INFO_UPLOAD"]  = (slot_function)&SGProtocol::slot_userBaseInfo_upload;
    m_strToCmd["SLEEP_UPLOAD"]      = (slot_function)&SGProtocol::slot_sleep_upload;
    m_strToCmd["LUNAR_QUERY"]       = (slot_function)&SGProtocol::slot_lunar_query;
    m_strToCmd["HEALTH_UPLOAD"]  = (slot_function)&SGProtocol::slot_health_upload;
    //-----------------------------------------------------------
    memset(&m_nb_context, 0, sizeof(m_nb_context));

    //qint64 secs = QDateTime::currentSecsSinceEpoch();
    //srand((int)secs);
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

void SGProtocol::slot_cmd_send(QString strCmd)
{
    qDebug() << QString("CMD=%1").arg(strCmd);
    /*
    slot_func_sg func = m_strToCmd_sg[strCmd];
    if(func) (this->*func)();//成员函数指针的使用
    */
}

void SGProtocol::slot_data_recv(char *data,int len)
{
    recvArray.append(data, len);
    //-----------------------------------------------//???
    //frame head = 01 00
    //frame tail = 23 23 5F 2A 2A
    static bool frame_head_flag = false;
    static bool frame_tail_flag = false;

    while(!frame_head_flag && recvArray.length()>2)
    {
        if(recvArray.at(0) == SG_PROTOCOL_VER \
           && recvArray.at(1) == SG_ENCRYPTION_TYPE)
        {
            frame_head_flag = true;
            break;
        }
        else
        {
            recvArray.remove(0,1);
        }
    }

    int recv_len = recvArray.size();
    while(frame_head_flag && !frame_tail_flag && recv_len>=sizeof(sg_commut_data_t)+sizeof(packEnd))
    {
        if(recvArray.at(recv_len-1) == packEnd[4] \
                && recvArray.at(recv_len-2) == packEnd[3] \
                && recvArray.at(recv_len-3) == packEnd[2] \
                && recvArray.at(recv_len-4) == packEnd[1] \
                && recvArray.at(recv_len-5) == packEnd[0])
        {
            frame_tail_flag = true;
        }
        else
        {
            //pack not complete
            if(recv_len >= 1024)
            {
                recvArray.clear();
                frame_head_flag = false;
                frame_tail_flag = false;
            }
            return;
        }
    }

   if(frame_head_flag && frame_tail_flag)
   {
        memset(recvBuffer, 0, sizeof(recvBuffer));
        memcpy(recvBuffer, recvArray.data(), recvArray.length());
        //------------------------------------------------------------
        //qDebug() << "recv-->" << recvArray.toHex().toUpper();
        int result = parse_pack((char*)recvBuffer, recvArray.length());

        //------------------------------
        if(result == 0 || result == -1)
        {
            recvArray.clear();//pack completion or pack error
            frame_head_flag = false;
            frame_tail_flag = false;
        }
   }
}

int SGProtocol::parse_pack(char *data, int len)
{
    sg_commut_data_t *p_pack = (sg_commut_data_t*)data;

    if(len >= sizeof(sg_commut_data_t)+p_pack->data_len+sizeof(packEnd))
    {
      unsigned char *p_pack_end = p_pack->data+p_pack->data_len;
      if(p_pack_end[0] == packEnd[0] && p_pack_end[1]==packEnd[1] && p_pack_end[2]==packEnd[2])
      {
            //pack ok
      }
      else
      {
          return -1;//pack error
      }
    }
    else
    {
        return 1; //pack no completion
    }

    memset(debugBuffer, 0, sizeof(debugBuffer));
    sprintf(debugBuffer, "<<-----recv:\tCMD:%04X\tdata_len:%d\n", p_pack->cmd, p_pack->data_len);
    switch(p_pack->cmd)
    {
    //local cmd
    case CMD_NB_VERS:
    {
        //display debug msg
        memcpy(m_nb_context.version, p_pack->data, sizeof(m_nb_context.version)-1);

        //display debug msg
        sprintf(debugBuffer+strlen(debugBuffer), "version=%s", m_nb_context.version);
        break;
    }
    case CMD_DEV_CCID:
    {
        memcpy(&m_nb_context.ccid_csq, p_pack->data, sizeof(ccid_csq_rsp_t));
        char ccid[20+1] = {0};
        memcpy(ccid, m_nb_context.ccid_csq.ccid, 20);
        //display debug msg
        sprintf(debugBuffer+strlen(debugBuffer), "csq=%d,iccid=%s",m_nb_context.ccid_csq.csq, ccid);
        break;
    }
    case CMD_DEV_IMSI:
    {
        memcpy(m_nb_context.imsi, p_pack->data, p_pack->data_len);

        //display debug msg
        sprintf(debugBuffer+strlen(debugBuffer), "imsi=%s",m_nb_context.imsi);
        break;
    }
    case CMD_NB_STATE:
    {
        //display debug msg

        break;
    }    
    case CMD_NB_NET_STATE:
    {
        m_nb_context.nb_net_state = *(uint8_t*)p_pack->data;
        //display debug msg
        sprintf(debugBuffer+strlen(debugBuffer), "nb_net=%d", m_nb_context.nb_net_state);
        break;
    }
    case CMD_NB_GPS:
    {
        memcpy(&m_nb_context.gps, p_pack->data, p_pack->data_len);
        sprintf(debugBuffer+strlen(debugBuffer), "longtitude=%f, latitude=%f, EW=%c, NS=%c", m_nb_context.gps.longitude, m_nb_context.gps.latitude\
                ,m_nb_context.gps.EW, m_nb_context.gps.NS);
        break;
    }
    case CMD_NB_GSV:
    {
        memcpy(&m_nb_context.gsv, p_pack->data, p_pack->data_len);
        sprintf(debugBuffer+strlen(debugBuffer), "cnts=%d; max_snr=%d,avg_snr=%d,min_snr=%d", m_nb_context.gsv.gsv_states_in_view,\
                m_nb_context.gsv.gsv_states_info[0].gsv_sate_snr, m_nb_context.gsv.gsv_states_info[1].gsv_sate_snr, \
                m_nb_context.gsv.gsv_states_info[2].gsv_sate_snr);
        break;
    }
    case CMD_NB_RFCAL:
    {
        memcpy(&m_nb_context.rfcal, p_pack->data, p_pack->data_len);
        sprintf(debugBuffer+strlen(debugBuffer), "nb_flag=%d, gsm_flag=%d, nb_rf=%d, gsm_rf=%d",\
                m_nb_context.rfcal.nb_flag, m_nb_context.rfcal.gsm_flag, \
                m_nb_context.rfcal.nb_rfcal_flag, m_nb_context.rfcal.gsm_rfcal_flag);
        break;
    }
    case CMD_NB_IMEI_SN:
    {
        memcpy(&m_nb_context.imei_sn, p_pack->data, p_pack->data_len);
        sprintf(debugBuffer+strlen(debugBuffer),"imei=%s, sn=%s", \
                m_nb_context.imei_sn.imei, m_nb_context.imei_sn.sn);
        break;
    }
    case CMD_DEV_UPDATE_TIME:
    {
        m_nb_context.time = *(int*)p_pack->data;
        sprintf(debugBuffer+strlen(debugBuffer),"stamp=%d", m_nb_context.time);
        break;
    }

    //net cmd
    case DEVICE_HEARTBEAT:
    {

        break;
    }
    case DEVICE_HELLO:
    {

        break;
    }
    case DEVICE_BIND:
    {

        break;
    }

    default:

        break;
    }

    //----------------------------------------------------------------------------------------------------------
    //display debug msg
    emit sig_send_display_data(debugBuffer, strlen(debugBuffer));

    //display raw data(raw data to string)
    debugArray.clear();
    debugArray.append(recvArray.toHex().toUpper());
    emit sig_send_display_data(debugArray.data(), debugArray.size());

    return 0;
}

//========================================================================
void SGProtocol::local_cmds_send(SG_COMMUT_CMD_E cmd)
{
    int pack_len = 0;
    uint8_t *p_pack_end;
    packInit();
    memcpy(p_sg_pack->token, m_nb_context.token, sizeof(m_nb_context.token));

    p_sg_pack->cmd = cmd;
    p_sg_pack->data_len = 0;
    p_pack_end = p_sg_pack->data;
    memcpy(p_pack_end, packEnd, sizeof(packEnd));
    pack_len = sizeof(sg_commut_data_t) + sizeof(packEnd);

    //debug msg
    memset(debugBuffer, 0, sizeof(debugBuffer));
    sprintf(debugBuffer, "------>send:\tCMD:%04X", p_sg_pack->cmd);
    emit sig_send_display_data(debugBuffer, strlen(debugBuffer));
    //send
    emit sig_send_data((char*)sendBuffer, pack_len);
}

void SGProtocol::slot_get_nb_vers()
{
    qDebug() << QString("SGProtocol::slot_get_nb_vers");
    local_cmds_send(CMD_NB_VERS);
}
void SGProtocol::slot_get_nb_ccid()
{
    qDebug() << QString("SGProtocol::slot_get_nb_ccid");
    local_cmds_send(CMD_DEV_CCID);
}
void SGProtocol::slot_get_nb_imsi()
{
    qDebug() << QString("SGProtocol::slot_get_nb_imsi");
    local_cmds_send(CMD_DEV_IMSI);
}
void SGProtocol::slot_get_nb_state()
{
    qDebug() << QString("SGProtocol::slot_get_net_state");
    local_cmds_send(CMD_NB_STATE);
}
void SGProtocol::slot_get_net_state()
{
    qDebug() << QString("SGProtocol::slot_get_net_state");
    local_cmds_send(CMD_NB_NET_STATE);
}
void SGProtocol::slot_get_nb_gps()
{
    qDebug() << QString("SGProtocol::slot_get_nb_gps");
    local_cmds_send(CMD_NB_GPS);
}
void SGProtocol::slot_get_nb_gsv()
{
    qDebug() << QString("SGProtocol::slot_get_nb_gsv");
    local_cmds_send(CMD_NB_GSV);
}
void SGProtocol::slot_get_nb_rfcal()
{
    qDebug() << QString("SGProtocol::slot_get_nb_rfcal");
    local_cmds_send(CMD_NB_RFCAL);
}
void SGProtocol::slot_get_nb_imeisn()
{
    qDebug() << QString("SGProtocol::slot_get_nb_imeisn");
    local_cmds_send(CMD_NB_IMEI_SN);
}

void SGProtocol::slot_get_nb_time()
{
    qDebug() << QString("SGProtocol::slot_get_nb_time");
    local_cmds_send(CMD_DEV_UPDATE_TIME);
}
//=======================================================================
void SGProtocol::net_cmds_send(SG_COMMUT_CMD_E cmd, char *data, int len)
{
    int pack_len = 0;
    uint8_t *p_pack_end;
    packInit();
    memcpy(p_sg_pack->token, m_nb_context.token, sizeof(m_nb_context.token));

    p_sg_pack->cmd = cmd;
    p_sg_pack->data_len = len;
    if(len>0 && data!=nullptr)
    {
        memcpy(p_sg_pack->data, data, len);
    }

    p_pack_end = p_sg_pack->data + len;
    memcpy(p_pack_end, packEnd, sizeof(packEnd));
    pack_len = sizeof(sg_commut_data_t) + len + sizeof(packEnd);

    //debug msg
    memset(debugBuffer, 0, sizeof(debugBuffer));
    sprintf(debugBuffer, "------>send:\tCMD:%08X", p_sg_pack->cmd);
    emit sig_send_display_data(debugBuffer, strlen(debugBuffer));

    //send
    emit sig_send_data((char*)sendBuffer, pack_len);
}

void SGProtocol::slot_device_hello_send()
{
    qDebug() << QString("slot_device_hello_send");

    net_cmds_send(DEVICE_HELLO, nullptr, 0);
}

void SGProtocol::slot_device_heartbeat_send()
{ 
    qDebug() << QString("SGProtocol::slot_device_heartbeat_send");
    static int cnts=0;
    cnts += 10;
    sg_walk_t walk;
    walk.count = cnts;
    walk.time  = (int)QDateTime::currentSecsSinceEpoch();

    net_cmds_send(DEVICE_HEARTBEAT, (char*)&walk, sizeof(walk));
}

void SGProtocol::slot_device_bind_send()
{
    qDebug() << QString("SGProtocol::slot_device_bind_send");

    dev_bind_req_t bind;
    memcpy(bind.userId, m_nb_context.userId, sizeof(m_nb_context.userId));
    bind.status = 1;

    net_cmds_send(DEVICE_BIND, (char*)&bind, sizeof(bind));
}

void SGProtocol::slot_device_software_version_upload()
{
    qDebug() << QString("SGProtocol::slot_device_software_version_upload");

    sg_version_t version;
    version.time = (int)QDateTime::currentSecsSinceEpoch();
    memcpy(version.version, "V0102V0103", strlen("V0102V0103"));

    //send
    net_cmds_send(DEVICE_SOFTWARE_VERSION_UPLOAD, (char*)&version, sizeof(sg_version_t));
}

void SGProtocol::slot_heart_upload()
{
    qDebug() << QString("SGProtocol::slot_heart_upload");

    heart_upload_req_t heart_upload[90];//max=90, general 30-60
    memset(&heart_upload, 0, sizeof(heart_upload));

    int time = QDateTime::currentSecsSinceEpoch();
    int size = rand()%30+30; //30-60
    for(int i=0; i<size; i++)
    {
        heart_upload[i].count = rand()%15+55;//55-70
        heart_upload[i].time  = time+i;
        //qDebug() << heart_upload[i].time << heart_upload[i].count;
    }

    qDebug() << QString("time:%1, size:%2").arg(time).arg(size);
    //send
    net_cmds_send(HEART_UPLOAD, (char*)heart_upload, sizeof(heart_upload_req_t)*size);

}

void SGProtocol::slot_userBaseInfo_upload()
{
    qDebug() << QString("SGProtocol::slot_userBaseInfo_upload");

    birthday_t birthday = {2001, 3,15};
    sg_user_info_t user_info = {birthday, 1, 175, 63};

    net_cmds_send(USER_BASE_INFO_UPLOAD, (char*)&user_info, sizeof(sg_user_info_t));
}
void SGProtocol::slot_sleep_upload()
{
    qDebug() << QString("SGProtocol::slot_sleep_upload");

    sg_sleep_t sleep = {1428551739, 1428551739, 5400, 3600, 1600};

    net_cmds_send(SLEEP_UPLOAD, (char*)&sleep, sizeof(sg_sleep_t));
}
void SGProtocol::slot_lunar_query()
{
    qDebug() << QString("SGProtocol::slot_lunar_query");

    QString data = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    qDebug() << data;
    long time = QDateTime::currentSecsSinceEpoch();

    net_cmds_send(LUNAR_QUERY, (char*)&time, sizeof(int));

    //----------------------------------------------------------------------
    struct tm *time_tm = localtime((time_t*)&time);
    //qDebug() << time_tm.tm_year+1900 << time_tm.tm_mon << time_tm.tm_mday;
    qDebug()<< time << time_tm->tm_year+1900 << time_tm->tm_mon+1 << time_tm->tm_mday;
}

void SGProtocol::slot_location_upload()
{
    qDebug() << QString("SGProtocol::slot_location_upload");

    sg_wifi_t    wifi[5];
    sg_walk_t    walk;
    sg_battery_t battery;

    memset(wifi, 0, sizeof(wifi));
    for(int i=0; i<5; i++)
    {
        sprintf(wifi[i].mac, "30fc68c20%03d", rand()%255);//000-255
        sprintf(wifi[i].mac_name, "name%d", i+1);
        wifi[i].signal = rand()%20+50;//-50~-70

        //qDebug() << wifi[i].mac << wifi[i].mac_name << wifi[i].signal;
    }

    walk.count = rand();
    walk.time = (int)QDateTime::currentSecsSinceEpoch();

    battery.value = rand()%100;
    battery.time = (int)QDateTime::currentSecsSinceEpoch();

    //-------------------------------------------------------
    char data[512] = {0};
    memset(data, 0, sizeof(data));
    char *p_data = data;

    memcpy(p_data, (char*)&wifi, sizeof(wifi));
    p_data += sizeof(wifi);
    memcpy(p_data, (char*)&walk, sizeof(walk));
    p_data += sizeof(walk);
    memcpy(p_data, (char*)&battery, sizeof(battery));
    p_data += sizeof(battery);

    net_cmds_send(LOCATION_UPLOAD, data, p_data-data);
    qDebug() << "p_data-data=" << p_data-data;
    //------------------------------------------------------
#if 0
    //repack test
    //    char *key_wifi = "wifi";
    //    char *key_mobile = "mobile";
    //    char *key_gps = "gps";
    //    char *key_walk = "walk";
    //    char *key_battery = "battery";

    //value
    char value[512] = {0};
    char *p_value = value;
    //wifi
    memset(value, 0, sizeof(value));
    value[0] = '['; value[1] = '{';
    p_value = &value[2];
    for(int i=0; i<5; i++)
    {
        sprintf(p_value, "\"mac%d\":\"%s\",\"macName%d\":\"%s\",\"signal%d\",%d,",
                i+1, wifi[i].mac, i+1, wifi[i].mac_name, i+1, wifi[i].signal);
        p_value = value + strlen(value);
    }
    *(p_value-1) = '}';
    *p_value = ']';
    *(p_value+1) = 0;

    qDebug() << "wifi_value_len=" << strlen(value);
    qDebug() << value;


    sg_mobile_t mobile;
    memset(&mobile, 0, sizeof(sg_mobile_t));
    mobile.time = (int)QDateTime::currentSecsSinceEpoch();
    memcpy(mobile.network, "GSM", strlen("GSM"));

    //mobile
    memset(value, 0, sizeof(value));
    value[0] = '['; value[1] = '{';
    p_value = &value[2];

    sprintf(p_value, "\"serverIp\":\"%s\",\"network\":\"%s\",\"mcc\":\"%s\",\"mnc\":\"%s\",\"time\":%d000,",
            mobile.dev_ip,mobile.network,mobile.mcc, mobile.mcc, mobile.time);
    p_value = value + strlen(value);

    for(int i=0; i<5; i++)
    {
        sprintf(p_value, "\"lac%d\":%d,\"ci%d\":%d,\"rssi%d\":%d,",
                i+1,mobile.lac_data[i].lac, i+1,mobile.lac_data[i].ci, i+1,mobile.lac_data[i].rssi);
        p_value = value+strlen(value);
    }
    *(p_value-1) = '}';
    *(p_value) = ']';
    *(p_value+1) = 0;

    qDebug() << "mobile_value_len=" << strlen(value);
    qDebug() << value;

    sg_gps_t gps;
    memset(&gps, 0, sizeof(gps));
    //gps
    memset(value, 0, sizeof(value));
    p_value = value;

    sprintf(p_value, "[{\"latitude\":%f,\"longitude\":%f,\"time\":%d000}]",
            gps.latitude, gps.longitude, gps.time);

    qDebug() << "gps_value_len=" << strlen(value);
    qDebug() << value;

    //walk
    memset(value, 0, sizeof(value));
    p_value = value;

    sprintf(p_value, "{\"count\":%d,\"time\":%d000}", walk.count, walk.time);

    qDebug() << "walk_value_len=" << strlen(value);
    qDebug() << value;

    //battery
    memset(value, 0, sizeof(value));
    p_value = value;

    sprintf(p_value, "{\"value\":%d,\"time\":%d000}", battery.value, battery.time);

    qDebug() << "battery_value_len=" << strlen(value);
    qDebug() << value;
#endif
}
void SGProtocol::slot_weather_query()
{
    qDebug() << QString("SGProtocol::slot_weather_query");

    //same as slot_location_upload();
}

void SGProtocol::slot_health_upload()
{
    qDebug() << QString("SGProtocol::slot_health_upload");
    long time = QDateTime::currentSecsSinceEpoch();
    //sg_bp_t bp = {};
    //sg_spo2_t spo2 = {};
    //sg_af_t af = {};
    sg_health_t health = {{128, 81, 64, (uint32_t)time, (uint32_t)time}, \
                          {(uint32_t)time, 256, 98},\
                          {(uint32_t)time, 256, 0}};

    net_cmds_send(HEALTH_UPLOAD, (char*)&health, sizeof(sg_health_t));
}
