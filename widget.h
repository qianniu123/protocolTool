#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSerialPort>
#include <QTimer>

#include "sg/sgprotocol.h"
#include "JL/jlprotocol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
private:
    Ui::Widget *ui;

public:
    void init();

    //void *m_protocol; //How to work with multiple protocols
    //JLProtocol  *m_protocol;//JL
    SGProtocol  *m_protocol;//SG

    QSerialPort *m_serialPort;
    QTimer      *m_portTimer;


public slots:
    void slot_port_timeout();
    void slot_port_readyRead();

private slots:
    void on_pushButton_open_clicked();
};
#endif // WIDGET_H
