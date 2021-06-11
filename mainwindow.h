#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
/*
#include <QtCharts>//faltaba incluir este header
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
*/
#include <QtSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>

#include <qapplication.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_scale_engine.h>
#include <qwt_legend.h>
#include "ledindicator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class QSerialPort;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void usbport(void);
    void file2export(void);
    void initChart(void);
    //////////////////////////

    //QSplineSeries *series;
    //QLineSeries *series;

    QPolygonF *points1;
    QwtPlotCurve *curve1;

    QPolygonF *points2;
    QwtPlotCurve *curve2;

    QPolygonF *points3;
    QwtPlotCurve *curve3;

    //////////////////////

private slots:
    void readSerial();

    //void on_pushButton_clicked();

    void on_recorrido_editingFinished();

    void on_doubleSpinBox_editingFinished();

    void on_intervalo_editingFinished();

    void on_pushButton_clicked();

    void on_autoscale_clicked();

    void on_generator_toggled(bool checked);

private:
    Ui::MainWindow *ui;

    //
    QSerialPort *usbCDC;
    /*
     * 1a86:7523 QinHeng Electronics CH340 serial converter
     */
    static const quint16 usbCDC_vendor_id = 6790;   //
    static const quint16 usbCDC_product_id = 29987; //
    QString usbCDC_port_name;
    bool usbCDC_is_available;

    LedIndicator *led_capture_mv1;
    LedIndicator *led_capture_mv2;
    LedIndicator *led_capture_mv3;
};
#endif // MAINWINDOW_H
