#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <QObject>
#include <QMap>

class Protocol;
typedef void (Protocol::*slot_function)(void);
typedef void (Protocol::*event_callback)(int32_t event_id, void* param, uint32_t param_len);

class Protocol : public QObject
{
    Q_OBJECT
public:
    explicit Protocol(QObject *parent = nullptr);

    //QStringList m_protocolList;
    QStringList m_cmdList;
    QMap<QString, slot_function> m_strToCmd;

    //virtual int parse_pack(char* data,int len);    
    virtual void register_event_callback(event_callback callback);

signals:
    void sig_send_data(char* data, int len);
    void sig_send_display_data(char *data,int len);

    void sig_plot(double x, double y);//for test
public slots:
    virtual void slot_cmd_send(QString strCmd) = 0;
    virtual void slot_data_recv(char *data, int len);

};

#endif // PROTOCOL_H
