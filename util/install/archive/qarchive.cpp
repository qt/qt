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
#include "qarchive.h"
#include <qdatastream.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qapplication.h>
#include "../../../src/3rdparty/zlib/zlib.h"
#include "../keygen/keyinfo.h"
#ifdef Q_OS_UNIX
#  include <sys/stat.h>
#  include <unistd.h>
#  include <sys/types.h>
#  include <utime.h>
#endif

enum ChunkType {
    ChunkDirectory	= 0,
    ChunkFile		= 1,
    ChunkSymlink	= 2,
    ChunkBeginHeader	= 3,
    ChunkEndHeader	= 4
};

static bool createDir( const QString& fullPath )
{
    QStringList hierarchy = QStringList::split( QDir::separator(), fullPath );
    QString pathComponent, tmpPath;
    QDir dirTmp;
#ifdef Q_OS_UNIX
    dirTmp = "/";
#endif

    for( QStringList::Iterator it = hierarchy.begin(); it != hierarchy.end(); ++it ) {
	pathComponent = *it + QDir::separator();
	tmpPath += pathComponent;
	if (!dirTmp.exists(tmpPath) && !dirTmp.mkdir(tmpPath))
            return false;
    }
    return true;
}

QArchive::QArchive( const QString& archivePath )
{
    setPath( archivePath );

    bufferSize = 512 * 1024;
}

QArchive::~QArchive()
{
}

void QArchive::setPath( const QString& archivePath )
{
    QString fullName = archivePath;
    if( fullName.right( 4 ) != ".arq" )
	fullName += ".arq";
    arcFile.setName( fullName );
}

bool QArchive::open( int mode )
{
    switch( mode ) {
    case IO_ReadOnly:
	// Fallthrough intentional
    case IO_WriteOnly:
	if( arcFile.open( mode ) )
	    return true;
	break;
    }
    return false;
}

void QArchive::close()
{
    if( arcFile.isOpen() )
	arcFile.close();
}

bool QArchive::writeFile( const QString& fileName, const QString& localPath )
{
    if( arcFile.isOpen() ) {
	QDataStream outStream( &arcFile );
	QFileInfo fi( fileName );

	QFile inFile( fi.absFilePath() );
	QByteArray inBuffer;
	QByteArray outBuffer( bufferSize );
	z_stream ztream;
	bool continueCompressing;

	if(symbolicLinks() && fi.isSymLink()) {
	    outStream << (int)ChunkSymlink;
	    outStream << fi.fileName().latin1();
	    outStream << fi.readLink().latin1();
	} else if( inFile.open( IO_ReadOnly ) ) {
	    if( inBuffer.resize( fi.size() ) ) {
		outStream << (int)ChunkFile;
		outStream << fi.fileName().latin1();
		outStream << fi.lastModified();
		{
		    int perm = -1;
#ifdef Q_OS_UNIX
		    struct stat st;
		    if(!::stat(fi.filePath().latin1(), &st))
			perm = (int)st.st_mode;
#endif
		    outStream << perm;
		}
	        if( verbosityMode & Source )
		    emit operationFeedback( "Deflating " + fi.absFilePath() + "..." );
		else if( verbosityMode & Destination )
		    emit operationFeedback( "Deflating " + localPath + "/" + fi.fileName() + "..." );
		ztream.next_in = (unsigned char*)inBuffer.data();
		ztream.avail_in = inBuffer.size();
		ztream.total_in = 0;
		ztream.next_out = (unsigned char*)outBuffer.data();
		ztream.avail_out = outBuffer.size();
		ztream.total_out = 0;
		ztream.msg = NULL;
		ztream.zalloc = (alloc_func)NULL;
		ztream.zfree = (free_func)NULL;
		ztream.opaque = (voidpf)NULL;
		ztream.data_type = Z_BINARY;
		deflateInit( &ztream, 9 );
		if ( inBuffer.data() )
		    inFile.readBlock( inBuffer.data(), inBuffer.size() );

		continueCompressing = true;
		while( continueCompressing ) {
		    if(qApp)
			qApp->processEvents();
		    continueCompressing = ( deflate( &ztream, Z_FINISH ) == Z_OK );
		    if( !ztream.avail_out ) {
			if( !outBuffer.resize( outBuffer.size() + bufferSize ) )
			    qFatal( "Could not allocate compression buffer!" );
			ztream.next_out = (unsigned char*)&outBuffer.data()[ ztream.total_out ];
			ztream.avail_out = bufferSize;
		    }
		}

		emit operationFeedback( QString( "done. %1 => %2 (%3%)\n" )
					.arg( ztream.total_in )
					.arg( ztream.total_out )
					.arg( int(
					    double( ztream.total_out ) / double( ztream.total_in ) * 100 ) ) );
		deflateEnd( &ztream );
		// Now write the compressed data to the output
		outStream << ztream.total_out;
		outStream.writeRawBytes( outBuffer.data(), ztream.total_out );
	    }
	    inFile.close();
	    return true;
	} else {
	    return false;
	}
    }
    return false;
}

bool QArchive::setDirectory( const QString& dirName )
{
    if( arcFile.isOpen() ) {
	QString fullName = dirName;
	QDataStream outStream( &arcFile );
	if( fullName.right( 1 ) != "/" )
	    fullName += "/";
	outStream << (int)ChunkDirectory;
	outStream << fullName.latin1();
	return true;
    }
    return false;
}

bool QArchive::writeHeader( const QArchiveHeader header )
{
    if( arcFile.isOpen() ) {
	QDataStream outStream( &arcFile );
	outStream << (int)ChunkBeginHeader;
	outStream << header.mayorVersion();
	outStream << header.minorVersion();
	outStream << header.features();
	outStream << header.description();
	outStream << header.extraData;
	outStream << (int)ChunkEndHeader;
	return true;
    }
    return false;
}

bool QArchive::writeDir( const QString &dirName1, bool includeLastComponent, const QString &localPath1 )
{
    if( arcFile.isOpen() ) {
	QString localPath = localPath1, dirName = dirName1;
	if(localPath.right(1) == "/")
	    localPath.truncate(localPath.length()-1);
	if(dirName.right(1) == "/")
	    dirName.truncate(dirName.length()-1);

	QFileInfo fi( dirName );

	if( includeLastComponent )
	    setDirectory( fi.fileName() );
	QDir dir( dirName );
	const QFileInfoList* dirEntries = dir.entryInfoList();
	QFileInfoListIterator dirIter( *dirEntries );
	QDataStream outStream( &arcFile );
	QFileInfo* pFi;

	dirIter.toLast();
	while( ( pFi = dirIter.current() ) ) {
	    if( pFi->fileName() != "." && pFi->fileName() != ".." ) {
		if( pFi->isDir() )
		    writeDir( pFi->absFilePath(), true, localPath + "/" +
			      pFi->fileName() ); // Subdirs should always get its name in the archive.
		else
		    writeFile( pFi->absFilePath(), localPath );
	    }
	    --dirIter;
	}
	setDirectory( ".." );
	return true;
    }
    return false;
}

bool QArchive::writeFileList( const QStringList fileList )
{
    for( QStringList::ConstIterator it = fileList.begin(); it != fileList.end(); ++it ) {
	if( !writeFile( (*it) ) )
	    return false;
    }
    return true;
}

bool QArchive::writeDirList( const QStringList dirList, bool includeLastComponent )
{
    for( QStringList::ConstIterator it = dirList.begin(); it != dirList.end(); ++it ) {
	QString lastComponent = (*it).mid( (*it).findRev( "/" ) + 1 );
	if( !writeDir( (*it), includeLastComponent, lastComponent ) )
	    return false;
    }
    return true;
}

void QArchive::setVerbosity( int verbosity )
{
    verbosityMode = verbosity;
}

QArchiveHeader* QArchive::readArchiveHeader()
{
    QDataStream inStream( &arcFile );
    return readArchiveHeader( &inStream );
}

/*
   Reads the archive header and returns it on success. If an error occurs, it
   returns 0. The caller has to delete the object.
*/
QArchiveHeader* QArchive::readArchiveHeader( QDataStream *inStream )
{
    int chunktype;
    QArchiveHeader *header = new QArchiveHeader;

    *inStream >> chunktype;
    if( chunktype == ChunkBeginHeader ) {
	*inStream >> header->_mayorVersion;
	*inStream >> header->_minorVersion;
	if ( header->mayorVersion()!=1 || header->minorVersion()!=0 ) {
	    emit operationFeedback( "Incompatible package version" );
	    delete header;
	    return 0;
	}
	*inStream >> header->_features;
	*inStream >> header->_description;
	*inStream >> header->extraData;
	*inStream >> chunktype;
	if ( chunktype != ChunkEndHeader ) {
	    emit operationFeedback( "Invalid package header" );
	    delete header;
	    return 0;
	}
    } else {
	emit operationFeedback( "No package header found." );
	delete header;
	return 0;
    }
    return header;
}

bool QArchive::readArchive( const QString &outpath, const QString &key )
{
    QDataStream inStream( &arcFile );
    return readArchive( &inStream, outpath, key );
}

bool QArchive::readArchive( QDataStream *inStream, const QString &outpath, const QString &key )
{
    QDataStream outStream;
    QFile outFile;
    QDir outDir;
    QByteArray inBuffer;
    QByteArray outBuffer( bufferSize );
    z_stream ztream;
    bool continueDeCompressing;
    QString entryName, dirName, symName;
    int entryLength, chunktype;

    //get the key
    QArchiveHeader *header = readArchiveHeader( inStream );
    if ( header == 0 )
	return false;
    uint infeatures = featuresForKey( key );
    if( (header->features() & infeatures) != header->features()) {
	emit operationFeedback( "Invalid key" );
	return false;
    }

    // Set up the initial directory.
    // If the dir does not exist, try to create it
    dirName = QDir::toNativeSeparators( outpath );
    outDir.setPath( dirName );
    if( !outDir.exists( dirName ) && !createDir( dirName ) )
	return false;
    outDir.cd( dirName );

    while(  !inStream->atEnd()  ) {
	//get our type
	*inStream >> chunktype;
	if(chunktype == ChunkDirectory) {
	    *inStream >> entryLength;
	    inBuffer.resize( entryLength );
	    inStream->readRawBytes( inBuffer.data(), entryLength );
	    entryName = inBuffer.data();

	    if( verbosityMode & Source )
		emit operationFeedback( "Directory " + entryName + "... " );
	    if( entryName == "../" ) {
		outDir.cdUp();
	    } else {
		dirName = QDir::toNativeSeparators( outDir.absPath() +
						   QString( "/" ) + entryName.left( entryName.length() - 1 ) );
		if( verbosityMode & Destination )
		    emit operationFeedback( "Directory " + dirName + "... " );

		if( !outDir.exists( dirName ) && !createDir( dirName ) ) {
		    emit operationFeedback( "Cannot create directory: " + dirName );
		    return false;
		}
		outDir.cd( dirName );
	    }
	} else if(chunktype == ChunkFile) {
	    *inStream >> entryLength;
	    inBuffer.resize( entryLength );
	    inStream->readRawBytes( inBuffer.data(), entryLength );
	    entryName = inBuffer.data();

	    int filePerm;
	    QDateTime timeStamp;
	    QString fileName = QDir::toNativeSeparators( outDir.absPath() + QString( "/" ) + entryName );
	    outFile.setName( fileName );
	    if( outFile.open( IO_WriteOnly ) ) {
		*inStream >> timeStamp; 		// Get timestamp from the archive
		*inStream >> filePerm;
		outStream.setDevice( &outFile );
		*inStream >> entryLength;
		if( verbosityMode & Source )
		    emit operationFeedback( "Expanding " + entryName + "..." );
		else if( verbosityMode & Destination )
		    emit operationFeedback( "Expanding " + fileName + "..." );
		inBuffer.resize( entryLength );
		inStream->readRawBytes( inBuffer.data(), entryLength );
		ztream.next_in = (unsigned char*)inBuffer.data();
		ztream.avail_in = entryLength;
		ztream.total_in = 0;
		ztream.msg = NULL;
		ztream.zalloc = (alloc_func)0;
		ztream.zfree = (free_func)0;
		ztream.opaque = (voidpf)0;
		ztream.data_type = Z_BINARY;
		inflateInit( &ztream );
		continueDeCompressing = true;
		while( continueDeCompressing ) {
		    ztream.next_out = (unsigned char*)outBuffer.data();
		    ztream.avail_out = outBuffer.size();
		    ztream.total_out = 0;
		    continueDeCompressing = ( inflate( &ztream, Z_NO_FLUSH ) == Z_OK );
		    outStream.writeRawBytes( outBuffer.data(), ztream.total_out );
		}
		inflateEnd( &ztream );
		outFile.close();
#ifdef Q_OS_UNIX
		QDateTime t; t.setTime_t(0); //epoch
		struct utimbuf tb;
		tb.actime = tb.modtime = t.secsTo(timeStamp);
		utime(fileName.local8Bit(), &tb);
		if(filePerm != -1)
		    chmod(fileName.local8Bit(), (mode_t)filePerm);
#endif
	    } else {
		emit operationFeedback( "Cannot open: " + fileName );
		return false;
	    }
	} else if(chunktype == ChunkSymlink) {
	    *inStream >> entryLength;
	    inBuffer.resize( entryLength );
	    inStream->readRawBytes( inBuffer.data(), entryLength );
	    entryName = inBuffer.data();
	    QString fileName = QDir::toNativeSeparators( outDir.absPath() + QString( "/" ) + entryName );

	    *inStream >> entryLength;
	    inBuffer.resize( entryLength );
	    inStream->readRawBytes( inBuffer.data(), entryLength );
	    symName = inBuffer.data();
	    if( verbosityMode & Source )
		emit operationFeedback( "Linking " + symName + "... " );
	    else if( verbosityMode & Destination )
		emit operationFeedback( "Linking " + fileName + "... " );
#ifdef Q_OS_UNIX
	    symlink( symName.local8Bit(), fileName.local8Bit() );
#endif
	} else {
	    if( verbosityMode & Source )
		emit operationFeedback( QString("Unknown chunk: %d") .arg(chunktype) );
	}
	if( verbosityMode & Progress )
	    emit operationFeedback( inStream->device()->at() );
    }
    return true;
}

