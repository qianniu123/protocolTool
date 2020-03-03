#include "widget.h"
#include "ui_widget.h"

#include <QTableWidgetItem>
#include <QPushButton>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);


    //---------------
    init();
}

void Widget::init()
{
    m_protocol = new SGProtocol();

    int row_cnt = m_protocol->m_cmdList.count();
    //ui->tableWidget_cmd->setRowCount(row_cnt);
    ui->tableWidget_cmd->setColumnCount(1);
    for(int row_index=0; row_index<row_cnt; row_index++)
    {
        QPushButton *p_button = new QPushButton();
        QString text = m_protocol->m_cmdList.at(row_index);
        p_button->setText(text);

        slot_function slot_fun = m_protocol->m_strToCmd[text];
        connect(p_button, &QPushButton::clicked, m_protocol, slot_fun); //&slot seem as a normal function pointer

        ui->tableWidget_cmd->insertRow(row_index);
        ui->tableWidget_cmd->setCellWidget(row_index, 0, p_button);
    }
}

Widget::~Widget()
{
    delete ui;
}

