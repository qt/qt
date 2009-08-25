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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qgl.h>
#include <qglcolormap.h>
#include <qpaintengine.h>

#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QVBoxLayout>

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
    void glWidgetRendering();
    void glWidgetReparent();
    void colormap();
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


class GLWidget : public QGLWidget
{
public:
    GLWidget(QWidget* p = 0)
            : QGLWidget(p), beginOk(false), engineType(QPaintEngine::MaxUser) {}
    bool beginOk;
    QPaintEngine::Type engineType;
    void paintGL()
    {
        QPainter p;
        beginOk = p.begin(this);
        QPaintEngine* pe = p.paintEngine();
        engineType = pe->type();

        // This test only ensures it's possible to paint onto a QGLWidget. Full
        // paint engine feature testing is way out of scope!

        p.fillRect(0, 0, width(), height(), Qt::red);
        // No p.end() or swap buffers, should be done automatically
    }

};

void tst_QGL::glWidgetRendering()
{
    GLWidget w;
    w.show();

#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&w);
#endif
    QTest::qWait(200);

    QVERIFY(w.beginOk);
    QVERIFY(w.engineType == QPaintEngine::OpenGL);

    QImage fb = w.grabFrameBuffer(false).convertToFormat(QImage::Format_RGB32);
    QImage reference(fb.size(), QImage::Format_RGB32);
    reference.fill(0xffff0000);

    QCOMPARE(fb, reference);
}

void tst_QGL::glWidgetReparent()
{
    // Try it as a top-level first:
    GLWidget *widget = new GLWidget;
    widget->setGeometry(0, 0, 200, 30);
    widget->show();

    QWidget grandParentWidget;
    grandParentWidget.setPalette(Qt::blue);
    QVBoxLayout grandParentLayout(&grandParentWidget);

    QWidget parentWidget(&grandParentWidget);
    grandParentLayout.addWidget(&parentWidget);
    parentWidget.setPalette(Qt::green);
    parentWidget.setAutoFillBackground(true);
    QVBoxLayout parentLayout(&parentWidget);

    grandParentWidget.setGeometry(0, 100, 200, 200);
    grandParentWidget.show();

#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(widget);
    qt_x11_wait_for_window_manager(&parentWidget);
#endif
    QTest::qWait(2000);

    QVERIFY(parentWidget.children().count() == 1); // The layout

    // Now both widgets should be created & shown, time to re-parent:
    parentLayout.addWidget(widget);

#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&parentWidget);
#endif
    QTest::qWait(2000);

    QVERIFY(parentWidget.children().count() == 2); // Layout & glwidget
    QVERIFY(parentWidget.children().contains(widget));
    QVERIFY(widget->height() > 30);

    delete widget;

#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&parentWidget);
#endif
    QTest::qWait(2000);

    QVERIFY(parentWidget.children().count() == 1); // The layout

    // Now do pretty much the same thing, but don't show the
    // widget first:
    widget = new GLWidget;
    parentLayout.addWidget(widget);

#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&parentWidget);
#endif
    QTest::qWait(2000);

    QVERIFY(parentWidget.children().count() == 2); // Layout & glwidget
    QVERIFY(parentWidget.children().contains(widget));
    QVERIFY(widget->height() > 30);

    delete widget;
}

class ColormapExtended : public QGLColormap
{
public:
    ColormapExtended() {}

    Qt::HANDLE handle() { return QGLColormap::handle(); }
    void setHandle(Qt::HANDLE handle) { QGLColormap::setHandle(handle); }
};

void tst_QGL::colormap()
{
    // Check the properties of the default empty colormap.
    QGLColormap cmap1;
    QVERIFY(cmap1.isEmpty());
    QCOMPARE(cmap1.size(), 0);
    QVERIFY(cmap1.entryRgb(0) == 0);
    QVERIFY(cmap1.entryRgb(-1) == 0);
    QVERIFY(cmap1.entryRgb(100) == 0);
    QVERIFY(!cmap1.entryColor(0).isValid());
    QVERIFY(!cmap1.entryColor(-1).isValid());
    QVERIFY(!cmap1.entryColor(100).isValid());
    QCOMPARE(cmap1.find(qRgb(255, 0, 0)), -1);
    QCOMPARE(cmap1.findNearest(qRgb(255, 0, 0)), -1);

    // Set an entry and re-test.
    cmap1.setEntry(56, qRgb(255, 0, 0));
    // The colormap is still considered "empty" even though it
    // has entries in it now.  The isEmpty() method is used to
    // detect when the colormap is in use by a GL widget,
    // not to detect when it is empty!
    QVERIFY(cmap1.isEmpty());
    QCOMPARE(cmap1.size(), 256);
    QVERIFY(cmap1.entryRgb(0) == 0);
    QVERIFY(cmap1.entryColor(0) == QColor(0, 0, 0, 255));
    QVERIFY(cmap1.entryRgb(56) == qRgb(255, 0, 0));
    QVERIFY(cmap1.entryColor(56) == QColor(255, 0, 0, 255));
    QCOMPARE(cmap1.find(qRgb(255, 0, 0)), 56);
    QCOMPARE(cmap1.findNearest(qRgb(255, 0, 0)), 56);

    // Set some more entries.
    static QRgb const colors[] = {
        qRgb(255, 0, 0),
        qRgb(0, 255, 0),
        qRgb(255, 255, 255),
        qRgb(0, 0, 255),
        qRgb(0, 0, 0)
    };
    cmap1.setEntry(57, QColor(0, 255, 0));
    cmap1.setEntries(3, colors + 2, 58);
    cmap1.setEntries(5, colors, 251);
    int idx;
    for (idx = 0; idx < 5; ++idx) {
        QVERIFY(cmap1.entryRgb(56 + idx) == colors[idx]);
        QVERIFY(cmap1.entryColor(56 + idx) == QColor(colors[idx]));
        QVERIFY(cmap1.entryRgb(251 + idx) == colors[idx]);
        QVERIFY(cmap1.entryColor(251 + idx) == QColor(colors[idx]));
    }
    QCOMPARE(cmap1.size(), 256);

    // Perform color lookups.
    QCOMPARE(cmap1.find(qRgb(255, 0, 0)), 56);
    QCOMPARE(cmap1.find(qRgb(0, 0, 0)), 60); // Actually finds 0, 0, 0, 255.
    QCOMPARE(cmap1.find(qRgba(0, 0, 0, 0)), 0);
    QCOMPARE(cmap1.find(qRgb(0, 255, 0)), 57);
    QCOMPARE(cmap1.find(qRgb(255, 255, 255)), 58);
    QCOMPARE(cmap1.find(qRgb(0, 0, 255)), 59);
    QCOMPARE(cmap1.find(qRgb(140, 0, 0)), -1);
    QCOMPARE(cmap1.find(qRgb(0, 140, 0)), -1);
    QCOMPARE(cmap1.find(qRgb(0, 0, 140)), -1);
    QCOMPARE(cmap1.find(qRgb(64, 0, 0)), -1);
    QCOMPARE(cmap1.find(qRgb(0, 64, 0)), -1);
    QCOMPARE(cmap1.find(qRgb(0, 0, 64)), -1);
    QCOMPARE(cmap1.findNearest(qRgb(255, 0, 0)), 56);
    QCOMPARE(cmap1.findNearest(qRgb(0, 0, 0)), 60);
    QCOMPARE(cmap1.findNearest(qRgba(0, 0, 0, 0)), 0);
    QCOMPARE(cmap1.findNearest(qRgb(0, 255, 0)), 57);
    QCOMPARE(cmap1.findNearest(qRgb(255, 255, 255)), 58);
    QCOMPARE(cmap1.findNearest(qRgb(0, 0, 255)), 59);
    QCOMPARE(cmap1.findNearest(qRgb(140, 0, 0)), 56);
    QCOMPARE(cmap1.findNearest(qRgb(0, 140, 0)), 57);
    QCOMPARE(cmap1.findNearest(qRgb(0, 0, 140)), 59);
    QCOMPARE(cmap1.findNearest(qRgb(64, 0, 0)), 0);
    QCOMPARE(cmap1.findNearest(qRgb(0, 64, 0)), 0);
    QCOMPARE(cmap1.findNearest(qRgb(0, 0, 64)), 0);

    // Make some copies of the colormap and check that they are the same.
    QGLColormap cmap2(cmap1);
    QGLColormap cmap3;
    cmap3 = cmap1;
    QVERIFY(cmap2.isEmpty());
    QVERIFY(cmap3.isEmpty());
    QCOMPARE(cmap2.size(), 256);
    QCOMPARE(cmap3.size(), 256);
    for (idx = 0; idx < 256; ++idx) {
        QCOMPARE(cmap1.entryRgb(idx), cmap2.entryRgb(idx));
        QCOMPARE(cmap1.entryRgb(idx), cmap3.entryRgb(idx));
    }

    // Modify an entry in one of the copies and recheck the original.
    cmap2.setEntry(45, qRgb(255, 0, 0));
    for (idx = 0; idx < 256; ++idx) {
        if (idx != 45)
            QCOMPARE(cmap1.entryRgb(idx), cmap2.entryRgb(idx));
        else
            QCOMPARE(cmap2.entryRgb(45), qRgb(255, 0, 0));
        QCOMPARE(cmap1.entryRgb(idx), cmap3.entryRgb(idx));
    }

    // Check that setting the handle will cause isEmpty() to work right.
    ColormapExtended cmap4;
    cmap4.setEntry(56, qRgb(255, 0, 0));
    QVERIFY(cmap4.isEmpty());
    QCOMPARE(cmap4.size(), 256);
    cmap4.setHandle(Qt::HANDLE(42));
    QVERIFY(cmap4.handle() == Qt::HANDLE(42));
    QVERIFY(!cmap4.isEmpty());
    QCOMPARE(cmap4.size(), 256);
}

QTEST_MAIN(tst_QGL)
#include "tst_qgl.moc"
