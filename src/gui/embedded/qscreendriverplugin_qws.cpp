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

#include "qscreendriverplugin_qws.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_LIBRARY

/*!
    \class QScreenDriverPlugin
    \ingroup plugins
    \ingroup qws

    \brief The QScreenDriverPlugin class is an abstract base class for
    screen driver plugins in Qt for Embedded Linux.

    Note that this class is only available in \l{Qt for Embedded Linux}.

    \l{Qt for Embedded Linux} provides ready-made drivers for several screen
    protocols, see the \l{Qt for Embedded Linux Display Management}{display
    management} documentation for details. Custom screen drivers can be
    implemented by subclassing the QScreen class and creating a screen
    driver plugin.

    A screen driver plugin can be created by subclassing
    QScreenDriverPlugin and reimplementing the pure virtual keys() and
    create() functions. By exporting the derived class using the
    Q_EXPORT_PLUGIN2() macro, The default implementation of the
    QScreenDriverFactory class will automatically detect the plugin
    and load the driver into the server application at run-time.  See
    \l{How to Create Qt Plugins} for details.

    \sa QScreen, QScreenDriverFactory
*/

/*!
    \fn QStringList QScreenDriverPlugin::keys() const

    Implement this function to return the list of valid keys, i.e. the
    screen drivers supported by this plugin.

    \l{Qt for Embedded Linux} provides ready-made drivers for several screen
    protocols, see the \l{Qt for Embedded Linux Display Management}{display
    management} documentation for details.

    \sa create()
*/

/*!
    Constructs a screen driver plugin with the given \a parent.

    Note that this constructor is invoked automatically by the
    Q_EXPORT_PLUGIN2() macro, so there is no need for calling it
    explicitly.
*/
QScreenDriverPlugin::QScreenDriverPlugin(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroys this screen driver plugin.

    Note that Qt destroys a plugin automatically when it is no longer
    used, so there is no need for calling the destructor explicitly.
*/
QScreenDriverPlugin::~QScreenDriverPlugin()
{
}


/*!
    \fn QScreen* QScreenDriverPlugin::create(const QString &key, int displayId)

    Implement this function to create a driver matching the type
    specified by the given \a key and \a displayId parameters. Note
    that keys are case-insensitive.

    \sa keys()
*/

#endif // QT_NO_LIBRARY

QT_END_NAMESPACE
