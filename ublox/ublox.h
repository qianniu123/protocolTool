#ifndef UBLOX_H
#define UBLOX_H

#include <stdint.h>

#pragma pack(0x1)
//"µb"
#define SYNC_CHAR1 0xB5 //'µ'
#define SYNC_CHAR2 0x62 //'b'


typedef enum
{
    UBX_NAV = 0x01,
    UBX_RMX = 0x02,
    UBX_INF = 0x04,
    UBX_ACK = 0x05, //Ack/Nak Messages: Acknowledge or Reject messages to UBX-CFG input messages
    UBX_CFG = 0x06, //Configure the receiver.
    UBX_UPD = 0x09,
    UBX_MON = 0x0A,
    UBX_AID = 0x0B, //AssistNow Aiding Messages:
    UBX_TIM = 0x0D,
    UBX_ESP = 0x10,
    UBX_MGA = 0x13,
    UBX_LOG = 0x21,
    UBX_SEC = 0x27,
    UBX_HNR = 0x28,
}msg_class_e;

/* Mnemonic Cls/ID      Length  Type    Description
 *
 * CFG-NMEA 0x06 0x17   20      Get/Set Extended NMEA protocol configuration V1
 * CFG-GNSS 0x06 0x3E   4+8*num Get/Set GNSS system configuration
 *
 */

typedef struct
{
    uint8_t sync_char1;
    uint8_t sync_char2;
    uint8_t msg_class;
    uint8_t id;
    uint16_t payload_len;//little endian
    uint8_t payload[0];
    //uint16_t ck_sum;
}ublox_frame_t;

#pragma pack()

//======================
#include <QObject>
#include "protocol.h"

class Ublox : public Protocol
{
    Q_OBJECT
public:
    Ublox();

    char debugBuffer[1024];
    uint8_t sendBuffer[1024];
    uint8_t recvBuffer[1024];
    QByteArray recvArray;

    void ublox_message_processing(char *data, int len);
    void nmea_sentence_process(char *data);
    void nmea_parse(char *data, int len);
    void ublox_parse(char *data, int len);

signals:
    //void sig_plot(double x, double y) override;

public slots:
    void slot_cmd_send(QString strCmd) override;
    void slot_data_recv(char *data, int len) override;

    //ublox config
    void slot_ublox_config_nmea(void);//config nmea protocol
    void slot_ublox_config_gnss(void);
};

#endif // UBLOX_H
