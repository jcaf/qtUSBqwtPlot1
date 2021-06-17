#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ledindicator.h"
#include <QFileDialog>
#include <QDateTime>
#include "libxlsxwriter/include/xlsxwriter.h"

float mv1_amplifier_K = 1;
struct mv1_data
{
       double posicion;
       double mv1_voltaje;
};
QList<mv1_data> mv1Matrix;
struct mv2mv3_data
{
       double posicion;
       double mv2_voltaje;
       double mv3_voltaje;
       double corriente;
};
QList<mv2mv3_data> mv2mv3Matrix;

//CSV
//QFile file_export_mv1("mv1.csv");
//QTextStream stream_export_mv1( &file_export_mv1);
//QFile file_export_mv2mv3("mv2mv3.csv");
//QTextStream stream_export_mv2mv3( &file_export_mv2mv3);

void MainWindow::file2export(void)
{
    //if (!file.open(QIODevice::WriteOnly | QIODevice::Append ) )
//    if (!file_export_mv1.open(QIODevice::WriteOnly | QIODevice::WriteOnly ) )
//    {
//        QMessageBox::warning(this, "Error", "No se puede crear el archivo mv1.csv");
//    }
//    else
//    {
//        stream_export_mv1 <<"POSICION(metros),MV1(mV)"<<Qt::endl;
//    }
//    //
//    if (!file_export_mv2mv3.open(QIODevice::WriteOnly | QIODevice::WriteOnly ) )
//    {
//        QMessageBox::warning(this, "Error", "No se puede crear el archivo mv2mv3.csv");
//    }
//    else
//    {
//        stream_export_mv2mv3 <<"POSICION(metros),MV2(mV),MV3(mV),CORRIENTE(mV)"<<Qt::endl;
//    }
    //file.close();
}
void MainWindow::usbport(void)
{
    usbCDC_is_available = false;
    usbCDC_port_name = "";
    usbCDC = new QSerialPort;

    //+- Solo para depurar
    qDebug()<< "number of available ports "<< QSerialPortInfo::availablePorts().length();
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        //qDebug()<< "Has vendor ID" << serialPortInfo.hasVendorIdentifier();
        if (serialPortInfo.hasVendorIdentifier()){
            //qDebug()<< "Vendor id"<< serialPortInfo.vendorIdentifier();
        }

        //qDebug()<< "Has product ID" << serialPortInfo.hasProductIdentifier();
        if (serialPortInfo.hasProductIdentifier())
        {
            //qDebug()<< "Product ID" << serialPortInfo.productIdentifier();
        }
    }
    //+-
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        if (serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier())
        {
            if ( (serialPortInfo.productIdentifier() == usbCDC_product_id) && (serialPortInfo.vendorIdentifier() == usbCDC_vendor_id) )
            {
                usbCDC_is_available = true;
                usbCDC_port_name  =serialPortInfo.portName();
                qDebug()<< usbCDC_port_name;
            }
        }
    }
    //+-
    if (usbCDC_is_available)
    {
        usbCDC->setPortName(usbCDC_port_name);
        //usbCDC->open(QSerialPort::WriteOnly);
        usbCDC->open(QSerialPort::ReadWrite);
        //usbCDC->open(QSerialPort::ReadOnly);//ok x lecturas
        //usbCDC->setBaudRate(QSerialPort::Baud38400);
        usbCDC->setBaudRate(QSerialPort::Baud115200);
        usbCDC->setDataBits(QSerialPort::Data8);
        usbCDC->setParity(QSerialPort::NoParity);
        usbCDC->setStopBits(QSerialPort::OneStop);
        usbCDC->setFlowControl(QSerialPort::NoFlowControl);
        //
        QObject::connect(usbCDC, SIGNAL(readyRead()), this, SLOT(readSerial()));

        ui->status->setText("Conexión OK: Tarjeta de control encontrado");
    }
    else
    {
        QMessageBox::warning(this, "Port error", "Tarjeta de control no encontrado");
        ui->status->setText("Port error, Tarjeta de control no encontrado");

    }


}
#define MV1_AXISX_LIMITE_MIN -1.5
#define MV1_AXISX_LIMITE_MAX 1.5
#define MV1_AXISX_STEPSIZE 1.5

#define MV2_AXISX_LIMITE_MIN -1.5
#define MV2_AXISX_LIMITE_MAX 1.5
#define MV2_AXISX_STEPSIZE 1.5

#define MV3_AXISX_LIMITE_MIN -1.5
#define MV3_AXISX_LIMITE_MAX 1.5
#define MV3_AXISX_STEPSIZE 1.5

int mv1_position_x;
int mv1_width;
int mv1_heigth;

int mv2_width;
int mv2_heigth;

int mv3_width;
int mv3_heigth;

void MainWindow::initChart(void)
{
    ui->mv1->setTitle("mv1");
    ui->mv1->setCanvasBackground(QColor(255,255,255));
    ui->mv1->setAxisScale(QwtPlot::xBottom,MV1_AXISX_LIMITE_MIN,MV1_AXISX_LIMITE_MAX,MV1_AXISX_STEPSIZE);
    //ui->qwtPlot1->axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Inverted, true);

//    QwtLinearScaleEngine *engine = new QwtLinearScaleEngine();
//    engine->setAttribute(QwtScaleEngine::Inverted, true);
//    ui->qwtPlot1->setAxisScaleEngine(QwtPlot::yLeft, engine);



    ui->mv1->setAxisScale(QwtPlot::yLeft,150,0,10);

    //QwtPlotCurve *curve = new QwtPlotCurve();
    curve1 = new QwtPlotCurve();
    curve1->setTitle( "Some Points" );
    curve1->setPen( Qt::blue, 2 ),
    curve1->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    //sin el simbolo de punto redondo
//    QwtSymbol *symbol1 = new QwtSymbol( QwtSymbol::Ellipse,
//      QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
//    curve1->setSymbol( symbol1 );

    //QPolygonF points;
   points1 = new QPolygonF;
//    *points1 << QPointF( 0.0, 0.0 );//set 0x0

 //    *points << QPointF( 0.0, 10 ) << QPointF( 1000, 20 )
//      << QPointF( -1000, 30) << QPointF( 1500, 40 )
//      << QPointF( -1500.0, 50 ) << QPointF( 5.0, 60 );

    curve1->setSamples( *points1 );
    curve1->attach( ui->mv1 );


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    ui->mv2->setTitle("mv2");
    ui->mv2->setCanvasBackground(QColor(255,255,255));
    ui->mv2->setAxisScale(QwtPlot::xBottom,MV2_AXISX_LIMITE_MIN,MV2_AXISX_LIMITE_MAX,MV2_AXISX_STEPSIZE);
    ui->mv2->setAxisScale(QwtPlot::yLeft,150,0,10);

    //QwtPlotCurve *curve = new QwtPlotCurve();
    curve2 = new QwtPlotCurve();
    //curve2->setTitle( "Some Points" );
    curve2->setPen( Qt::red, 2 ),
    curve2->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    //sin el simbolo de punto redondo
//    QwtSymbol *symbol2 = new QwtSymbol( QwtSymbol::Ellipse,
//      QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
//    curve2->setSymbol( symbol2 );

    //QPolygonF points;
    points2 = new QPolygonF;
    //*points2 << QPointF( 0.0, 0.0 );

    curve2->setSamples( *points2 );
    curve2->attach( ui->mv2 );

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    ui->mv3->setTitle("mv3");
    ui->mv3->setCanvasBackground(QColor(255,255,255));
    ui->mv3->setAxisScale(QwtPlot::xBottom,MV3_AXISX_LIMITE_MIN,MV3_AXISX_LIMITE_MAX, MV3_AXISX_STEPSIZE);
    ui->mv3->setAxisScale(QwtPlot::yLeft,150,0,10);

    //QwtPlotCurve *curve = new QwtPlotCurve();
    curve3 = new QwtPlotCurve();
    //curve2->setTitle( "Some Points" );
    curve3->setPen( Qt::green, 2 ),
    curve3->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    //sin el simbolo de punto redondo
//    QwtSymbol *symbol3 = new QwtSymbol( QwtSymbol::Ellipse,
//      QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
//    curve3->setSymbol( symbol3 );

    //QPolygonF points;
    points3 = new QPolygonF;
    //*points3 << QPointF( 0.0, 0.0 );

    curve3->setSamples( *points3 );
    curve3->attach( ui->mv3 );

    /////////////////////
    mv1_width = ui->mv1->width();
    mv1_heigth = ui->mv1->height();
    mv1_position_x = ui->mv1->x();

    mv2_width = ui->mv2->width();
    mv2_heigth = ui->mv2->height();

    mv3_width = ui->mv3->width();
    mv3_heigth = ui->mv3->height();


}

int QMAINWINDOWS_WIDTH;
int QMAINWINDOWS_HEIGHT;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMAINWINDOWS_WIDTH = this->size().width();
    QMAINWINDOWS_HEIGHT = this->size().height();


    usbport();
    initChart();
    file2export();
    //
    ui->posicion->setFontPointSize(14);
    ui->current->setFontPointSize(14);


    //code for the led widget
    led_capture_mv1 = new LedIndicator();
    led_capture_mv2 = new LedIndicator();
    led_capture_mv3 = new LedIndicator();
    ui->gridLayoutLedsCapture->addWidget(led_capture_mv1,1,0);
    ui->gridLayoutLedsCapture->addWidget(led_capture_mv2,2,0);
    ui->gridLayoutLedsCapture->addWidget(led_capture_mv3,3,0);

    //generator button -coloring background
    QPalette pal = ui->generator->palette();
    pal.setColor(QPalette::Button, QColor(Qt::gray));
    ui->generator->setAutoFillBackground(true);
    ui->generator->setPalette(pal);
    ui->generator->update();
    //
    //generator button -coloring background
    //QPalette
            pal = ui->captura1->palette();
    pal.setColor(QPalette::Button, QColor(Qt::gray));
    ui->captura1->setAutoFillBackground(true);
    ui->captura1->setPalette(pal);
    ui->captura1->update();

    //generator button -coloring background
    //QPalette
            pal = ui->captura2->palette();
    pal.setColor(QPalette::Button, QColor(Qt::gray));
    ui->captura2->setAutoFillBackground(true);
    ui->captura2->setPalette(pal);
    ui->captura2->update();
}

MainWindow::~MainWindow()
{
    delete ui;
    if (usbCDC->isOpen())
    {
        usbCDC->close();
    }

    //file_export_mv1.close();
    //file_export_mv2mv3.close();
}

/* Final USB rx
 *
 */

QString str_acc = "";
float posicion = 0;
struct mv1_data mv1data;
struct mv2mv3_data mv2mv3data;
//float current = 0;  //el uC envia en miliamperios, con 2 decimales
//float mv1 = 0;  //el uC envia en milivoltios, con 2 decimales
//float mv2 = 0;  //el uC envia en milivoltios, con 2 decimales
//float mv3 = 0;  //el uC envia en milivoltios, con 2 decimales
bool mv1_captura1_end = false;
bool mv2mv3_captura2_end = false;

#define USB_DATACODE_MV1 'X'
#define USB_DATACODE_MV2 'Y'
#define USB_DATACODE_MV3 'Z'
#define USB_DATACODE_CURRENT 'C'
#define USB_DATACODE_POSICION 'P'
//
#define USB_DATACODE_CAPTURA1_ON 'A'
#define USB_DATACODE_CAPTURA1_OFF 'B'
#define USB_DATACODE_CAPTURA2_ON 'D'
#define USB_DATACODE_CAPTURA2_OFF 'E'

#define USB_DATACODE_AMPLIFICARx10_ON 'F'
#define USB_DATACODE_AMPLIFICARx10_OFF 'G'

#define USB_DATACODE_OUT1_ON 'H'
#define USB_DATACODE_OUT1_OFF 'I'
#define USB_DATACODE_OUT2_ON 'J'
#define USB_DATACODE_OUT2_OFF 'K'

#define USB_DATACODE_MV1_CAPTURA1_END 'L'
#define USB_DATACODE_MV2MV3_CAPTURA2_END 'M'

#define USB_DATACODE_TOKEN_BEGIN '@'
#define USB_DATACODE_TOKEN_END '\r'


void MainWindow::USB_commands(char USB_DATACODE, char *USB_payload_char )
{
    float payload_f;// = atof(USB_payload_char);

    switch (USB_DATACODE)
    {
       case USB_DATACODE_MV1:
            payload_f = atof(USB_payload_char);
            mv1data.mv1_voltaje= payload_f;

            //Solo a mv1 le afecta el factor de amplifacion x10
            mv1data.mv1_voltaje *=mv1_amplifier_K;

            *points1 << QPointF(mv1data.mv1_voltaje/1000.0f, mv1data.posicion);//grafica mv1 en voltios, NO milivoltios
            curve1->setSamples( *points1 );
            //stream_export_mv1 << posicion<<"," << mv1 << Qt::endl;
            mv1Matrix.append(mv1data);
        break;

        case USB_DATACODE_MV2:
            payload_f = atof(USB_payload_char);
            mv2mv3data.mv2_voltaje = payload_f;    //mv1, mv2 y mv3 se graban en archivo en milivoltios
//                *points2 << QPointF(mv2mv3data.mv2_voltaje/1000.0f, posicion);//grafica mv2 en voltios, NO milivoltios
//                curve2->setSamples( *points2 );
         break;

        case USB_DATACODE_MV3://grafica a la misma vez
            payload_f = atof(USB_payload_char);
            mv2mv3data.mv3_voltaje= payload_f;
            *points2 << QPointF(mv2mv3data.mv2_voltaje/1000.0f, mv2mv3data.posicion);//grafica mv2 en voltios, NO milivoltios
            *points3 << QPointF(mv2mv3data.mv3_voltaje/1000.0f, mv2mv3data.posicion);//grafica mv3 en voltios, NO milivoltios
            curve2->setSamples( *points2 );
            curve3->setSamples( *points3 );
            //stream_export_mv2mv3 <<posicion<<"," << mv2 <<"," << mv3 <<","<< current << Qt::endl;
            mv2mv3Matrix.append(mv2mv3data);
         break;

        case USB_DATACODE_CURRENT:
            //current = payload_f;
            //qDebug() << "current: " <<atof(USB_payload_char) << Qt::endl;

            if (ui->current-isEnabled())
            {
                payload_f = atof(USB_payload_char);
                mv2mv3data.corriente = payload_f;
                ui->current->setText(USB_payload_char);
            }
            else
            {
                mv2mv3data.corriente = 0.00f;
            }

         break;

        case USB_DATACODE_POSICION:
            payload_f = atof(USB_payload_char);

            //qDebug() << "posicion: " <<payload_f << Qt::endl;

            posicion = payload_f;
            if (posicion <= 0.000f)
            {
                posicion = 0.000f;
                ui->posicion->setText("0.000");
                //
                ui->captura1->setEnabled(false);
                ui->captura2->setEnabled(false);
                ui->autoscale->setEnabled(false);
            }
            else
            {
                ui->posicion->setText(USB_payload_char);
                //
                ui->autoscale->setEnabled(true);
                //
                //ui->captura1->setEnabled(true);
                //ui->captura2->setEnabled(true);

                if (!ui->captura1->isChecked())
                {
                    ui->captura2->setEnabled(true);
                }
                //
                if (!ui->captura2->isChecked())
                {
                    ui->captura1->setEnabled(true);
                }

            }
            mv1data.posicion = posicion;
            mv2mv3data.posicion = posicion;

            //
         break;

        case USB_DATACODE_MV1_CAPTURA1_END:
            ui->captura1->setChecked(false);
            mv1_captura1_end = true;

        break;

        case USB_DATACODE_MV2MV3_CAPTURA2_END:
            ui->captura2->setChecked(false);
            mv2mv3_captura2_end = true;
        break;


    default: break;
    }

}
//int yy;
void MainWindow::readSerial()
{
    QByteArray serialBuff = usbCDC->readAll();
    QString str_payload = QString::fromStdString(serialBuff.toStdString());
    //str_acc += str_payload;         //va acumulando y formando el string
    str_acc = str_payload;         //va acumulando y formando el string
    //qDebug()<< "str_acc:"<<++yy<<" - "<<str_acc<<Qt::endl;

    char USB_DATACODE;
    char USB_payload_char[30];
    char c;

    static int8_t USB_payload_idx;
    static int8_t sm0;

    std::string Cstr = str_acc.toStdString();
    int length = Cstr.length();

    //sm0 = 0;
    for (int i=0; i< length; i++)
    {
        c =  Cstr[i];
        if (sm0 == 0)
        {
            if ( c == USB_DATACODE_TOKEN_BEGIN)
            {
                USB_payload_idx = 0;
                sm0++;
            }
        }
        else if (sm0 == 1)
        {
            USB_DATACODE = c;
            sm0++;
        }
        else if (sm0 == 2)//storage payload
        {
            if (c == USB_DATACODE_TOKEN_END)
            {
                USB_payload_char[USB_payload_idx] = '\0';
                //
                //str_acc = "";
                //str_acc.clear();

                sm0 = 0;
                //newData = true;
                //break;
                USB_commands(USB_DATACODE, USB_payload_char);
            }
            else
            {
                USB_payload_char[USB_payload_idx++] = c;
            }
        }
    }

}


/*
 *Actualiza una nueva escala de acuerdo a lo ingresado en recorrido
*/
void MainWindow::on_recorrido_editingFinished()
{
    /*
    double recorrido = ui->recorrido->value();

    ui->mv1->setAxisScale(QwtPlot::yLeft,recorrido,0,10);
    ui->mv2->setAxisScale(QwtPlot::yLeft,recorrido,0,10);
    ui->mv3->setAxisScale(QwtPlot::yLeft,recorrido,0,10);
    */
}

/*
* @i____'\r'
**/
void MainWindow::on_doubleSpinBox_editingFinished()
{

}

/*
 * update nuevo itervalo, enviar data al uC
 */


void MainWindow::on_intervalo_editingFinished()
{
    //
    /*
    double intervalo = ui->intervalo->value();

    char str[10];
    char buff[10];

    strcpy(str,"@i");

    int ret = snprintf(buff, sizeof(buff), "%.2f", intervalo);

    if (ret < 0)//error
    {
        strcpy(buff,"0");
    }
    if (ret >= sizeof(buff) )
    {
         //Result was truncated - resize the buffer and retry.
         strcpy(buff,"0");
    }
    strcat(str,buff);
    strcat(str,"\r");

    qDebug() << "write usb: intervalo" <<str;
    usbCDC->write(str);
    */
}

//#define USB_DATACODE_START 'S'
//#define USB_DATACODE_PAUSE 'P'
//#define USB_DATACODE_RESET 'R'
//#define USB_DATACODE_INTERVALO 'I'
//#define USB_DATACODE_MODE_INTERVALO_METERS 'M'
//#define USB_DATACODE_MODE_INTERVALO_TIME 'T'
void MainWindow::on_pushButton_clicked()
{
//    char str[10];
//    char buff[10];
//    strcpy(str,"@");
//    buff[0] = USB_DATACODE_START;
//    buff[1] = '\0';
    usbCDC->write("@S\r");
}

/*
 * Autoscale: afecta las "y" de las 3 graficas
 */
void MainWindow::on_autoscale_clicked()
{
    ui->mv1->setAxisScale(QwtPlot::yLeft,posicion,0,posicion/4.0f);
    ui->mv2->setAxisScale(QwtPlot::yLeft,posicion,0,posicion/4.0f);
    ui->mv3->setAxisScale(QwtPlot::yLeft,posicion,0,posicion/4.0f);

    if (mv1Matrix.empty())
    {
//        *points1 << QPointF( 0.0, posicion );
        curve1->setSamples( *points1 );
    }
    if (mv2mv3Matrix.empty())
    {
  //      *points2 << QPointF( 0.0, posicion );
//        *points3 << QPointF( 0.0, posicion );

        curve2->setSamples( *points2 );
        curve3->setSamples( *points3 );
    }
}

void MainWindow::on_generator_toggled(bool checked)
{
    char str[10];
    str[0] = USB_DATACODE_TOKEN_BEGIN;
    str[2] = USB_DATACODE_TOKEN_END;
    str[3] = '\0';

    QPalette pal = ui->generator->palette();
    if (checked == true)
    {
        pal.setColor(QPalette::Button, QColor(Qt::green));
        str[1] = USB_DATACODE_OUT2_ON;
    }
    else
    {
        pal.setColor(QPalette::Button, QColor(Qt::gray));
        str[1] = USB_DATACODE_OUT2_OFF;
        ui->current->setText("0.00");
        //
        mv2mv3data.corriente = 0.00f;
    }
    usbCDC->write(str);
    //
    ui->generator->setAutoFillBackground(true);
    ui->generator->setPalette(pal);
    ui->generator->update();
}


void MainWindow::on_amplificarx10_stateChanged(int arg1)
{
}


void MainWindow::on_amplificarx10_toggled(bool checked)
{
//    char str[10];
//    str[0] = USB_DATACODE_TOKEN_BEGIN;
//    str[2] = USB_DATACODE_TOKEN_END;
//    str[3] = '\0';

    if (checked == true)
    {

        mv1_amplifier_K = 10.0;
        //str[1] = USB_DATACODE_AMPLIFICARx10_ON;
    }
    else
    {
        //str[1] = USB_DATACODE_AMPLIFICARx10_OFF;
        mv1_amplifier_K = 1.0;

    }
    //usbCDC->write(str);
    //qDebug()<<str;
}

/*
 * ACTIVA OUT1,
 * captura mv1 @ 500ms
 */
void MainWindow::on_captura1_toggled(bool checked)
{
    char str[10];

    str[0] = USB_DATACODE_TOKEN_BEGIN;
    str[2] = USB_DATACODE_TOKEN_END;
    str[3] = '\0';

    QPalette pal = ui->captura1->palette();
    if (checked == true)
    {
        pal.setColor(QPalette::Button, QColor(Qt::green));
        str[1] = USB_DATACODE_CAPTURA1_ON;
        //
        led_capture_mv1->setState(true);
        //
        ui->captura2->setEnabled(false);

        if (mv1_captura1_end == true)
        {
            mv1_captura1_end = false;
            //
            delete points1;
            points1 = new QPolygonF;
            //*points1 << QPointF( 0.0, posicion );
            curve1->setSamples( *points1 );
        }

    }
    else
    {
        pal.setColor(QPalette::Button, QColor(Qt::gray));
        str[1] = USB_DATACODE_CAPTURA1_OFF;
        //
        led_capture_mv1->setState(false);

        qDebug()<<"led_capture_mv1->setState(false)" << Qt::endl;

    }
    usbCDC->write(str);
    //
    ui->captura1->setAutoFillBackground(true);
    ui->captura1->setPalette(pal);
    ui->captura1->update();

}


void MainWindow::on_captura2_toggled(bool checked)
{
    char str[10];

    str[0] = USB_DATACODE_TOKEN_BEGIN;
    str[2] = USB_DATACODE_TOKEN_END;
    str[3] = '\0';

    QPalette pal = ui->captura2->palette();
    if (checked == true)
    {
        pal.setColor(QPalette::Button, QColor(Qt::green));
        str[1] = USB_DATACODE_CAPTURA2_ON;
        //
        led_capture_mv2->setState(true);
        led_capture_mv3->setState(true);

        ui->captura1->setEnabled(false);
        if (mv2mv3_captura2_end == true)
        {
            mv2mv3_captura2_end = false;
            //
            delete points2;
            delete points3;
            points2 = new QPolygonF;
            points3 = new QPolygonF;
            //*points2 << QPointF( 0.0, posicion );
            //*points3 << QPointF( 0.0, posicion );
            curve2->setSamples( *points2 );
            curve3->setSamples( *points3 );
            //
        }
    }
    else
    {
        pal.setColor(QPalette::Button, QColor(Qt::gray));
        str[1] = USB_DATACODE_CAPTURA2_OFF;
        //
        led_capture_mv2->setState(false);
        led_capture_mv3->setState(false);
    }
    usbCDC->write(str);
    //
    ui->captura2->setAutoFillBackground(true);
    ui->captura2->setPalette(pal);
    ui->captura2->update();

}


void MainWindow::on_exportar_clicked()
{
    QString filenameQString_datetimeStamp = QString("%1.xlsx").arg(QDateTime::currentDateTime().toString("ddMMyyyy-hh_mm_ss"));
    QString filenameQString = "mv1mv2mv3-" + filenameQString_datetimeStamp;

    //Se propone el filenameQString
    QString fileName2save = QFileDialog::getSaveFileName(this, tr("Save File"),
                               filenameQString,
                               tr("Excel (*.xlsx)"));

    //convertir de QString to const char*
    QByteArray filenameQByteArray = fileName2save.toLocal8Bit();
    const char *filename = filenameQByteArray.data();
    //Create a workbook and add a worksheet.
    lxw_workbook  *workbook  = workbook_new(filename);
    lxw_worksheet *worksheet_mv1 = workbook_add_worksheet(workbook, "mv1");
    lxw_worksheet *worksheet_mv2mv3 = workbook_add_worksheet(workbook, "mv2mv3");
    //
    int row = 0;
    int col = 0;
    struct mv1_data mv1data;
    struct mv2mv3_data mv2mv3data;
    //
    //Escribir en fila 0 la cabecera con los titulos de cada hoja
    worksheet_write_string (worksheet_mv1, row, col++, "Posicion(metros)",NULL);
    worksheet_write_string (worksheet_mv1, row, col++, "mV1(mv)",NULL);
    col = 0;
    worksheet_write_string (worksheet_mv2mv3, row, col++, "Posicion(metros)",NULL);
    worksheet_write_string (worksheet_mv2mv3, row, col++, "mV2(mv)",NULL);
    worksheet_write_string (worksheet_mv2mv3, row, col++, "mV3(mv)",NULL);
    worksheet_write_string (worksheet_mv2mv3, row, col++, "Corriente(mA)",NULL);

    //Para ambas hojas, se escribe a partir de la fila 1, fila 0 es la cabecera con los titulos
//    lxw_format    *format01   = workbook_add_format(workbook);
//      lxw_format    *format02   = workbook_add_format(workbook);
//      lxw_format    *format03   = workbook_add_format(workbook);
      lxw_format    *format2dec   = workbook_add_format(workbook);
      lxw_format    *format3dec   = workbook_add_format(workbook);
//      lxw_format    *format05   = workbook_add_format(workbook);
//      lxw_format    *format06   = workbook_add_format(workbook);
//      lxw_format    *format07   = workbook_add_format(workbook);
//      lxw_format    *format08   = workbook_add_format(workbook);
//      lxw_format    *format09   = workbook_add_format(workbook);
//      lxw_format    *format10   = workbook_add_format(workbook);
//      lxw_format    *format11   = workbook_add_format(workbook);
    // Set some example number formats.
//    format_set_num_format(format01, "0.000");
//    format_set_num_format(format02, "#,##0");
//    format_set_num_format(format03, "#,##0.00");
        format_set_num_format(format2dec, "0.00");
        format_set_num_format(format3dec, "0.000");
//    format_set_num_format(format05, "mm/dd/yy");
//    format_set_num_format(format06, "mmm d yyyy");
//    format_set_num_format(format07, "d mmmm yyyy");
//    format_set_num_format(format08, "dd/mm/yyyy hh:mm AM/PM");
//    format_set_num_format(format09, "0 \"dollar and\" .00 \"cents\"");

    for (row = 0; row < mv1Matrix.count(); row++)
    {
        col = 0;
        mv1data = mv1Matrix.at(row);
        worksheet_write_number(worksheet_mv1, row+1, col++, mv1data.posicion, format3dec);
        worksheet_write_number(worksheet_mv1, row+1, col++, mv1data.mv1_voltaje, format2dec);
    }
    for (row = 0; row < mv2mv3Matrix.count(); row++)
    {
        col = 0;
        mv2mv3data = mv2mv3Matrix.at(row);
        worksheet_write_number(worksheet_mv2mv3, row+1, col++,  mv2mv3data.posicion, format3dec);
        worksheet_write_number(worksheet_mv2mv3, row+1, col++, mv2mv3data.mv2_voltaje, format2dec);
        worksheet_write_number(worksheet_mv2mv3, row+1, col++, mv2mv3data.mv3_voltaje, format2dec);
        worksheet_write_number(worksheet_mv2mv3, row+1, col++, mv2mv3data.corriente, format2dec);
    }
    workbook_close(workbook);
}


void MainWindow::on_borrar_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Pregunta", "Desea borrar los datos/gráficos?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        //Borra todos los datos de las matrices
        mv1Matrix.clear();
        mv2mv3Matrix.clear();
        //Limpia graficos
        delete points1;
        delete points2;
        delete points3;
        points1 = new QPolygonF;
        points2 = new QPolygonF;
        points3 = new QPolygonF;
        curve1->setSamples( *points1 );
        curve2->setSamples( *points2 );
        curve3->setSamples( *points3 );
    }
    else
    {
    }
}

//#define QMAINWINDOWS_WIDTH 1302
//#define QMAINWINDOWS_HEIGHT 680
void MainWindow::resizeEvent(QResizeEvent *event)
{
    int mv1w = this->width()*mv1_width/QMAINWINDOWS_WIDTH;
    int mv1h = this->height()*mv1_heigth/QMAINWINDOWS_HEIGHT;

    ui->mv1->resize(mv1w, mv1h);

    int mv2w = this->width()*mv2_width/QMAINWINDOWS_WIDTH;
    int mv2h = this->height()*mv2_heigth/QMAINWINDOWS_HEIGHT;

    ui->mv2->setGeometry(mv1_position_x+50+mv1w,100, mv2w, mv2h);
    //ui->mv2->resize(mv2w, mv2h );

    int mv3w = this->width()*mv3_width/QMAINWINDOWS_WIDTH;
    int mv3h = this->height()*mv3_heigth/QMAINWINDOWS_HEIGHT;

    ui->mv3->setGeometry(mv1_position_x+50+50+mv1w+mv2w,100, mv3w, mv3h);
    //ui->mv3->resize(mv3w, mv3h);


}
