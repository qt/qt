#include <QApplication>
#include <QWidget>
#include <QDial>

#include "ui_dials.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QWidget window;
    Ui::Dials dialsUi;
    dialsUi.setupUi(&window);    
    QList<QAbstractSlider *> sliders = window.findChildren<QAbstractSlider *>();
    foreach (QAbstractSlider *slider, sliders)
        slider->setAttribute(Qt::WA_AcceptTouchEvents);
    window.showMaximized();
    return app.exec();
}
