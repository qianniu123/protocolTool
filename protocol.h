#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QObject>
#include <QMap>

class Protocol;
typedef void (Protocol::*slot_function)(void);

class Protocol : public QObject
{
    Q_OBJECT
public:
    explicit Protocol(QObject *parent = nullptr);

    //static QStringList m_protocolList;
    QStringList m_cmdList;
    QMap<QString, slot_function> m_strToCmd;


signals:

public slots:
    virtual void slot_cmd_send(QString strCmd) = 0;
};

#endif // PROTOCOL_H
