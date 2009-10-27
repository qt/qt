#include "canvasframerate.h"
#include <QtGui/qwidget.h>
#include <QtGui/qpainter.h>
#include <QtGui/qscrollbar.h>
#include <QtDeclarative/qmldebugclient.h>
#include <QtCore/qdebug.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qdatastream.h>
#include <QtGui/qboxlayout.h>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTabWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>

QT_BEGIN_NAMESPACE

class QLineGraph : public QWidget
{
Q_OBJECT
public:
    QLineGraph(QWidget * = 0);

    void setPosition(int);

public slots:
    void addSample(int, int, int, bool);
    void setResolutionForHeight(int);

protected:
    virtual void paintEvent(QPaintEvent *);

private slots:
    void scrollbarChanged(int);

private:
    void updateScrollbar();
    void drawSample(QPainter *, int, const QRect &);
    void drawTime(QPainter *, const QRect &);
    struct Sample { 
        int sample[3];
        bool isBreak;
    };
    QList<Sample> _samples;

    QScrollBar sb;
    int position;
    int samplesPerWidth;
    int resolutionForHeight;
    bool ignoreScroll;
};

QLineGraph::QLineGraph(QWidget *parent)
: QWidget(parent), sb(Qt::Horizontal, this), position(-1), samplesPerWidth(99), resolutionForHeight(50), ignoreScroll(false)
{
    setMinimumHeight(180);

    sb.setMaximum(0);
    sb.setMinimum(0);
    sb.setSingleStep(1);

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);
    layout->addStretch(2);
    layout->addWidget(&sb);
    QObject::connect(&sb, SIGNAL(valueChanged(int)), this, SLOT(scrollbarChanged(int)));
}

void QLineGraph::scrollbarChanged(int v)
{
    if(ignoreScroll)
        return;

    if (v == sb.maximum())
        position = -1;
    else
        position = v;
    update();
}

void QLineGraph::updateScrollbar()
{
    ignoreScroll = true;
    sb.setMaximum(qMax(0, _samples.count() - samplesPerWidth - 1));

    if(position == -1) {
        sb.setValue(sb.maximum());
    } else {
        sb.setValue(position);
    }
    ignoreScroll = false;
}

void QLineGraph::addSample(int a, int b, int d, bool isBreak)
{
    Sample s;
    s.isBreak = isBreak;
    s.sample[0] = a;
    s.sample[1] = b;
    s.sample[2] = d;
    _samples << s;
    updateScrollbar();
    update();
}

void QLineGraph::setPosition(int p)
{
    scrollbarChanged(p);
}

void QLineGraph::drawTime(QPainter *p, const QRect &rect)
{
    if(_samples.isEmpty())
        return;

    int first = position;
    if(first == -1)
        first = qMax(0, _samples.count() - samplesPerWidth - 1);
    int last = qMin(_samples.count() - 1, first + samplesPerWidth);

    qreal scaleX = qreal(rect.width()) / qreal(samplesPerWidth);

    int t = 0;

    for(int ii = first; ii <= last; ++ii) {
        int sampleTime = _samples.at(ii).sample[2] / 1000;
        if(sampleTime != t) {

            int xEnd = rect.left() + scaleX * (ii - first);
            p->drawLine(xEnd, rect.bottom(), xEnd, rect.bottom() + 7);

            QRect text(xEnd - 30, rect.bottom() + 10, 60, 30);

            p->drawText(text, Qt::AlignHCenter | Qt::AlignTop, QString::number(_samples.at(ii).sample[2]));

            t = sampleTime;
        }
    }

}

void QLineGraph::drawSample(QPainter *p, int s, const QRect &rect)
{
    if(_samples.isEmpty())
        return;

    int first = position;
    if(first == -1)
        first = qMax(0, _samples.count() - samplesPerWidth - 1);
    int last = qMin(_samples.count() - 1, first + samplesPerWidth);

    qreal scaleY = qreal(rect.height()) / resolutionForHeight;
    qreal scaleX = qreal(rect.width()) / qreal(samplesPerWidth);

    int xEnd;
    int lastXEnd = rect.left();

    p->save();
    p->setPen(Qt::NoPen);
    for(int ii = first + 1; ii <= last; ++ii) {

        xEnd = rect.left() + scaleX * (ii - first);
        int yEnd = rect.bottom() - _samples.at(ii).sample[s] * scaleY;

        if (!(s == 0 && _samples.at(ii).isBreak)) 
            p->drawRect(QRect(lastXEnd, yEnd, scaleX, _samples.at(ii).sample[s] * scaleY));

        lastXEnd = xEnd;
    }
    p->restore();
}

void QLineGraph::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRect r(50, 10, width() - 60, height() - 60);
    p.setBrush(QColor("lightsteelblue"));
    drawSample(&p, 0, r);

    p.setBrush(QColor("pink"));
    drawSample(&p, 1, r);

    p.setBrush(Qt::NoBrush);
    p.drawRect(r);

    for(int ii = 0; ii <= resolutionForHeight; ++ii) {
        int y = 1 + r.bottom() - ii * r.height() / resolutionForHeight;

        if((ii % 10) == 0) {
            p.drawLine(r.left() - 20, y, r.left(), y);
            QRect text(r.left() - 20 - 53, y - 10, 50, 20);
            p.drawText(text, Qt::AlignRight | Qt::AlignVCenter, QString::number(ii));
        } else {
            p.drawLine(r.left() - 7, y, r.left(), y);
        }
    }

    drawTime(&p, r);
}

void QLineGraph::setResolutionForHeight(int resolution)
{
    resolutionForHeight = resolution;
    update();
}

class CanvasFrameRatePlugin : public QmlDebugClient
{
Q_OBJECT
public:
    CanvasFrameRatePlugin(QmlDebugConnection *client);

signals:
    void sample(int, int, int, bool);

protected:
    virtual void messageReceived(const QByteArray &);

private:
    int lb;
    int ld;
};

CanvasFrameRatePlugin::CanvasFrameRatePlugin(QmlDebugConnection *client)
: QmlDebugClient(QLatin1String("CanvasFrameRate"), client), lb(-1)
{
}

void CanvasFrameRatePlugin::messageReceived(const QByteArray &data)
{
    QByteArray rwData = data;
    QDataStream stream(&rwData, QIODevice::ReadOnly);

    int b; int c; int d; bool isBreak;
    stream >> b >> c >> d >> isBreak;

    if (lb != -1)
        emit sample(c, lb, ld, isBreak);

    lb = b;
    ld = d;
}

CanvasFrameRate::CanvasFrameRate(QWidget *parent)
: QWidget(parent),
  m_plugin(0)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    setLayout(layout);

    m_tabs = new QTabWidget(this);
    layout->addWidget(m_tabs);

    QHBoxLayout *bottom = new QHBoxLayout;
    bottom->setContentsMargins(5, 0, 5, 0);
    bottom->setSpacing(10);
    layout->addLayout(bottom);

    QLabel *label = new QLabel("Resolution", this);
    bottom->addWidget(label);

    m_spin = new QSpinBox(this);
    m_spin->setRange(50,200);
    m_spin->setValue(50);
    m_spin->setSuffix("ms");
    bottom->addWidget(m_spin);

    bottom->addStretch(2);

    m_enabledCheckBox = new QCheckBox("Enable", this);
    bottom->addWidget(m_enabledCheckBox);
    QObject::connect(m_enabledCheckBox, SIGNAL(stateChanged(int)), 
                     this, SLOT(enabledStateChanged(int)));

    QPushButton *pb = new QPushButton(tr("New Tab"), this);
    QObject::connect(pb, SIGNAL(clicked()), this, SLOT(newTab()));
    bottom->addWidget(pb);
}

void CanvasFrameRate::reset(QmlDebugConnection *conn)
{
    delete m_plugin;
    m_plugin = 0;

    QWidget *w;
    for (int i=0; i<m_tabs->count(); i++) {
        w = m_tabs->widget(i);
        m_tabs->removeTab(i);
        delete w;
    }

    if (conn) {
        connect(conn, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                SLOT(connectionStateChanged(QAbstractSocket::SocketState)));
        if (conn->state() == QAbstractSocket::ConnectedState)
            handleConnected(conn);
    }
}

void CanvasFrameRate::connectionStateChanged(QAbstractSocket::SocketState state)
{
    if (state == QAbstractSocket::UnconnectedState) {
        delete m_plugin;
        m_plugin = 0;
    } else if (state == QAbstractSocket::ConnectedState) {
        handleConnected(qobject_cast<QmlDebugConnection*>(sender()));
    }        
}

void CanvasFrameRate::handleConnected(QmlDebugConnection *conn)
{
    delete m_plugin;
    m_plugin = new CanvasFrameRatePlugin(conn);
    enabledStateChanged(m_enabledCheckBox->checkState());
    newTab();
}

void CanvasFrameRate::setSizeHint(const QSize &size)
{
    m_sizeHint = size;
}

QSize CanvasFrameRate::sizeHint() const
{
    return m_sizeHint;
}

void CanvasFrameRate::newTab()
{
    if (!m_plugin)
        return;

    if (m_tabs->count()) {
        QWidget *w = m_tabs->widget(m_tabs->count() - 1);
        QObject::disconnect(m_plugin, SIGNAL(sample(int,int,int,bool)),
                            w, SLOT(addSample(int,int,int,bool)));
    }

    int id = m_tabs->count();

    QLineGraph *graph = new QLineGraph(this);
    QObject::connect(m_plugin, SIGNAL(sample(int,int,int,bool)),
                     graph, SLOT(addSample(int,int,int,bool)));
    QObject::connect(m_spin, SIGNAL(valueChanged(int)), graph, SLOT(setResolutionForHeight(int)));

    QString name = QLatin1String("Graph ") + QString::number(id);
    m_tabs->addTab(graph, name);
    m_tabs->setCurrentIndex(id);
}

void CanvasFrameRate::enabledStateChanged(int s)
{
    bool checked = s != 0;

    if (m_plugin)
        static_cast<QmlDebugClient *>(m_plugin)->setEnabled(checked);
}

QT_END_NAMESPACE

#include "canvasframerate.moc"
