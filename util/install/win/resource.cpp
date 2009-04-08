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
#include "resource.h"
#include <qfile.h>
#include <qfileinfo.h>
#include <qapplication.h>

#ifdef Q_OS_WIN32
#include <windows.h>
#endif

/*
   Tries to load the binary resource \a resourceName. If the resource is
   smaller than \a minimumSize, the resource is not loaded and isValid()
   returns false. isValid() returns also false when the loading failed.
 */
ResourceLoader::ResourceLoader( char *resourceName, int minimumSize )
{
#if defined(Q_OS_WIN32)
    valid = true;

    HMODULE hmodule = GetModuleHandle( 0 );
    // we don't need wide character versions
    HRSRC resource = FindResourceA( hmodule, resourceName, MAKEINTRESOURCEA( 10 ) );
    HGLOBAL hglobal = LoadResource( hmodule, resource );
    arSize = SizeofResource( hmodule, resource );
    if ( arSize == 0 ) {
	valid = false;
	return;
    }
    if ( arSize < minimumSize ) {
	valid = false;
	return;
    }
    arData = (char*)LockResource( hglobal );
    if ( arData == 0 ) {
	valid = false;
	return;
    }
    ba.setRawData( arData, arSize );
#elif defined(Q_OS_MAC)
    valid = false;
    arSize = 0;
    arData = 0;
    QFile f;
    QString appDir = qApp->argv()[0];
    int truncpos = appDir.findRev( "/Contents/MacOS/" );
    if (truncpos != -1)
	appDir.truncate( truncpos );
    QString path = appDir + "/Contents/Qt/";
    path += resourceName;
    f.setName( path );
    if (!f.open( IO_ReadOnly ))
	return;
    QFileInfo fi(f);
    arSize = fi.size();
    arData = new char[arSize];
    if (f.readBlock( arData, arSize ) != arSize)
    {
	delete[] arData;
	return;
    }
    ba.setRawData( arData, arSize );
    valid = true;
    return;
#endif
}

ResourceLoader::~ResourceLoader()
{
    if ( isValid() )
	ba.resetRawData( arData, arSize );
#if defined(Q_OS_MAC)
    delete[] arData;
#endif
}

bool ResourceLoader::isValid() const
{
    return valid;
}

QByteArray ResourceLoader::data()
{
    return ba;
}


#if defined(Q_OS_WIN32)
ResourceSaver::ResourceSaver( const QString& appName )
    : applicationName(appName)
{
}

ResourceSaver::~ResourceSaver()
{
}

bool ResourceSaver::setData( char *resourceName, const QByteArray &data, QString *errorMessage )
{
    // we don't need wide character versions
    HANDLE hExe = BeginUpdateResourceA( applicationName.latin1(), false );
    if ( hExe == 0 ) {
	if ( errorMessage )
	    *errorMessage = QString("Could not load the executable %1.").arg(applicationName);
	return false;
    }
    if ( !UpdateResourceA(hExe,(char*)RT_RCDATA,resourceName,0,data.data(),data.count()) ) {
	EndUpdateResource( hExe, true );
	if ( errorMessage )
	    *errorMessage = QString("Could not update the executable %1.").arg(applicationName);
	return false;
    }
    if ( !EndUpdateResource(hExe,false) ) {
	if ( errorMessage )
	    *errorMessage = QString("Could not update the executable %1.").arg(applicationName);
	return false;
    }

    if ( errorMessage )
	*errorMessage = QString("Updated the executable %1.").arg(applicationName);
    return true;
}
#endif
