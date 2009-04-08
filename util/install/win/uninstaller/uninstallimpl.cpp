/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the utils of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "uninstallimpl.h"
#include <qsettings.h>
#include <qmessagebox.h>

UninstallDlgImpl::UninstallDlgImpl( QWidget* parent, const char* name, bool modal, WindowFlags f )
    : UninstallDlg( parent, name, modal, f )
{
}

UninstallDlgImpl::~UninstallDlgImpl()
{
}

void UninstallDlgImpl::cleanRegistry()
{
    cleanRegistryHelper( "/Trolltech/Qt" );
    cleanRegistryHelper( "/Trolltech/Qt Designer" );
    cleanRegistryHelper( "/Trolltech/Qt Assistant" );
    cleanRegistryHelper( "/Trolltech/Qt Linguist" );
}

void UninstallDlgImpl::cleanRegistryHelper( const QString& key )
{
    QSettings settings;
    QStringList::Iterator it;
    QStringList keys = settings.subkeyList( key );
    for ( it = keys.begin(); it != keys.end(); ++it ) {
        cleanRegistryHelper( key + "/" + *it );
    }
    QStringList entries = settings.entryList( key );
    for ( it = entries.begin(); it != entries.end(); ++it ) {
        settings.removeEntry( key + "/" + *it );
    }
    settings.removeEntry( key + "/." );
}
