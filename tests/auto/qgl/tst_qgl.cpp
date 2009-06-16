/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#include <qcoreapplication.h>
#include <qdebug.h>
#ifndef QT_NO_OPENGL
#include <qgl.h>
#endif

#include <QGraphicsView>
#include <QGraphicsProxyWidget>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QGL : public QObject
{
Q_OBJECT

public:
    tst_QGL();
    virtual ~tst_QGL();

private slots:
    void getSetCheck();
    void openGLVersionCheck();
    void graphicsViewClipping();
    void partialGLWidgetUpdates_data();
    void partialGLWidgetUpdates();
};

tst_QGL::tst_QGL()
{
}

tst_QGL::~tst_QGL()
{
}

#ifndef QT_NO_OPENGL
class MyGLContext : public QGLContext
{
public:
    MyGLContext(const QGLFormat& format) : QGLContext(format) {}
    bool windowCreated() const { return QGLContext::windowCreated(); }
    void setWindowCreated(bool on) { QGLContext::setWindowCreated(on); }
    bool initialized() const { return QGLContext::initialized(); }
    void setInitialized(bool on) { QGLContext::setInitialized(on); }
};

class MyGLWidget : public QGLWidget
{
public:
    MyGLWidget() : QGLWidget() {}
    bool autoBufferSwap() const { return QGLWidget::autoBufferSwap(); }
    void setAutoBufferSwap(bool on) { QGLWidget::setAutoBufferSwap(on); }
};
#endif
// Testing get/set functions
void tst_QGL::getSetCheck()
{
#ifdef QT_NO_OPENGL
    QSKIP("QGL not yet supported", SkipAll);
#else
    if (!QGLFormat::hasOpenGL())
        QSKIP("QGL not supported on this platform", SkipAll);

    QGLFormat obj1;
    // int QGLFormat::depthBufferSize()
    // void QGLFormat::setDepthBufferSize(int)
    obj1.setDepthBufferSize(0);
    QCOMPARE(0, obj1.depthBufferSize());
    obj1.setDepthBufferSize(INT_MIN);
    QCOMPARE(0, obj1.depthBufferSize()); // Makes no sense with a negative buffer size
    obj1.setDepthBufferSize(INT_MAX);
    QCOMPARE(INT_MAX, obj1.depthBufferSize());

    // int QGLFormat::accumBufferSize()
    // void QGLFormat::setAccumBufferSize(int)
    obj1.setAccumBufferSize(0);
    QCOMPARE(0, obj1.accumBufferSize());
    obj1.setAccumBufferSize(INT_MIN);
    QCOMPARE(0, obj1.accumBufferSize()); // Makes no sense with a negative buffer size
    obj1.setAccumBufferSize(INT_MAX);
    QCOMPARE(INT_MAX, obj1.accumBufferSize());

    // int QGLFormat::alphaBufferSize()
    // void QGLFormat::setAlphaBufferSize(int)
    obj1.setAlphaBufferSize(0);
    QCOMPARE(0, obj1.alphaBufferSize());
    obj1.setAlphaBufferSize(INT_MIN);
    QCOMPARE(0, obj1.alphaBufferSize()); // Makes no sense with a negative buffer size
    obj1.setAlphaBufferSize(INT_MAX);
    QCOMPARE(INT_MAX, obj1.alphaBufferSize());

    // int QGLFormat::stencilBufferSize()
    // void QGLFormat::setStencilBufferSize(int)
    obj1.setStencilBufferSize(0);
    QCOMPARE(0, obj1.stencilBufferSize());
    obj1.setStencilBufferSize(INT_MIN);
    QCOMPARE(0, obj1.stencilBufferSize()); // Makes no sense with a negative buffer size
    obj1.setStencilBufferSize(INT_MAX);
    QCOMPARE(INT_MAX, obj1.stencilBufferSize());

    // bool QGLFormat::sampleBuffers()
    // void QGLFormat::setSampleBuffers(bool)
    obj1.setSampleBuffers(false);
    QCOMPARE(false, obj1.sampleBuffers());
    obj1.setSampleBuffers(true);
    QCOMPARE(true, obj1.sampleBuffers());

    // int QGLFormat::samples()
    // void QGLFormat::setSamples(int)
    obj1.setSamples(0);
    QCOMPARE(0, obj1.samples());
    obj1.setSamples(INT_MIN);
    QCOMPARE(0, obj1.samples());  // Makes no sense with a negative sample size
    obj1.setSamples(INT_MAX);
    QCOMPARE(INT_MAX, obj1.samples());

    // bool QGLFormat::doubleBuffer()
    // void QGLFormat::setDoubleBuffer(bool)
    obj1.setDoubleBuffer(false);
    QCOMPARE(false, obj1.doubleBuffer());
    obj1.setDoubleBuffer(true);
    QCOMPARE(true, obj1.doubleBuffer());

    // bool QGLFormat::depth()
    // void QGLFormat::setDepth(bool)
    obj1.setDepth(false);
    QCOMPARE(false, obj1.depth());
    obj1.setDepth(true);
    QCOMPARE(true, obj1.depth());

    // bool QGLFormat::rgba()
    // void QGLFormat::setRgba(bool)
    obj1.setRgba(false);
    QCOMPARE(false, obj1.rgba());
    obj1.setRgba(true);
    QCOMPARE(true, obj1.rgba());

    // bool QGLFormat::alpha()
    // void QGLFormat::setAlpha(bool)
    obj1.setAlpha(false);
    QCOMPARE(false, obj1.alpha());
    obj1.setAlpha(true);
    QCOMPARE(true, obj1.alpha());

    // bool QGLFormat::accum()
    // void QGLFormat::setAccum(bool)
    obj1.setAccum(false);
    QCOMPARE(false, obj1.accum());
    obj1.setAccum(true);
    QCOMPARE(true, obj1.accum());

    // bool QGLFormat::stencil()
    // void QGLFormat::setStencil(bool)
    obj1.setStencil(false);
    QCOMPARE(false, obj1.stencil());
    obj1.setStencil(true);
    QCOMPARE(true, obj1.stencil());

    // bool QGLFormat::stereo()
    // void QGLFormat::setStereo(bool)
    obj1.setStereo(false);
    QCOMPARE(false, obj1.stereo());
    obj1.setStereo(true);
    QCOMPARE(true, obj1.stereo());

    // bool QGLFormat::directRendering()
    // void QGLFormat::setDirectRendering(bool)
    obj1.setDirectRendering(false);
    QCOMPARE(false, obj1.directRendering());
    obj1.setDirectRendering(true);
    QCOMPARE(true, obj1.directRendering());

    // int QGLFormat::plane()
    // void QGLFormat::setPlane(int)
    obj1.setPlane(0);
    QCOMPARE(0, obj1.plane());
    obj1.setPlane(INT_MIN);
    QCOMPARE(INT_MIN, obj1.plane());
    obj1.setPlane(INT_MAX);
    QCOMPARE(INT_MAX, obj1.plane());

    MyGLContext obj2(obj1);
    // bool QGLContext::windowCreated()
    // void QGLContext::setWindowCreated(bool)
    obj2.setWindowCreated(false);
    QCOMPARE(false, obj2.windowCreated());
    obj2.setWindowCreated(true);
    QCOMPARE(true, obj2.windowCreated());

    // bool QGLContext::initialized()
    // void QGLContext::setInitialized(bool)
    obj2.setInitialized(false);
    QCOMPARE(false, obj2.initialized());
    obj2.setInitialized(true);
    QCOMPARE(true, obj2.initialized());

    MyGLWidget obj3;
    // bool QGLWidget::autoBufferSwap()
    // void QGLWidget::setAutoBufferSwap(bool)
    obj3.setAutoBufferSwap(false);
    QCOMPARE(false, obj3.autoBufferSwap());
    obj3.setAutoBufferSwap(true);
    QCOMPARE(true, obj3.autoBufferSwap());
#endif
}

#ifndef QT_NO_OPENGL
QT_BEGIN_NAMESPACE
extern QGLFormat::OpenGLVersionFlags qOpenGLVersionFlagsFromString(const QString &versionString);
QT_END_NAMESPACE
#endif

void tst_QGL::openGLVersionCheck()
{
#ifdef QT_NO_OPENGL
    QSKIP("QGL not yet supported", SkipAll);
#else
    if (!QGLFormat::hasOpenGL())
        QSKIP("QGL not supported on this platform", SkipAll);

    QString versionString;
    QGLFormat::OpenGLVersionFlags expectedFlag;
    QGLFormat::OpenGLVersionFlags versionFlag;

    versionString = "1.1 Irix 6.5";
    expectedFlag = QGLFormat::OpenGL_Version_1_1;
    versionFlag = qOpenGLVersionFlagsFromString(versionString);
    QCOMPARE(versionFlag, expectedFlag);

    versionString = "1.2 Microsoft";
    expectedFlag = QGLFormat::OpenGL_Version_1_2 | QGLFormat::OpenGL_Version_1_1;
    versionFlag = qOpenGLVersionFlagsFromString(versionString);
    QCOMPARE(versionFlag, expectedFlag);

    versionString = "1.2.1";
    expectedFlag = QGLFormat::OpenGL_Version_1_2 | QGLFormat::OpenGL_Version_1_1;
    versionFlag = qOpenGLVersionFlagsFromString(versionString);
    QCOMPARE(versionFlag, expectedFlag);

    versionString = "1.3 NVIDIA";
    expectedFlag = QGLFormat::OpenGL_Version_1_3 | QGLFormat::OpenGL_Version_1_2 | QGLFormat::OpenGL_Version_1_1;
    versionFlag = qOpenGLVersionFlagsFromString(versionString);
    QCOMPARE(versionFlag, expectedFlag);

    versionString = "1.4";
    expectedFlag = QGLFormat::OpenGL_Version_1_4 | QGLFormat::OpenGL_Version_1_3 | QGLFormat::OpenGL_Version_1_2 | QGLFormat::OpenGL_Version_1_1;
    versionFlag = qOpenGLVersionFlagsFromString(versionString);
    QCOMPARE(versionFlag, expectedFlag);

    versionString = "1.5 NVIDIA";
    expectedFlag = QGLFormat::OpenGL_Version_1_5 | QGLFormat::OpenGL_Version_1_4 | QGLFormat::OpenGL_Version_1_3 | QGLFormat::OpenGL_Version_1_2 | QGLFormat::OpenGL_Version_1_1;
    versionFlag = qOpenGLVersionFlagsFromString(versionString);
    QCOMPARE(versionFlag, expectedFlag);

    versionString = "2.0.2 NVIDIA 87.62";
    expectedFlag = QGLFormat::OpenGL_Version_2_0 | QGLFormat::OpenGL_Version_1_5 | QGLFormat::OpenGL_Version_1_4 | QGLFormat::OpenGL_Version_1_3 | QGLFormat::OpenGL_Version_1_2 | QGLFormat::OpenGL_Version_1_1;
    versionFlag = qOpenGLVersionFlagsFromString(versionString);
    QCOMPARE(versionFlag, expectedFlag);

    versionString = "2.1 NVIDIA";
    expectedFlag = QGLFormat::OpenGL_Version_2_1 | QGLFormat::OpenGL_Version_2_0 | QGLFormat::OpenGL_Version_1_5 | QGLFormat::OpenGL_Version_1_4 | QGLFormat::OpenGL_Version_1_3 | QGLFormat::OpenGL_Version_1_2 | QGLFormat::OpenGL_Version_1_1;
    versionFlag = qOpenGLVersionFlagsFromString(versionString);
    QCOMPARE(versionFlag, expectedFlag);

    versionString = "2.1";
    expectedFlag = QGLFormat::OpenGL_Version_2_1 | QGLFormat::OpenGL_Version_2_0 | QGLFormat::OpenGL_Version_1_5 | QGLFormat::OpenGL_Version_1_4 | QGLFormat::OpenGL_Version_1_3 | QGLFormat::OpenGL_Version_1_2 | QGLFormat::OpenGL_Version_1_1;
    versionFlag = qOpenGLVersionFlagsFromString(versionString);
    QCOMPARE(versionFlag, expectedFlag);

    versionString = "OpenGL ES-CM 1.0 ATI";
    expectedFlag = QGLFormat::OpenGL_ES_Common_Version_1_0 | QGLFormat::OpenGL_ES_CommonLite_Version_1_0;
    versionFlag = qOpenGLVersionFlagsFromString(versionString);
    QCOMPARE(versionFlag, expectedFlag);

    versionString = "OpenGL ES-CL 1.0 ATI";
    expectedFlag = QGLFormat::OpenGL_ES_CommonLite_Version_1_0;
    versionFlag = qOpenGLVersionFlagsFromString(versionString);
    QCOMPARE(versionFlag, expectedFlag);

    versionString = "OpenGL ES-CM 1.1 ATI";
    expectedFlag = QGLFormat::OpenGL_ES_Common_Version_1_1 | QGLFormat::OpenGL_ES_CommonLite_Version_1_1 | QGLFormat::OpenGL_ES_Common_Version_1_0 | QGLFormat::OpenGL_ES_CommonLite_Version_1_0;
    versionFlag = qOpenGLVersionFlagsFromString(versionString);
    QCOMPARE(versionFlag, expectedFlag);

    versionString = "OpenGL ES-CL 1.1 ATI";
    expectedFlag = QGLFormat::OpenGL_ES_CommonLite_Version_1_1 | QGLFormat::OpenGL_ES_CommonLite_Version_1_0;
    versionFlag = qOpenGLVersionFlagsFromString(versionString);
    QCOMPARE(versionFlag, expectedFlag);

    versionString = "OpenGL ES 2.0 ATI";
    expectedFlag = QGLFormat::OpenGL_ES_Version_2_0;
    versionFlag = qOpenGLVersionFlagsFromString(versionString);
    QCOMPARE(versionFlag, expectedFlag);

    versionString = "3.0";
    expectedFlag = QGLFormat::OpenGL_Version_3_0 | QGLFormat::OpenGL_Version_2_1 | QGLFormat::OpenGL_Version_2_0 | QGLFormat::OpenGL_Version_1_5 | QGLFormat::OpenGL_Version_1_4 | QGLFormat::OpenGL_Version_1_3 | QGLFormat::OpenGL_Version_1_2 | QGLFormat::OpenGL_Version_1_1;
    versionFlag = qOpenGLVersionFlagsFromString(versionString);
    QCOMPARE(versionFlag, expectedFlag);

    QGLWidget glWidget;
    glWidget.show();
    glWidget.makeCurrent();

    // This is unfortunately the only test we can make on the actual openGLVersionFlags()
    // However, the complicated parts are in openGLVersionFlags(const QString &versionString)
    // tested above

    QVERIFY(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_1);
#endif
}

class UnclippedWidget : public QWidget
{
public:
    void paintEvent(QPaintEvent *)
    {
        QPainter p(this);
        p.fillRect(rect().adjusted(-1000, -1000, 1000, 1000), Qt::black);
    }
};

void tst_QGL::graphicsViewClipping()
{
#ifdef QT_NO_OPENGL
    QSKIP("QGL not supported", SkipAll);
#else
    const int size = 64;
    UnclippedWidget *widget = new UnclippedWidget;
    widget->setFixedSize(size, size);

    QGraphicsScene scene;

    scene.addWidget(widget)->setPos(0, 0);

    QGraphicsView view(&scene);
    view.resize(2*size, 2*size);

    QGLWidget *viewport = new QGLWidget;
    view.setViewport(viewport);
    view.show();

    if (!viewport->isValid())
        return;

    scene.setSceneRect(view.viewport()->rect());

#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif
    QTest::qWait(500);

    QImage image = viewport->grabFrameBuffer();
    QImage expected = image;

    QPainter p(&expected);
    p.fillRect(expected.rect(), Qt::white);
    p.fillRect(QRect(0, 0, size, size), Qt::black);
    p.end();

    QCOMPARE(image, expected);
#endif
}

void tst_QGL::partialGLWidgetUpdates_data()
{
    QTest::addColumn<bool>("doubleBufferedContext");
    QTest::addColumn<bool>("autoFillBackground");
    QTest::addColumn<bool>("supportsPartialUpdates");

    QTest::newRow("Double buffered context") << true << true << false;
    QTest::newRow("Double buffered context without auto-fill background") << true << false << false;
    QTest::newRow("Single buffered context") << false << true << false;
    QTest::newRow("Single buffered context without auto-fill background") << false << false << true;
}

void tst_QGL::partialGLWidgetUpdates()
{
#ifdef QT_NO_OPENGL
    QSKIP("QGL not yet supported", SkipAll);
#else
    if (!QGLFormat::hasOpenGL())
        QSKIP("QGL not supported on this platform", SkipAll);

    QFETCH(bool, doubleBufferedContext);
    QFETCH(bool, autoFillBackground);
    QFETCH(bool, supportsPartialUpdates);

    class MyGLWidget : public QGLWidget
    {
        public:
            QRegion paintEventRegion;
            void paintEvent(QPaintEvent *e)
            {
                paintEventRegion = e->region();
            }
    };

    QGLFormat format = QGLFormat::defaultFormat();
    format.setDoubleBuffer(doubleBufferedContext);
    QGLFormat::setDefaultFormat(format);

    MyGLWidget widget;
    widget.setFixedSize(150, 150);
    widget.setAutoFillBackground(autoFillBackground);
    widget.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&widget);
#endif
    QTest::qWait(200);

    if (widget.format().doubleBuffer() != doubleBufferedContext)
        QSKIP("Platform does not support requested format", SkipAll);

    widget.paintEventRegion = QRegion();
    widget.repaint(50, 50, 50, 50);
#ifdef Q_WS_MAC
    // repaint() is not immediate on the Mac; it has to go through the event loop.
    QTest::qWait(200);
#endif
    if (supportsPartialUpdates)
        QCOMPARE(widget.paintEventRegion, QRegion(50, 50, 50, 50));
    else
        QCOMPARE(widget.paintEventRegion, QRegion(widget.rect()));
#endif
}

QTEST_MAIN(tst_QGL)
#include "tst_qgl.moc"
