#include "ublox.h"
#include <QDebug>
#include "minmea.h"

#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace  std;
Ublox::Ublox()
{
    m_cmdList.clear();
    m_cmdList << "UBLOX_CONFIG_GNSS" << "UBLOX_CONFIG_NMEA";

    //-------------------
    m_strToCmd.clear();
    m_strToCmd["UBLOX_CONFIG_GNSS"] = (slot_function)&Ublox::slot_ublox_config_gnss;
    m_strToCmd["UBLOX_CONFIG_NMEA"] = (slot_function)&Ublox::slot_ublox_config_nmea;

}
Ublox::~Ublox()
{
    qDebug() << __FUNCTION__ ;
    //freeUblox();
#ifdef UBLOX_OBSERVER
    free_event_list();
#endif
}
/*
Ublox* Ublox::mUblox = nullptr;
Ublox* Ublox::getUblox()
{
    if(mUblox==nullptr)
    {
        mUblox = new Ublox();
    }
    return mUblox;
}
void Ublox::freeUblox()
{
    if(mUblox)
    {
        delete mUblox;
    }
    mUblox = nullptr;
}
*/
void Ublox::slot_cmd_send(QString strCmd)
{
    qDebug() << QString("Ublox::slot_cmd_send(%1)").arg(strCmd);
}
void Ublox::slot_data_recv(char* data, int len)
{
    recvArray.append(data, len);
    //qDebug() << "slot_data_recv------------------------------";
    //qDebug() << recvArray;

    ublox_message_processing(data, len);

    recvArray.clear();
}

void Ublox::slot_ublox_config_nmea()
{
    qDebug() << "Ublox::slot_ublox_config_nmea";
    //B5 62 06 17 14 00 00 41 00 02 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 75 57
    int frame_len = 0;
    char payload[0x14] = {0x00, 0x41, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    memset(sendBuffer, 0, sizeof(sendBuffer));
    ublox_frame_t *p_frame = (ublox_frame_t*)sendBuffer;
    p_frame->sync_char1 = SYNC_CHAR1;
    p_frame->sync_char2 = SYNC_CHAR2;
    p_frame->msg_class = UBX_CFG;
    p_frame->id = 0x17; //NMEA
    p_frame->payload_len = sizeof(payload);
    memcpy(p_frame->payload, payload, sizeof(payload));
    *(p_frame->payload+p_frame->payload_len) = 0x75;
    *(p_frame->payload+p_frame->payload_len+1) = 0x57;

    frame_len = sizeof(ublox_frame_t) + p_frame->payload_len + 2;

    //send
    //debug msg
    memset(debugBuffer, 0, sizeof(debugBuffer));
    char *tmp = "------>ublox_send: UBX_CFG --> NMEA";
    memcpy(debugBuffer, tmp, strlen(tmp));
    emit sig_send_display_data(debugBuffer, strlen(debugBuffer));

    emit sig_send_data((char*)sendBuffer, frame_len);
    //for test
    /*QByteArray tmp;
    tmp.append((char*)sendBuffer, frame_len);
    qDebug() << tmp.toHex().toUpper();
    */
}

void Ublox::slot_ublox_config_gnss()
{
    qDebug() << "Ublox::slot_ublox_config_gnss";
    //[Grant]  Set to GPS+Galileo+BD
    /*{0xB5, 0x62, 0x06, 0x3E, 0x3C, 0x00, 0x00, 0x00, 0x1C, 0x07, 0x00, 0x08, 0x10, 0x00, 0x01,
      0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, 0x02, 0x04, 0x08, 0x00,
      0x01, 0x00, 0x01, 0x01, 0x03, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, 0x04, 0x00, 0x08,
      0x00, 0x00, 0x00, 0x01, 0x01, 0x05 ,0x00, 0x03, 0x00, 0x01 ,0x00, 0x01, 0x01, 0x06, 0x08,
      0x0E, 0x00, 0x00 ,0x00, 0x01 ,0x01, 0x2C, 0xDD};//10X5+2 = 52
    */
    int frame_len = 0;
    uint8_t payload[] =
    {
      0x00, 0x00, 0x1C, 0x07, 0x00, 0x08, 0x10, 0x00, 0x01,
      0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, 0x02, 0x04, 0x08, 0x00,
      0x01, 0x00, 0x01, 0x01, 0x03, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, 0x04, 0x00, 0x08,
      0x00, 0x00, 0x00, 0x01, 0x01, 0x05 ,0x00, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, 0x06, 0x08,
      0x0E, 0x00, 0x00 ,0x00, 0x01 ,0x01
    };

    memset(sendBuffer, 0, sizeof(sendBuffer));
    ublox_frame_t *p_frame = (ublox_frame_t*)sendBuffer;
    p_frame->sync_char1 = SYNC_CHAR1;
    p_frame->sync_char2 = SYNC_CHAR2;
    p_frame->msg_class = UBX_CFG;
    p_frame->id = 0x3E;//GNSS
    p_frame->payload_len = sizeof(payload);
    memcpy(p_frame->payload, payload, sizeof(payload));
    *(p_frame->payload+p_frame->payload_len) = 0x75;
    *(p_frame->payload+p_frame->payload_len+1) = 0x57;

    frame_len = sizeof(ublox_frame_t) + p_frame->payload_len + 2;
    //send
    //debug msg
    memset(debugBuffer, 0, sizeof(debugBuffer));
    char *tmp = "------>ublox_send: UBX_CFG --> GNSS";
    memcpy(debugBuffer, tmp, strlen(tmp));
    emit sig_send_display_data(debugBuffer, strlen(debugBuffer));

    emit sig_send_data((char*)sendBuffer, frame_len);
    //for test
    /*QByteArray tmp;
    tmp.append((char*)sendBuffer, frame_len);
    qDebug() << tmp.toHex().toUpper();
    */
}

void Ublox::ublox_message_processing(char *data, int len)
{
    ublox_parse(data, len);
    nmea_parse(data, len);
}

void Ublox::ublox_parse(char *data, int len)
{
    //uint8_t ubox_msg_head[2+1] = {0xB5, 0x62, 0};
    char *p_data = data;

    if(len < sizeof(ublox_frame_t)+2) return;
    for(int i=0; i<len-2; i++)
    {
        if(p_data[i] == 0xb5 && p_data[i+1] == 0x62)
        {
            qDebug() << "==========================================";
            //for test
            QByteArray ubloxArray;
            ubloxArray.append(p_data, len-i);
            qDebug() << ubloxArray.toHex().toUpper();
        }

    }

}

void Ublox::nmea_parse(char *data, int len)
{
    char sentence_buf[MINMEA_MAX_LENGTH+3] = {0};
    int sentence_len = 0;

    //find '$' --> one sentence begin
    char *p_data = strchr(data, '$');
    char *p_next = nullptr;
    while(p_data && strlen(p_data))
    {
        if(p_data)
        {
            p_next = strchr(p_data+1, '$');
            if(p_next)
            {
                memset(sentence_buf, 0, sizeof(sentence_buf));
                if(p_next)
                {
                    sentence_len = (p_next-p_data);
                }
                else
                {
                    sentence_len = (int)strlen(p_data);
                }
                sentence_len = (sentence_len>MINMEA_MAX_LENGTH?MINMEA_MAX_LENGTH:sentence_len);
                memcpy(sentence_buf, p_data, (size_t)sentence_len);

                nmea_sentence_process(sentence_buf);
            }
        }

        if(p_next)
        {
            p_data = p_next;
        }
        else
        {
            p_data = nullptr;//return;
        }
    }
}

void Ublox::nmea_sentence_process(char *data)
{ 
    memset(debugBuffer, 0, sizeof(debugBuffer));
    char *tmp = "<<-----recv:\n";
    memcpy(debugBuffer, tmp, strlen(tmp));

    switch(minmea_sentence_id(data, false))
    {
        case MINMEA_SENTENCE_RMC:
        {
            //qDebug() << "MINMEA_SENTENCE_RMC:" << data;
            struct minmea_sentence_rmc frame;
            if (minmea_parse_rmc(&frame, data))
            {
                /*qDebug("$xxRMC: raw coordinates and speed: (%d/%d,%d/%d) %d/%d\n",
                        frame.latitude.value, frame.latitude.scale,
                        frame.longitude.value, frame.longitude.scale,
                        frame.speed.value, frame.speed.scale);
                qDebug("$xxRMC fixed-point coordinates and speed scaled to three decimal places: (%d,%d) %d\n",
                        minmea_rescale(&frame.latitude, 1000),
                        minmea_rescale(&frame.longitude, 1000),
                        minmea_rescale(&frame.speed, 1000));
                qDebug("$xxRMC floating point degree coordinates and speed: (%f,%f) %f\n",
                        minmea_tocoord(&frame.latitude),
                        minmea_tocoord(&frame.longitude),
                        minmea_tofloat(&frame.speed));
                */

                sprintf(debugBuffer+strlen(debugBuffer), "latitude: %f, longitude: %f, NS: %c, EW: %c",
                        minmea_tocoord(&frame.latitude), minmea_tocoord(&frame.longitude),\
                        frame.NS, frame.EW);
                if(frame.valid)
                {
                    static int cnt=0;
                    cnt++;
                    notify_event(cnt, debugBuffer);//
                }
            }
            else
            {
                qDebug("$xxRMC sentence is not parsed\n");
            }
        }
        break;
        case MINMEA_SENTENCE_GSV:
        {
            //qDebug() << "MINMEA_SENTENCE_GSV:" << data;
            struct minmea_sentence_gsv frame;
            if (minmea_parse_gsv(&frame, data))
            {
                /*qDebug("$xxGSV: message %d of %d\n", frame.msg_nr, frame.total_msgs);
                qDebug("$xxGSV: sattelites in view: %d\n", frame.total_sats);

                qDebug("$xxGSV: snr1: %d, snr2: %d, snr3: %d, snr4: %d\n",
                        frame.sats[0].snr,
                        frame.sats[1].snr,
                        frame.sats[2].snr,
                        frame.sats[3].snr);
                */
               sprintf(debugBuffer+strlen(debugBuffer),"msg %d of %d;satte in view: %d; snr1:%d, snr2:%d, snr3:%d, snr4:%d",
                       frame.msg_nr, frame.total_msgs, frame.total_sats, frame.sats[0].snr, frame.sats[1].snr, frame.sats[2].snr,
                       frame.sats[3].snr);

               //for test
               if(strstr(data, "GPGSV") && frame.msg_nr==1)
                emit sig_plot(0, frame.sats[0].snr);
            }
            else
            {
                qDebug("$xxGSV sentence is not parsed\n");
            }
        }
        break;
        default:
            return;
        break;
    }


    emit sig_send_display_data(debugBuffer, strlen(debugBuffer));
}


#ifdef UBLOX_OBSERVER

void Ublox::register_event_callback(int32_t mask, event_callback callback)
{
    if(callback!=nullptr)
    {
        for(int i=0; i<UBLOX_EVENT_LIST_MAX; i++)
        {
            if(ublox_event_list[i].callback == nullptr)
            {
                ublox_event_list[i].callback = callback;
            }
        }
    }

}

void Ublox::notify_event(int32_t event_id, void *param)
{

    for(int i=0;i<UBLOX_EVENT_LIST_MAX;i++)
    {
        if(ublox_event_list[i].callback != nullptr)
        {
            //ublox_event_list[i].callback(event_id, param, strlen((char*)param));
            //----
            (Protocol::*)(ublox_event_list[i].callback)(event_id, param, strlen((char*)param));
           //event_callback *p_cb = Protocol::*(ublox_event_list[i].callback);

        }
    }
}

void Ublox::free_event_list()
{
    memset((char*)ublox_event_list, 0, sizeof(ublox_event_list));
}
#endif
