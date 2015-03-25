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

#include <QtCore/qdebug.h>
#include <QtCore/qstringlist.h>

#include "qeglproperties_p.h"
#include "qeglcontext_p.h"

QT_BEGIN_NAMESPACE

static void noegl(const char *fn)
{
    qWarning() << fn << " called, but Qt configured without EGL" << endl;
}

#define NOEGL noegl(__FUNCTION__);

// Initialize a property block.
QEglProperties::QEglProperties()
{
    NOEGL
}

QEglProperties::QEglProperties(EGLConfig cfg)
{
    Q_UNUSED(cfg)
    NOEGL
}

// Fetch the current value associated with a property.
int QEglProperties::value(int name) const
{
    Q_UNUSED(name)
    NOEGL
    return 0;
}

// Set the value associated with a property, replacing an existing
// value if there is one.
void QEglProperties::setValue(int name, int value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)
    NOEGL
}

// Remove a property value.  Returns false if the property is not present.
bool QEglProperties::removeValue(int name)
{
    Q_UNUSED(name)
    NOEGL
    return false;
}

void QEglProperties::setDeviceType(int devType)
{
    Q_UNUSED(devType)
    NOEGL
}


// Sets the red, green, blue, and alpha sizes based on a pixel format.
// Normally used to match a configuration request to the screen format.
void QEglProperties::setPixelFormat(QImage::Format pixelFormat)
{
    Q_UNUSED(pixelFormat)
    NOEGL

}

void QEglProperties::setRenderableType(QEgl::API api)
{
    Q_UNUSED(api);
    NOEGL
}

// Reduce the complexity of a configuration request to ask for less
// because the previous request did not result in success.  Returns
// true if the complexity was reduced, or false if no further
// reductions in complexity are possible.
bool QEglProperties::reduceConfiguration()
{
    NOEGL
    return false;
}

static void addTag(QString& str, const QString& tag)
{
    Q_UNUSED(str)
    Q_UNUSED(tag)
    NOEGL
}

// Convert a property list to a string suitable for debug output.
QString QEglProperties::toString() const
{
    NOEGL
    return QString();
}

void QEglProperties::setPaintDeviceFormat(QPaintDevice *dev)
{
    Q_UNUSED(dev)
    NOEGL
}

QT_END_NAMESPACE


