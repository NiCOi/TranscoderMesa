#include "dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    printf("asd");
    QApplication a(argc, argv);
    Dialog w;
    w.show();
    return a.exec();
}
