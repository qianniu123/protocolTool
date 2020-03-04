#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <QObject>
#include <QMap>

class Protocol;
typedef void (Protocol::*slot_function)(void);

class Protocol : public QObject
{
    Q_OBJECT
public:
    explicit Protocol(QObject *parent = nullptr);

    //QStringList m_protocolList;
    QStringList m_cmdList;
    QMap<QString, slot_function> m_strToCmd;


signals:
    void sig_send_data(uint8_t* data, int len);
    void sig_send_debug_data(char *data);

public slots:
    virtual void slot_cmd_send(QString strCmd) = 0;
};

#endif // PROTOCOL_H
