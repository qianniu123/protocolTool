#include "widget.h"
#include "ui_widget.h"
#include <QSerialPortInfo>
#include <QSplitter>
#include <QDebug>

#include <QTableWidgetItem>
#include <QPushButton>

Widget::Widget(QWidget *parent, Protocol *protocol): QWidget(parent) ,m_protocol(protocol)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle(QString("protocolTool_V1.0"));

    //layout
    this->setLayout(ui->horizontalLayout_all);

    //splitter
    /*
    QSplitter *splitter = new QSplitter(Qt::Horizontal, nullptr);
    splitter->addWidget(ui->textEdit);
    splitter->addWidget(ui->frame_right);
    splitter->show();
    */
    //--------------------------------------------------------------------------------
    m_portTimer = new QTimer();
    m_portTimer->setInterval(1000);
    connect(m_portTimer, &QTimer::timeout, this, &Widget::slot_port_timeout);
    m_portTimer->start();
    m_readyReadTimer = new QTimer();

    m_serialPort = new QSerialPort();
#if 0
    connect(m_serialPort, &QSerialPort::readyRead, this, &Widget::slot_port_readyRead);
#else
    connect(m_serialPort, &QSerialPort::readyRead, this, &Widget::slot_port_readReadTimer_start);
    connect(m_readyReadTimer, &QTimer::timeout, this, &Widget::slot_port_readyRead);
#endif
    connect(ui->pushButton_clearText, &QPushButton::clicked, ui->textEdit, &QTextEdit::clear);
    //--------------------------------------------------------------------------------------------------
    m_protocolList.clear();
    m_protocolList << "SG_PROTO" << "JL_PROTO" << "UBLOX";
    ui->comboBox_protocol->addItems(m_protocolList);
    connect(ui->comboBox_protocol, &QComboBox::currentTextChanged, this, &Widget::slot_protocol_changed);

    QStringList baudRateList;
    baudRateList << "115200" << "9600";
    ui->comboBox_baudRate->addItems(baudRateList);
    //-----------
    init();
}

void Widget::init()
{
    slot_protocol_changed(ui->comboBox_protocol->currentText());


}

void Widget::slot_protocol_changed(QString protocolName)
{
    if(m_protocol)
    {
        delete m_protocol;
        m_protocol = nullptr;
    }

    if(protocolName == "SG_PROTO")
    {
        m_protocol = new SGProtocol();
    }
    else if(protocolName == "JL_PROTO")
    {
        m_protocol = new JLProtocol();
    }
    else if(protocolName == "UBLOX")
    {
        m_protocol = new Ublox();
    }
    else
    {
        return;
    }

    connect(m_protocol, &Protocol::sig_send_data, this, &Widget::slot_send_data);
    connect(m_protocol, &Protocol::sig_send_display_data, this, &Widget::slot_send_display_data);
    connect(this, &Widget::sig_data_recv, m_protocol, &Protocol::slot_data_recv);

    int row_cnt = m_protocol->m_cmdList.count();
    ui->tableWidget_cmd->clear();
    ui->tableWidget_cmd->setRowCount(0);
    ui->tableWidget_cmd->setColumnCount(1);

    QStringList headerList;
    headerList << protocolName;
    ui->tableWidget_cmd->setHorizontalHeaderLabels(headerList);
    ui->tableWidget_cmd->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_cmd->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    for(int row_index=0; row_index<row_cnt; row_index++)
    {
        QPushButton *p_button = new QPushButton();
        QString text = m_protocol->m_cmdList.at(row_index);
        p_button->setText(text);

        slot_function slot_fun = m_protocol->m_strToCmd[text];
        if(slot_fun != nullptr) //!!!
        {
            connect(p_button, &QPushButton::clicked, m_protocol, slot_fun); //&slot seem as a normal function pointer
        }
        else
        {
            qDebug() << QString("slot_fun = nullptr!");
        }

        //or
        //maybe Mypushbutton inherit form qpushbutton, realize Mypushbutton::click(QString buttonName)
        //connect(p_button, &QPushButton::click, m_protocol, &Protocol::slot_cmd_send);//QPushButton::click has no param

        ui->tableWidget_cmd->insertRow(row_index);
        ui->tableWidget_cmd->setCellWidget(row_index, 0, p_button);
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::slot_port_timeout()
{
    static QList<QSerialPortInfo> oldComPorts;
    QList<QSerialPortInfo> comPorts = QSerialPortInfo::availablePorts();
    if(oldComPorts.size() != comPorts.size())
    {
        oldComPorts.clear();
        oldComPorts = comPorts;

        //refresh com ports
        QStringList portList;
        foreach (const QSerialPortInfo &portInfo, comPorts)
        {
              portList << portInfo.portName();
        }
        ui->comboBox_comPort->clear();
        ui->comboBox_comPort->addItems(portList);
    }
}

void Widget::on_pushButton_open_clicked()
{
    int baudRate = ui->comboBox_baudRate->currentText().toInt();
    m_serialPort->setBaudRate(baudRate);//(QSerialPort::Baud115200);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);

    m_serialPort->setReadBufferSize(1024);
    //m_serialPort->waitForReadyRead(100);
    m_serialPort->setPortName(ui->comboBox_comPort->currentText());

    if(ui->pushButton_open->text() == QString("打开串口"))
    {
        bool ret = m_serialPort->open(QIODevice::ReadWrite);
        if(ret)
        {
            ui->pushButton_open->setText(QString("关闭串口"));
        }
    }
    else if(ui->pushButton_open->text() == QString("关闭串口"))
    {
        m_serialPort->close();
        ui->pushButton_open->setText(QString("打开串口"));
    }
}

void Widget::slot_send_data(char *data, int len)
{
    qDebug() << QString("Widget::slot_send_data: len = %1").arg(len);
    QByteArray txArray(data, len);
    //send
    if(m_serialPort && m_serialPort->isOpen())
    {
        m_serialPort->write(txArray);
    }

    //dispaly
    QString strData; strData.append(txArray.toHex().toUpper());
    ui->textEdit->append(strData);
}

void Widget::slot_send_display_data(char *data, int len)
{
    //qDebug() << QString("Widget::slot_send_display_data(data:%1, len:%2)").arg(data).arg(len);
    //just for display
    QByteArray dispArray(data, len);
    ui->textEdit->append(QString(dispArray));
}

void Widget::slot_port_readReadTimer_start()
{
    m_readyReadTimer->start(100);

}
void Widget::slot_port_readyRead()
{
    m_readyReadTimer->stop();

    QByteArray rxArray = m_serialPort->readAll();
    //m_serialPort->waitForReadyRead(100);

    if(!rxArray.isEmpty())
    {
        //ui->textEdit->append(QString(rxArray.toHex().toUpper()));

        emit sig_data_recv(rxArray.data(), rxArray.size());//send to protocl
        //or
        //m_protocol->parse_pack(rxArray.data(), rxArray.size());
    }

}

void Widget::on_pushButton_plot_clicked()
{
    mPlot = new Plot();
    mPlot->show();

    connect(m_protocol, &Protocol::sig_plot, mPlot, &Plot::slot_plot_xy);
}
