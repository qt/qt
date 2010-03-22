/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>

#include <QtMultimedia/qvideowidget.h>

#include <QtMultimedia/qmediaobject.h>
#include <QtMultimedia/qmediaservice.h>
#include <QtMultimedia/private/qpaintervideosurface_p.h>
#include <QtMultimedia/qvideooutputcontrol.h>
#include <QtMultimedia/qvideowindowcontrol.h>
#include <QtMultimedia/qvideowidgetcontrol.h>
#include <QtMultimedia/qvideorenderercontrol.h>
#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideosurfaceformat.h>

#include <QtGui/qapplication.h>


class tst_QVideoWidget : public QObject
{
    Q_OBJECT
private slots:
    void nullObject();
    void nullService();
    void nullOutputControl();
    void noOutputs();
    void serviceDestroyed();
    void objectDestroyed();
    void setMediaObject();

    void showWindowControl();
    void aspectRatioWindowControl();
    void sizeHintWindowControl_data() { sizeHint_data(); }
    void sizeHintWindowControl();
    void brightnessWindowControl_data() { color_data(); }
    void brightnessWindowControl();
    void contrastWindowControl_data() { color_data(); }
    void contrastWindowControl();
    void hueWindowControl_data() { color_data(); }
    void hueWindowControl();
    void saturationWindowControl_data() { color_data(); }
    void saturationWindowControl();

    void showWidgetControl();
    void aspectRatioWidgetControl();
    void sizeHintWidgetControl_data() { sizeHint_data(); }
    void sizeHintWidgetControl();
    void brightnessWidgetControl_data() { color_data(); }
    void brightnessWidgetControl();
    void contrastWidgetControl_data() { color_data(); }
    void contrastWidgetControl();
    void hueWidgetControl_data() { color_data(); }
    void hueWidgetControl();
    void saturationWidgetControl_data() { color_data(); }
    void saturationWidgetControl();

    void showRendererControl();
    void aspectRatioRendererControl();
    void sizeHintRendererControl_data();
    void sizeHintRendererControl();
    void brightnessRendererControl_data() { color_data(); }
    void brightnessRendererControl();
    void contrastRendererControl_data() { color_data(); }
    void contrastRendererControl();
    void hueRendererControl_data() { color_data(); }
    void hueRendererControl();
    void saturationRendererControl_data() { color_data(); }
    void saturationRendererControl();

    void paintRendererControl();

#ifndef Q_WS_X11
    void fullScreenWindowControl();
    void fullScreenWidgetControl();
    void fullScreenRendererControl();
#endif

private:
    void sizeHint_data();
    void color_data();
};

Q_DECLARE_METATYPE(Qt::AspectRatioMode)
Q_DECLARE_METATYPE(const uchar *)

class QtTestOutputControl : public QVideoOutputControl
{
public:
    QtTestOutputControl() : m_output(NoOutput) {}

    QList<Output> availableOutputs() const { return m_outputs; }
    void setAvailableOutputs(const QList<Output> outputs) { m_outputs = outputs; }

    Output output() const { return m_output; }
    virtual void setOutput(Output output) { m_output = output; }

private:
    Output m_output;
    QList<Output> m_outputs;
};

class QtTestWindowControl : public QVideoWindowControl
{
public:
    QtTestWindowControl()
        : m_winId(0)
        , m_repaintCount(0)
        , m_brightness(0)
        , m_contrast(0)
        , m_saturation(0)
        , m_aspectRatioMode(Qt::KeepAspectRatio)
        , m_fullScreen(0)
    {
    }

    WId winId() const { return m_winId; }
    void setWinId(WId id) { m_winId = id; }

    QRect displayRect() const { return m_displayRect; }
    void setDisplayRect(const QRect &rect) { m_displayRect = rect; }

    bool isFullScreen() const { return m_fullScreen; }
    void setFullScreen(bool fullScreen) { emit fullScreenChanged(m_fullScreen = fullScreen); }

    int repaintCount() const { return m_repaintCount; }
    void setRepaintCount(int count) { m_repaintCount = count; }
    void repaint() { ++m_repaintCount; }

    QSize nativeSize() const { return m_nativeSize; }
    void setNativeSize(const QSize &size) { m_nativeSize = size; emit nativeSizeChanged(); }

    Qt::AspectRatioMode aspectRatioMode() const { return m_aspectRatioMode; }
    void setAspectRatioMode(Qt::AspectRatioMode mode) { m_aspectRatioMode = mode; }

    int brightness() const { return m_brightness; }
    void setBrightness(int brightness) { emit brightnessChanged(m_brightness = brightness); }

    int contrast() const { return m_contrast; }
    void setContrast(int contrast) { emit contrastChanged(m_contrast = contrast); }

    int hue() const { return m_hue; }
    void setHue(int hue) { emit hueChanged(m_hue = hue); }

    int saturation() const { return m_saturation; }
    void setSaturation(int saturation) { emit saturationChanged(m_saturation = saturation); }

private:
    WId m_winId;
    int m_repaintCount;
    int m_brightness;
    int m_contrast;
    int m_hue;
    int m_saturation;
    Qt::AspectRatioMode m_aspectRatioMode;
    QRect m_displayRect;
    QSize m_nativeSize;
    bool m_fullScreen;
};

class QtTestWidgetControl : public QVideoWidgetControl
{
public:
    QtTestWidgetControl()
        : m_brightness(1.0)
        , m_contrast(1.0)
        , m_hue(1.0)
        , m_saturation(1.0)
        , m_aspectRatioMode(Qt::KeepAspectRatio)
        , m_fullScreen(false)
    {
    }

    bool isFullScreen() const { return m_fullScreen; }
    void setFullScreen(bool fullScreen) { emit fullScreenChanged(m_fullScreen = fullScreen); }

    Qt::AspectRatioMode aspectRatioMode() const { return m_aspectRatioMode; }
    void setAspectRatioMode(Qt::AspectRatioMode mode) { m_aspectRatioMode = mode; }

    int brightness() const { return m_brightness; }
    void setBrightness(int brightness) { emit brightnessChanged(m_brightness = brightness); }

    int contrast() const { return m_contrast; }
    void setContrast(int contrast) { emit contrastChanged(m_contrast = contrast); }

    int hue() const { return m_hue; }
    void setHue(int hue) { emit hueChanged(m_hue = hue); }

    int saturation() const { return m_saturation; }
    void setSaturation(int saturation) { emit saturationChanged(m_saturation = saturation); }

    void setSizeHint(const QSize &size) { m_widget.setSizeHint(size); }

    QWidget *videoWidget() { return &m_widget; }

private:
    class Widget : public QWidget
    {
    public:
        QSize sizeHint() const { return m_sizeHint; }
        void setSizeHint(const QSize &size) { m_sizeHint = size; updateGeometry(); }
    private:
        QSize m_sizeHint;
    } m_widget;
    int m_brightness;
    int m_contrast;
    int m_hue;
    int m_saturation;
    Qt::AspectRatioMode m_aspectRatioMode;
    QSize m_sizeHint;
    bool m_fullScreen;
};

class QtTestRendererControl : public QVideoRendererControl
{
public:
    QtTestRendererControl()
        : m_surface(0)
    {
    }

    QAbstractVideoSurface *surface() const { return m_surface; }
    void setSurface(QAbstractVideoSurface *surface) { m_surface = surface; }

private:
    QAbstractVideoSurface *m_surface;
};

class QtTestVideoService : public QMediaService
{
    Q_OBJECT
public:
    QtTestVideoService(
            QtTestOutputControl *output,
            QtTestWindowControl *window,
            QtTestWidgetControl *widget,
            QtTestRendererControl *renderer)
        : QMediaService(0)
        , outputControl(output)
        , windowControl(window)
        , widgetControl(widget)
        , rendererControl(renderer)
    {
    }

    ~QtTestVideoService()
    {
        delete outputControl;
        delete windowControl;
        delete widgetControl;
        delete rendererControl;
    }

    QMediaControl *control(const char *name) const
    {
        if (qstrcmp(name, QVideoOutputControl_iid) == 0)
            return outputControl;
        else if (qstrcmp(name, QVideoWindowControl_iid) == 0)
            return windowControl;
        else if (qstrcmp(name, QVideoWidgetControl_iid) == 0)
            return widgetControl;
        else if (qstrcmp(name, QVideoRendererControl_iid) == 0)
            return rendererControl;
        else
            return 0;
    }

    QtTestOutputControl *outputControl;
    QtTestWindowControl *windowControl;
    QtTestWidgetControl *widgetControl;
    QtTestRendererControl *rendererControl;
};

class QtTestVideoObject : public QMediaObject
{
    Q_OBJECT
public:
    QtTestVideoObject(
            QtTestWindowControl *window,
            QtTestWidgetControl *widget,
            QtTestRendererControl *renderer):
        QMediaObject(0, new QtTestVideoService(new QtTestOutputControl, window, widget, renderer))
    {
        testService = qobject_cast<QtTestVideoService*>(service());
        QList<QVideoOutputControl::Output> outputs;

        if (window)
            outputs.append(QVideoOutputControl::WindowOutput);
        if (widget)
            outputs.append(QVideoOutputControl::WidgetOutput);
        if (renderer)
            outputs.append(QVideoOutputControl::RendererOutput);

        testService->outputControl->setAvailableOutputs(outputs);
    }

    QtTestVideoObject(QtTestVideoService *service):
        QMediaObject(0, service),
        testService(service)
    {
    }

    ~QtTestVideoObject()
    {
        delete testService;
    }

    QtTestVideoService *testService;
};

void tst_QVideoWidget::nullObject()
{
    QVideoWidget widget;
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    QVERIFY(widget.sizeHint().isEmpty());

    widget.setFullScreen(true);
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.isFullScreen(), true);

    widget.setAspectRatioMode(Qt::IgnoreAspectRatio);
    QCOMPARE(widget.aspectRatioMode(), Qt::IgnoreAspectRatio);

    {
        QSignalSpy spy(&widget, SIGNAL(brightnessChanged(int)));

        widget.setBrightness(100);
        QCOMPARE(widget.brightness(), 100);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.value(0).value(0).toInt(), 100);

        widget.setBrightness(100);
        QCOMPARE(widget.brightness(), 100);
        QCOMPARE(spy.count(), 1);

        widget.setBrightness(-120);
        QCOMPARE(widget.brightness(), -100);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(spy.value(1).value(0).toInt(), -100);
    } {
        QSignalSpy spy(&widget, SIGNAL(contrastChanged(int)));

        widget.setContrast(100);
        QCOMPARE(widget.contrast(), 100);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.value(0).value(0).toInt(), 100);

        widget.setContrast(100);
        QCOMPARE(widget.contrast(), 100);
        QCOMPARE(spy.count(), 1);

        widget.setContrast(-120);
        QCOMPARE(widget.contrast(), -100);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(spy.value(1).value(0).toInt(), -100);
    } {
        QSignalSpy spy(&widget, SIGNAL(hueChanged(int)));

        widget.setHue(100);
        QCOMPARE(widget.hue(), 100);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.value(0).value(0).toInt(), 100);

        widget.setHue(100);
        QCOMPARE(widget.hue(), 100);
        QCOMPARE(spy.count(), 1);

        widget.setHue(-120);
        QCOMPARE(widget.hue(), -100);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(spy.value(1).value(0).toInt(), -100);
    } {
        QSignalSpy spy(&widget, SIGNAL(saturationChanged(int)));

        widget.setSaturation(100);
        QCOMPARE(widget.saturation(), 100);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.value(0).value(0).toInt(), 100);

        widget.setSaturation(100);
        QCOMPARE(widget.saturation(), 100);
        QCOMPARE(spy.count(), 1);

        widget.setSaturation(-120);
        QCOMPARE(widget.saturation(), -100);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(spy.value(1).value(0).toInt(), -100);
    }
}

void tst_QVideoWidget::nullService()
{
    QtTestVideoObject object(0);

    QVideoWidget widget;
    widget.setMediaObject(&object);

    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    QVERIFY(widget.sizeHint().isEmpty());

    widget.setFullScreen(true);
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.isFullScreen(), true);

    widget.setAspectRatioMode(Qt::IgnoreAspectRatio);
    QCOMPARE(widget.aspectRatioMode(), Qt::IgnoreAspectRatio);

    widget.setBrightness(100);
    QCOMPARE(widget.brightness(), 100);

    widget.setContrast(100);
    QCOMPARE(widget.contrast(), 100);

    widget.setHue(100);
    QCOMPARE(widget.hue(), 100);

    widget.setSaturation(100);
    QCOMPARE(widget.saturation(), 100);
}

void tst_QVideoWidget::nullOutputControl()
{
    QtTestVideoObject object(new QtTestVideoService(0, 0, 0, 0));

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    QVERIFY(widget.sizeHint().isEmpty());

    widget.setFullScreen(true);
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.isFullScreen(), true);

    widget.setBrightness(100);
    QCOMPARE(widget.brightness(), 100);

    widget.setContrast(100);
    QCOMPARE(widget.contrast(), 100);

    widget.setHue(100);
    QCOMPARE(widget.hue(), 100);

    widget.setSaturation(100);
    QCOMPARE(widget.saturation(), 100);
}

void tst_QVideoWidget::noOutputs()
{
    QtTestVideoObject object(0, 0, 0);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    QVERIFY(widget.sizeHint().isEmpty());

    widget.setFullScreen(true);
    QCOMPARE(widget.isFullScreen(), true);

    widget.setBrightness(100);
    QCOMPARE(widget.brightness(), 100);

    widget.setContrast(100);
    QCOMPARE(widget.contrast(), 100);

    widget.setHue(100);
    QCOMPARE(widget.hue(), 100);

    widget.setSaturation(100);
    QCOMPARE(widget.saturation(), 100);
}

void tst_QVideoWidget::serviceDestroyed()
{
    QtTestVideoObject object(new QtTestWindowControl, new QtTestWidgetControl, 0);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    widget.show();
    QTest::qWaitForWindowShown(&widget);

    widget.setBrightness(100);
    widget.setContrast(100);
    widget.setHue(100);
    widget.setSaturation(100);

    delete object.testService;
    object.testService = 0;

    QCOMPARE(widget.mediaObject(), static_cast<QMediaObject *>(&object));

    QCOMPARE(widget.brightness(), 100);
    QCOMPARE(widget.contrast(), 100);
    QCOMPARE(widget.hue(), 100);
    QCOMPARE(widget.saturation(), 100);

    widget.setFullScreen(true);
    QCOMPARE(widget.isFullScreen(), true);
}

void tst_QVideoWidget::objectDestroyed()
{
    QtTestVideoObject *object = new QtTestVideoObject(
            new QtTestWindowControl,
            new QtTestWidgetControl,
            0);

    QVideoWidget widget;
    widget.setMediaObject(object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    widget.show();
    QTest::qWaitForWindowShown(&widget);

    widget.setBrightness(100);
    widget.setContrast(100);
    widget.setHue(100);
    widget.setSaturation(100);

    // Delete the media object without deleting the service.
    QtTestVideoService *service = object->testService;
    object->testService = 0;

    delete object;
    object = 0;

    QCOMPARE(widget.mediaObject(), static_cast<QMediaObject *>(object));

    QCOMPARE(service->outputControl->output(), QVideoOutputControl::NoOutput);

    QCOMPARE(widget.brightness(), 100);
    QCOMPARE(widget.contrast(), 100);
    QCOMPARE(widget.hue(), 100);
    QCOMPARE(widget.saturation(), 100);

    widget.setFullScreen(true);
    QCOMPARE(widget.isFullScreen(), true);

    delete service;
}

void tst_QVideoWidget::setMediaObject()
{
    QMediaObject *nullObject = 0;
    QtTestVideoObject windowObject(new QtTestWindowControl, 0, 0);
    QtTestVideoObject widgetObject(0, new QtTestWidgetControl, 0);
    QtTestVideoObject rendererObject(0, 0, new QtTestRendererControl);

    QVideoWidget widget;
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    widget.show();
    QTest::qWaitForWindowShown(&widget);

    QCOMPARE(widget.mediaObject(), nullObject);
    QCOMPARE(windowObject.testService->outputControl->output(), QVideoOutputControl::NoOutput);
    QCOMPARE(widgetObject.testService->outputControl->output(), QVideoOutputControl::NoOutput);
    QCOMPARE(rendererObject.testService->outputControl->output(), QVideoOutputControl::NoOutput);

    widget.setMediaObject(&windowObject);
    QCOMPARE(widget.mediaObject(), static_cast<QMediaObject *>(&windowObject));
    QCOMPARE(windowObject.testService->outputControl->output(), QVideoOutputControl::WindowOutput);
    QVERIFY(windowObject.testService->windowControl->winId() != 0);


    widget.setMediaObject(&widgetObject);
    QCOMPARE(widget.mediaObject(), static_cast<QMediaObject *>(&widgetObject));
    QCOMPARE(widgetObject.testService->outputControl->output(), QVideoOutputControl::WidgetOutput);

    QCoreApplication::processEvents(QEventLoop::AllEvents);
    QCOMPARE(widgetObject.testService->widgetControl->videoWidget()->isVisible(), true);

    QCOMPARE(windowObject.testService->outputControl->output(), QVideoOutputControl::NoOutput);

    widget.setMediaObject(&rendererObject);
    QCOMPARE(widget.mediaObject(), static_cast<QMediaObject *>(&rendererObject));
    QCOMPARE(rendererObject.testService->outputControl->output(), QVideoOutputControl::RendererOutput);
    QVERIFY(rendererObject.testService->rendererControl->surface() != 0);

    QCOMPARE(widgetObject.testService->outputControl->output(), QVideoOutputControl::NoOutput);

    widget.setMediaObject(0);
    QCOMPARE(widget.mediaObject(), nullObject);

    QCOMPARE(rendererObject.testService->outputControl->output(), QVideoOutputControl::NoOutput);
}

void tst_QVideoWidget::showWindowControl()
{
    QtTestVideoObject object(new QtTestWindowControl, 0, 0);
    object.testService->windowControl->setNativeSize(QSize(240, 180));

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::NoOutput);

    widget.show();
    QTest::qWaitForWindowShown(&widget);

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::WindowOutput);
    QVERIFY(object.testService->windowControl->winId() != 0);

    QVERIFY(object.testService->windowControl->repaintCount() > 0);

    widget.resize(640, 480);
    QCOMPARE(object.testService->windowControl->displayRect(), QRect(0, 0, 640, 480));

    widget.move(10, 10);
    QCOMPARE(object.testService->windowControl->displayRect(), QRect(0, 0, 640, 480));

    widget.hide();

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::WindowOutput);
}

void tst_QVideoWidget::showWidgetControl()
{
    QtTestVideoObject object(0, new QtTestWidgetControl, 0);
    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::NoOutput);

    widget.show();
    QTest::qWaitForWindowShown(&widget);

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::WidgetOutput);
    QCOMPARE(object.testService->widgetControl->videoWidget()->isVisible(), true);

    widget.resize(640, 480);

    widget.move(10, 10);

    widget.hide();

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::WidgetOutput);
    QCOMPARE(object.testService->widgetControl->videoWidget()->isVisible(), false);
}

void tst_QVideoWidget::showRendererControl()
{
    QtTestVideoObject object(0, 0, new QtTestRendererControl);
    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::NoOutput);

    widget.show();
    QTest::qWaitForWindowShown(&widget);

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::RendererOutput);
    QVERIFY(object.testService->rendererControl->surface() != 0);

    widget.resize(640, 480);

    widget.move(10, 10);

    widget.hide();

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::RendererOutput);
}

void tst_QVideoWidget::aspectRatioWindowControl()
{
    QtTestVideoObject object(new QtTestWindowControl, 0, 0);
    object.testService->windowControl->setAspectRatioMode(Qt::IgnoreAspectRatio);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    // Test the aspect ratio defaults to keeping the aspect ratio.
    QCOMPARE(widget.aspectRatioMode(), Qt::KeepAspectRatio);

    // Test the control has been informed of the aspect ratio change, post show.
    widget.show();
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.aspectRatioMode(), Qt::KeepAspectRatio);
    QCOMPARE(object.testService->windowControl->aspectRatioMode(), Qt::KeepAspectRatio);

    // Test an aspect ratio change is enforced immediately while visible.
    widget.setAspectRatioMode(Qt::IgnoreAspectRatio);
    QCOMPARE(widget.aspectRatioMode(), Qt::IgnoreAspectRatio);
    QCOMPARE(object.testService->windowControl->aspectRatioMode(), Qt::IgnoreAspectRatio);

    // Test an aspect ratio set while not visible is respected.
    widget.hide();
    widget.setAspectRatioMode(Qt::KeepAspectRatio);
    QCOMPARE(widget.aspectRatioMode(), Qt::KeepAspectRatio);
    widget.show();
    QCOMPARE(widget.aspectRatioMode(), Qt::KeepAspectRatio);
    QCOMPARE(object.testService->windowControl->aspectRatioMode(), Qt::KeepAspectRatio);
}

void tst_QVideoWidget::aspectRatioWidgetControl()
{
    QtTestVideoObject object(0, new QtTestWidgetControl, 0);
    object.testService->widgetControl->setAspectRatioMode(Qt::IgnoreAspectRatio);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    // Test the aspect ratio defaults to keeping the aspect ratio.
    QCOMPARE(widget.aspectRatioMode(), Qt::KeepAspectRatio);

    // Test the control has been informed of the aspect ratio change, post show.
    widget.show();
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.aspectRatioMode(), Qt::KeepAspectRatio);
    QCOMPARE(object.testService->widgetControl->aspectRatioMode(), Qt::KeepAspectRatio);

    // Test an aspect ratio change is enforced immediately while visible.
    widget.setAspectRatioMode(Qt::IgnoreAspectRatio);
    QCOMPARE(widget.aspectRatioMode(), Qt::IgnoreAspectRatio);
    QCOMPARE(object.testService->widgetControl->aspectRatioMode(), Qt::IgnoreAspectRatio);

    // Test an aspect ratio set while not visible is respected.
    widget.hide();
    widget.setAspectRatioMode(Qt::KeepAspectRatio);
    QCOMPARE(widget.aspectRatioMode(), Qt::KeepAspectRatio);
    widget.show();
    QCOMPARE(widget.aspectRatioMode(), Qt::KeepAspectRatio);
    QCOMPARE(object.testService->widgetControl->aspectRatioMode(), Qt::KeepAspectRatio);
}

void tst_QVideoWidget::aspectRatioRendererControl()
{
    QtTestVideoObject object(0, 0, new QtTestRendererControl);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    // Test the aspect ratio defaults to keeping the aspect ratio.
    QCOMPARE(widget.aspectRatioMode(), Qt::KeepAspectRatio);

    // Test the control has been informed of the aspect ratio change, post show.
    widget.show();
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.aspectRatioMode(), Qt::KeepAspectRatio);

    // Test an aspect ratio change is enforced immediately while visible.
    widget.setAspectRatioMode(Qt::IgnoreAspectRatio);
    QCOMPARE(widget.aspectRatioMode(), Qt::IgnoreAspectRatio);

    // Test an aspect ratio set while not visible is respected.
    widget.hide();
    widget.setAspectRatioMode(Qt::KeepAspectRatio);
    QCOMPARE(widget.aspectRatioMode(), Qt::KeepAspectRatio);
    widget.show();
    QCOMPARE(widget.aspectRatioMode(), Qt::KeepAspectRatio);
}

void tst_QVideoWidget::sizeHint_data()
{
    QTest::addColumn<QSize>("size");
    
    QTest::newRow("720x576")
            << QSize(720, 576);
}

void tst_QVideoWidget::sizeHintWindowControl()
{
    QFETCH(QSize, size);

    QtTestVideoObject object(new QtTestWindowControl, 0, 0);
    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);
    widget.show();
    QTest::qWaitForWindowShown(&widget);

    QVERIFY(widget.sizeHint().isEmpty());

    object.testService->windowControl->setNativeSize(size);
    QCOMPARE(widget.sizeHint(), size);
}

void tst_QVideoWidget::sizeHintWidgetControl()
{
    QFETCH(QSize, size);

    QtTestVideoObject object(0, new QtTestWidgetControl, 0);
    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);
    widget.show();
    QTest::qWaitForWindowShown(&widget);

    QVERIFY(widget.sizeHint().isEmpty());

    object.testService->widgetControl->setSizeHint(size);
    QCOMPARE(widget.sizeHint(), size);
}

void tst_QVideoWidget::sizeHintRendererControl_data()
{
    QTest::addColumn<QSize>("frameSize");
    QTest::addColumn<QRect>("viewport");
    QTest::addColumn<QSize>("pixelAspectRatio");
    QTest::addColumn<QSize>("expectedSize");

    QTest::newRow("640x480")
            << QSize(640, 480)
            << QRect(0, 0, 640, 480)
            << QSize(1, 1)
            << QSize(640, 480);

    QTest::newRow("800x600, (80,60, 640x480) viewport")
            << QSize(800, 600)
            << QRect(80, 60, 640, 480)
            << QSize(1, 1)
            << QSize(640, 480);

    QTest::newRow("800x600, (80,60, 640x480) viewport, 4:3")
            << QSize(800, 600)
            << QRect(80, 60, 640, 480)
            << QSize(4, 3)
            << QSize(853, 480);

}

void tst_QVideoWidget::sizeHintRendererControl()
{
    QFETCH(QSize, frameSize);
    QFETCH(QRect, viewport);
    QFETCH(QSize, pixelAspectRatio);
    QFETCH(QSize, expectedSize);

    QtTestVideoObject object(0, 0, new QtTestRendererControl);
    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    widget.show();
    QTest::qWaitForWindowShown(&widget);

    QVideoSurfaceFormat format(frameSize, QVideoFrame::Format_ARGB32);
    format.setViewport(viewport);
    format.setPixelAspectRatio(pixelAspectRatio);

    QVERIFY(object.testService->rendererControl->surface()->start(format));

    QCOMPARE(widget.sizeHint(), expectedSize);
}

#ifndef Q_WS_X11

void tst_QVideoWidget::fullScreenWindowControl()
{
    QtTestVideoObject object(new QtTestWindowControl, 0, 0);
    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.show();
    QTest::qWaitForWindowShown(&widget);

    Qt::WindowFlags windowFlags = widget.windowFlags();

    QSignalSpy spy(&widget, SIGNAL(fullScreenChanged(bool)));

    // Test showing full screen with setFullScreen(true).
    widget.setFullScreen(true);
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(object.testService->windowControl->isFullScreen(), true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toBool(), true);

    // Test returning to normal with setFullScreen(false).
    widget.setFullScreen(false);
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(object.testService->windowControl->isFullScreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toBool(), false);
    QCOMPARE(widget.windowFlags(), windowFlags);

    // Test showing full screen with showFullScreen().
    widget.showFullScreen();
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(object.testService->windowControl->isFullScreen(), true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 3);
    QCOMPARE(spy.value(2).value(0).toBool(), true);

    // Test returning to normal with showNormal().
    widget.showNormal();
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(object.testService->windowControl->isFullScreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);
    QCOMPARE(spy.value(3).value(0).toBool(), false);
    QCOMPARE(widget.windowFlags(), windowFlags);

    // Test setFullScreen(false) and showNormal() do nothing when isFullScreen() == false.
    widget.setFullScreen(false);
    QCOMPARE(object.testService->windowControl->isFullScreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);
    widget.showNormal();
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(object.testService->windowControl->isFullScreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);

    // Test setFullScreen(true) and showFullScreen() do nothing when isFullScreen() == true.
    widget.showFullScreen();
    QTest::qWaitForWindowShown(&widget);
    widget.setFullScreen(true);
    QCOMPARE(object.testService->windowControl->isFullScreen(), true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 5);
    widget.showFullScreen();
    QCOMPARE(object.testService->windowControl->isFullScreen(), true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 5);

    // Test if the window control exits full screen mode, the widget follows suit.
    object.testService->windowControl->setFullScreen(false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 6);
    QCOMPARE(spy.value(5).value(0).toBool(), false);

    // Test if the window control enters full screen mode, the widget does nothing.
    object.testService->windowControl->setFullScreen(false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 6);
}

void tst_QVideoWidget::fullScreenWidgetControl()
{
    QtTestVideoObject object(0, new QtTestWidgetControl, 0);
    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.show();
    QTest::qWaitForWindowShown(&widget);

    Qt::WindowFlags windowFlags = widget.windowFlags();

    QSignalSpy spy(&widget, SIGNAL(fullScreenChanged(bool)));

    // Test showing full screen with setFullScreen(true).
    widget.setFullScreen(true);
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(object.testService->widgetControl->isFullScreen(), true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toBool(), true);

    // Test returning to normal with setFullScreen(false).
    widget.setFullScreen(false);
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(object.testService->widgetControl->isFullScreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toBool(), false);
    QCOMPARE(widget.windowFlags(), windowFlags);

    // Test showing full screen with showFullScreen().
    widget.showFullScreen();
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(object.testService->widgetControl->isFullScreen(), true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 3);
    QCOMPARE(spy.value(2).value(0).toBool(), true);

    // Test returning to normal with showNormal().
    widget.showNormal();
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(object.testService->widgetControl->isFullScreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);
    QCOMPARE(spy.value(3).value(0).toBool(), false);
    QCOMPARE(widget.windowFlags(), windowFlags);

    // Test setFullScreen(false) and showNormal() do nothing when isFullScreen() == false.
    widget.setFullScreen(false);
    QCOMPARE(object.testService->widgetControl->isFullScreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);
    widget.showNormal();
    QCOMPARE(object.testService->widgetControl->isFullScreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);

    // Test setFullScreen(true) and showFullScreen() do nothing when isFullScreen() == true.
    widget.showFullScreen();
    QTest::qWaitForWindowShown(&widget);
    widget.setFullScreen(true);
    QCOMPARE(object.testService->widgetControl->isFullScreen(), true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 5);
    widget.showFullScreen();
    QCOMPARE(object.testService->widgetControl->isFullScreen(), true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 5);

    // Test if the window control exits full screen mode, the widget follows suit.
    object.testService->widgetControl->setFullScreen(false);
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 6);
    QCOMPARE(spy.value(5).value(0).toBool(), false);

    // Test if the window control enters full screen mode, the widget does nothing.
    object.testService->widgetControl->setFullScreen(false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 6);
}


void tst_QVideoWidget::fullScreenRendererControl()
{
    QtTestVideoObject object(0, 0, new QtTestRendererControl);
    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.show();
    QTest::qWaitForWindowShown(&widget);

    Qt::WindowFlags windowFlags = widget.windowFlags();

    QSignalSpy spy(&widget, SIGNAL(fullScreenChanged(bool)));

    // Test showing full screen with setFullScreen(true).
    widget.setFullScreen(true);
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toBool(), true);

    // Test returning to normal with setFullScreen(false).
    widget.setFullScreen(false);
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toBool(), false);
    QCOMPARE(widget.windowFlags(), windowFlags);

    // Test showing full screen with showFullScreen().
    widget.showFullScreen();
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 3);
    QCOMPARE(spy.value(2).value(0).toBool(), true);

    // Test returning to normal with showNormal().
    widget.showNormal();
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);
    QCOMPARE(spy.value(3).value(0).toBool(), false);
    QCOMPARE(widget.windowFlags(), windowFlags);

    // Test setFullScreen(false) and showNormal() do nothing when isFullScreen() == false.
    widget.setFullScreen(false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);
    widget.showNormal();
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);

    // Test setFullScreen(true) and showFullScreen() do nothing when isFullScreen() == true.
    widget.showFullScreen();
    QTest::qWaitForWindowShown(&widget);
    widget.setFullScreen(true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 5);
    widget.showFullScreen();
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 5);
}

#endif

void tst_QVideoWidget::color_data()
{
    QTest::addColumn<int>("controlValue");
    QTest::addColumn<int>("value");
    QTest::addColumn<int>("expectedValue");

    QTest::newRow("12")
            << 0
            << 12
            << 12;
    QTest::newRow("-56")
            << 87
            << -56
            << -56;
    QTest::newRow("100")
            << 32
            << 100
            << 100;
    QTest::newRow("1294")
            << 0
            << 1294
            << 100;
    QTest::newRow("-102")
            << 34
            << -102
            << -100;
}

void tst_QVideoWidget::brightnessWindowControl()
{
    QFETCH(int, controlValue);
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(new QtTestWindowControl, 0, 0);
    object.testService->windowControl->setBrightness(controlValue);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);
    widget.show();
    QTest::qWaitForWindowShown(&widget);

    // Test the video widget resets the controls starting brightness to the default.
    QCOMPARE(widget.brightness(), 0);

    QSignalSpy spy(&widget, SIGNAL(brightnessChanged(int)));

    // Test the video widget sets the brightness value, bounded if necessary and emits a changed
    // signal.
    widget.setBrightness(value);
    QCOMPARE(widget.brightness(), expectedValue);
    QCOMPARE(object.testService->windowControl->brightness(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    // Test the changed signal isn't emitted if the value is unchanged.
    widget.setBrightness(value);
    QCOMPARE(widget.brightness(), expectedValue);
    QCOMPARE(object.testService->windowControl->brightness(), expectedValue);
    QCOMPARE(spy.count(), 1);

    // Test the changed signal is emitted if the brightness is changed internally.
    object.testService->windowControl->setBrightness(controlValue);
    QCOMPARE(widget.brightness(), controlValue);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), controlValue);
}

void tst_QVideoWidget::brightnessWidgetControl()
{
    QFETCH(int, controlValue);
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(0, new QtTestWidgetControl, 0);
    object.testService->widgetControl->setBrightness(controlValue);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    QCOMPARE(widget.brightness(), 0);

    widget.show();
    QTest::qWaitForWindowShown(&widget);

    QSignalSpy spy(&widget, SIGNAL(brightnessChanged(int)));

    widget.setBrightness(value);
    QCOMPARE(widget.brightness(), expectedValue);
    QCOMPARE(object.testService->widgetControl->brightness(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setBrightness(value);
    QCOMPARE(widget.brightness(), expectedValue);
    QCOMPARE(object.testService->widgetControl->brightness(), expectedValue);
    QCOMPARE(spy.count(), 1);

    object.testService->widgetControl->setBrightness(controlValue);
    QCOMPARE(widget.brightness(), controlValue);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), controlValue);
}

void tst_QVideoWidget::brightnessRendererControl()
{
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(0, 0, new QtTestRendererControl);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);
    widget.show();
    QTest::qWaitForWindowShown(&widget);

    QSignalSpy spy(&widget, SIGNAL(brightnessChanged(int)));

    widget.setBrightness(value);
    QCOMPARE(widget.brightness(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setBrightness(value);
    QCOMPARE(widget.brightness(), expectedValue);
    QCOMPARE(spy.count(), 1);
}

void tst_QVideoWidget::contrastWindowControl()
{
    QFETCH(int, controlValue);
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(new QtTestWindowControl, 0, 0);
    object.testService->windowControl->setContrast(controlValue);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    QCOMPARE(widget.contrast(), 0);

    widget.show();
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.contrast(), 0);

    QSignalSpy spy(&widget, SIGNAL(contrastChanged(int)));

    widget.setContrast(value);
    QCOMPARE(widget.contrast(), expectedValue);
    QCOMPARE(object.testService->windowControl->contrast(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setContrast(value);
    QCOMPARE(widget.contrast(), expectedValue);
    QCOMPARE(object.testService->windowControl->contrast(), expectedValue);
    QCOMPARE(spy.count(), 1);

    object.testService->windowControl->setContrast(controlValue);
    QCOMPARE(widget.contrast(), controlValue);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), controlValue);
}

void tst_QVideoWidget::contrastWidgetControl()
{
    QFETCH(int, controlValue);
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(0, new QtTestWidgetControl, 0);
    object.testService->widgetControl->setContrast(controlValue);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);
    QCOMPARE(widget.contrast(), 0);

    widget.show();
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.contrast(), 0);

    QSignalSpy spy(&widget, SIGNAL(contrastChanged(int)));

    widget.setContrast(value);
    QCOMPARE(widget.contrast(), expectedValue);
    QCOMPARE(object.testService->widgetControl->contrast(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setContrast(value);
    QCOMPARE(widget.contrast(), expectedValue);
    QCOMPARE(object.testService->widgetControl->contrast(), expectedValue);
    QCOMPARE(spy.count(), 1);

    object.testService->widgetControl->setContrast(controlValue);
    QCOMPARE(widget.contrast(), controlValue);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), controlValue);
}

void tst_QVideoWidget::contrastRendererControl()
{
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(0, 0, new QtTestRendererControl);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);
    widget.show();
    QTest::qWaitForWindowShown(&widget);

    QSignalSpy spy(&widget, SIGNAL(contrastChanged(int)));

    widget.setContrast(value);
    QCOMPARE(widget.contrast(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setContrast(value);
    QCOMPARE(widget.contrast(), expectedValue);
    QCOMPARE(spy.count(), 1);
}

void tst_QVideoWidget::hueWindowControl()
{
    QFETCH(int, controlValue);
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(new QtTestWindowControl, 0, 0);
    object.testService->windowControl->setHue(controlValue);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);
    QCOMPARE(widget.hue(), 0);

    widget.show();
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.hue(), 0);

    QSignalSpy spy(&widget, SIGNAL(hueChanged(int)));

    widget.setHue(value);
    QCOMPARE(widget.hue(), expectedValue);
    QCOMPARE(object.testService->windowControl->hue(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setHue(value);
    QCOMPARE(widget.hue(), expectedValue);
    QCOMPARE(object.testService->windowControl->hue(), expectedValue);
    QCOMPARE(spy.count(), 1);

    object.testService->windowControl->setHue(controlValue);
    QCOMPARE(widget.hue(), controlValue);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), controlValue);
}

void tst_QVideoWidget::hueWidgetControl()
{
    QFETCH(int, controlValue);
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(0, new QtTestWidgetControl, 0);
    object.testService->widgetControl->setHue(controlValue);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);
    QCOMPARE(widget.hue(), 0);

    widget.show();
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.hue(), 0);

    QSignalSpy spy(&widget, SIGNAL(hueChanged(int)));

    widget.setHue(value);
    QCOMPARE(widget.hue(), expectedValue);
    QCOMPARE(object.testService->widgetControl->hue(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setHue(value);
    QCOMPARE(widget.hue(), expectedValue);
    QCOMPARE(object.testService->widgetControl->hue(), expectedValue);
    QCOMPARE(spy.count(), 1);

    object.testService->widgetControl->setHue(controlValue);
    QCOMPARE(widget.hue(), controlValue);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), controlValue);
}

void tst_QVideoWidget::hueRendererControl()
{
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(0, 0, new QtTestRendererControl);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);
    widget.show();
    QTest::qWaitForWindowShown(&widget);

    QSignalSpy spy(&widget, SIGNAL(hueChanged(int)));

    widget.setHue(value);
    QCOMPARE(widget.hue(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setHue(value);
    QCOMPARE(widget.hue(), expectedValue);
    QCOMPARE(spy.count(), 1);
}

void tst_QVideoWidget::saturationWindowControl()
{
    QFETCH(int, controlValue);
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(new QtTestWindowControl, 0, 0);
    object.testService->windowControl->setSaturation(controlValue);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);
    QCOMPARE(widget.saturation(), 0);
    widget.show();
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.saturation(), 0);

    QSignalSpy spy(&widget, SIGNAL(saturationChanged(int)));

    widget.setSaturation(value);
    QCOMPARE(widget.saturation(), expectedValue);
    QCOMPARE(object.testService->windowControl->saturation(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setSaturation(value);
    QCOMPARE(widget.saturation(), expectedValue);
    QCOMPARE(object.testService->windowControl->saturation(), expectedValue);
    QCOMPARE(spy.count(), 1);

    object.testService->windowControl->setSaturation(controlValue);
    QCOMPARE(widget.saturation(), controlValue);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), controlValue);
}

void tst_QVideoWidget::saturationWidgetControl()
{
    QFETCH(int, controlValue);
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(0, new QtTestWidgetControl, 0);
    object.testService->widgetControl->setSaturation(controlValue);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);

    QCOMPARE(widget.saturation(), 0);
    widget.show();
    QTest::qWaitForWindowShown(&widget);
    QCOMPARE(widget.saturation(), 0);

    QSignalSpy spy(&widget, SIGNAL(saturationChanged(int)));

    widget.setSaturation(value);
    QCOMPARE(widget.saturation(), expectedValue);
    QCOMPARE(object.testService->widgetControl->saturation(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setSaturation(value);
    QCOMPARE(widget.saturation(), expectedValue);
    QCOMPARE(object.testService->widgetControl->saturation(), expectedValue);
    QCOMPARE(spy.count(), 1);

    object.testService->widgetControl->setSaturation(controlValue);
    QCOMPARE(widget.saturation(), controlValue);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), controlValue);

}

void tst_QVideoWidget::saturationRendererControl()
{
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(0, 0, new QtTestRendererControl);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);
    widget.show();
    QTest::qWaitForWindowShown(&widget);
    QSignalSpy spy(&widget, SIGNAL(saturationChanged(int)));

    widget.setSaturation(value);
    QCOMPARE(widget.saturation(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setSaturation(value);
    QCOMPARE(widget.saturation(), expectedValue);
    QCOMPARE(spy.count(), 1);
}

static const uchar rgb32ImageData[] =
{
    0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00,
    0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00
};

void tst_QVideoWidget::paintRendererControl()
{
    QtTestVideoObject object(0, 0, new QtTestRendererControl);

    QVideoWidget widget;
    widget.setMediaObject(&object);
    widget.setWindowFlags(Qt::X11BypassWindowManagerHint);
    widget.show();
    QTest::qWaitForWindowShown(&widget);

    QPainterVideoSurface *surface = qobject_cast<QPainterVideoSurface *>(
            object.testService->rendererControl->surface());

    QVideoSurfaceFormat format(QSize(2, 2), QVideoFrame::Format_RGB32);

    QVERIFY(surface->start(format));
    QCOMPARE(surface->isActive(), true);
    QCOMPARE(surface->isReady(), true);

    QCoreApplication::processEvents(QEventLoop::AllEvents);

    QCOMPARE(surface->isActive(), true);
    QCOMPARE(surface->isReady(), true);

    QVideoFrame frame(sizeof(rgb32ImageData), QSize(2, 2), 8, QVideoFrame::Format_RGB32);

    frame.map(QAbstractVideoBuffer::WriteOnly);
    memcpy(frame.bits(), rgb32ImageData, frame.mappedBytes());
    frame.unmap();

    QVERIFY(surface->present(frame));
    QCOMPARE(surface->isActive(), true);
    QCOMPARE(surface->isReady(), false);

    QCoreApplication::processEvents(QEventLoop::AllEvents);

    QCOMPARE(surface->isActive(), true);
    QCOMPARE(surface->isReady(), true);
}

QTEST_MAIN(tst_QVideoWidget)

#include "tst_qvideowidget.moc"
