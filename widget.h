#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSerialPort>
#include <QTimer>

#include "protocol.h"
#include "sg/sgprotocol.h"
#include "JL/jlprotocol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr, Protocol *protocol=nullptr);
    ~Widget();

public:
    void init();

    QStringList m_protocolList;
    //void *m_protocol; //How to work with multiple protocols
    //JLProtocol  *m_protocol;//JL
    //SGProtocol  *m_protocol;//SG
    Protocol    *m_protocol;

    QSerialPort *m_serialPort;
    QTimer      *m_portTimer;


public slots:
    void slot_port_timeout();
    void slot_port_readyRead();
    void slot_protocol_changed(QString protocolName);

    void slot_send_data(uint8_t* data, int len);
    void slot_send_debug_data(char *data);

private slots:
    void on_pushButton_open_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
