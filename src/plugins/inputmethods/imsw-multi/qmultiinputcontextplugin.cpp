/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
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

/****************************************************************************
**
** Implementation of QMultiInputContextPlugin class
**
** Copyright (C) 2004 immodule for Qt Project.  All rights reserved.
**
** This file is written to contribute to Nokia Corporation and/or its subsidiary(-ies) under their own
** license. You may use this file under your Qt license. Following
** description is copied from their original file headers. Contact
** immodule-qt@freedesktop.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifndef QT_NO_IM
#include "qmultiinputcontext.h"
#include "qmultiinputcontextplugin.h"
#include <qinputcontextplugin.h>
#include <qstringlist.h>

QT_BEGIN_NAMESPACE

QMultiInputContextPlugin::QMultiInputContextPlugin()
{
}

QMultiInputContextPlugin::~QMultiInputContextPlugin()
{
}

QStringList QMultiInputContextPlugin::keys() const
{
    // input method switcher should named with "imsw-" prefix to
    // prevent to be listed in ordinary input method list.
    return QStringList( QLatin1String("imsw-multi") );
}

QInputContext *QMultiInputContextPlugin::create( const QString &key )
{
    if (key != QLatin1String("imsw-multi"))
        return 0;
    return new QMultiInputContext;
}

QStringList QMultiInputContextPlugin::languages( const QString & )
{
    return QStringList();
}

QString QMultiInputContextPlugin::displayName( const QString &key )
{
    if (key != QLatin1String("imsw-multi"))
        return QString();
    return tr( "Multiple input method switcher" );
}

QString QMultiInputContextPlugin::description( const QString &key )
{
    if (key != QLatin1String("imsw-multi"))
        return QString();
    return tr( "Multiple input method switcher that uses the context menu of the text widgets" );
}


Q_EXPORT_STATIC_PLUGIN(QMultiInputContextPlugin)
Q_EXPORT_PLUGIN2(qimsw_multi, QMultiInputContextPlugin)

QT_END_NAMESPACE

#endif
