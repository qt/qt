#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QTcpSocket>
#include <QScrollBar>

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
    virtual void mousePressEvent(QMouseEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void showEvent(QShowEvent *);

private slots:
    void scrollbarChanged(int);

private:
    void positionScrollbar();
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

    QObject::connect(&sb, SIGNAL(valueChanged(int)), this, SLOT(scrollbarChanged(int)));
}

void QLineGraph::scrollbarChanged(int v)
{
    if(ignoreScroll)
        return;

    position = v;
    update();
}

void QLineGraph::positionScrollbar()
{
    sb.setFixedWidth(width());
    sb.move(0, height() - sb.height());
}

void QLineGraph::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    positionScrollbar();
}

void QLineGraph::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    positionScrollbar();
}

void QLineGraph::mousePressEvent(QMouseEvent *)
{
    if(position == -1) {
        position = qMax(0, _samples.count() - samplesPerWidth - 1);
    } else {
        position = -1;
    }
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

class MyReader : public QObject
{
Q_OBJECT
public:
    MyReader(const QString &host, int port);

signals:
    void sample(int, int, int, int, bool);

private slots:
    void readyRead();

private:
    QTcpSocket *socket;

    int la;
    int lb;
    int ld;
};

MyReader::MyReader(const QString &host, int port)
: socket(0), la(-1)
{
    socket = new QTcpSocket(this);
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    socket->connectToHost(host, port);
    socket->waitForConnected();
}

void MyReader::readyRead()
{
    static int la = -1;
    static int lb;
    static int ld;

    if(socket->canReadLine()) {
        QString line = socket->readLine();

        int a;
        int b;
        int c;
        int d;
        int isBreak;
        sscanf(line.toLatin1().constData(), "%d %d %d %d %d", &a, &b, &c, &d, &isBreak);

        if (la != -1) 
            emit sample(c, lb, la, ld, isBreak);

        la = a;
        lb = b;
        ld = d;
    }
}

int main(int argc, char ** argv)
{
    if(argc != 3) {
        qWarning() << "Usage:" << argv[0] << "host port";
        return -1;
    }

    QApplication app(argc, argv);

    MyReader reader(argv[1], atoi(argv[2]));

    QLineGraph graph;
    QObject::connect(&reader, SIGNAL(sample(int,int,int,int,bool)), &graph, SLOT(addSample(int,int,int,int,bool)));
    graph.setFixedSize(800, 600);
    graph.show();

    return app.exec();
}

#include "main.moc"
