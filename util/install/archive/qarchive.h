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
#ifndef QARCHIVE_H
#define QARCHIVE_H

#include <qstring.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qobject.h>

class QArchiveHeader
{
public:
    QArchiveHeader( uint feat, const QString& desc=QString(), uchar mayorVer = 1, uchar minorVer = 0 )
	: _features(feat), _description(desc), _mayorVersion(mayorVer), _minorVersion(minorVer)
    {}
    
    uint features() const
    { return _features; }

    QString description() const
    { return _description; }

    uchar mayorVersion() const
    { return _mayorVersion; }

    uchar minorVersion() const
    { return _minorVersion; }

    void addExtraData( const QString& key, const QString& value )
    { extraData.insert( key, value ); }

    QString findExtraData( const QString& key ) const
    { return extraData[key]; }

private:
    QArchiveHeader()
    {}

    uint _features;
    QString _description;
    uchar _mayorVersion;
    uchar _minorVersion;
    QMap<QString,QString> extraData;

    friend class QArchive;
};

class QArchive : public QObject
{
    Q_OBJECT
public:
    QArchive( const QString& archivePath = QString() );
    ~QArchive();

    void setPath( const QString& archivePath );
    void setVerbosity( int verbosity );

    bool symbolicLinks() const { return doSyms; }
    void setSymbolicLinks(bool b) { doSyms = b; }

    bool open( int mode );
    void close();
    bool isOpen() { return arcFile.isOpen(); }

    bool writeHeader( const QArchiveHeader header );
    bool writeFile( const QString& fileName, const QString& localPath = QString() );
    bool writeFileList( const QStringList fileList );
    bool writeDir( const QString& dirName, bool includeLastComponent = false, 
		   const QString& localPath = QString() );
    bool writeDirList( const QStringList dirList, bool includeLastComponent = true );

    QArchiveHeader* readArchiveHeader();
    QArchiveHeader* readArchiveHeader( QDataStream *inStream );

    bool readArchive( const QString &outpath, const QString &key = QString() );
    bool readArchive( QDataStream *inStream, const QString &outpath, const QString &key = QString() );
private:
    QFile arcFile;

    int bufferSize;
    int verbosityMode;
    uint doSyms : 1;
    bool setDirectory( const QString& dirName );
signals:
    void operationFeedback( const QString& );
    void operationFeedback( int );

public:
    enum {
	NoFeedback = 0x00,
	OnlyNames = 0x01,
	Verbose = 0x02,
	Source = 0x10,
	Destination = 0x20,
	Progress = 0x40
    };
};

#endif
