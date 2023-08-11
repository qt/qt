/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qwindowformat_qpa.h"

#include "qplatformglcontext_qpa.h"

#include <QtCore/QDebug>

class QWindowFormatPrivate
{
public:
    QWindowFormatPrivate()
        : ref(1)
        , opts(QWindowFormat::DoubleBuffer | QWindowFormat::WindowSurface)
        , redBufferSize(-1)
        , greenBufferSize(-1)
        , blueBufferSize(-1)
        , alphaBufferSize(-1)
        , depthSize(-1)
        , stencilSize(-1)
        , swapBehavior(QWindowFormat::DefaultSwapBehavior)
        , numSamples(-1)
        , sharedContext(0)
    {
    }

    QWindowFormatPrivate(const QWindowFormatPrivate *other)
        : ref(1),
          opts(other->opts),
          redBufferSize(other->redBufferSize),
          greenBufferSize(other->greenBufferSize),
          blueBufferSize(other->blueBufferSize),
          alphaBufferSize(other->alphaBufferSize),
          depthSize(other->depthSize),
          stencilSize(other->stencilSize),
          swapBehavior(other->swapBehavior),
          numSamples(other->numSamples),
          sharedContext(other->sharedContext)
    {
    }
    QAtomicInt ref;
    QWindowFormat::FormatOptions opts;
    int redBufferSize;
    int greenBufferSize;
    int blueBufferSize;
    int alphaBufferSize;
    int depthSize;
    int stencilSize;
    QWindowFormat::SwapBehavior swapBehavior;
    int numSamples;
    QWindowContext *sharedContext;
};

QWindowFormat::QWindowFormat()
{
    d = new QWindowFormatPrivate;
}

QWindowFormat::QWindowFormat(QWindowFormat::FormatOptions options)
{
    d = new QWindowFormatPrivate;
    d->opts = options;
}

/*!
    \internal
*/
void QWindowFormat::detach()
{
    if (d->ref != 1) {
        QWindowFormatPrivate *newd = new QWindowFormatPrivate(d);
        if (!d->ref.deref())
            delete d;
        d = newd;
    }
}

/*!
    Constructs a copy of \a other.
*/

QWindowFormat::QWindowFormat(const QWindowFormat &other)
{
    d = other.d;
    d->ref.ref();
}

/*!
    Assigns \a other to this object.
*/

QWindowFormat &QWindowFormat::operator=(const QWindowFormat &other)
{
    if (d != other.d) {
        other.d->ref.ref();
        if (!d->ref.deref())
            delete d;
        d = other.d;
    }
    return *this;
}

/*!
    Destroys the QWindowFormat.
*/
QWindowFormat::~QWindowFormat()
{
    if (!d->ref.deref())
        delete d;
}

/*!
    \fn bool QWindowFormat::stereo() const

    Returns true if stereo buffering is enabled; otherwise returns
    false. Stereo buffering is disabled by default.

    \sa setStereo()
*/

/*!
    If \a enable is true enables stereo buffering; otherwise disables
    stereo buffering.

    Stereo buffering is disabled by default.

    Stereo buffering provides extra color buffers to generate left-eye
    and right-eye images.

    \sa stereo()
*/

void QWindowFormat::setStereo(bool enable)
{
    if (enable) {
        d->opts |= QWindowFormat::StereoBuffers;
    } else {
        d->opts &= ~QWindowFormat::StereoBuffers;
    }
}

/*!
    Returns the number of samples per pixel when multisampling is
    enabled. By default, the highest number of samples that is
    available is used.

    \sa setSampleBuffers(), sampleBuffers(), setSamples()
*/
int QWindowFormat::samples() const
{
   return d->numSamples;
}

/*!
    Set the preferred number of samples per pixel when multisampling
    is enabled to \a numSamples. By default, the highest number of
    samples available is used.

    \sa setSampleBuffers(), sampleBuffers(), samples()
*/
void QWindowFormat::setSamples(int numSamples)
{
    detach();
    d->numSamples = numSamples;
}



void QWindowFormat::setSharedContext(QWindowContext *context)
{
    d->sharedContext = context;
}

QWindowContext *QWindowFormat::sharedContext() const
{
    return d->sharedContext;
}

/*!
    \fn bool QWindowFormat::hasWindowSurface() const

    Returns true if the corresponding widget has an instance of QWindowSurface.

    Otherwise returns false.

    WindowSurface is enabled by default.
*/

void QWindowFormat::setWindowSurface(bool enable)
{
    if (enable) {
        d->opts |= QWindowFormat::WindowSurface;
    } else {
        d->opts &= ~QWindowFormat::WindowSurface;
}

/*!
    Sets the format option to \a opt.

    \sa testOption()
*/

void QWindowFormat::setOption(QWindowFormat::FormatOptions opt)
{
    detach();
    d->opts |= opt;
}

/*!
    Returns true if format option \a opt is set; otherwise returns false.

    \sa setOption()
*/

bool QWindowFormat::testOption(QWindowFormat::FormatOptions opt) const
{
    return d->opts & opt;
}

/*!
    Set the minimum depth buffer size to \a size.

    \sa depthBufferSize(), setDepth(), depth()
*/
void QWindowFormat::setDepthBufferSize(int size)
{
    detach();
    d->depthSize = size;
}

/*!
    Returns the depth buffer size.

    \sa depth(), setDepth(), setDepthBufferSize()
*/
int QWindowFormat::depthBufferSize() const
{
   return d->depthSize;
}

void QWindowFormat::setSwapBehavior(SwapBehavior behavior)
{
    d->swapBehavior = behavior;
}

QWindowFormat::SwapBehavior QWindowFormat::swapBehavior() const
{
    return d->swapBehavior;
}

bool QWindowFormat::hasAlpha() const
{
    return d->alphaBufferSize > 0;
}

/*!
    Set the preferred stencil buffer size to \a size.

    \sa stencilBufferSize(), setStencil(), stencil()
*/
void QWindowFormat::setStencilBufferSize(int size)
{
    detach();
    d->stencilSize = size;
}

/*!
    Returns the stencil buffer size.

    \sa stencil(), setStencil(), setStencilBufferSize()
*/
int QWindowFormat::stencilBufferSize() const
{
   return d->stencilSize;
}

int QWindowFormat::redBufferSize() const
{
    return d->redBufferSize;
}

int QWindowFormat::greenBufferSize() const
{
    return d->greenBufferSize;
}

int QWindowFormat::blueBufferSize() const
{
    return d->blueBufferSize;
}

int QWindowFormat::alphaBufferSize() const
{
    return d->alphaBufferSize;
}

void QWindowFormat::setRedBufferSize(int size)
{
    d->redBufferSize = size;
}

void QWindowFormat::setGreenBufferSize(int size)
{
    d->greenBufferSize = size;
}

void QWindowFormat::setBlueBufferSize(int size)
{
    d->blueBufferSize = size;
}

void QWindowFormat::setAlphaBufferSize(int size)
{
    d->alphaBufferSize = size;
}

bool operator==(const QWindowFormat& a, const QWindowFormat& b)
{
    return (a.d == b.d) || ((int) a.d->opts == (int) b.d->opts
        && a.d->stencilSize == b.d->stencilSize
        && a.d->redBufferSize == b.d->redBufferSize
        && a.d->greenBufferSize == b.d->greenBufferSize
        && a.d->blueBufferSize == b.d->blueBufferSize
        && a.d->alphaBufferSize == b.d->alphaBufferSize
        && a.d->depthSize == b.d->depthSize
        && a.d->numSamples == b.d->numSamples
        && a.d->swapBehavior == b.d->swapBehavior
        && a.d->sharedContext == b.d->sharedContext);
}


/*!
    \since 4.8

    Returns false if all the options of the two QWindowFormat objects
    \a a and \a b are equal; otherwise returns true.

    \relates QWindowFormat
*/

bool operator!=(const QWindowFormat& a, const QWindowFormat& b)
{
    return !(a == b);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QWindowFormat &f)
{
    const QWindowFormatPrivate * const d = f.d;

    dbg.nospace() << "QWindowFormat("
                  << "options " << d->opts
                  << ", depthBufferSize " << d->depthSize
                  << ", redBufferSize " << d->redBufferSize
                  << ", greenBufferSize " << d->greenBufferSize
                  << ", blueBufferSize " << d->blueBufferSize
                  << ", alphaBufferSize " << d->alphaBufferSize
                  << ", stencilBufferSize " << d->stencilSize
                  << ", samples " << d->numSamples
                  << ", swapBehavior " << d->swapBehavior
                  << ", sharedContext " << d->sharedContext
                  << ')';

    return dbg.space();
}
#endif
