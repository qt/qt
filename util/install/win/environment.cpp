/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the utils of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
#include "environment.h"
#include <qnamespace.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qsettings.h>
#if defined(Q_OS_WIN32)
#include <windows.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <errno.h>

QString QEnvironment::getEnv( const QString &varName, int envBlock )
{
#if defined(Q_OS_WIN32)
    OSVERSIONINFOA osvi;
    HKEY hkKey;
    bool isWinMe = false;

    if( envBlock & GlobalEnv )
	hkKey = HKEY_LOCAL_MACHINE;
    else
	hkKey = HKEY_CURRENT_USER;

    osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFOA );
    GetVersionExA( &osvi );
    if( int( qWinVersion() ) & int( Qt::WV_98 ) ) {
	if( osvi.dwMinorVersion == 90 )
	    isWinMe = true;
    }

    if( envBlock & PersistentEnv ) {
	if( int( qWinVersion() ) & int( Qt::WV_NT_based ) ) {
	    HKEY env;
	    QByteArray buffer;
	    DWORD size( 0 );
	    QString value;

	    if( RegOpenKeyExW( hkKey, L"Environment", 0, KEY_READ, &env ) == ERROR_SUCCESS ) {
		RegQueryValueExW( env, (const wchar_t*) varName.ucs2(), 0, NULL, NULL, &size );
		buffer.resize( size );
		RegQueryValueExW( env, (const wchar_t*) varName.ucs2(), 0, NULL, (unsigned char*)buffer.data(), &size );
		for( int i = 0; i < ( int )buffer.size(); i += 2 ) {
		    QChar c( buffer[ i ], buffer[ i + 1 ] );
		    if( !c.isNull() )
			value += c;
		}
		RegCloseKey( env );
		return value;
	    }
	    else {
		return QString();
	    }
	}
	else { //  Win 9x
	    // Persistent environment on Windows 9x is not fully supported yet.
	    return QString( getenv( varName ) );
	}
    }
    if( envBlock & LocalEnv ) {
	if( int( qWinVersion() ) & int( Qt::WV_NT_based ) ) {
	    int size = GetEnvironmentVariableW( (TCHAR*)varName.ucs2(), 0, 0 );
	    if ( size == 0 )
		return QString();
	    TCHAR *data = new TCHAR[ size ];
	    GetEnvironmentVariableW( (TCHAR*)varName.ucs2(), data, size );
	    QString ret = QString::fromUcs2( data );
	    delete[] data;
	    return ret;
	} else {
	    QCString varNameL = varName.local8Bit();
	    int size = GetEnvironmentVariableA( varNameL.data(), 0, 0 );
	    if ( size == 0 )
		return QString();
	    char *data = new char[ size ];
	    GetEnvironmentVariableA( varNameL.data(), data, size );
	    QString ret = QString::fromLocal8Bit( data );
	    delete[] data;
	    return ret;
	}
    }
#elif defined(Q_OS_UNIX)
// Persistent environment on Unix is not supported yet.
    if( envBlock & LocalEnv ) {
	return QString( getenv( varName ) );
    }
#endif
    return QString();
}

void QEnvironment::putEnv( const QString &varName, const QString &varValue, int envBlock )
{
#if defined(Q_OS_WIN32)
    OSVERSIONINFOA osvi;
    HKEY hkKey;
    bool isWinMe = false;

    if( envBlock & GlobalEnv )
	hkKey = HKEY_LOCAL_MACHINE;
    else
	hkKey = HKEY_CURRENT_USER;

    osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFOA );
    GetVersionExA( &osvi );
    if( int( qWinVersion() ) & int( Qt::WV_98 ) ) {
	if( osvi.dwMinorVersion == 90 )
	    isWinMe = true;
    }

    if( envBlock & PersistentEnv ) {
	if( int( qWinVersion() ) & int( Qt::WV_NT_based ) ) {

	    HKEY env;
	    QByteArray buffer;

	    buffer.resize( varValue.length() * 2 + 2 );
	    const QChar *data = varValue.unicode();
	    int i;
	    for ( i = 0; i < (int)varValue.length(); ++i ) {
		buffer[ 2*i ] = data[ i ].cell();
		buffer[ (2*i)+1 ] = data[ i ].row();
	    }
	    buffer[ (2*i) ] = 0;
	    buffer[ (2*i)+1 ] = 0;

	    if( RegOpenKeyExW( hkKey, L"Environment", 0, KEY_WRITE, &env ) == ERROR_SUCCESS ) {
		RegSetValueExW( env, (const wchar_t*) varName.ucs2(), 0, REG_EXPAND_SZ, (const unsigned char*)buffer.data(), buffer.size() );
		RegCloseKey( env );
	    }
	    DWORD res;
	    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, LPARAM("Environment"), SMTO_ABORTIFHUNG | SMTO_BLOCK, 1, &res);
	}
	else { // Win 9x
	    QFile autoexec( "c:\\autoexec.bat" );
	    QTextStream ostream( &autoexec );
	    ostream.setEncoding( QTextStream::Locale );

	    if( autoexec.open( IO_Append | IO_ReadWrite | IO_Translate ) ) {
		ostream << "set " << varName << "=" << varValue << endl;
		autoexec.close();
	    }
	}
    }
    if( envBlock & LocalEnv ) {
	if( int( qWinVersion() ) & int( Qt::WV_NT_based ) ) {
	    SetEnvironmentVariableW( (TCHAR*)varName.ucs2(), (const wchar_t*) varValue.ucs2() );
	} else {
	    SetEnvironmentVariableA( varName.local8Bit(), varValue.local8Bit() );
	}
    }
#else
    if( envBlock & LocalEnv )
	setenv( varName, varValue, 1 );
#endif
}

void QEnvironment::removeEnv( const QString &varName, int envBlock )
{
#if defined(Q_OS_WIN32)
    HKEY hkKey;
    if( envBlock & GlobalEnv )
	hkKey = HKEY_LOCAL_MACHINE;
    else
	hkKey = HKEY_CURRENT_USER;

    if( envBlock & PersistentEnv ) {
	if( int( qWinVersion() ) & int( Qt::WV_NT_based ) ) {
	    HKEY env;
	    if( RegOpenKeyExW( hkKey, L"Environment", 0, KEY_WRITE, &env ) == ERROR_SUCCESS ) {
		RegDeleteValue( env, (const wchar_t*) varName.ucs2() );
		RegCloseKey( env );
	    }
	}
	else { // Win 9x
	    QFile autoexec( "c:\\autoexec.bat" );
	    QTextStream ostream( &autoexec );
	    ostream.setEncoding( QTextStream::Locale );

	    if( autoexec.open( IO_Append | IO_ReadWrite | IO_Translate ) ) {
		ostream << "set " << varName << "=" << endl;
		autoexec.close();
	    }
	}
    }

    if( envBlock & LocalEnv ) {
	if( int( qWinVersion() ) & int( Qt::WV_NT_based ) ) {
	    SetEnvironmentVariableW( (TCHAR*)varName.ucs2(), 0 );
	} else {
	    SetEnvironmentVariableA( varName.local8Bit(), 0 );
	}
    }
#endif
}

#if defined(Q_OS_WIN32)
void QEnvironment::recordUninstall( const QString &displayName, const QString &cmdString )
{
    QSettings settings;
    settings.insertSearchPath(QSettings::Windows, "/Microsoft/Windows/CurrentVersion/Uninstall");
    settings.beginGroup("/" + displayName);
    settings.writeEntry("/DisplayName", displayName);
    settings.writeEntry("/Publisher", "Nokia Corporation and/or its subsidiary(-ies)");
    settings.writeEntry("/URLInfoAbout", "http://qtsoftware.com");
    settings.writeEntry("/HelpLink", "http://qtsoftware.com/support");
    settings.writeEntry("/UninstallString", cmdString);
    settings.endGroup();
}

void QEnvironment::removeUninstall( const QString &displayName )
{
    QSettings settings;
    settings.insertSearchPath(QSettings::Windows, "/Microsoft/Windows/CurrentVersion/Uninstall");
    settings.beginGroup("/" + displayName);
    settings.removeEntry("/DisplayName");
    settings.removeEntry("/Publisher");
    settings.removeEntry("/URLInfoAbout");
    settings.removeEntry("/DisplayVersion");
    settings.removeEntry("/HelpLink");
    settings.removeEntry("/UninstallString");
    settings.removeEntry("/.");
    settings.endGroup();
}

QString QEnvironment::getRegistryString( const QString &keyName, const QString &valueName, int scope )
{
    QString value;
    HKEY scopeKeys[] = { HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE };
    HKEY key;
    DWORD valueSize( 0 );
    QByteArray buffer, expBuffer;

    if( int( qWinVersion() ) & int(Qt::WV_NT_based) ) {
	if( RegOpenKeyExW( scopeKeys[ scope ], (const wchar_t*) keyName.ucs2(), 0, KEY_READ, &key ) == ERROR_SUCCESS ) {
	    if( RegQueryValueExW( key, (const wchar_t*) valueName.ucs2(), NULL, NULL, NULL, &valueSize ) == ERROR_SUCCESS ) {
		buffer.resize( valueSize );
		if( RegQueryValueExW( key, (const wchar_t*) valueName.ucs2(), NULL, NULL, (unsigned char*)buffer.data(), &valueSize ) == ERROR_SUCCESS ) {
		    valueSize = ExpandEnvironmentStringsW( (WCHAR*)buffer.data(), NULL, 0 );
		    expBuffer.resize( valueSize * 2 );
		    ExpandEnvironmentStringsW( (WCHAR*)buffer.data(), (WCHAR*)expBuffer.data(), valueSize );
		    for( int i = 0; i < ( int )expBuffer.size(); i += 2 ) {
			QChar c( expBuffer[ i ], expBuffer[ i + 1 ] );
			if ( !c.isNull() )
			    value += c;
		    }
		}
	    }
	    RegCloseKey( key );
	}
    }
    else {
	if( RegOpenKeyExA( scopeKeys[ scope ], keyName.local8Bit(), 0, KEY_READ, &key ) == ERROR_SUCCESS ) {
	    if( RegQueryValueExA( key, valueName.local8Bit(), NULL, NULL, NULL, &valueSize ) == ERROR_SUCCESS ) {
		buffer.resize( valueSize );
		if( RegQueryValueExA( key, valueName.local8Bit(), NULL, NULL, (unsigned char*)buffer.data(), &valueSize ) == ERROR_SUCCESS ) {
		    valueSize = ExpandEnvironmentStringsA( buffer.data(), NULL, 0 );
		    expBuffer.resize( valueSize );
		    ExpandEnvironmentStringsA( buffer.data(), expBuffer.data(), valueSize );
		    value = expBuffer.data();
		}
	    }
	    RegCloseKey( key );
	}
    }
    return value;
}
#endif

QString QEnvironment::getTempPath()
{
#if defined(Q_OS_WIN32)
    DWORD tmpSize;
    QByteArray tmp;
    QString tmpPath;

    if( int( qWinVersion() ) & int( Qt::WV_NT_based ) ) {
	tmpSize = GetTempPathW( 0, NULL );
	tmp.resize( tmpSize * 2 );
	GetTempPathW( tmpSize, (WCHAR*)tmp.data() );
	for( int i = 0; i < ( int )tmp.size(); i += 2 ) {
	    QChar c( tmp[ i ], tmp[ i + 1 ] );
	    if( !c.isNull() )
		tmpPath += c;
	}
    }
    else {
	tmpSize = GetTempPathA( 0, NULL );
	tmp.resize( tmpSize * 2 );
	GetTempPathA( tmpSize, tmp.data() );
	tmpPath = tmp.data();
    }
#elif defined(Q_OS_UNIX)
    QString tmpPath = "/tmp";
#endif
    return tmpPath;
}

QString QEnvironment::getLastError()
{
    return strerror( errno );
}

QString QEnvironment::getFSFileName( const QString& fileName )
{
#if defined(Q_OS_WIN32)
    QByteArray buffer( MAX_PATH );
    QString tmp( fileName );

    GetVolumeInformationA( fileName.left( fileName.find( '\\' ) + 1 ).local8Bit(), NULL, NULL, NULL, NULL, NULL, buffer.data(), buffer.size() );
    if( QString( buffer.data() ) != "NTFS" ) {
	DWORD dw;
	dw = GetShortPathNameA( fileName.local8Bit(), (char*)buffer.data(), buffer.size() );
	if( dw > 0 )
	    tmp = buffer.data();
    }
#elif defined(Q_OS_UNIX)
    QString tmp( fileName );
#endif
    return tmp;
}
