/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>

#include <QtMultimedia/QAbstractVideoSurface>
#include <QtMultimedia/QVideoSurfaceFormat>

class tst_QAbstractVideoSurface : public QObject
{
    Q_OBJECT
public:
    tst_QAbstractVideoSurface();
    ~tst_QAbstractVideoSurface();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void setError();
    void isFormatSupported_data();
    void isFormatSupported();
    void nearestFormat_data();
    void nearestFormat();
    void start_data();
    void start();
};

typedef QMap<QAbstractVideoBuffer::HandleType, QVideoFrame::PixelFormat> SupportedFormatMap;

Q_DECLARE_METATYPE(SupportedFormatMap)
Q_DECLARE_METATYPE(QVideoSurfaceFormat)
Q_DECLARE_METATYPE(QAbstractVideoSurface::Error);

class QtTestVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit QtTestVideoSurface(QObject *parent = 0) : QAbstractVideoSurface(parent) {}
    explicit QtTestVideoSurface(SupportedFormatMap formats, QObject *parent = 0)
            : QAbstractVideoSurface(parent), supportedFormats(formats) {}

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const
    {
        return supportedFormats.values(handleType);
    }

    bool present(const QVideoFrame &) { return false; }

    using QAbstractVideoSurface::setError;

private:
    SupportedFormatMap supportedFormats;
};

tst_QAbstractVideoSurface::tst_QAbstractVideoSurface()
{
}

tst_QAbstractVideoSurface::~tst_QAbstractVideoSurface()
{
}

void tst_QAbstractVideoSurface::initTestCase()
{
}

void tst_QAbstractVideoSurface::cleanupTestCase()
{
}

void tst_QAbstractVideoSurface::init()
{
}

void tst_QAbstractVideoSurface::cleanup()
{
}

void tst_QAbstractVideoSurface::setError()
{
    qRegisterMetaType<QAbstractVideoSurface::Error>();

    QtTestVideoSurface surface;

    QCOMPARE(surface.error(), QAbstractVideoSurface::NoError);

    surface.setError(QAbstractVideoSurface::StoppedError);
    QCOMPARE(surface.error(), QAbstractVideoSurface::StoppedError);

    surface.setError(QAbstractVideoSurface::ResourceError);
    QCOMPARE(surface.error(), QAbstractVideoSurface::ResourceError);

    surface.setError(QAbstractVideoSurface::NoError);
    QCOMPARE(surface.error(), QAbstractVideoSurface::NoError);
}

void tst_QAbstractVideoSurface::isFormatSupported_data()
{
    QTest::addColumn<SupportedFormatMap>("supportedFormats");
    QTest::addColumn<QVideoSurfaceFormat>("format");
    QTest::addColumn<bool>("supported");

    SupportedFormatMap formats;

    QTest::newRow("no formats: rgb32")
            << formats
            << QVideoSurfaceFormat(QSize(800, 600), QVideoFrame::Format_RGB32)
            << false;
    QTest::newRow("no formats: yv12")
            << formats
            << QVideoSurfaceFormat(QSize(800, 600), QVideoFrame::Format_YV12)
            << false;
    QTest::newRow("no formats: rgb32 gl")
            << formats
            << QVideoSurfaceFormat(
                    QSize(800, 600),
                    QVideoFrame::Format_RGB32,
                    QAbstractVideoBuffer::GLTextureHandle)
            << false;
    QTest::newRow("no formats: rgb24 gl")
            << formats
            << QVideoSurfaceFormat(
                    QSize(800, 600),
                    QVideoFrame::Format_RGB24,
                    QAbstractVideoBuffer::GLTextureHandle)
            << false;

    formats.insertMulti(QAbstractVideoBuffer::NoHandle, QVideoFrame::Format_RGB32);
    formats.insertMulti(QAbstractVideoBuffer::NoHandle, QVideoFrame::Format_RGB24);
    formats.insertMulti(QAbstractVideoBuffer::NoHandle, QVideoFrame::Format_YUV444);
    formats.insertMulti(QAbstractVideoBuffer::GLTextureHandle, QVideoFrame::Format_RGB32);

    QTest::newRow("supported: rgb32")
            << formats
            << QVideoSurfaceFormat(QSize(800, 600), QVideoFrame::Format_RGB32)
            << true;
    QTest::newRow("supported: rgb24")
            << formats
            << QVideoSurfaceFormat(QSize(800, 600), QVideoFrame::Format_RGB24)
            << true;
    QTest::newRow("unsupported: yv12")
            << formats
            << QVideoSurfaceFormat(QSize(800, 600), QVideoFrame::Format_YV12)
            << false;
    QTest::newRow("supported: rgb32 gl")
            << formats
            << QVideoSurfaceFormat(
                    QSize(800, 600),
                    QVideoFrame::Format_RGB32,
                    QAbstractVideoBuffer::GLTextureHandle)
            << true;
    QTest::newRow("unsupported: rgb24 gl")
            << formats
            << QVideoSurfaceFormat(
                    QSize(800, 600),
                    QVideoFrame::Format_RGB24,
                    QAbstractVideoBuffer::GLTextureHandle)
            << false;
    QTest::newRow("unsupported: yv12 gl")
            << formats
            << QVideoSurfaceFormat(
                    QSize(800, 600),
                    QVideoFrame::Format_YV12,
                    QAbstractVideoBuffer::GLTextureHandle)
            << false;

    formats.insertMulti(QAbstractVideoBuffer::NoHandle, QVideoFrame::Format_YV12);
    formats.insertMulti(QAbstractVideoBuffer::GLTextureHandle, QVideoFrame::Format_RGB24);

    QTest::newRow("supported: yv12")
            << formats
            << QVideoSurfaceFormat(QSize(800, 600), QVideoFrame::Format_YV12)
            << true;
    QTest::newRow("supported: rgb24 gl")
            << formats
            << QVideoSurfaceFormat(
                    QSize(800, 600),
                    QVideoFrame::Format_RGB24,
                    QAbstractVideoBuffer::GLTextureHandle)
            << true;
}

void tst_QAbstractVideoSurface::isFormatSupported()
{
    QFETCH(SupportedFormatMap, supportedFormats);
    QFETCH(QVideoSurfaceFormat, format);
    QFETCH(bool, supported);

    QtTestVideoSurface surface(supportedFormats);

    QCOMPARE(surface.isFormatSupported(format), supported);
}

void tst_QAbstractVideoSurface::nearestFormat_data()
{
    isFormatSupported_data();
}

void tst_QAbstractVideoSurface::nearestFormat()
{
    QFETCH(SupportedFormatMap, supportedFormats);
    QFETCH(QVideoSurfaceFormat, format);
    QFETCH(bool, supported);

    QtTestVideoSurface surface(supportedFormats);

    QCOMPARE(surface.nearestFormat(format) == format, supported);
}

void tst_QAbstractVideoSurface::start_data()
{
    QTest::addColumn<QVideoSurfaceFormat>("format");

    QTest::newRow("rgb32") << QVideoSurfaceFormat(
            QSize(800, 600),
            QVideoFrame::Format_RGB32);
    QTest::newRow("yv12") << QVideoSurfaceFormat(
            QSize(800, 600),
            QVideoFrame::Format_YV12);
    QTest::newRow("rgb32 gl") << QVideoSurfaceFormat(
            QSize(800, 600),
            QVideoFrame::Format_RGB32,
            QAbstractVideoBuffer::GLTextureHandle);
}

void tst_QAbstractVideoSurface::start()
{
    QFETCH(QVideoSurfaceFormat, format);

    QtTestVideoSurface surface;
    surface.setError(QAbstractVideoSurface::ResourceError);

    QSignalSpy formatSpy(&surface, SIGNAL(surfaceFormatChanged(QVideoSurfaceFormat)));
    QSignalSpy activeSpy(&surface, SIGNAL(activeChanged(bool)));

    QVERIFY(!surface.isActive());
    QCOMPARE(surface.surfaceFormat(), QVideoSurfaceFormat());

    QVERIFY(surface.start(format));

    QVERIFY(surface.isActive());
    QCOMPARE(surface.surfaceFormat(), format);

    QCOMPARE(formatSpy.count(), 1);
    QCOMPARE(qvariant_cast<QVideoSurfaceFormat>(formatSpy.last().at(0)), format);

    QCOMPARE(activeSpy.count(), 1);
    QCOMPARE(activeSpy.last().at(0).toBool(), true);

    // error() is reset on a successful start.
    QCOMPARE(surface.error(), QAbstractVideoSurface::NoError);

    surface.stop();

    QVERIFY(!surface.isActive());
    QCOMPARE(surface.surfaceFormat(), QVideoSurfaceFormat());

    QCOMPARE(formatSpy.count(), 2);
    QCOMPARE(qvariant_cast<QVideoSurfaceFormat>(formatSpy.last().at(0)), QVideoSurfaceFormat());

    QCOMPARE(activeSpy.count(), 2);
    QCOMPARE(activeSpy.last().at(0).toBool(), false);
}

QTEST_MAIN(tst_QAbstractVideoSurface)

#include "tst_qabstractvideosurface.moc"
