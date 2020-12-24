#include "math_model.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    math_model w;
    w.show();
    return a.exec();
}
