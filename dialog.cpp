// dialog.cpp

#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>
#include <QString>
#include <QProcess>
#include <QScrollBar>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QStringBuilder>
#include <QTime>

// FFPROBE_TIEMPO_DURACION_TOTAL_EN_SEGUNDOS % "un archivo"
#define FFPROBE_TIEMPO_DURACION_TOTAL_EN_SEGUNDOS "ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 -i "

// "ffmpeg -i \"" % archivoDeOrigen % FFMPEG_XDCAM_PISTA_DE_AUDIO_NO_MONO % "\"" % archivoDeDestino % "\""
#define FFMPEG_XDCAM_PISTA_DE_AUDIO_NO_MONO "\" -y -v quiet -stats -map 0:v -pix_fmt yuv422p -vcodec mpeg2video -non_linear_quant 1 -r 25 -s 1920x1080 -vf \"scale=1920:1080:force_original_aspect_ratio=decrease,pad=1920:1080:(ow-iw)/2:(oh-ih)/2\" -aspect 16:9 -flags +ildct+ilme -top 1 -dc 10 -intra_vlc 1 -qmax 3 -lmin \"1*QP2LAMBDA\" -vtag xd5c -rc_max_vbv_use 1 -rc_min_vbv_use 1 -g 12 -bf 2 -color_primaries bt709 -color_trc bt709 -colorspace bt709 -map 0:a:0 -c:a:0 pcm_s16le -map 0:a:1? -c:a:1 pcm_s16le -map 0:a:2? -c:a:2 pcm_s16le -map 0:a:3? -c:a:3 pcm_s16le -map 0:a:4? -c:a:4 pcm_s16le -map 0:a:5? -c:a:5 pcm_s16le -map 0:a:6? -c:a:6 pcm_s16le -map 0:a:7? -c:a:7 pcm_s16le -ar 48000 -b:v 50000k -minrate 50000k -maxrate 50000k -bufsize 8000k -trellis 2 -f mxf "



Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    // Create processes
    // transcoding
    procesoFFMPEG = new QProcess(this);
    procesoFFPROBE = new QProcess(this);

    connect(procesoFFMPEG, SIGNAL(started()), this, SLOT(inicioProcesoFFMPEG()));
    connect(procesoFFMPEG, SIGNAL(readyReadStandardOutput()), this, SLOT(listoParaLeerStdOutFFMPEG()));
    connect(procesoFFMPEG, SIGNAL(finished(int)), this, SLOT(finProcesoFFMPEG(int)));

    connect(procesoFFPROBE, SIGNAL(started()), this, SLOT(inicioProcesoFFPROBE()));
    connect(procesoFFPROBE, SIGNAL(readyReadStandardOutput()),this,SLOT(listoParaLeerStdOutFFPROBE()));
    connect(procesoFFPROBE, SIGNAL(finished(int)), this, SLOT(finProcesoFFPROBE(int)));
}

Dialog::~Dialog()
{
    delete ui;
}
// ------------ PROCESO FFPROBE     -------------
void Dialog::inicioProcesoFFPROBE()
{
    qDebug() << "inicioProcesoFFPROBE()";
}
void Dialog::finProcesoFFPROBE(int retorno)
{
    qDebug() << "finProcesoFFPROBE(int retorno)";
    if (retorno == 0){
        qDebug() << "retorno == 0";
    }else{
        qDebug() << "retorno != 0";
    }
}
void Dialog::obtenerDatos(QString strParametros){
    qDebug() << strParametros;
    procesoFFPROBE->setProcessChannelMode(QProcess::MergedChannels);
    procesoFFPROBE->start(strParametros);
}

void Dialog::listoParaLeerStdOutFFPROBE()
{
    qDebug() << "listoParaLeerStdOutFFPROBE()";
    QString strStdout = procesoFFPROBE->readAllStandardOutput().simplified();
    qDebug() << strStdout;
    int intDuracionTotal = parsearTiempoDeStdOutDeFFPROBEyDevolverSegundos(strStdout);
    qDebug() << intDuracionTotal;
    ui->progressBar->setMaximum(intDuracionTotal + 1);
}

int Dialog::parsearTiempoDeStdOutDeFFPROBEyDevolverSegundos(QString strStdOutDeFFPROBE){
    QString strDuracionTotal = strStdOutDeFFPROBE.split(".")[0];
    return strDuracionTotal.toInt();
}
// ------------ FIN PROCESO FFPROBE -------------

// ------------ PROCESO FFMPEG      -------------
void Dialog::inicioProcesoFFMPEG()
{
    qDebug() << "inicioProcesoFFMPEG()";
}

void Dialog::transcodear(QString strParametros){
    qDebug() << strParametros;
    ui->progressBar->setValue(0);
    ui->labelEstadoDeTranscodificacion->setText("Estado de Transcodificacion: Transcodificando");
    procesoFFMPEG->setProcessChannelMode(QProcess::MergedChannels);
    procesoFFMPEG->start(strParametros);
}

void Dialog::on_botonDeComenzar_clicked()
{
    QString archivoDeOrigen = ui->textBoxRutaDeOrigen->text();
    QString archivoDeDestino = ui->textBoxRutaDeDestino->text();

    qDebug() << "Argumentos: " << FFMPEG_XDCAM_PISTA_DE_AUDIO_NO_MONO;
    qDebug() << "ArchivoOrigen: " << archivoDeOrigen;
    qDebug() << "ArchivoDeDestino: " << archivoDeDestino;

    obtenerDatos(FFPROBE_TIEMPO_DURACION_TOTAL_EN_SEGUNDOS + "\"" % archivoDeOrigen % "\"");
    transcodear("ffmpeg -i \"" % archivoDeOrigen % FFMPEG_XDCAM_PISTA_DE_AUDIO_NO_MONO % "\"" % archivoDeDestino % "\"");
}

void Dialog::listoParaLeerStdOutFFMPEG()
{
    qDebug() << "listoParaLeerStdOutFFMPEG()";
    QString strStdout = procesoFFMPEG->readAllStandardOutput();
    qDebug() << strStdout;
    int intTiempoProcesado;
    intTiempoProcesado = parsearTiempoDeStdOutDeFFMPEGyDevolverSegundos(strStdout);
    qDebug() << intTiempoProcesado;
    ui->progressBar->setValue(intTiempoProcesado);
    ui->textEdit->setText(strStdout);
}

int Dialog::parsearTiempoDeStdOutDeFFMPEGyDevolverSegundos(QString strStdOutDeFFMPEG){
    QString strTiempoProcesado = strStdOutDeFFMPEG.split("time=")[1].left(10); //hh:mm:ss.z
    QTime qTimeTiempoProcesado = QTime::fromString(strTiempoProcesado, "hh:mm:ss.z");
    return QTime(0, 0, 0).secsTo(qTimeTiempoProcesado);
}
void Dialog::finProcesoFFMPEG(int retorno)
{
    qDebug() << "Valor de retorno" << retorno;
    if (retorno == 0){
        ui->labelEstadoDeTranscodificacion->setText("Estado de Transcodificacion: EXITO");
        ui->progressBar->setMaximum(100);
        ui->progressBar->setValue(100);
    }else{
        ui->labelEstadoDeTranscodificacion->setText("Estado de Transcodificacion: FALLO");
        ui->progressBar->setValue(0);
    }
}
// ------------ FIN PROCESO FFMPEG  -------------


// Browse... button clicked - this is for input file
void Dialog::on_botonDeSeleccionDeOrigen_clicked()
{
    QString fileName =
            QFileDialog::getOpenFileName(
                this,
                tr("Archivo Origen"),
                "/home/madmin/Downloads/",
                tr("videos (*)"));
    if (!fileName.isEmpty()) {
        ui->textBoxRutaDeOrigen->setText(fileName);
    }
}

void Dialog::on_botonDeSeleccionDeDestino_clicked()
{
    QString fileName =
            QFileDialog::getSaveFileName(
                this,
                tr("Archivo Destino"),
                "/home/madmin/Downloads/",
                tr("Video (*.MXF)"));

    if (!fileName.isEmpty()) {
        ui->textBoxRutaDeDestino->setText(fileName);
    }
}


void Dialog::on_pushButton_3_clicked()
{
    qDebug() << "No hago nada todavia";
    QMessageBox mensaje;
    mensaje.setText("No hago nada todavia");
    mensaje.exec();
}
