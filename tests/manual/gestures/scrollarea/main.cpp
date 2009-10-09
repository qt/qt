#include <QtGui>

#include "mousepangesturerecognizer.h"

class ScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    ScrollArea(QWidget *parent = 0)
        : QScrollArea(parent), outside(false)
    {
        viewport()->grabGesture(Qt::PanGesture);
    }

protected:
    bool viewportEvent(QEvent *event)
    {
        if (event->type() == QEvent::Gesture) {
            gestureEvent(static_cast<QGestureEvent *>(event));
            return true;
        } else if (event->type() == QEvent::GestureOverride) {
            QGestureEvent *ge = static_cast<QGestureEvent *>(event);
            if (QPanGesture *pan = static_cast<QPanGesture *>(ge->gesture(Qt::PanGesture)))
                if (pan->state() == Qt::GestureStarted) {
                    outside = false;
                }
        }
        return QScrollArea::viewportEvent(event);
    }
    void gestureEvent(QGestureEvent *event)
    {
        QPanGesture *pan = static_cast<QPanGesture *>(event->gesture(Qt::PanGesture));
        if (pan) {
            switch(pan->state()) {
            case Qt::GestureStarted: qDebug("area: Pan: started"); break;
            case Qt::GestureFinished: qDebug("area: Pan: finished"); break;
            case Qt::GestureCanceled: qDebug("area: Pan: canceled"); break;
            case Qt::GestureUpdated: break;
            default: qDebug("area: Pan: <unknown state>"); break;
            }

            if (pan->state() == Qt::GestureStarted)
                outside = false;
            event->ignore();
            event->ignore(pan);
            if (outside)
                return;

            const QSizeF offset = pan->offset();
            const QSizeF totalOffset = pan->totalOffset();
            QScrollBar *vbar = verticalScrollBar();
            QScrollBar *hbar = horizontalScrollBar();

            if ((vbar->value() == vbar->minimum() && totalOffset.height() > 10) ||
                (vbar->value() == vbar->maximum() && totalOffset.height() < -10)) {
                outside = true;
                return;
            }
            if ((hbar->value() == hbar->minimum() && totalOffset.width() > 10) ||
                (hbar->value() == hbar->maximum() && totalOffset.width() < -10)) {
                outside = true;
                return;
            }
            vbar->setValue(vbar->value() - offset.height());
            hbar->setValue(hbar->value() - offset.width());
            event->accept(pan);
        }
    }

private:
    bool outside;
};

class Slider : public QSlider
{
public:
    Slider(Qt::Orientation orientation, QWidget *parent = 0)
        : QSlider(orientation, parent)
    {
        grabGesture(Qt::PanGesture);
    }
protected:
    bool event(QEvent *event)
    {
        if (event->type() == QEvent::Gesture) {
            gestureEvent(static_cast<QGestureEvent *>(event));
            return true;
        }
        return QSlider::event(event);
    }
    void gestureEvent(QGestureEvent *event)
    {
        QPanGesture *pan = static_cast<QPanGesture *>(event->gesture(Qt::PanGesture));
        if (pan) {
            switch (pan->state()) {
            case Qt::GestureStarted: qDebug("slider: Pan: started"); break;
            case Qt::GestureFinished: qDebug("slider: Pan: finished"); break;
            case Qt::GestureCanceled: qDebug("slider: Pan: canceled"); break;
            case Qt::GestureUpdated: break;
            default: qDebug("slider: Pan: <unknown state>"); break;
            }

            if (pan->state() == Qt::GestureStarted)
                outside = false;
            event->ignore();
            event->ignore(pan);
            if (outside)
                return;
            const QSizeF offset = pan->offset();
            const QSizeF totalOffset = pan->totalOffset();
            if (orientation() == Qt::Horizontal) {
                if ((value() == minimum() && totalOffset.width() < -10) ||
                    (value() == maximum() && totalOffset.width() > 10)) {
                    outside = true;
                    return;
                }
                if (totalOffset.height() < 40 && totalOffset.height() > -40) {
                    setValue(value() + offset.width());
                    event->accept(pan);
                } else {
                    outside = true;
                }
            } else if (orientation() == Qt::Vertical) {
                if ((value() == maximum() && totalOffset.height() < -10) ||
                    (value() == minimum() && totalOffset.height() > 10)) {
                    outside = true;
                    return;
                }
                if (totalOffset.width() < 40 && totalOffset.width() > -40) {
                    setValue(value() - offset.height());
                    event->accept(pan);
                } else {
                    outside = true;
                }
            }
        }
    }
private:
    bool outside;
};

class MainWindow : public QMainWindow
{
public:
    MainWindow()
    {
        rootScrollArea = new ScrollArea;
        setCentralWidget(rootScrollArea);

        QWidget *root = new QWidget;
        root->setFixedSize(3000, 3000);
        rootScrollArea->setWidget(root);

        Slider *verticalSlider = new Slider(Qt::Vertical, root);
        verticalSlider ->move(650, 1100);
        Slider *horizontalSlider = new Slider(Qt::Horizontal, root);
        horizontalSlider ->move(600, 1000);

        childScrollArea = new ScrollArea(root);
        childScrollArea->move(500, 500);
        QWidget *w = new QWidget;
        w->setMinimumWidth(400);
        QVBoxLayout *l = new QVBoxLayout(w);
        l->setMargin(20);
        for (int i = 0; i < 100; ++i) {
            QWidget *w = new QWidget;
            QHBoxLayout *ll = new QHBoxLayout(w);
            ll->addWidget(new QLabel(QString("Label %1").arg(i)));
            ll->addWidget(new QPushButton(QString("Button %1").arg(i)));
            l->addWidget(w);
        }
        childScrollArea->setWidget(w);
    }
private:
    ScrollArea *rootScrollArea;
    ScrollArea *childScrollArea;
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.registerGestureRecognizer(new MousePanGestureRecognizer);
    MainWindow w;
    w.show();
    return app.exec();
}

#include "main.moc"
