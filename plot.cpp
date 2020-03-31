#include "plot.h"
#include "ui_plot.h"
#include "qcustomplot.h"
#include <math.h>

Plot::Plot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Plot)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("Plot"));
    setLayout(ui->verticalLayout);

    //-------------------------------------
    QCustomPlot *p_plot = ui->plot;
    p_plot->addGraph();
    p_plot->xAxis->setRange(0,100);
    p_plot->yAxis->setRange(0,50);

}

Plot::~Plot()
{
    delete ui;
}


void Plot::slot_plot_xy(double x, double y)
{
    //qDebug() << QString("Plot::slot_plot_xy(x=%1,y=%2)").arg(x).arg(y);
    static int i = 0;
    i++;
    this->x.push_back(i);//no use x
    this->y.push_back(y);

    if(i>100)
    {
        ui->plot->xAxis->setRange(i-100, i);
    }
    ui->plot->graph(0)->setData(this->x, this->y);
    ui->plot->replot();
}
