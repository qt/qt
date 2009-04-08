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
#include <qapplication.h>
#include <qfileinfo.h>
#include "keyinfo.h"
#if defined(Q_OS_WIN32)
#include <windows.h>
#endif

class ConsoleOutput : public QObject
{
    Q_OBJECT
public:
    ConsoleOutput() : QObject() { }
    ~ConsoleOutput() { }
public slots:
    void updateProgress( const QString& str) {	qDebug("%s", str.latin1());   }
};

static int usage(const char *argv0, const char *un=NULL) {
    if(un)
	fprintf(stderr, "Unknown command: %s\n", un);
    else
	fprintf(stderr, "Usage:\n");
    fprintf(stderr, "%s [options] [keyinfo] files...\n", argv0);

    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, " -o file       : Outputs archive to file\n");
    fprintf(stderr, " -n            : Follow symbolic links, do not archive the link\n");
    fprintf(stderr, " -s            : Quiet mode, will not output process\n");
    fprintf(stderr, " -desc text    : Add the description text to the archive (e.g. \"Qt 3.0.0\")\n");
    fprintf(stderr, " -extra k v    : Adds the extra value v with the key k to the archive\n");
    fprintf(stderr, " -h            : This help\n");

    fprintf(stderr, "\nKey Info:\n");
    fprintf(stderr, " -win          : Windows Archive\n");
    fprintf(stderr, " -unix         : Unix Archive\n");
    fprintf(stderr, " -mac          : Mac OS X Archive\n");
    fprintf(stderr, " -embedded     : Embedded Archive\n");

    fprintf(stderr, "\nThe following options are not for packaging and can't be\n" );
    fprintf(stderr, "combined with the options above or any other option:\n\n" );
    fprintf(stderr, " -unpack file        : Unpack the archive file to the current directory\n");
    fprintf(stderr, " -getdesc file       : Print the description text of the archive file\n");
    fprintf(stderr, " -getextra k file    : Print the extra value for the key k of the archive file\n");
#if defined(Q_OS_WIN32)
    fprintf(stderr, " -res file1 file2    : Add the archive file1 as the binary resource\n");
    fprintf(stderr, "                       QT_ARQ to the excutable file2\n");
    fprintf(stderr, " -getres file        : Get the binary resource QT_ARQ from the executable\n" );
    fprintf(stderr, "                       file and store it under qt.arq\n");
    fprintf(stderr, " -namedres RES_NAME file1 file2:\n");
    fprintf(stderr, "                       Add the file1 as the binary resource\n");
    fprintf(stderr, "                       RES_NAME to the excutable file2\n");
    fprintf(stderr, " -license LICENSE file2:\n");
    fprintf(stderr, "                       Add the license LICENSE as the binary resource\n");
    fprintf(stderr, "                       LICENSE to the excutable file2\n");
    fprintf(stderr, " -license-us LICENSE-US file2:\n");
    fprintf(stderr, "                       Add the license LICENSE-US as the binary resource\n");
    fprintf(stderr, "                       LICENSE-US to the excutable file2\n");
#endif
    return 665;
}

static int unpack( const char *filename, bool verbose )
{
    QArchive arq( filename );
    ConsoleOutput output;
    if ( verbose )
	output.connect( &arq, SIGNAL(operationFeedback(const QString&)), SLOT(updateProgress(const QString&)) );
    if ( !arq.open( IO_ReadOnly ) ) {
	fprintf(stderr, "Can't open the archive %s file to unpack", filename);
	return 42;
    }
    if ( !arq.readArchive( ".", "G87A-QJFE-DQF9" ) ) {
	fprintf(stderr, "Couldn't unpack the archive %s", filename);
	return 42;
    }
    return 0;
}

static int getdesc( const char *filename )
{
    QArchive arq( filename );
    ConsoleOutput output;
    output.connect( &arq, SIGNAL(operationFeedback(const QString&)), SLOT(updateProgress(const QString&)) );
    if ( !arq.open( IO_ReadOnly ) ) {
	fprintf(stderr, "Can't open the archive %s file to get description", filename);
	return 42;
    }
    QArchiveHeader *header = arq.readArchiveHeader();
    if ( !header ) {
	fprintf(stderr, "Can't find the header in the archive %s file", filename);
	return 42;
    }
    if ( !header->description().isNull() ) {
	fprintf(stdout, header->description().latin1() );
    }
    return 0;
}

static int getextra( const char *key, const char *filename )
{
    QArchive arq( filename );
    ConsoleOutput output;
    output.connect( &arq, SIGNAL(operationFeedback(const QString&)), SLOT(updateProgress(const QString&)) );
    if ( !arq.open( IO_ReadOnly ) ) {
	fprintf(stderr, "Can't open the archive %s file to get description", filename);
	return 42;
    }
    QArchiveHeader *header = arq.readArchiveHeader();
    if ( !header ) {
	fprintf(stderr, "Can't find the header in the archive %s file", filename);
	return 42;
    }
    QString extraData = header->findExtraData(key);
    if ( !extraData.isNull() ) {
	fprintf(stdout, extraData.latin1() );
    }
    return 0;
}


int main( int argc, char** argv )
{
    uint features = 0;
    bool output = true, doSyms = true;
    QString desc;
    QString dest;
    QMap<QString,QString> extra;
#if defined(Q_OS_WIN32)
    QString arq, exe;
    QString resName;
    bool doRes = false;
    bool doLicense = false;
    bool doLicenseUs = false;
    bool getRes = false;
#endif
    QStringList files;
    int i;
    for( i = 1; i < argc; i++ ) {
	//options
	if(!strcmp(argv[i], "-o")) {
	    if ( ++i < argc )
		dest = argv[i];
	    else
		return usage(argv[0]);
	} else if(!strcmp(argv[i], "-n")) {
	    doSyms = false;
	} else if(!strcmp(argv[i], "-s")) {
	    output = false;
	} else if(!strcmp(argv[i], "-desc")) {
	    if ( ++i < argc )
		desc = argv[i];
	    else
		return usage(argv[0]);
	} else if(!strcmp(argv[i], "-extra")) {
	    QString key, value;
	    if ( ++i < argc )
		key = argv[i];
	    else
		return usage(argv[0]);
	    if ( ++i < argc )
		value = argv[i];
	    else
		return usage(argv[0]);
	    extra.insert( key, value );
	} else if(!strcmp(argv[i], "-h")) {
	    return usage(argv[0]);
	//keyinfo
	} else if(!strcmp(argv[i], "-unix")) {
	    features |= Feature_Unix;
	} else if(!strcmp(argv[i], "-win")) {
	    features |= Feature_Windows;
	} else if(!strcmp(argv[i], "-mac")) {
	    features |= Feature_Mac;
	} else if(!strcmp(argv[i], "-embedded")) {
	    features |= Feature_Embedded;
	//unpack
	} else if(!strcmp(argv[i], "-unpack")) {
	    if ( ++i < argc )
		return unpack( argv[i], output );
	//getdesc
	} else if(!strcmp(argv[i], "-getdesc")) {
	    if ( ++i < argc )
		return getdesc( argv[i] );
	//getextra
	} else if(!strcmp(argv[i], "-getextra")) {
	    if ( ++i < argc ) {
		if ( ++i < argc )
		    return getextra( argv[i-1], argv[i] );
	    }
#if defined(Q_OS_WIN32)
	//res (Windows only)
	} else if(!strcmp(argv[i], "-res")) {
	    doRes = true;
	    if ( ++i < argc )
		arq = argv[i];
	    if ( ++i < argc )
		exe = argv[i];
	//getres (Windows only)
	} else if(!strcmp(argv[i], "-getres")) {
	    getRes = true;
	    if ( ++i < argc )
		exe = argv[i];
	//res (Windows only)
	} else if(!strcmp(argv[i], "-namedres")) {
	    doRes = true;
	    if ( ++i < argc )
		resName = argv[i];
	    if ( ++i < argc )
		arq = argv[i];
	    if ( ++i < argc )
		exe = argv[i];
	//license (Windows only)
	} else if(!strcmp(argv[i], "-license")) {
	    doLicense = true;
	    if ( ++i < argc )
		arq = argv[i];
	    if ( ++i < argc )
		exe = argv[i];
	//licenseUs (Windows only)
	} else if(!strcmp(argv[i], "-license-us")) {
	    doLicenseUs = true;
	    if ( ++i < argc )
		arq = argv[i];
	    if ( ++i < argc )
		exe = argv[i];
#endif
	//files
	} else if(*(argv[i]) != '-') {
	    files.append(argv[i]);
	//unknown
	} else {
	    return usage(argv[0], argv[i]);
	}
    }
#if defined(Q_OS_WIN32)
    if ( doRes || doLicense || doLicenseUs ) {
	if ( arq.isEmpty() || exe.isEmpty() )
	    return usage(argv[0], argv[i]);
	QFile fArq( arq );
	if ( !fArq.open( IO_ReadOnly ) ) {
	    if ( doRes ) {
		fprintf(stderr, "Could not open archive %s", arq.latin1() );
	    } else {
		fprintf(stderr, "Could not open license %s", arq.latin1() );
	    }
	    return -1;
	}
	QByteArray ba = fArq.readAll();
	// ignore wide character versions (this is for internal use only)
	HANDLE hExe = BeginUpdateResourceA( exe.latin1(), false );
	if ( hExe == 0 ) {
	    fprintf(stderr, "Could not load executable %s\n", exe.latin1() );
	    qSystemWarning( "" );
	    return -1;
	}
	if ( resName.isEmpty() ) {
	    if ( doRes ) {
		resName = "QT_ARQ";
	    } else if ( doLicense ){
		resName = "LICENSE";
	    } else {
		resName = "LICENSE-US";
	    }
	}
	if ( !UpdateResourceA(hExe,RT_RCDATA,resName.latin1(),0,ba.data(),ba.count()) ) {
	    EndUpdateResource( hExe, true );
	    fprintf(stderr, "Could not update executable %s\n", exe.latin1() );
	    qSystemWarning( "" );
	    return -1;
	}
	if ( !EndUpdateResource(hExe,false) ) {
	    fprintf(stderr, "Could not update executable %s\n", exe.latin1() );
	    qSystemWarning( "" );
	    return -1;
	}
	return 0;
    }
    if ( getRes ) {
	if ( exe.isEmpty() )
	    return usage(argv[0], argv[i]);
	arq = "qt.arq";
	QFile fArq( arq );
	if ( !fArq.open( IO_WriteOnly ) ) {
	    fprintf(stderr, "Could not open archive %s\n", arq.latin1() );
	    return -1;
	}
	// ignore wide character versions (this is for internal use only)
	HMODULE hExe = LoadLibraryA( exe.latin1() );
	if ( hExe == NULL ) {
	    fprintf(stderr, "Could not load executable %s\n", exe.latin1() );
	    qSystemWarning( "" );
	    return -1;
	}
	HRSRC resource = FindResource( hExe, "QT_ARQ", RT_RCDATA );
	HGLOBAL hglobal = LoadResource( hExe, resource );
	int arSize = SizeofResource( hExe, resource );
	if ( arSize == 0 ) {
	    fprintf(stderr, "Could not get size of resource\n" );
	    qSystemWarning( "" );
	    return -1;
	}
	char *arData = (char*)LockResource( hglobal );
	if ( arData == 0 ) {
	    fprintf(stderr, "Could not lock resource\n" );
	    qSystemWarning( "" );
	    return -1;
	}
	fArq.writeBlock( arData, arSize );
	FreeLibrary( hExe );
	return 0;
    }
#endif
    if(!files.isEmpty()) {
	if(dest.isEmpty()) {
	    qDebug("Please specify an output package");
	    return 666;
	}

	QArchive archive;
	ConsoleOutput out;
	if(output) {
	    QObject::connect( &archive, SIGNAL( operationFeedback( const QString& ) ),
		    &out, SLOT( updateProgress( const QString& ) ) );
	    archive.setVerbosity( QArchive::Destination | QArchive::Verbose );
	}
	archive.setSymbolicLinks(doSyms);
	archive.setPath( dest );
	if( !archive.open( IO_WriteOnly ) ) {
	    qDebug("Failed to open output %s", dest.latin1());
	    return 666;
	}
	QArchiveHeader header( features, desc );
	QMap<QString,QString>::Iterator exIt;
	for ( exIt = extra.begin(); exIt != extra.end(); ++exIt ) {
	    header.addExtraData( exIt.key(), exIt.data() );
	}
	archive.writeHeader( header );
	for(QStringList::Iterator it = files.begin(); it != files.end(); ++it) {
	    QFileInfo f((*it));
	    if(!f.exists()) {
		qDebug("Failed to open %s", (*it).latin1());
		continue;
	    }
	    if(f.isDir())
		archive.writeDir( (*it), true, (*it) );
	    else
		archive.writeFile( (*it), (*it) );
	}
	archive.close();
    } else {
	return usage(argv[0]);
    }
    return 0;
}

#include "main.moc"
