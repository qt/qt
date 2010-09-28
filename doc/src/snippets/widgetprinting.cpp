
#include <QtGui>

class Window : public QWidget
{
    Q_OBJECT

public:
    Window() {
        myWidget = new QPushButton("Print Me");
        connect(myWidget, SIGNAL(clicked()), this, SLOT(print()));

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(myWidget);
        setLayout(layout);
    }

private slots:
    void print() {
        QPrinter printer(QPrinter::HighResolution);
        
        printer.setOutputFileName("test.pdf");

//! [0]
        QPainter painter;
        painter.begin(&printer);
        double xscale = printer.pageRect().width()/double(myWidget->width());
        double yscale = printer.pageRect().height()/double(myWidget->height());
        double scale = qMin(xscale, yscale);
        painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,
                           printer.paperRect().y() + printer.pageRect().height()/2);
        painter.scale(scale, scale);
        painter.translate(-width()/2, -height()/2);

        myWidget->render(&painter);
//! [0]
    }

private:
    QPushButton *myWidget;
};

int main(int argv, char **args)
{
    QApplication app(argv, args);

    Window window;
    window.show();

    return app.exec();
}

#include "main.moc"

