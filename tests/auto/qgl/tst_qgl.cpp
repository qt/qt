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
#include <qgl.h>

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

// Using INT_MIN and INT_MAX will cause failures on systems
// where "int" is 64-bit, so use the explicit values instead.
#define TEST_INT_MIN    (-2147483647 - 1)
#define TEST_INT_MAX    2147483647

// Testing get/set functions
void tst_QGL::getSetCheck()
{
    if (!QGLFormat::hasOpenGL())
        QSKIP("QGL not supported on this platform", SkipAll);

    QGLFormat obj1;
    // int QGLFormat::depthBufferSize()
    // void QGLFormat::setDepthBufferSize(int)
    QCOMPARE(-1, obj1.depthBufferSize());
    obj1.setDepthBufferSize(0);
    QCOMPARE(0, obj1.depthBufferSize());
    QTest::ignoreMessage(QtWarningMsg, "QGLFormat::setDepthBufferSize: Cannot set negative depth buffer size -2147483648");
    obj1.setDepthBufferSize(TEST_INT_MIN);
    QCOMPARE(0, obj1.depthBufferSize()); // Makes no sense with a negative buffer size
    obj1.setDepthBufferSize(3);
    QTest::ignoreMessage(QtWarningMsg, "QGLFormat::setDepthBufferSize: Cannot set negative depth buffer size -1");
    obj1.setDepthBufferSize(-1);
    QCOMPARE(3, obj1.depthBufferSize());
    obj1.setDepthBufferSize(TEST_INT_MAX);
    QCOMPARE(TEST_INT_MAX, obj1.depthBufferSize());

    // int QGLFormat::accumBufferSize()
    // void QGLFormat::setAccumBufferSize(int)
    QCOMPARE(-1, obj1.accumBufferSize());
    obj1.setAccumBufferSize(0);
    QCOMPARE(0, obj1.accumBufferSize());
    QTest::ignoreMessage(QtWarningMsg, "QGLFormat::setAccumBufferSize: Cannot set negative accumulate buffer size -2147483648");
    obj1.setAccumBufferSize(TEST_INT_MIN);
    QCOMPARE(0, obj1.accumBufferSize()); // Makes no sense with a negative buffer size
    obj1.setAccumBufferSize(3);
    QTest::ignoreMessage(QtWarningMsg, "QGLFormat::setAccumBufferSize: Cannot set negative accumulate buffer size -1");
    obj1.setAccumBufferSize(-1);
    QCOMPARE(3, obj1.accumBufferSize());
    obj1.setAccumBufferSize(TEST_INT_MAX);
    QCOMPARE(TEST_INT_MAX, obj1.accumBufferSize());

    // int QGLFormat::redBufferSize()
    // void QGLFormat::setRedBufferSize(int)
    QCOMPARE(-1, obj1.redBufferSize());
    obj1.setRedBufferSize(0);
    QCOMPARE(0, obj1.redBufferSize());
    QTest::ignoreMessage(QtWarningMsg, "QGLFormat::setRedBufferSize: Cannot set negative red buffer size -2147483648");
    obj1.setRedBufferSize(TEST_INT_MIN);
    QCOMPARE(0, obj1.redBufferSize()); // Makes no sense with a negative buffer size
    obj1.setRedBufferSize(3);
    QTest::ignoreMessage(QtWarningMsg, "QGLFormat::setRedBufferSize: Cannot set negative red buffer size -1");
    obj1.setRedBufferSize(-1);
    QCOMPARE(3, obj1.redBufferSize());
    obj1.setRedBufferSize(TEST_INT_MAX);
    QCOMPARE(TEST_INT_MAX, obj1.redBufferSize());

    // int QGLFormat::greenBufferSize()
    // void QGLFormat::setGreenBufferSize(int)
    QCOMPARE(-1, obj1.greenBufferSize());
    obj1.setGreenBufferSize(0);
    QCOMPARE(0, obj1.greenBufferSize());
    QTest::ignoreMessage(QtWarningMsg, "QGLFormat::setGreenBufferSize: Cannot set negative green buffer size -2147483648");
    obj1.setGreenBufferSize(TEST_INT_MIN);
    QCOMPARE(0, obj1.greenBufferSize()); // Makes no sense with a negative buffer size
    obj1.setGreenBufferSize(3);
    QTest::ignoreMessage(QtWarningMsg, "QGLFormat::setGreenBufferSize: Cannot set negative green buffer size -1");
    obj1.setGreenBufferSize(-1);
    QCOMPARE(3, obj1.greenBufferSize());
    obj1.setGreenBufferSize(TEST_INT_MAX);
    QCOMPARE(TEST_INT_MAX, obj1.greenBufferSize());

    // int QGLFormat::blueBufferSize()
    // void QGLFormat::setBlueBufferSize(int)
    QCOMPARE(-1, obj1.blueBufferSize());
    obj1.setBlueBufferSize(0);
    QCOMPARE(0, obj1.blueBufferSize());
    QTest::ignoreMessage(QtWarningMsg, "QGLFormat::setBlueBufferSize: Cannot set negative blue buffer size -2147483648");
    obj1.setBlueBufferSize(TEST_INT_MIN);
    QCOMPARE(0, obj1.blueBufferSize()); // Makes no sense with a negative buffer size
    obj1.setBlueBufferSize(3);
    QTest::ignoreMessage(QtWarningMsg, "QGLFormat::setBlueBufferSize: Cannot set negative blue buffer size -1");
    obj1.setBlueBufferSize(-1);
    QCOMPARE(3, obj1.blueBufferSize());
    obj1.setBlueBufferSize(TEST_INT_MAX);
    QCOMPARE(TEST_INT_MAX, obj1.blueBufferSize());

    // int QGLFormat::alphaBufferSize()
    // void QGLFormat::setAlphaBufferSize(int)
    QCOMPARE(-1, obj1.alphaBufferSize());
    QCOMPARE(false, obj1.alpha());
    QVERIFY(!obj1.testOption(QGL::AlphaChannel));
    QVERIFY(obj1.testOption(QGL::NoAlphaChannel));
    obj1.setAlphaBufferSize(0);
    QCOMPARE(true, obj1.alpha());   // setAlphaBufferSize() enables alpha.
    QCOMPARE(0, obj1.alphaBufferSize());
    QTest::ignoreMessage(QtWarningMsg, "QGLFormat::setAlphaBufferSize: Cannot set negative alpha buffer size -2147483648");
    obj1.setAlphaBufferSize(TEST_INT_MIN);
    QCOMPARE(0, obj1.alphaBufferSize()); // Makes no sense with a negative buffer size
    obj1.setAlphaBufferSize(3);
    QTest::ignoreMessage(QtWarningMsg, "QGLFormat::setAlphaBufferSize: Cannot set negative alpha buffer size -1");
    obj1.setAlphaBufferSize(-1);
    QCOMPARE(3, obj1.alphaBufferSize());
    obj1.setAlphaBufferSize(TEST_INT_MAX);
    QCOMPARE(TEST_INT_MAX, obj1.alphaBufferSize());

    // int QGLFormat::stencilBufferSize()
    // void QGLFormat::setStencilBufferSize(int)
    QCOMPARE(-1, obj1.stencilBufferSize());
    obj1.setStencilBufferSize(0);
    QCOMPARE(0, obj1.stencilBufferSize());
    QTest::ignoreMessage(QtWarningMsg, "QGLFormat::setStencilBufferSize: Cannot set negative stencil buffer size -2147483648");
    obj1.setStencilBufferSize(TEST_INT_MIN);
    QCOMPARE(0, obj1.stencilBufferSize()); // Makes no sense with a negative buffer size
    obj1.setStencilBufferSize(3);
    QTest::ignoreMessage(QtWarningMsg, "QGLFormat::setStencilBufferSize: Cannot set negative stencil buffer size -1");
    obj1.setStencilBufferSize(-1);
    QCOMPARE(3, obj1.stencilBufferSize());
    obj1.setStencilBufferSize(TEST_INT_MAX);
    QCOMPARE(TEST_INT_MAX, obj1.stencilBufferSize());

    // bool QGLFormat::sampleBuffers()
    // void QGLFormat::setSampleBuffers(bool)
    QCOMPARE(false, obj1.sampleBuffers());
    QVERIFY(!obj1.testOption(QGL::SampleBuffers));
    QVERIFY(obj1.testOption(QGL::NoSampleBuffers));
    obj1.setSampleBuffers(false);
    QCOMPARE(false, obj1.sampleBuffers());
    QVERIFY(obj1.testOption(QGL::NoSampleBuffers));
    obj1.setSampleBuffers(true);
    QCOMPARE(true, obj1.sampleBuffers());
    QVERIFY(obj1.testOption(QGL::SampleBuffers));

    // int QGLFormat::samples()
    // void QGLFormat::setSamples(int)
    QCOMPARE(-1, obj1.samples());
    obj1.setSamples(0);
    QCOMPARE(0, obj1.samples());
    QTest::ignoreMessage(QtWarningMsg, "QGLFormat::setSamples: Cannot have negative number of samples per pixel -2147483648");
    obj1.setSamples(TEST_INT_MIN);
    QCOMPARE(0, obj1.samples());  // Makes no sense with a negative sample size
    obj1.setSamples(3);
    QTest::ignoreMessage(QtWarningMsg, "QGLFormat::setSamples: Cannot have negative number of samples per pixel -1");
    obj1.setSamples(-1);
    QCOMPARE(3, obj1.samples());
    obj1.setSamples(TEST_INT_MAX);
    QCOMPARE(TEST_INT_MAX, obj1.samples());

    // int QGLFormat::swapInterval()
    // void QGLFormat::setSwapInterval(int)
    QCOMPARE(-1, obj1.swapInterval());
    obj1.setSwapInterval(0);
    QCOMPARE(0, obj1.swapInterval());
    obj1.setSwapInterval(TEST_INT_MIN);
    QCOMPARE(TEST_INT_MIN, obj1.swapInterval());
    obj1.setSwapInterval(-1);
    QCOMPARE(-1, obj1.swapInterval());
    obj1.setSwapInterval(TEST_INT_MAX);
    QCOMPARE(TEST_INT_MAX, obj1.swapInterval());

    // bool QGLFormat::doubleBuffer()
    // void QGLFormat::setDoubleBuffer(bool)
    QCOMPARE(true, obj1.doubleBuffer());
    QVERIFY(obj1.testOption(QGL::DoubleBuffer));
    QVERIFY(!obj1.testOption(QGL::SingleBuffer));
    obj1.setDoubleBuffer(false);
    QCOMPARE(false, obj1.doubleBuffer());
    QVERIFY(!obj1.testOption(QGL::DoubleBuffer));
    QVERIFY(obj1.testOption(QGL::SingleBuffer));
    obj1.setDoubleBuffer(true);
    QCOMPARE(true, obj1.doubleBuffer());
    QVERIFY(obj1.testOption(QGL::DoubleBuffer));
    QVERIFY(!obj1.testOption(QGL::SingleBuffer));

    // bool QGLFormat::depth()
    // void QGLFormat::setDepth(bool)
    QCOMPARE(true, obj1.depth());
    QVERIFY(obj1.testOption(QGL::DepthBuffer));
    QVERIFY(!obj1.testOption(QGL::NoDepthBuffer));
    obj1.setDepth(false);
    QCOMPARE(false, obj1.depth());
    QVERIFY(!obj1.testOption(QGL::DepthBuffer));
    QVERIFY(obj1.testOption(QGL::NoDepthBuffer));
    obj1.setDepth(true);
    QCOMPARE(true, obj1.depth());
    QVERIFY(obj1.testOption(QGL::DepthBuffer));
    QVERIFY(!obj1.testOption(QGL::NoDepthBuffer));

    // bool QGLFormat::rgba()
    // void QGLFormat::setRgba(bool)
    QCOMPARE(true, obj1.rgba());
    QVERIFY(obj1.testOption(QGL::Rgba));
    QVERIFY(!obj1.testOption(QGL::ColorIndex));
    obj1.setRgba(false);
    QCOMPARE(false, obj1.rgba());
    QVERIFY(!obj1.testOption(QGL::Rgba));
    QVERIFY(obj1.testOption(QGL::ColorIndex));
    obj1.setRgba(true);
    QCOMPARE(true, obj1.rgba());
    QVERIFY(obj1.testOption(QGL::Rgba));
    QVERIFY(!obj1.testOption(QGL::ColorIndex));

    // bool QGLFormat::alpha()
    // void QGLFormat::setAlpha(bool)
    QVERIFY(obj1.testOption(QGL::AlphaChannel));
    QVERIFY(!obj1.testOption(QGL::NoAlphaChannel));
    obj1.setAlpha(false);
    QCOMPARE(false, obj1.alpha());
    QVERIFY(!obj1.testOption(QGL::AlphaChannel));
    QVERIFY(obj1.testOption(QGL::NoAlphaChannel));
    obj1.setAlpha(true);
    QCOMPARE(true, obj1.alpha());
    QVERIFY(obj1.testOption(QGL::AlphaChannel));
    QVERIFY(!obj1.testOption(QGL::NoAlphaChannel));

    // bool QGLFormat::accum()
    // void QGLFormat::setAccum(bool)
    QCOMPARE(false, obj1.accum());
    QVERIFY(!obj1.testOption(QGL::AccumBuffer));
    QVERIFY(obj1.testOption(QGL::NoAccumBuffer));
    obj1.setAccum(false);
    QCOMPARE(false, obj1.accum());
    QVERIFY(!obj1.testOption(QGL::AccumBuffer));
    QVERIFY(obj1.testOption(QGL::NoAccumBuffer));
    obj1.setAccum(true);
    QCOMPARE(true, obj1.accum());
    QVERIFY(obj1.testOption(QGL::AccumBuffer));
    QVERIFY(!obj1.testOption(QGL::NoAccumBuffer));

    // bool QGLFormat::stencil()
    // void QGLFormat::setStencil(bool)
    QCOMPARE(true, obj1.stencil());
    QVERIFY(obj1.testOption(QGL::StencilBuffer));
    QVERIFY(!obj1.testOption(QGL::NoStencilBuffer));
    obj1.setStencil(false);
    QCOMPARE(false, obj1.stencil());
    QVERIFY(!obj1.testOption(QGL::StencilBuffer));
    QVERIFY(obj1.testOption(QGL::NoStencilBuffer));
    obj1.setStencil(true);
    QCOMPARE(true, obj1.stencil());
    QVERIFY(obj1.testOption(QGL::StencilBuffer));
    QVERIFY(!obj1.testOption(QGL::NoStencilBuffer));

    // bool QGLFormat::stereo()
    // void QGLFormat::setStereo(bool)
    QCOMPARE(false, obj1.stereo());
    QVERIFY(!obj1.testOption(QGL::StereoBuffers));
    QVERIFY(obj1.testOption(QGL::NoStereoBuffers));
    obj1.setStereo(false);
    QCOMPARE(false, obj1.stereo());
    QVERIFY(!obj1.testOption(QGL::StereoBuffers));
    QVERIFY(obj1.testOption(QGL::NoStereoBuffers));
    obj1.setStereo(true);
    QCOMPARE(true, obj1.stereo());
    QVERIFY(obj1.testOption(QGL::StereoBuffers));
    QVERIFY(!obj1.testOption(QGL::NoStereoBuffers));

    // bool QGLFormat::directRendering()
    // void QGLFormat::setDirectRendering(bool)
    QCOMPARE(true, obj1.directRendering());
    QVERIFY(obj1.testOption(QGL::DirectRendering));
    QVERIFY(!obj1.testOption(QGL::IndirectRendering));
    obj1.setDirectRendering(false);
    QCOMPARE(false, obj1.directRendering());
    QVERIFY(!obj1.testOption(QGL::DirectRendering));
    QVERIFY(obj1.testOption(QGL::IndirectRendering));
    obj1.setDirectRendering(true);
    QCOMPARE(true, obj1.directRendering());
    QVERIFY(obj1.testOption(QGL::DirectRendering));
    QVERIFY(!obj1.testOption(QGL::IndirectRendering));

    // bool QGLFormat::overlay()
    // void QGLFormat::setOverlay(bool)
    QCOMPARE(false, obj1.hasOverlay());
    QVERIFY(!obj1.testOption(QGL::HasOverlay));
    QVERIFY(obj1.testOption(QGL::NoOverlay));
    obj1.setOverlay(false);
    QCOMPARE(false, obj1.hasOverlay());
    QVERIFY(!obj1.testOption(QGL::HasOverlay));
    QVERIFY(obj1.testOption(QGL::NoOverlay));
    obj1.setOverlay(true);
    QCOMPARE(true, obj1.hasOverlay());
    QVERIFY(obj1.testOption(QGL::HasOverlay));
    QVERIFY(!obj1.testOption(QGL::NoOverlay));

    // int QGLFormat::plane()
    // void QGLFormat::setPlane(int)
    QCOMPARE(0, obj1.plane());
    obj1.setPlane(0);
    QCOMPARE(0, obj1.plane());
    obj1.setPlane(TEST_INT_MIN);
    QCOMPARE(TEST_INT_MIN, obj1.plane());
    obj1.setPlane(TEST_INT_MAX);
    QCOMPARE(TEST_INT_MAX, obj1.plane());

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
}

#ifdef QT_BUILD_INTERNAL
QT_BEGIN_NAMESPACE
extern QGLFormat::OpenGLVersionFlags qOpenGLVersionFlagsFromString(const QString &versionString);
QT_END_NAMESPACE
#endif

void tst_QGL::openGLVersionCheck()
{
#ifdef QT_BUILD_INTERNAL
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
}

QTEST_MAIN(tst_QGL)
#include "tst_qgl.moc"
