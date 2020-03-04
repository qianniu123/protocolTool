#ifndef JLPROTOCOL_H
#define JLPROTOCOL_H

#include <QObject>
#include "protocol.h"

#include <QString>
#include <QMap>

class JLProtocol;
//typedef void (JLProtocol::*slot_function_jl)(void);

class JLProtocol : public Protocol //,public QObject
{
    Q_OBJECT
public:
    explicit JLProtocol(Protocol *parent = nullptr);

    //QStringList m_cmdList;
    //QMap<QString, slot_function_jl> m_strToCmd;


signals:

public slots:
    void slot_heartbeat_send(void);
    void slot_test_send(void);

};

#endif // JLPROTOCOL_H
