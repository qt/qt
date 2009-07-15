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

QT_BEGIN_NAMESPACE

class QLineGraph : public QWidget
{
Q_OBJECT
public:
    QLineGraph(QWidget * = 0);

    void setPosition(int);

public slots:
    void addSample(int, int, int, int, bool);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual QSize sizeHint() const;

private slots:
    void scrollbarChanged(int);

private:
    void updateScrollbar();
    void drawSample(QPainter *, int, const QRect &);
    void drawTime(QPainter *, const QRect &);
    struct Sample { 
        int sample[4];
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
    sb.setMaximum(0);
    sb.setMinimum(0);
    sb.setSingleStep(1);

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);
    layout->addStretch(2);
    layout->addWidget(&sb);
    QObject::connect(&sb, SIGNAL(valueChanged(int)), this, SLOT(scrollbarChanged(int)));
}

QSize QLineGraph::sizeHint() const
{
    return QSize(800, 600);
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

void QLineGraph::addSample(int a, int b, int c, int d, bool isBreak)
{
    Sample s;
    s.isBreak = isBreak;
    s.sample[0] = a;
    s.sample[1] = b;
    s.sample[2] = c;
    s.sample[3] = d;
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
        int sampleTime = _samples.at(ii).sample[3] / 1000;
        if(sampleTime != t) {

            int xEnd = rect.left() + scaleX * (ii - first);
            p->drawLine(xEnd, rect.bottom(), xEnd, rect.bottom() + 7);

            QRect text(xEnd - 30, rect.bottom() + 10, 60, 30);

            p->drawText(text, Qt::AlignHCenter | Qt::AlignTop, QString::number(_samples.at(ii).sample[3]));

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

    qreal scaleY = rect.height() / resolutionForHeight;
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

    p.setBrush(QColor("green"));
    drawSample(&p, 2, r);

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

class CanvasFrameRatePlugin : public QmlDebugClient
{
Q_OBJECT
public:
    CanvasFrameRatePlugin(QmlDebugConnection *client);

signals:
    void sample(int, int, int, int, bool);

protected:
    virtual void messageReceived(const QByteArray &);

private:
    int la;
    int lb;
    int ld;
};

CanvasFrameRatePlugin::CanvasFrameRatePlugin(QmlDebugConnection *client)
: QmlDebugClient(QLatin1String("CanvasFrameRate"), client), la(-1)
{
}

void CanvasFrameRatePlugin::messageReceived(const QByteArray &data)
{
    QByteArray rwData = data;
    QDataStream stream(&rwData, QIODevice::ReadOnly);

    int a; int b; int c; int d; bool isBreak;
    stream >> a >> b >> c >> d >> isBreak;

    if (la != -1) 
        emit sample(c, lb, la, ld, isBreak);

    la = a;
    lb = b;
    ld = d;
}

CanvasFrameRate::CanvasFrameRate(QmlDebugConnection *client, QWidget *parent)
: QWidget(parent)
{
    m_plugin = new CanvasFrameRatePlugin(client);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    setLayout(layout);

    m_tabs = new QTabWidget(this);
    layout->addWidget(m_tabs);

    QHBoxLayout *bottom = new QHBoxLayout;
    layout->addLayout(bottom);
    bottom->addStretch(2);

    QCheckBox *check = new QCheckBox("Enable", this);
    bottom->addWidget(check);
    QObject::connect(check, SIGNAL(stateChanged(int)), 
                     this, SLOT(stateChanged(int)));

    QPushButton *pb = new QPushButton(tr("New Tab"), this);
    QObject::connect(pb, SIGNAL(clicked()), this, SLOT(newTab()));
    bottom->addWidget(pb);

    newTab();
}

void CanvasFrameRate::newTab()
{
    if (m_tabs->count()) {
        QWidget *w = m_tabs->widget(m_tabs->count() - 1);
        QObject::disconnect(m_plugin, SIGNAL(sample(int,int,int,int,bool)), 
                            w, SLOT(addSample(int,int,int,int,bool)));
    }

    int id = m_tabs->count();

    QLineGraph *graph = new QLineGraph(this);
    QObject::connect(m_plugin, SIGNAL(sample(int,int,int,int,bool)), 
                     graph, SLOT(addSample(int,int,int,int,bool)));

    QString name = QLatin1String("Graph ") + QString::number(id);
    m_tabs->addTab(graph, name);
    m_tabs->setCurrentIndex(id);
}

void CanvasFrameRate::stateChanged(int s)
{
    bool checked = s != 0;

    static_cast<QmlDebugClient *>(m_plugin)->setEnabled(checked);
}

QT_END_NAMESPACE

#include "canvasframerate.moc"
