#include <QtGui/qgraphicsproxywidget.h>
#include <QtGui/qlabel.h>
#include "flowlayout.h"
#include "window.h"

Window::Window()
: QGraphicsWidget(0, Qt::Window)
{
    FlowLayout *lay = new FlowLayout;
    QString sentence(QLatin1String("I am not bothered by the fact that I am unknown. I am bothered when I do not know others."));
    QStringList words = sentence.split(QLatin1Char(' '), QString::SkipEmptyParts);
    for (int i = 0; i < words.count(); ++i) {
        QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(this);
        QLabel *label = new QLabel(words.at(i));
        label->setFrameStyle(QFrame::Box | QFrame::Plain);
        proxy->setWidget(label);
        lay->addItem(proxy);
    }
    setLayout(lay);
}