#include "ublox.h"
#include <QDebug>
#include "minmea.h"

Ublox::Ublox()
{
    m_cmdList.clear();
    m_cmdList << "UBLOX_CONFIG_BD" ;

    //-------------------
    m_strToCmd.clear();
    m_strToCmd["UBLOX_CONFIG_BD"] = (slot_function)&Ublox::slot_ublox_config_bd;
}

void Ublox::slot_cmd_send(QString strCmd)
{
    qDebug() << QString("Ublox::slot_cmd_send(%1)").arg(strCmd);
}
void Ublox::slot_data_recv(char* data, int len)
{
    recvArray.append(data, len);

    qDebug() << "slot_data_recv------------------------------";
    qDebug() << recvArray;
    ublox_message_processing(data, len);
    //nmea_parse()
    //ublox_parse()

    recvArray.clear();
}

void Ublox::slot_ublox_config_bd()
{
    qDebug() << "Ublox::slot_ublox_config_bd";

}


void Ublox::nmea_parse(char *data, int len)
{

}

void Ublox::ublox_parse(char *data, int len)
{

}

void Ublox::ublox_message_processing(char *data, int len)
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
    switch(minmea_sentence_id(data, false))
    {
        case MINMEA_SENTENCE_RMC:
        {
            qDebug() << "MINMEA_SENTENCE_RMC:" << data;
        }
        break;
        case MINMEA_SENTENCE_GSV:
        {
            qDebug() << "MINMEA_SENTENCE_GSV:" << data;
        }
        break;
        default:
        break;
    }

}
