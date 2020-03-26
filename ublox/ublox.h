#ifndef UBLOX_H
#define UBLOX_H

//======================
#include <QObject>
#include "protocol.h"


class Ublox : public Protocol
{
    Q_OBJECT
public:
    Ublox();

    uint8_t sendBuffer[1024];
    uint8_t recvBuffer[1024];
    QByteArray recvArray;

    void ublox_message_processing(char *data, int len);
    void nmea_sentence_process(char *data);
    void nmea_parse(char *data, int len);
    void ublox_parse(char *data, int len);

signals:

public slots:
    void slot_cmd_send(QString strCmd) override;
    void slot_data_recv(char *data, int len) override;

    //ublox config
    void slot_ublox_config_bd(void);
};

#endif // UBLOX_H
