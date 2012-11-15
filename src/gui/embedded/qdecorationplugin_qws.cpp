/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdecorationplugin_qws.h"
#include "qdecoration_qws.h"

QT_BEGIN_NAMESPACE

/*!
    \class QDecorationPlugin
    \ingroup qws
    \ingroup plugins

    \brief The QDecorationPlugin class is an abstract base class for
    window decoration plugins in Qt for Embedded Linux.

    Note that this class is only available in \l{Qt for Embedded Linux}.

    \l{Qt for Embedded Linux} provides three ready-made decoration styles: \c
    Default, \c Styled and \c Windows. Custom decorations can be
    implemented by subclassing the QDecoration class and creating a
    decoration plugin.

    A decoration plugin can be created by subclassing
    QDecorationPlugin and implementing the pure virtual keys() and
    create() functions. By exporting the derived class using the
    Q_EXPORT_PLUGIN2() macro, the default implementation of the
    QDecorationFactory class will automatically detect the plugin and
    load the driver into the application at run-time. See \l{How to
    Create Qt Plugins} for details.

    To actually apply a decoration, use the
    QApplication::qwsSetDecoration() function.

    \sa QDecoration, QDecorationFactory
*/

/*!
    \fn QStringList QDecorationPlugin::keys() const

    Returns the list of valid keys, i.e., the decorations supported by
    this plugin.

    \sa create()
*/

/*!
    \fn QDecoration *QDecorationPlugin::create(const QString &key)

    Creates a decoration matching the given \a key. Note that keys are
    case-insensitive.

    \sa keys()
*/

/*!
    Constructs a decoration plugin with the given \a parent.

    Note that this constructor is invoked automatically by the
    Q_EXPORT_PLUGIN2() macro, so there is no need for calling it
    explicitly.
*/
QDecorationPlugin::QDecorationPlugin(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroys the decoration plugin.

    Note that Qt destroys a plugin automatically when it is no longer
    used, so there is no need for calling the destructor explicitly.
*/
QDecorationPlugin::~QDecorationPlugin()
{
}

QT_END_NAMESPACE
