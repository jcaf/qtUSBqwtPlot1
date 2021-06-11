#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ledindicator.h"

QFile file("data.csv");
QTextStream stream( &file);

void MainWindow::file2export(void)
{
    //if (!file.open(QIODevice::WriteOnly | QIODevice::Append ) )
    if (!file.open(QIODevice::WriteOnly | QIODevice::WriteOnly ) )
    {
        QMessageBox::warning(this, "Error", "No se puede crear el archivo");
    }
    else
    {
        //write Header csv file
        stream <<"MV1(mV),RECORRIDO(m),MV2(mV),CORRIENTE(mA),RECORRIDO(m),MV3(mV),CORRIENTE(mA),RECORRIDO(m)"<<Qt::endl;
    }
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
    }
    else
    {
        QMessageBox::warning(this, "Port error", "Tarjeta de control no encontrado");
    }


}

void MainWindow::initChart(void)
{
    ui->mv1->setTitle("mv1");
    ui->mv1->setCanvasBackground(QColor(255,255,255));
    ui->mv1->setAxisScale(QwtPlot::xBottom,-1500,1500,1500);
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

    QwtSymbol *symbol1 = new QwtSymbol( QwtSymbol::Ellipse,
      QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
    curve1->setSymbol( symbol1 );

    //QPolygonF points;
    points1 = new QPolygonF;
    *points1 << QPointF( 0.0, 0.0 );
//    *points << QPointF( 0.0, 10 ) << QPointF( 1000, 20 )
//      << QPointF( -1000, 30) << QPointF( 1500, 40 )
//      << QPointF( -1500.0, 50 ) << QPointF( 5.0, 60 );

    curve1->setSamples( *points1 );
    curve1->attach( ui->mv1 );

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    ui->mv2->setTitle("mv2");
    ui->mv2->setCanvasBackground(QColor(255,255,255));
    ui->mv2->setAxisScale(QwtPlot::xBottom,-1500,1500,1500);
    ui->mv2->setAxisScale(QwtPlot::yLeft,150,0,10);

    //QwtPlotCurve *curve = new QwtPlotCurve();
    curve2 = new QwtPlotCurve();
    //curve2->setTitle( "Some Points" );
    curve2->setPen( Qt::red, 2 ),
    curve2->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    QwtSymbol *symbol2 = new QwtSymbol( QwtSymbol::Ellipse,
      QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
    curve2->setSymbol( symbol2 );

    //QPolygonF points;
    points2 = new QPolygonF;
    *points2 << QPointF( 0.0, 0.0 );

    curve2->setSamples( *points2 );
    curve2->attach( ui->mv2 );

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    ui->mv3->setTitle("mv3");
    ui->mv3->setCanvasBackground(QColor(255,255,255));
    ui->mv3->setAxisScale(QwtPlot::xBottom,-1500,1500,1500);
    ui->mv3->setAxisScale(QwtPlot::yLeft,150,0,10);

    //QwtPlotCurve *curve = new QwtPlotCurve();
    curve3 = new QwtPlotCurve();
    //curve2->setTitle( "Some Points" );
    curve3->setPen( Qt::green, 2 ),
    curve3->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    QwtSymbol *symbol3 = new QwtSymbol( QwtSymbol::Ellipse,
      QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
    curve3->setSymbol( symbol3 );

    //QPolygonF points;
    points3 = new QPolygonF;
    *points3 << QPointF( 0.0, 0.0 );

    curve3->setSamples( *points3 );
    curve3->attach( ui->mv3 );
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

    //coloring background
    QPalette pal = ui->generator->palette();
    pal.setColor(QPalette::Button, QColor(Qt::gray));
    ui->generator->setAutoFillBackground(true);
    ui->generator->setPalette(pal);
    ui->generator->update();
    //

}

MainWindow::~MainWindow()
{
    delete ui;
    if (usbCDC->isOpen())
    {
        usbCDC->close();
    }

    file.close();
}

/* Final USB rx
 *
 */

QString str_acc = "";
//float recorrido = 0;
float posicion = 0;
float mv1 = 0;  //el uC envia en milivoltios, con 2 decimales
float mv2 = 0;  //el uC envia en milivoltios, con 2 decimales
float mv3 = 0;  //el uC envia en milivoltios, con 2 decimales
float mv2_current = 0;  //el uC envia en miliamperios, con 2 decimales
float mv3_current = 0;  //el uC envia en miliamperios, con 2 decimales

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

#define USB_DATACODE_TOKEN_BEGIN '@'
#define USB_DATACODE_TOKEN_END '\r'

void MainWindow::readSerial()
{
    QByteArray serialBuff = usbCDC->readAll();
    QString str_payload = QString::fromStdString(serialBuff.toStdString());
    str_acc += str_payload;         //va acumulando y formando el string

    qDebug()<< str_acc;

    char USB_DATACODE;
    char USB_payload_char[30];
    int8_t USB_payload_idx=0;
    char c;
    int8_t sm0;
    bool newData = false;

    std::string Cstr = str_acc.toStdString();
    int length = Cstr.length();

    sm0 = 0;
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
                str_acc = "";
                sm0 = 0;
                newData = true;
                break;
            }
            else
            {
                USB_payload_char[USB_payload_idx++] = c;
            }
        }
    }
    if (newData == true)
    {
        float payload_f = atof(USB_payload_char);

        switch (USB_DATACODE)
        {

           case USB_DATACODE_MV1:

                led_capture_mv1->toggle();

                mv1 = payload_f;
                *points1 << QPointF(mv1/1000.0f, posicion);//grafica mv1 en voltios, NO milivoltios
                curve1->setSamples( *points1 );
                stream << mv1 <<"," << posicion;
            break;
            case USB_DATACODE_MV2:
//                led_capture_mv1->toggle();
//                led_capture_mv2->toggle();

//                mv2 = payload_f;    //mv1, mv2 y mv3 se graban en archivo en milivoltios
//                *points2 << QPointF(mv2/1000.0f, recorrido);//grafica mv2 en voltios, NO milivoltios
//                curve2->setSamples( *points2 );
             break;
            case USB_DATACODE_MV3:
//                led_capture_mv2->toggle();
//                led_capture_mv3->toggle();

//                mv3 = payload_f;
//                *points3 << QPointF(mv3/1000.0f, recorrido);//grafica mv3 en voltios, NO milivoltios
//                curve3->setSamples( *points3 );
             break;
            case USB_DATACODE_CURRENT:
//                //mv2_current = payload_f * 1000;
//                //sprintf(str,"%.1f", mv2_current);
//                //ui->mv2_current->setText(str);

//                mv2_current = payload_f;
//                ui->mv2_current->setText(USB_payload_char);

//                stream <<"," << mv2 <<"," << mv2_current <<","<< recorrido;
             break;
//            case USB_DATACODE_MV3CURRENT:
//                //mv3_current = payload_f * 1000;
//                //sprintf(str,"%.1f", mv3_current);
//                //ui->mv3_current->setText(str);


//                mv3_current = payload_f;
//                ui->mv3_current->setText(USB_payload_char);


//                stream <<"," << mv3 <<"," << mv3_current <<","<< recorrido << Qt::endl;

//                led_capture_mv3->toggle();
             break;
            //case USB_DATACODE_RECORRIDO:

            case USB_DATACODE_POSICION:
                //recorrido = payload_f;
                posicion = payload_f;
                ui->posicion->setText(USB_payload_char);

             break;

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
    ui->mv1->setAxisScale(QwtPlot::yLeft,posicion,0,10);
    ui->mv2->setAxisScale(QwtPlot::yLeft,posicion,0,10);
    ui->mv3->setAxisScale(QwtPlot::yLeft,posicion,0,10);
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
    }
    usbCDC->write(str);
    //
    ui->generator->setAutoFillBackground(true);
    ui->generator->setPalette(pal);
    ui->generator->update();


}

