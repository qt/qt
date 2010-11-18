/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#include "qplatformglcontext_qpa.h"

#include <QtCore/QThreadStorage>
#include <QtCore/QThread>

#include <QDebug>

class QPlatformGLThreadContext
{
public:
    ~QPlatformGLThreadContext() {
        if (context)
            context->doneCurrent();
    }
    QPlatformGLContext *context;
};

static QThreadStorage<QPlatformGLThreadContext *> qplatformgl_context_storage;

class QPlatformGLContextPrivate
{
public:
    QPlatformGLContextPrivate(QPlatformWindow *platformWindow)
        :qGLContextHandle(0),platformWindow(platformWindow)
    {
    }

    virtual ~QPlatformGLContextPrivate()
    {
        //do not delete the QGLContext handle here as it is deleted in
        //QWidgetPrivate::deleteTLSysExtra()
    }
    void *qGLContextHandle;
    void (*qGLContextDeleteFunction)(void *handle);
    QPlatformWindow *platformWindow;
    static QPlatformGLContext *staticSharedContext;

    static void setCurrentContext(QPlatformGLContext *context);
};

QPlatformGLContext *QPlatformGLContextPrivate::staticSharedContext = 0;

void QPlatformGLContextPrivate::setCurrentContext(QPlatformGLContext *context)
{
    QPlatformGLThreadContext *threadContext = qplatformgl_context_storage.localData();
    if (!threadContext) {
        if (!QThread::currentThread()) {
            qWarning("No QTLS available. currentContext wont work");
            return;
        }
        threadContext = new QPlatformGLThreadContext;
        qplatformgl_context_storage.setLocalData(threadContext);
    }
    threadContext->context = context;
}

QPlatformWindow *QPlatformGLContext::platformWindow() const
{
    Q_D(const QPlatformGLContext);
    return d->platformWindow;
}

const QPlatformGLContext* QPlatformGLContext::currentContext()
{
    QPlatformGLThreadContext *threadContext = qplatformgl_context_storage.localData();
    if(threadContext) {
        return threadContext->context;
    }
    return 0;
}

QPlatformGLContext::QPlatformGLContext(QPlatformWindow *platformWindow)
    :d_ptr(new QPlatformGLContextPrivate(platformWindow))
{
}

QPlatformGLContext::~QPlatformGLContext()
{
    if (QPlatformGLContext::currentContext() == this) {
        doneCurrent();
    }

}

void QPlatformGLContext::setDefaultSharedContext(QPlatformGLContext *sharedContext)
{
    QPlatformGLContextPrivate::staticSharedContext = sharedContext;
}

const QPlatformGLContext *QPlatformGLContext::defaultSharedContext()
{
    return QPlatformGLContextPrivate::staticSharedContext;
}

void QPlatformGLContext::makeCurrent()
{
    QPlatformGLContextPrivate::setCurrentContext(this);
}

void QPlatformGLContext::doneCurrent()
{
    QPlatformGLContextPrivate::setCurrentContext(0);
}

void *QPlatformGLContext::qGLContextHandle() const
{
    Q_D(const QPlatformGLContext);
    return d->qGLContextHandle;
}

void QPlatformGLContext::setQGLContextHandle(void *handle,void (*qGLContextDeleteFunction)(void *))
{
    Q_D(QPlatformGLContext);
    d->qGLContextHandle = handle;
    d->qGLContextDeleteFunction = qGLContextDeleteFunction;
}

void QPlatformGLContext::deleteQGLContext()
{
    Q_D(QPlatformGLContext);
    d->qGLContextDeleteFunction(d->qGLContextHandle);
}
