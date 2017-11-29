// dialog.h

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QProcess>
#include <QFile>
#include <QTextEdit>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

public slots:

public:
private slots:
    //Botones
    void on_botonDeComenzar_clicked();
    void on_botonDeSeleccionDeOrigen_clicked();
    void on_botonDeSeleccionDeDestino_clicked();
    void on_pushButton_3_clicked();

    //Seniales
    //Proceso FFMPEG
    void listoParaLeerStdOutFFMPEG();
    void inicioProcesoFFMPEG();
    void finProcesoFFMPEG(int);

    //Proceso FFPROBE
    void inicioProcesoFFPROBE();
    void listoParaLeerStdOutFFPROBE();
    void finProcesoFFPROBE(int);

    //Funciones
    void transcodear(QString);
    void obtenerDatos(QString);
    int parsearTiempoDeStdOutDeFFMPEGyDevolverSegundos(QString);
    int parsearTiempoDeStdOutDeFFPROBEyDevolverSegundos(QString);

private:
    Ui::Dialog *ui;
    QProcess *procesoFFMPEG;
    QProcess *procesoFFPROBE;
    QString mOutputString;
};

#endif // DIALOG_H
