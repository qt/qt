#include <QApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QTabletEvent>
#include <QWidget>

class EventReportWidget : public QWidget
{
public:
    EventReportWidget();
protected:
    void mouseDoubleClickEvent(QMouseEvent *event) { outputMouseEvent(event); }
    void mouseMoveEvent(QMouseEvent *event) { outputMouseEvent(event); }
    void mousePressEvent(QMouseEvent *event) { outputMouseEvent(event); }
    void mouseReleaseEvent(QMouseEvent *event) { outputMouseEvent(event); }

    void tabletEvent(QTabletEvent *);

private:
    void outputMouseEvent(QMouseEvent *event);

    bool m_lastIsMouseMove;
    bool m_lastIsTabletMove;
};

EventReportWidget::EventReportWidget()
    : m_lastIsMouseMove(false)
    , m_lastIsTabletMove(false)
{ }

void EventReportWidget::tabletEvent(QTabletEvent *event)
{
    QWidget::tabletEvent(event);

    QString type;
    switch (event->type()) {
    case QEvent::TabletEnterProximity:
        m_lastIsTabletMove = false;
        type = QString::fromLatin1("TabletEnterProximity");
        break;
    case QEvent::TabletLeaveProximity:
        m_lastIsTabletMove = false;
        type = QString::fromLatin1("TabletLeaveProximity");
        break;
    case QEvent::TabletMove:
        if (m_lastIsTabletMove)
            return;

        m_lastIsTabletMove = true;
        type = QString::fromLatin1("TabletMove");
        break;
    case QEvent::TabletPress:
        m_lastIsTabletMove = false;
        type = QString::fromLatin1("TabletPress");
        break;
    case QEvent::TabletRelease:
        m_lastIsTabletMove = false;
        type = QString::fromLatin1("TabletRelease");
        break;
    default:
        Q_ASSERT(false);
        break;
    }

    qDebug() << "Tablet event, type = " << type
             << " position = " << event->pos()
             << " global position = " << event->globalPos();
}

void EventReportWidget::outputMouseEvent(QMouseEvent *event)
{
    QString type;
    switch (event->type()) {
    case QEvent::MouseButtonDblClick:
        m_lastIsMouseMove = false;
        type = QString::fromLatin1("MouseButtonDblClick");
        break;
    case QEvent::MouseButtonPress:
        m_lastIsMouseMove = false;
        type = QString::fromLatin1("MouseButtonPress");
        break;
    case QEvent::MouseButtonRelease:
        m_lastIsMouseMove = false;
        type = QString::fromLatin1("MouseButtonRelease");
        break;
    case QEvent::MouseMove:
        if (m_lastIsMouseMove)
            return; // only show one move to keep things readable

        m_lastIsMouseMove = true;
        type = QString::fromLatin1("MouseMove");
        break;
    default:
        Q_ASSERT(false);
        break;
    }

    qDebug() << "Mouse event, type = " << type
             << " position = " << event->pos()
             << " global position = " << event->globalPos();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    EventReportWidget widget;
    widget.show();
    return app.exec();
}
