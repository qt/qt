/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


#include "testcompiler.h"
#include <stdlib.h>
#include <qapplication.h>

#ifdef QT3_SUPPORT

#include <q3process.h>
#include <qtimer.h>
#ifdef Q_OS_WIN32
#   include <windows.h>
#endif

#include <QtTest/QtTest>

#undef SHOW_QDEBUG
#undef SHOW_COMPLETENESS

QString targetName( BuildType buildMode, const QString& target, const QString& version )
{
    QString targetName = target;

#if defined (Q_OS_WIN32)
    switch (buildMode)
    {
    case Exe: // app
	targetName.append(".exe");
	break;
    case Dll: // dll
	if (version != "") {
	    QStringList ver = QStringList::split(".", version);
	    targetName.append(ver.first());
	}
        targetName.append(".dll");
	break;
    case Lib: // lib
#ifdef Q_CC_GNU
        targetName.prepend("lib");
        targetName.append(".a");
#else
	targetName.append(".lib");
#endif
	break;
    case Plain:
        // no conversion
        break;
    }
#elif defined( Q_OS_MAC )
    switch (buildMode)
    {
    case Exe: // app
	targetName += ".app/Contents/MacOS/" + target.section('/', -1);
	break;
    case Dll: // dll
	targetName.prepend("lib");
        targetName.append("." + version + ".dylib");
	break;
    case Lib: // lib
	targetName.prepend("lib");
	targetName.append(".a");
	break;
    case Plain:
        // no conversion
        break;
    }
#else
    switch (buildMode)
    {
    case Exe: // app
	break;
    case Dll: // dll
	targetName.prepend("lib");
#if defined (Q_OS_HPUX) && !defined (__ia64)
        targetName.append(".sl");
#elif defined (Q_OS_AIX)
        targetName.append(".a");
#else
        targetName.append(".so");
#endif
	break;
    case Lib: // lib
	targetName.prepend("lib");
	targetName.append(".a");
	break;
    case Plain:
        // no conversion
        break;
    }
#endif
    return targetName;
}



TestCompiler::TestCompiler()
{
    exit_ok = FALSE;
    childProc = 0;
    setBaseCommands( "", "", FALSE );
}

TestCompiler::~TestCompiler()
{
    if (childProc)
	delete childProc;
}

bool TestCompiler::runChild( bool showOutput, QStringList argList, QStringList *envList )
{
    //qDebug() << "executing" << argList;
    exit_ok = FALSE;
    if (childProc)
	delete childProc;

    child_show = showOutput;
    if ( showOutput ) {

	QString S = argList.join(" ");
	addMakeResult( S );
    }

    childProc = new Q3Process(argList, this, argList.join(" ").latin1());
    Q_ASSERT(childProc);

    connect(childProc,SIGNAL(readyReadStdout()),this,SLOT(childHasData()));
    connect(childProc,SIGNAL(processExited()),this,SLOT(childReady()));

    if (!childProc->start( envList )) {

	addMakeResult( "Error executing '" + argList[0] + "'." );
	childReady();
	return FALSE;
    }

    while (childProc != 0 && childProc->isRunning()) {
	qApp->processEvents();
    }

    childReady();

    return exit_ok;
}

void TestCompiler::childReady()
{
    if (childProc != 0) {
	childHasData();

	QString S;
	int pos;
	while (childProc->canReadLineStderr()) {
	    S = childProc->readLineStderr();
	    do {
		pos = S.find("\t");
		if (pos >= 0) {
		    S.remove(pos,1);
		    S.insert(pos,"    ");
		}
	    } while (pos >= 0);

	    if (child_show)
		addMakeResult( S );
	}

	exit_ok = childProc->normalExit() && childProc->exitStatus() == 0;
	delete childProc;
    }
    childProc = 0;
}

void TestCompiler::childHasData()
{
    QString S;
    int pos;
    while (childProc->canReadLineStderr()) {

	S = childProc->readLineStderr();
	do {
	    pos = S.find("\t");
	    if (pos >= 0) {
		S.remove(pos,1);
		S.insert(pos,"    ");
	    }

	} while (pos >= 0);

        if ( S.startsWith("Project MESSAGE: FAILED") )
	    QTest::qFail( S, __FILE__, __LINE__ );
        else if ( S.startsWith("Project MESSAGE: SKIPPED") )
            QTest::qSkip( S, QTest::SkipSingle, __FILE__, __LINE__ );
	else if (child_show)
	    addMakeResult( S );
    }
}

void TestCompiler::setBaseCommands( QString makeCmd, QString qmakeCmd, bool qwsMode )
{
    qws_mode = qwsMode;
    make_cmd = makeCmd;

    // not sure if i need this, but it doesn't hurt
    if (make_cmd.startsWith("\""))
        make_cmd = make_cmd.remove(0,1);
    if (make_cmd.endsWith("\""))
        make_cmd = make_cmd.remove(make_cmd.length()-1,1);

    qmake_cmd = qmakeCmd;
    // also not sure if i need this, but it doesn't hurt...
    if(qmake_cmd.length() >= 2 && (qmake_cmd.at(0) == '"' || qmake_cmd.at(0) == '\'') && qmake_cmd.at(qmake_cmd.length()-1) == qmake_cmd.at(0))
        qmake_cmd = qmake_cmd.mid(1, qmake_cmd.length()-2);
}

bool TestCompiler::cleanAll( const QString &workPath, const QString &destPath, const QString &exeName, const QString &exeExt )
{
    QDir D(workPath);
    if (!D.exists()) {

        addMakeResult( "Directory '" + workPath + "' doesn't exist" );
        return FALSE;
    }

    D.setCurrent(workPath);
    // must delete at least the executable file to be able to easily and safely
    // verify that the compilation was a success.
    D.remove( destPath + "/" + exeName + exeExt );
    D.remove( workPath + "/Makefile");
    QFileInfo Fi( workPath + "/Makefile");
    if (Fi.exists()) {

	// Run make clean
	QStringList args;
        args = QStringList::split( " ", make_cmd );
	args.append("clean");

	return runChild( FALSE, args, 0 );
    }

    return TRUE;
}

bool TestCompiler::makeClean( const QString &workPath )
{
    QDir D;
    if (!D.exists(workPath)) {

        addMakeResult( "Directory '" + workPath + "' doesn't exist" );
        return FALSE;
    }

    D.setCurrent(workPath);
    QFileInfo Fi( workPath + "/Makefile");
    if (Fi.exists()) {

	// Run make clean
	QStringList args;
        args = QStringList::split( " ", make_cmd );
	args.append("clean");

	return runChild( FALSE, args, 0 );
    }

    return TRUE;
}

bool TestCompiler::makeDistClean( const QString &workPath )
{
    QDir D;
    if (!D.exists(workPath)) {
        addMakeResult( "Directory '" + workPath + "' doesn't exist" );
        return FALSE;
    }

    D.setCurrent(workPath);
    QFileInfo Fi( workPath + "/Makefile");
    if (Fi.exists()) {
	// Run make distclean
	QStringList args;
        args = QStringList::split( " ", make_cmd );
	args.append("distclean");

	return runChild( FALSE, args, 0 );
    }

    return TRUE;

}

bool TestCompiler::qmake( const QString &workDir, const QString &proName, const QString &buildDir )
{
    // Now start qmake and generate the makefile

    QDir D( workDir );
    // Make sure we start in the right directory
    D.setCurrent( workDir );

    if (D.exists("Makefile"))
	D.remove("Makefile");

    QStringList args;
    args = QStringList::split( " ", qmake_cmd );

    QString project_fname = workDir + "/" + proName + ".pro";
    QString makefile_fname = (buildDir.isNull()?QString():(buildDir + "/")) + "Makefile";

    args.append( project_fname );
    args.append( "-o" );
    args.append( makefile_fname );

    return runChild( TRUE, args, 0 );
}

bool TestCompiler::make( const QString &workPath, const QString &target )
{
    QDir D;
    D.setCurrent( workPath );

    QStringList args;
    args = QStringList::split( " ", make_cmd );
    if ( make_cmd.lower().find("nmake") >= 0)
	args.append("/NOLOGO");
    if ( !target.isNull() )
        args.append(target);

    return runChild( TRUE, args, 0 );
}

bool TestCompiler::exists( const QString &destDir, const QString &exeName, BuildType buildType, const QString &version )
{
    QFileInfo f(destDir + "/" + targetName(buildType, exeName, version));
    return f.exists();
}

void TestCompiler::addMakeResult( const QString &result )
{
    make_result.append( result );
}

bool TestCompiler::removeMakefile( const QString &workPath )
{
    QDir D;
    D.setCurrent( workPath );
    if ( D.exists( "Makefile" ) )
	return D.remove( "Makefile" );
    else
	return TRUE;
}

#endif //QT3_SUPPORT

