/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include <private/qgraphicssystem_runtime_p.h>
#include <private/qgraphicssystem_raster_p.h>
#include <private/qgraphicssystemfactory_p.h>
#include <private/qapplication_p.h>
#include <private/qwidget_p.h>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtGui/QBitmap>

QT_BEGIN_NAMESPACE

static int qt_pixmap_serial = 0;

#define READBACK(f)                                         \
    m_graphicsSystem->decreaseMemoryUsage(memoryUsage());   \
    f                                                       \
    readBackInfo();                                         \
    m_graphicsSystem->increaseMemoryUsage(memoryUsage());   \


class QDeferredGraphicsSystemChange : public QObject
{
    Q_OBJECT

public:
    QDeferredGraphicsSystemChange(QRuntimeGraphicsSystem *gs, const QString& graphicsSystemName)
    : m_graphicsSystem(gs), m_graphicsSystemName(graphicsSystemName)
    {
    }

    void launch()
    {
        QTimer::singleShot(0, this, SLOT(doChange()));
    }

private slots:

    void doChange()
    {
        m_graphicsSystem->setGraphicsSystem(m_graphicsSystemName);
        deleteLater();
    }

private:

    QRuntimeGraphicsSystem *m_graphicsSystem;
    QString m_graphicsSystemName;
};

QRuntimePixmapData::QRuntimePixmapData(const QRuntimeGraphicsSystem *gs, PixelType type)
        : QPixmapData(type, RuntimeClass), m_graphicsSystem(gs)
{
    setSerialNumber(++qt_pixmap_serial);
}

QRuntimePixmapData::~QRuntimePixmapData()
{
    m_graphicsSystem->removePixmapData(this);
    delete m_data;
}

void QRuntimePixmapData::readBackInfo()
{
    w = m_data->width();
    h = m_data->height();
    d = m_data->depth();
    is_null = m_data->isNull();
}


QPixmapData *QRuntimePixmapData::createCompatiblePixmapData() const
{
    QRuntimePixmapData *rtData = new QRuntimePixmapData(m_graphicsSystem, pixelType());
    rtData->m_data = m_data->createCompatiblePixmapData();
    return rtData;
}


void QRuntimePixmapData::resize(int width, int height)
{
    READBACK(
        m_data->resize(width, height);
    )
}


void QRuntimePixmapData::fromImage(const QImage &image,
                                   Qt::ImageConversionFlags flags)
{
    READBACK(
        m_data->fromImage(image, flags);
    )
}


bool QRuntimePixmapData::fromFile(const QString &filename, const char *format,
                                  Qt::ImageConversionFlags flags)
{
    bool success(false);
    READBACK(
        success = m_data->fromFile(filename, format, flags);
    )
    return success;
}

bool QRuntimePixmapData::fromData(const uchar *buffer, uint len, const char *format,
                                  Qt::ImageConversionFlags flags)
{
    bool success(false);
    READBACK(
        success = m_data->fromData(buffer, len, format, flags);
    )
    return success;
}


void QRuntimePixmapData::copy(const QPixmapData *data, const QRect &rect)
{
    if (data->runtimeData()) {
        READBACK(
            m_data->copy(data->runtimeData(), rect);
        )
    } else {
        READBACK(
            m_data->copy(data, rect);
        )
    }
}

bool QRuntimePixmapData::scroll(int dx, int dy, const QRect &rect)
{
    return m_data->scroll(dx, dy, rect);
}


int QRuntimePixmapData::metric(QPaintDevice::PaintDeviceMetric metric) const
{
    return m_data->metric(metric);
}

void QRuntimePixmapData::fill(const QColor &color)
{
    return m_data->fill(color);
}

QBitmap QRuntimePixmapData::mask() const
{
    return m_data->mask();
}

void QRuntimePixmapData::setMask(const QBitmap &mask)
{
    READBACK(
        m_data->setMask(mask);
    )
}

bool QRuntimePixmapData::hasAlphaChannel() const
{
    return m_data->hasAlphaChannel();
}

QPixmap QRuntimePixmapData::transformed(const QTransform &matrix,
                                        Qt::TransformationMode mode) const
{
    return m_data->transformed(matrix, mode);
}

void QRuntimePixmapData::setAlphaChannel(const QPixmap &alphaChannel)
{
    READBACK(
        m_data->setAlphaChannel(alphaChannel);
    )
}

QPixmap QRuntimePixmapData::alphaChannel() const
{
    return m_data->alphaChannel();
}

QImage QRuntimePixmapData::toImage() const
{
    return m_data->toImage();
}

QPaintEngine* QRuntimePixmapData::paintEngine() const
{
    return m_data->paintEngine();
}

QImage* QRuntimePixmapData::buffer()
{
    return m_data->buffer();
}

#if defined(Q_OS_SYMBIAN)
void* QRuntimePixmapData::toNativeType(NativeType type)
{
    return m_data->toNativeType(type);
}

void QRuntimePixmapData::fromNativeType(void *pixmap, NativeType type)
{
    m_data->fromNativeType(pixmap, type);
    readBackInfo();
}
#endif

QPixmapData* QRuntimePixmapData::runtimeData() const
{
    return m_data;
}

uint QRuntimePixmapData::memoryUsage() const
{
    if(is_null || d == 0)
        return 0;
    return w * h * (d / 8);
}


QRuntimeWindowSurface::QRuntimeWindowSurface(const QRuntimeGraphicsSystem *gs, QWidget *window)
    : QWindowSurface(window), m_windowSurface(0), m_pendingWindowSurface(0), m_graphicsSystem(gs)
{

}

QRuntimeWindowSurface::~QRuntimeWindowSurface()
{
    m_graphicsSystem->removeWindowSurface(this);
    delete m_windowSurface;
}

QPaintDevice *QRuntimeWindowSurface::paintDevice()
{
    return m_windowSurface->paintDevice();
}

void QRuntimeWindowSurface::flush(QWidget *widget, const QRegion &region,
                                  const QPoint &offset)
{
    m_windowSurface->flush(widget, region, offset);

    int destroyPolicy = m_graphicsSystem->windowSurfaceDestroyPolicy();
    if(m_pendingWindowSurface &&
        destroyPolicy == QRuntimeGraphicsSystem::DestroyAfterFirstFlush) {
#ifdef QT_DEBUG
        qDebug() << "QRuntimeWindowSurface::flush() - destroy pending window surface";
#endif
        delete m_pendingWindowSurface;
        m_pendingWindowSurface = 0;
    }
}

void QRuntimeWindowSurface::setGeometry(const QRect &rect)
{
    m_graphicsSystem->decreaseMemoryUsage(memoryUsage());
    m_windowSurface->setGeometry(rect);
    m_graphicsSystem->increaseMemoryUsage(memoryUsage());
}

bool QRuntimeWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    return m_windowSurface->scroll(area, dx, dy);
}

void QRuntimeWindowSurface::beginPaint(const QRegion &rgn)
{
    m_windowSurface->beginPaint(rgn);
}

void QRuntimeWindowSurface::endPaint(const QRegion &rgn)
{
    m_windowSurface->endPaint(rgn);
}

QImage* QRuntimeWindowSurface::buffer(const QWidget *widget)
{
    return m_windowSurface->buffer(widget);
}

QPixmap QRuntimeWindowSurface::grabWidget(const QWidget *widget, const QRect& rectangle) const
{
    return m_windowSurface->grabWidget(widget, rectangle);
}

QPoint QRuntimeWindowSurface::offset(const QWidget *widget) const
{
    return m_windowSurface->offset(widget);
}

uint QRuntimeWindowSurface::memoryUsage() const
{
    QPaintDevice *pdev = m_windowSurface->paintDevice();
    if (pdev && pdev->depth() != 0)
        return pdev->width() * pdev->height() * (pdev->depth()/8);

    return 0;
}

QRuntimeGraphicsSystem::QRuntimeGraphicsSystem()
    : m_memoryUsage(0), m_windowSurfaceDestroyPolicy(DestroyImmediately),
	m_graphicsSystem(0), m_graphicsSystemChangeMemoryLimit(0)
{
    QApplicationPrivate::graphics_system_name = QLatin1String("runtime");
    QApplicationPrivate::runtime_graphics_system = true;

#ifdef Q_OS_SYMBIAN
    m_graphicsSystemName = QLatin1String("openvg");
    m_windowSurfaceDestroyPolicy = DestroyAfterFirstFlush;
#else
    m_graphicsSystemName = QLatin1String("raster");
#endif

    m_graphicsSystem = QGraphicsSystemFactory::create(m_graphicsSystemName);
}


QPixmapData *QRuntimeGraphicsSystem::createPixmapData(QPixmapData::PixelType type) const
{
    Q_ASSERT(m_graphicsSystem);
    QPixmapData *data = m_graphicsSystem->createPixmapData(type);

    QRuntimePixmapData *rtData = new QRuntimePixmapData(this, type);
    rtData->m_data = data;
    m_pixmapDatas << rtData;

    return rtData;
}

QWindowSurface *QRuntimeGraphicsSystem::createWindowSurface(QWidget *widget) const
{
    Q_ASSERT(m_graphicsSystem);
    QRuntimeWindowSurface *rtSurface = new QRuntimeWindowSurface(this, widget);
    rtSurface->m_windowSurface = m_graphicsSystem->createWindowSurface(widget);
    widget->setWindowSurface(rtSurface);
    m_windowSurfaces << rtSurface;
    increaseMemoryUsage(rtSurface->memoryUsage());
    return rtSurface;
}

/*!
    Sets graphics system when resource memory consumption is under /a memoryUsageLimit.
*/
void QRuntimeGraphicsSystem::setGraphicsSystem(const QString &name, uint memoryUsageLimit)
{
#ifdef QT_DEBUG
    qDebug() << "QRuntimeGraphicsSystem::setGraphicsSystem( "<< name <<", " << memoryUsageLimit << ")";
    qDebug() << "        current approximated graphics system memory usage " << memoryUsage() << " bytes";
#endif
    if (memoryUsage() >= memoryUsageLimit) {
        m_graphicsSystemChangeMemoryLimit = memoryUsageLimit;
        m_pendingGraphicsSystemName = name;
    } else {
        setGraphicsSystem(name);
    }
}

void QRuntimeGraphicsSystem::setGraphicsSystem(const QString &name)
{
    if (m_graphicsSystemName == name)
        return;
#ifdef QT_DEBUG
    qDebug() << "QRuntimeGraphicsSystem::setGraphicsSystem( " << name << " )";
    qDebug() << "        current approximated graphics system memory usage "<< memoryUsage() << " bytes";
#endif
    delete m_graphicsSystem;
    m_graphicsSystem = QGraphicsSystemFactory::create(name);
    m_graphicsSystemName = name;

    Q_ASSERT(m_graphicsSystem);

    m_graphicsSystemChangeMemoryLimit = 0;
    m_pendingGraphicsSystemName = QString();

    for (int i = 0; i < m_pixmapDatas.size(); ++i) {
        QRuntimePixmapData *proxy = m_pixmapDatas.at(i);
        QPixmapData *newData = m_graphicsSystem->createPixmapData(proxy->m_data);
        // ### TODO Optimize. Openvg and s60raster graphics systems could switch internal ARGB32_PRE QImage buffers.
        newData->fromImage(proxy->m_data->toImage(), Qt::NoOpaqueDetection);
        delete proxy->m_data;
        proxy->m_data = newData;
        proxy->readBackInfo();
    }

    for (int i = 0; i < m_windowSurfaces.size(); ++i) {
        QRuntimeWindowSurface *proxy = m_windowSurfaces.at(i);
        QWidget *widget = proxy->m_windowSurface->window();

        if(m_windowSurfaceDestroyPolicy == DestroyImmediately) {
            delete proxy->m_windowSurface;
            proxy->m_pendingWindowSurface = 0;
        } else {
            proxy->m_pendingWindowSurface = proxy->m_windowSurface;
        }

        proxy->m_windowSurface = m_graphicsSystem->createWindowSurface(widget);
        qt_widget_private(widget)->invalidateBuffer(widget->rect());
    }
}

void QRuntimeGraphicsSystem::removePixmapData(QRuntimePixmapData *pixmapData) const
{
    int index = m_pixmapDatas.lastIndexOf(pixmapData);
    m_pixmapDatas.removeAt(index);
    decreaseMemoryUsage(pixmapData->memoryUsage(), true);
}

void QRuntimeGraphicsSystem::removeWindowSurface(QRuntimeWindowSurface *windowSurface) const
{
    int index = m_windowSurfaces.lastIndexOf(windowSurface);
    m_windowSurfaces.removeAt(index);
    decreaseMemoryUsage(windowSurface->memoryUsage(), true);
}

void QRuntimeGraphicsSystem::increaseMemoryUsage(uint amount) const
{
    m_memoryUsage += amount;

    if (m_graphicsSystemChangeMemoryLimit &&
        m_memoryUsage < m_graphicsSystemChangeMemoryLimit) {

        QRuntimeGraphicsSystem *gs = const_cast<QRuntimeGraphicsSystem*>(this);
        QDeferredGraphicsSystemChange *deferredChange =
                    new QDeferredGraphicsSystemChange(gs, m_pendingGraphicsSystemName);
        deferredChange->launch();
    }
}

void QRuntimeGraphicsSystem::decreaseMemoryUsage(uint amount, bool persistent) const
{
    m_memoryUsage -= amount;

    if (persistent && m_graphicsSystemChangeMemoryLimit &&
        m_memoryUsage < m_graphicsSystemChangeMemoryLimit) {

        QRuntimeGraphicsSystem *gs = const_cast<QRuntimeGraphicsSystem*>(this);
        QDeferredGraphicsSystemChange *deferredChange =
                    new QDeferredGraphicsSystemChange(gs, m_pendingGraphicsSystemName);
        deferredChange->launch();
    }
}

#include "qgraphicssystem_runtime.moc"

QT_END_NAMESPACE
