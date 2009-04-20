#include <QtGui>
#include <QtWebKit>

class PannableWebView : public QWebView
{
public:
    PannableWebView(QWidget *parent = 0)
        : QWebView(parent)
    {
#if 0
        QPushButton *btn = new QPushButton("Some test button", this);
        btn->resize(300, 200);
        btn->move(40, 300);
#endif
        grabGesture(Qt::PanGesture);
    }
protected:
    bool event(QEvent *event)
    {
        if (event->type() == QEvent::Gesture)
        {
            QGestureEvent *ev = static_cast<QGestureEvent*>(event);
            if (const QGesture *g = ev->gesture(Qt::PanGesture)) {
                if (QWebFrame *frame = page()->mainFrame()) {
                    QPoint offset = g->pos() - g->lastPos();
                    frame->setScrollPosition(frame->scrollPosition() - offset);
                }
                event->accept();
            }
            return true;
        }
        return QWebView::event(event);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QGraphicsScene scene;
    QGraphicsView w(&scene);
    
    QWebView *wv = new PannableWebView;
    wv->resize(480, 800);
    wv->setUrl(QUrl("http://www.trolltech.com"));
    scene.addWidget(wv);
    w.show();
    
    return app.exec();
}
