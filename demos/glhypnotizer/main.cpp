/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtOpenGL>
#include <QtSvg>

#define NUM_SWIRLY_ITEMS 10

class GLPainter : public QObject
{
    Q_OBJECT
public:
    GLPainter(QGLWidget *widget);
    void stop();
    void resizeViewport(const QSize &size);

public slots:
    void start();

protected:
    void timerEvent(QTimerEvent *event);
    void paint();
    void initSwirlyItems();
    void updateSwirlyItems();
    void drawSuggestiveMessages(QPainter *p);

private:
    QMutex mutex;
    QGLWidget *glWidget;
    int viewportWidth;
    int viewportHeight;
    bool doRendering;
    qreal rotationAngle;
    bool swirlClockwise;
    QSvgRenderer svgRenderer;
    QPixmap logo;
    QPainter::PixmapFragment swirlyItems[NUM_SWIRLY_ITEMS];
    int swirlyCounter;
    int textCounter;
    int messageYPos;
    qreal scaleFactor;
};


GLPainter::GLPainter(QGLWidget *widget)
    : glWidget(widget)
    , doRendering(true)
    , rotationAngle(rand() % 360)
    , swirlClockwise((rand() % 2) == 1)
    , svgRenderer(QLatin1String(":/spiral.svg"), this)
    , logo(QLatin1String(":/qt-logo.png"))
{
}

void GLPainter::start()
{
    glWidget->makeCurrent();
    startTimer(20);
}

void GLPainter::stop()
{
    QMutexLocker locker(&mutex);
    doRendering = false;
}

void GLPainter::resizeViewport(const QSize &size)
{
    QMutexLocker locker(&mutex);
    viewportWidth = size.width();
    viewportHeight = size.height();
    initSwirlyItems();
    textCounter = 0;
    messageYPos = -1;
}

void GLPainter::timerEvent(QTimerEvent *event)
{
    QMutexLocker locker(&mutex);
    if (!doRendering) {
        killTimer(event->timerId());
        QThread::currentThread()->quit();
        return;
    }
    updateSwirlyItems();
    paint();
}

void GLPainter::paint()
{
    QPainter p(glWidget);
    p.fillRect(QRect(0, 0, viewportWidth, viewportHeight), Qt::white);
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    p.translate(viewportWidth / 2, viewportHeight / 2);
    p.rotate(rotationAngle * (swirlClockwise ? 1 : -1));
    p.scale(svgRenderer.viewBox().width() / 200.0 * 0.65,
            svgRenderer.viewBox().height() / 200.0 * 0.65);
    p.translate(-viewportWidth / 2, -viewportHeight / 2);
    svgRenderer.render(&p);
    p.resetTransform();
    p.drawPixmapFragments(swirlyItems, NUM_SWIRLY_ITEMS, logo);
    drawSuggestiveMessages(&p);
    p.end();
    rotationAngle += 1.2;
}

void GLPainter::drawSuggestiveMessages(QPainter *p)
{
    const int numSuggestiveMessages = 7;
    const char *texts[numSuggestiveMessages] = {" You feel relaxed.. ",
                                                " Let your mind wander.. ",
                                                " Look deep into the swirls.. ",
                                                " Even deeper.. ",
                                                " Qt is good! ",
                                                " Qt is good for you! ",
                                                " You MUST use Qt! "};
    QFont font(p->font());
    font.setPointSizeF(font.pointSizeF() * viewportWidth/200.0);
    p->setFont(font);
    QFontMetrics fm(font);
    int messageNo = textCounter/314;
    if (messageNo > 6 || messageNo < 0) {
        qFatal("This should not happen: %d - %d", messageNo, textCounter);
    }
    QLatin1String text(texts[textCounter / 314]);
    qreal textWidth = fm.width(text);
    int alpha = 255 * qAbs(qSin(textCounter * 0.01));
    if (messageYPos < 0 || (textCounter % 314 == 0)) {
        messageYPos = qBound(fm.height(), rand() % viewportHeight,
                             viewportHeight - fm.height());
    }
    p->setPen(QColor(255, 255, 255, alpha));
    p->setBackground(QColor(50, 50, 50, qBound(0, alpha, 50)));
    p->setBackgroundMode(Qt::OpaqueMode);
    p->drawText((viewportWidth / 2) - (textWidth/ 2), messageYPos, text);
    ++textCounter;
    if (textCounter >= (314 * numSuggestiveMessages))
        textCounter = 0;
}

void GLPainter::initSwirlyItems()
{
    swirlyCounter = swirlClockwise ? 0 : 360;
    scaleFactor = viewportWidth / 200.0;
    QRectF logoRect(0, 0, logo.width(), logo.height());

    for (int i=0; i<NUM_SWIRLY_ITEMS; ++i) {
        if (swirlClockwise) {
            swirlyItems[i] = QPainter::PixmapFragment::create(QPointF(0, 0), logoRect,
                                                              scaleFactor,
                                                              scaleFactor);
        } else {
            swirlyItems[i] = QPainter::PixmapFragment::create(QPointF(0, 0), logoRect,
                                                              scaleFactor * 1 / 360.0,
                                                              scaleFactor * 1 / 360.0);
        }
    }
}

void GLPainter::updateSwirlyItems()
{
    swirlyCounter += swirlClockwise ? 1 : -1;

    for (int i=0; i<NUM_SWIRLY_ITEMS; ++i) {
        int factor;
        factor = swirlClockwise ? (swirlyCounter - i * 20) : (swirlyCounter + i * 20);
        if (factor > 0 && factor <= 360) {
            swirlyItems[i].x = viewportWidth / 2.0 + qSin(factor * 0.05) * (viewportWidth / 2.0) * (100.0 / factor);
            swirlyItems[i].y = viewportHeight / 2.0 + qCos(factor * 0.05) * (viewportHeight / 2.0) * (100.0 / factor);
            swirlyItems[i].rotation += -swirlyCounter * 0.01;
            swirlyItems[i].scaleX += swirlClockwise ? -scaleFactor * 1 / 360.0 : scaleFactor * 1 / 360.0;
            if (swirlClockwise) {
                if (swirlyItems[i].scaleX < 0)
                    swirlyItems[i].scaleX = scaleFactor;
            } else {
                if (swirlyItems[i].scaleX > scaleFactor)
                    swirlyItems[i].scaleX = scaleFactor * 1 / 360.0;
            }
            swirlyItems[i].scaleY = swirlyItems[i].scaleX;
        } else {
            swirlyItems[i].scaleX = swirlyItems[i].scaleY = 0;
        }
    }
    if (swirlClockwise) {
        if (swirlyCounter > (360 + NUM_SWIRLY_ITEMS * 20))
            swirlyCounter = 0;
    } else {
        if (swirlyCounter < -NUM_SWIRLY_ITEMS * 20)
            swirlyCounter = 360;
    }
}

class GLWidget : public QGLWidget
{
public:
    GLWidget(QWidget *parent, QGLWidget *shareWidget = 0);
    ~GLWidget();
    void startRendering();
    void stopRendering();

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
    QSize sizeHint() const { return QSize(200, 200); }

private:
    GLPainter glPainter;
    QThread glThread;
};

GLWidget::GLWidget(QWidget *parent, QGLWidget *share)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent, share)
    , glPainter(this)
    , glThread(this)
{
    setAttribute(Qt::WA_PaintOutsidePaintEvent);
    setAttribute(Qt::WA_DeleteOnClose);
}

GLWidget::~GLWidget()
{
    stopRendering();
}

void GLWidget::startRendering()
{
    glPainter.moveToThread(&glThread);
    connect(&glThread, SIGNAL(started()), &glPainter, SLOT(start()));
    glThread.start();
}

void GLWidget::stopRendering()
{
    glPainter.stop();
    glThread.wait();
}

void GLWidget::resizeEvent(QResizeEvent *event)
{
    glPainter.resizeViewport(event->size());
}

void GLWidget::paintEvent(QPaintEvent *)
{
    // Handled by GLPainter.
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

private slots:
    void newThread();
    void killThread();

private:
    QMdiArea mdiArea;
    QGLWidget shareWidget;
};

MainWindow::MainWindow()
    : QMainWindow(0)
    , mdiArea(this)
    , shareWidget(this)
{
    setWindowTitle("Qt GL Hypnotizer");
    QMenu *menu = menuBar()->addMenu("&Hypnotizers");
    menu->addAction("&New hypnotizer thread", this, SLOT(newThread()), QKeySequence("Ctrl+N"));
    menu->addAction("&End current hypnotizer thread", this, SLOT(killThread()), QKeySequence("Ctrl+K"));
    menu->addSeparator();
    menu->addAction("E&xit", qApp, SLOT(quit()), QKeySequence("Ctrl+Q"));

    setCentralWidget(&mdiArea);
    shareWidget.resize(1, 1);
    newThread();
}

void MainWindow::newThread()
{
    static int windowCount = 1;
    if (mdiArea.subWindowList().count() > 9)
        return;
    GLWidget *widget = new GLWidget(&mdiArea, &shareWidget);
    mdiArea.addSubWindow(widget);
    widget->setWindowTitle("Thread #" + QString::number(windowCount++));
    widget->show();
    widget->startRendering();
}

void MainWindow::killThread()
{
    delete mdiArea.activeSubWindow();
}

int main(int argc, char *argv[])
{
    // Make Xlib and GLX thread safe under X11
    QApplication::setAttribute(Qt::AA_X11InitThreads);
    QApplication application(argc, argv);

    // Using QPainter to draw into QGLWidgets is only supported with GL 2.0
    if (!((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_0) ||
          (QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Version_2_0))) {
        QMessageBox::information(0, "Qt GL Hypnotizer",
                                 "This system does not support OpenGL 2.0 or OpenGL ES 2.0, "
                                 "which is required for this example to work.");
        return 0;
    }

    MainWindow mainWindow;
    mainWindow.show();
    return application.exec();
}

#include "main.moc"
