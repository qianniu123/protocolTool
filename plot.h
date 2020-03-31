#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class Plot;
}

class Plot : public QWidget
{
    Q_OBJECT

public:
    explicit Plot(QWidget *parent = nullptr);
    ~Plot();

    QVector<double> x, y;
    //--------------------------------



public slots:   
    void slot_plot_xy(double x, double y);

private:
    Ui::Plot *ui;
};

#endif // PLOT_H
