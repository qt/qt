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

#include "unpackdlgimpl.h"
#include "qarchive.h"
#include <qapplication.h>
#include <qdir.h>
#include <qfileinfo.h>

class ConsoleOutput : public QObject
{
    Q_OBJECT
public:
    ConsoleOutput() : QObject() { }
    ~ConsoleOutput() { }
public slots:
    void updateProgress( const QString& str) {	qDebug("%s", str.latin1());   }
};
#include "main.moc"

static int usage(const char *argv0, const char *un=NULL) {
    if(un)
	fprintf(stderr, "Unknown command: %s\n", un);
    else
	fprintf(stderr, "Usage:\n");
    fprintf(stderr, "%s [options] files...\n", argv0);

    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, " -k [k]     : Use k as the key to open provided files\n");
    fprintf(stderr, " -s         : Quiet mode, will not output process\n");
    fprintf(stderr, " -h         : This help\n");
    return 665;
}

int main( int argc, char** argv )
{
    QString key;
    bool output = true;
    QStringList files;
    QApplication app( argc, argv );
    for(int i = 1; i < argc; i++) {
	//options
	if(!strcmp(argv[i], "-s")) 
	    output = false;
	else if(!strcmp(argv[i], "-k")) 
	    key = argv[++i];
	else if(!strcmp(argv[i], "-h"))
	    return usage(argv[0]);
	//files
	else if(*(argv[i]) != '-')  
	    files.append(argv[i]); 
	//unknown
	else 
	    return usage(argv[0], argv[i]); 
    }
    if(!files.isEmpty()) {
	QArchive archive;
	ConsoleOutput out;
	QObject::connect( &archive, SIGNAL( operationFeedback( const QString& ) ), 
			  &out, SLOT( updateProgress( const QString& ) ) );
	if(output) 
	    archive.setVerbosity( QArchive::Destination | QArchive::Verbose );
	for(QStringList::Iterator it = files.begin(); it != files.end(); ++it) {
	    archive.setPath( (*it) );
	    if( !archive.open( IO_ReadOnly ) ) {
		qDebug("Failed to open input %s", (*it).latin1());
		continue;
	    } 
	    if(!archive.readArchive( QDir::currentDirPath(), key )) 
		qDebug("Failed to unpack %s", (*it).latin1());
	    archive.close();
	}
    } else {
	UnpackDlgImpl dlg(key);
	dlg.exec();
    }
    return 0;
}

