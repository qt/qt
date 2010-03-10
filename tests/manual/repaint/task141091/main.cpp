#include <QtGui>
#include <QDebug>

class MyWidget : public QWidget
{
public:
    MyWidget() : QWidget() { 
        
        
        setAttribute(Qt::WA_OpaquePaintEvent);
        setAttribute(Qt::WA_StaticContents); }
protected:
    void paintEvent(QPaintEvent *e) { qDebug() << e->rect(); }
};

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    MyWidget w;
    w.show();
    return a.exec();
}