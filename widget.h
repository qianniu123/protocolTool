#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSerialPort>
#include <QTimer>

#include "protocol.h"
#include "sg/sgprotocol.h"
#include "JL/jlprotocol.h"
#include "ublox/ublox.h"
#include "plot.h"

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
    Protocol    *m_protocol;

    QSerialPort *m_serialPort;
    QTimer      *m_portTimer;
    QTimer      *m_readyReadTimer;

    Plot        *mPlot;

public slots:
    void slot_port_timeout();
    void slot_port_readyRead();
    void slot_port_readReadTimer_start();
    void slot_protocol_changed(QString protocolName);

    void slot_send_data(char* data, int len);
    void slot_send_display_data(char *data,int len);

private slots:
    void on_pushButton_open_clicked();
    void on_pushButton_plot_clicked();

signals:
    void sig_data_recv(char* data, int len);

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
