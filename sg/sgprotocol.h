#ifndef SGPROTOCOL_H
#define SGPROTOCOL_H

//==========================================
//frame define
#include <stdint.h>

#pragma pack(0x1)

#define SG_PROTOCOL_VER         (0x01)
#define SG_ENCRYPTION_TYPE      (0)
#define SG_PACKET_END1          (0x23)
#define SG_PACKET_END2          (0x23)
#define SG_PACKET_END3          (0x5F)
#define SG_PACKET_END4          (0x2A)
#define SG_PACKET_END5          (0x2A)

/* global CMD */
typedef enum SG_COMMUT_CMD
{
    DEVICE_HEARTBEAT                = 62001, //F231 /* Device heart beat */
    DEVICE_SOFTWARE_VERSION_QUERY   = 60101, /* Fevice sfotware version query */
    DEVICE_HELLO                    = 60201, //0000EB29 /* Greeting the server after the device is powered on for the first time (unbound) */
    DEVICE_BIND                     = 60202, //EB2A /* Device binding confirmation */
    DEVICE_UNBIND                   = 60103, /* Device unbundling */
    USER_INFO_GET                   = 60501, /* Get user information */
    USER_INFO_PUSH                  = 60502, /* User information change push */
    USER_BASE_INFO_UPLOAD           = 60503, /* Upload user watch basic information upload */
    LOCATION_UPLOAD                 = 60403, // EBF3/* (Position, steps, power) report */
    LOCATION_QUERY                  = 60901, /* (location, step, power) query */
    LOCATION_TRRIGE					= 60905, //EDE9
    HEART_QUERY                     = 60902, /* heart rate data query */
    HEART_UPLOAD                    = 60302, //EB8E /* Heart rate data upload */
    SLEEP_QUERY                     = 62003, /* Sleep data query */
    SLEEP_UPLOAD                    = 60305, /* Sleep data upload */
    WEATHER_QUERY                   = 60608, /* The watch actively queries the weather data (1.each time it is turned on; 2.every three hours) */
    LUNAR_QUERY                     = 60609, /* The watch actively obtains the lunar calendar data (1.each time it is turned on; 2.every three hours) */
    DEVICE_OPERATION                = 60306, /* Report the key operation of the watch */
    DEVICE_PARAM_SET                = 60610, /* Device parameter setting (new) */
    DEVICE_PARAM_GET                = 62005, /* Device parameter query (new) */
    CONTROL_SWATCH_SERVICE          = 60607, /* Control watch service */
    HEALTH_TEST                     = 60606, /* The server actively tests the health data */
    DEVICE_COERCE_UNBIND            = 29901, /* The device requested to unbind */
    DEVICE_SOFTWARE_VERSION_UPLOAD  = 60105, /* Device software version upload (with IMSI) */
    HEALTH_UPLOAD                   = 60702, /* Health data upload (depending on project) */
    LOCATION_POLICY_SET             = 60405, /* Position strategy adjustment */

    /* Lcacl cmd, define by cenon (7F xx 00 00)*/
    CMD_GET_SERV_IP                 = 2130771968, /* 7F 01 Get server ip */
    CMD_DEV_UPDATE_TIME             = 2130837504, /* 7F 02 Device update time */
    CMD_DEV_VERS_UPDATE             = 2130903040, /* 7F 03 Device version update */
    CMD_GET_QRCODE                  = 2130968576, /* 7F 04 Device get QRcode */
    CMD_DEV_PARA_INIT               = 2131034112, /* 7F 05 Device param init */

    CMD_NB_GSV                      = 2146893824, /* 7F F7 00 00 Get factory GPS status */
    CMD_NB_STATE                    = 2146959360, /* 7F F8 00 00 Get/Set NB status */
    CMD_NB_WRITE_AGPS               = 2147024896, /* 7F F9 00 00 Write aGPS */
    CMD_NB_RFCAL                    = 2147090432, /* 7F FA 00 00 Get RF calibration state */
    CMD_NB_GPS                      = 2147155968, /* 7F FB 00 00 Get GPS state */
    CMD_NB_VERS                     = 2147221504, /* 7F FC 00 00 Get NB net state */
    CMD_NB_IMEI_SN                  = 2147287040, /* 7F FD 00 00 Get NB net state */
    CMD_NB_NET_STATE                = 2147352576, /* 7F FE 00 00 Get NB net state */
    CMD_DEV_CCID                    = 2147418112, /* 7F FF 00 00 Get sim CCID */
    CMD_DEV_IMSI					= 2147418113, /* 7F FF 00 01 Get IMSI */
}SG_COMMUT_CMD_E;

typedef struct sg_commut_data
{
    uint8_t protocol_ver;
    uint8_t enryption_type;
    int status;
    uint8_t token[32];
    int cmd;
    int data_len;
    uint8_t data[0];
}sg_commut_data_t;



//SG struct
typedef struct
{
    char userId[10+1];
    int32_t status;
}dev_bind_req_t;

typedef struct
{
    uint8_t count;
    int time;
}heart_upload_req_t;

typedef struct
{
    char ccid[20];
    int32_t csq;
}ccid_csq_rsp_t;

typedef struct
{
    uint8_t gsv_states_in_view;
    struct
    {
        uint8_t gsv_sate_id;
        uint8_t gsv_sate_snr;
    }gsv_states_info[3];

}gsv_rsp_t;

typedef struct
{
    int count;
    int time;
}sg_walk_t;

typedef struct
{
    double latitude;
    double longitude;
    uint8_t NS;
    uint8_t EW;
    int time;
}sg_gps_t;

typedef struct
{
    uint8_t value;
    int time;
}sg_battery_t;

#define LAC_FIX_CNT 5
typedef struct
{
    char dev_ip[64+1];
    char network[5+1];
    char mcc[8+1];
    char mnc[8+1];
    int time;
    struct
    {
        int32_t lac;
        int32_t ci;
        int32_t rssi;
    }lac_data[LAC_FIX_CNT];
}sg_mobile_t;

#define WIFI_FIX_CNT 5
typedef struct
{
    char mac[32+1];
    char mac_name[30+1];
    int32_t  signal;
}sg_wifi_t;

typedef struct
{
    int time;// to long int
    char version[10+1];
}sg_version_t;


typedef struct birthday
{
    short int tm_year;
    char tm_mon;
    char tm_day;
}birthday_t;

typedef struct
{
    birthday_t birthday;//
    uint8_t sex;
    uint8_t height;
    uint8_t weight;
}sg_user_info_t;

typedef struct
{
    int time_begin;
    int time_end;
    int time_deep;
    int time_wake;
    int time_light;
}sg_sleep_t;

typedef struct
{
    char date[10+1];
    char week[9+1];
    char lunar[18+1];
    char ganzhi[50+1];//?
    char zodiac[6+1];
    char fitavoid[50+1];//?
    int time;
}sg_lunar_t;

#define WEATHER_CNT 3
typedef struct
{
    char date[10+1];
    char weather[20+1];//?
    char temperature[30+1];//?
    char wind[20+1];//?
    char dress[20+1];//?
    char cur_temp[20+1];//?
    char airQuality[20+1];//
    char city[20+1];//?
}sg_weather_t;

#pragma pack()


//===============================================================
#include <QObject>
#include "protocol.h"

#include <QString>
#include <QMap>

using namespace std;

class SGProtocol;
typedef void (SGProtocol::*slot_func_sg)(void);

class SGProtocol : public Protocol //,public QObject
{
    Q_OBJECT
public:
    explicit SGProtocol(Protocol *parent = nullptr);
    void packInit();

    //QStringList m_cmdList;
    QMap<QString, slot_func_sg> m_strToCmd_sg; //cmd_str --> cmd_function()

    uint8_t sendBuffer[1024];
    //uint8_t recvBuffer[1024];
    QByteArray recvArray;
    char    debugBuffer[1024];

    sg_commut_data_t *p_sg_pack;
     //userId token
    uint8_t token[32];
    uint8_t userId[6];
    uint8_t packEnd[5];

    int parse_pack(char *data, int len) override;

signals:
    //void sig_SendData(uint8_t data, int len);//in Base class

public slots:
    void slot_cmd_send(QString strCmd) override;
    void slot_data_recv(char *data, int len) override;

    void slot_device_heartbeat_send(void);
    void slot_device_hello_send(void);
    void slot_device_bind_send(void);
    void slot_device_software_version_upload(void);

};

#endif // SGPROTOCOL_H
