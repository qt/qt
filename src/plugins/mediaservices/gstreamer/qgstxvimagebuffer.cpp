/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include <QtCore/qdebug.h>
#include <QtCore/qthread.h>
#include <QtCore/qvariant.h>
#include <QtGui/qx11info_x11.h>

#include "qgstxvimagebuffer.h"
#include "qvideosurfacegstsink.h"


QT_BEGIN_NAMESPACE

GstBufferClass *QGstXvImageBuffer::parent_class = NULL;

GType QGstXvImageBuffer::get_type(void)
{
    static GType buffer_type = 0;

    if (buffer_type == 0) {
        static const GTypeInfo buffer_info = {
            sizeof (GstBufferClass),
            NULL,
            NULL,
            QGstXvImageBuffer::class_init,
            NULL,
            NULL,
            sizeof(QGstXvImageBuffer),
            0,
            (GInstanceInitFunc)QGstXvImageBuffer::buffer_init,
            NULL
        };
        buffer_type = g_type_register_static(GST_TYPE_BUFFER,
                                             "QGstXvImageBuffer", &buffer_info, GTypeFlags(0));
    }
    return buffer_type;
}

void QGstXvImageBuffer::class_init(gpointer g_class, gpointer class_data)
{
    Q_UNUSED(class_data);
    GST_MINI_OBJECT_CLASS(g_class)->finalize =
            (GstMiniObjectFinalizeFunction)buffer_finalize;
    parent_class = (GstBufferClass*)g_type_class_peek_parent(g_class);
}

void QGstXvImageBuffer::buffer_init(QGstXvImageBuffer *xvImage, gpointer g_class)
{
    Q_UNUSED(g_class);
    xvImage->pool = 0;
    xvImage->shmInfo.shmaddr = ((char *) -1);
    xvImage->shmInfo.shmid = -1;
    xvImage->markedForDeletion = false;
}

void QGstXvImageBuffer::buffer_finalize(QGstXvImageBuffer * xvImage)
{
    if (xvImage->pool) {
        if (xvImage->markedForDeletion)
            xvImage->pool->destroyBuffer(xvImage);
        else
            xvImage->pool->recycleBuffer(xvImage);
    }
}


QGstXvImageBufferPool::QGstXvImageBufferPool(QObject *parent)
    :QObject(parent)
{
}

QGstXvImageBufferPool::~QGstXvImageBufferPool()
{
}

bool QGstXvImageBufferPool::isFormatSupported(const QVideoSurfaceFormat &surfaceFormat)
{
    bool ok = true;
    surfaceFormat.property("portId").toULongLong(&ok);
    if (!ok)
        return false;

    int xvFormatId = surfaceFormat.property("xvFormatId").toInt(&ok);
    if (!ok || xvFormatId < 0)
        return false;

    int dataSize = surfaceFormat.property("dataSize").toInt(&ok);
    if (!ok || dataSize<=0)
        return false;

    return true;
}

QGstXvImageBuffer *QGstXvImageBufferPool::takeBuffer(const QVideoSurfaceFormat &format, GstCaps *caps)
{
    m_poolMutex.lock();

    m_caps = caps;
    if (format != m_format) {
        doClear();
        m_format = format;
    }


    if (m_pool.isEmpty()) {
        //qDebug() << "QGstXvImageBufferPool::takeBuffer: no buffer available, allocate the new one";
        if (QThread::currentThread() == thread()) {
            m_poolMutex.unlock();
            queuedAlloc();
            m_poolMutex.lock();
        } else {
            QMetaObject::invokeMethod(this, "queuedAlloc", Qt::QueuedConnection);
            m_allocWaitCondition.wait(&m_poolMutex, 300);
        }
    }
    QGstXvImageBuffer *res = 0;

    if (!m_pool.isEmpty()) {
        res = m_pool.takeLast();
    }

    m_poolMutex.unlock();

    return res;
}

void QGstXvImageBufferPool::queuedAlloc()
{
    QMutexLocker lock(&m_poolMutex);

    Q_ASSERT(QThread::currentThread() == thread());

    QGstXvImageBuffer *xvBuffer = (QGstXvImageBuffer *)gst_mini_object_new(QGstXvImageBuffer::get_type());

    quint64 portId = m_format.property("portId").toULongLong();
    int xvFormatId = m_format.property("xvFormatId").toInt();

    xvBuffer->xvImage = XvShmCreateImage(
            QX11Info::display(),
            portId,
            xvFormatId,
            0,
            m_format.frameWidth(),
            m_format.frameHeight(),
            &xvBuffer->shmInfo
            );

    if (!xvBuffer->xvImage) {
//        qDebug() << "QGstXvImageBufferPool: XvShmCreateImage failed";
        m_allocWaitCondition.wakeOne();
        return;
    }

    xvBuffer->shmInfo.shmid = shmget(IPC_PRIVATE, xvBuffer->xvImage->data_size, IPC_CREAT | 0777);
    xvBuffer->shmInfo.shmaddr = xvBuffer->xvImage->data = (char*)shmat(xvBuffer->shmInfo.shmid, 0, 0);
    xvBuffer->shmInfo.readOnly = False;

    if (!XShmAttach(QX11Info::display(), &xvBuffer->shmInfo)) {
//        qDebug() << "QGstXvImageBufferPool: XShmAttach failed";
        m_allocWaitCondition.wakeOne();
        return;
    }

    shmctl (xvBuffer->shmInfo.shmid, IPC_RMID, NULL);

    xvBuffer->pool = this;
    GST_MINI_OBJECT_CAST(xvBuffer)->flags = 0;
    gst_buffer_set_caps(GST_BUFFER_CAST(xvBuffer), m_caps);
    GST_BUFFER_DATA(xvBuffer) = (uchar*)xvBuffer->xvImage->data;
    GST_BUFFER_SIZE(xvBuffer) = xvBuffer->xvImage->data_size;

    m_allBuffers.append(xvBuffer);
    m_pool.append(xvBuffer);

    m_allocWaitCondition.wakeOne();
}


void QGstXvImageBufferPool::clear()
{
    QMutexLocker lock(&m_poolMutex);
    doClear();
}

void QGstXvImageBufferPool::doClear()
{
    foreach (QGstXvImageBuffer *xvBuffer, m_allBuffers) {
        xvBuffer->markedForDeletion = true;
    }
    m_allBuffers.clear();

    foreach (QGstXvImageBuffer *xvBuffer, m_pool) {
        gst_buffer_unref(GST_BUFFER(xvBuffer));
    }
    m_pool.clear();

    m_format = QVideoSurfaceFormat();
}

void QGstXvImageBufferPool::queuedDestroy()
{
    QMutexLocker lock(&m_destroyMutex);

    foreach(XvShmImage xvImage, m_imagesToDestroy) {
        if (xvImage.shmInfo.shmaddr != ((void *) -1)) {
            XShmDetach(QX11Info::display(), &xvImage.shmInfo);
            XSync(QX11Info::display(), false);

            shmdt(xvImage.shmInfo.shmaddr);
        }

        if (xvImage.xvImage)
            XFree(xvImage.xvImage);
    }

    m_imagesToDestroy.clear();

    XSync(QX11Info::display(), false);
}

void QGstXvImageBufferPool::recycleBuffer(QGstXvImageBuffer *xvBuffer)
{
    QMutexLocker lock(&m_poolMutex);
    gst_buffer_ref(GST_BUFFER_CAST(xvBuffer));
    m_pool.append(xvBuffer);
}

void QGstXvImageBufferPool::destroyBuffer(QGstXvImageBuffer *xvBuffer)
{
    XvShmImage imageToDestroy;
    imageToDestroy.xvImage = xvBuffer->xvImage;
    imageToDestroy.shmInfo = xvBuffer->shmInfo;

    m_destroyMutex.lock();
    m_imagesToDestroy.append(imageToDestroy);
    m_destroyMutex.unlock();

    if (m_imagesToDestroy.size() == 1)
        QMetaObject::invokeMethod(this, "queuedDestroy", Qt::QueuedConnection);
}

QT_END_NAMESPACE

