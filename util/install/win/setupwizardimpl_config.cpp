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
#include "setupwizardimpl.h"
#include "environment.h"
#include <qfiledialog.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qprogressbar.h>
#include <qtextview.h>
#include <qmultilineedit.h>
#include <qbuttongroup.h>
#include <qsettings.h>
#include <qlistview.h>
#include <qlistbox.h>
#include <qapplication.h>
#include <qcheckbox.h>
#include <qtextstream.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qtabwidget.h>
#include <qarchive.h>
#include <qvalidator.h>
#include <qdatetime.h>
#include <qpainter.h>

bool findFile(const QString &);

CheckListItem::CheckListItem(QListView *listview, const QString &text, Type type)
: QCheckListItem(listview, text, type), help_display(0), conflict_with(0), critical(false)
{
    if (type == RadioButtonController || type == CheckBoxController)
	setOpen(true);
}

CheckListItem::CheckListItem(QCheckListItem *parent, const QString &text, Type type)
: QCheckListItem(parent, text, type), help_display(0), conflict_with(0), critical(false)
{
    if (type == RadioButtonController || type == CheckBoxController)
	setOpen(true);
}

int CheckListItem::RTTI = 666;

int CheckListItem::rtti() const
{
    return RTTI;
}

void CheckListItem::setHelpText(const QString &help, QTextView *display)
{
    help_text = help;
    help_display = display;
}

void CheckListItem::setWarningText(const QString &warning, CheckListItem *conflict)
{
    conflict_with = conflict;
    warning_text = warning;
}

void CheckListItem::addRequiredFiles(const QString &file)
{
    QStringList files = QStringList::split(",", file);
    for (QStringList::ConstIterator it = files.begin(); it != files.end(); ++it) {
	QString f(*it);
	required_files << f;
	if (!findFile(f)) {
	    if (type() == QCheckListItem::RadioButtonController
		|| type() == QCheckListItem::CheckBoxController) {
		setOpen(false);
	    } else {
		setOn(false);
		setEnabled(false);
	    }
	}
    }
}

void CheckListItem::setRequiredFileLocation(const QString &location)
{
    file_location = location;
}

bool CheckListItem::verify() const
{
    if (text(0) == "Off")
	return true;

    if (!required_files.count()) {
	if (parent() && parent()->rtti() == RTTI)
	    return ((CheckListItem*)parent())->verify();
	return true;
    }

    QStringList::ConstIterator it;
    for (it = required_files.begin(); it != required_files.end(); ++it) {
	QString file(*it);
	if (!findFile(file))
	    return false;
    }
    return true;
}

bool CheckListItem::testAndWarn()
{
    if (!warning_text.isEmpty()) {
	if (!conflict_with || conflict_with->isOn()) {
	    int r = QMessageBox::warning(listView()->window(), "Warning",
			warning_text + "<br>Select anyway?", "Yes", "No" );
	    return r == 0;
	} else {
	    return true;
	}
    }

    QStringList files(required_files);
    if (!required_files.count() && text(0) != "Off") {
	if (parent() && parent()->rtti() == RTTI)
	    return ((CheckListItem*)parent())->testAndWarn();
	return true;
    }

    if (!verify()) {
	QString message = QString("<p>The option '%1' is <b>not verified</b> by the installer. One or more of the following "
		    "files could not be located on the system:"
		    "<p>%2"
		    "<p>Continuing with this option selected might <b>break the installation</b> process.")
		    .arg(text(0)).arg(required_files.join(", "));
	if (!file_location.isEmpty()) {
	    message += QString("<p>The requested files are %1 and need to be installed "
		               "in the INCLUDE, PATH and LIBS environment as appropriate.").arg(file_location);
	}

	int r = QMessageBox::warning(listView()->window(), "Option not Verified",
		    message + "<p>Select anyway?", "Yes", "No" );
	return r == 0;
    }
    return true;
}

void CheckListItem::displayHelp()
{
    if (help_display) {
	help_display->setText(help_text);
	return;
    }
    QListViewItem *p = parent();
    if (p && p->rtti() == RTTI)
	((CheckListItem*)p)->displayHelp();
}

void CheckListItem::setOn(bool on)
{
    if (on && (type() == RadioButton || type() == CheckBox) && !testAndWarn())
	return;
    QCheckListItem::setOn(on);

    SetupWizardImpl* wizard = qobject_cast<SetupWizardImpl*>(listView()->window());
    if (wizard && listView()->isVisible() && listView()->updatesEnabled())
	wizard->optionClicked(this);
}

void CheckListItem::activate()
{
    displayHelp();
    QCheckListItem::activate();
}

void CheckListItem::setOpen(bool on)
{
    if (on && listView()->isVisible()) {
	if (!testAndWarn())
	    return;
	displayHelp();
    }
    QCheckListItem::setOpen(on);
}

void CheckListItem::setCritical(bool on)
{
    if (critical == on)
	return;

    critical = on;
    repaint();
}

void CheckListItem::paintCell( QPainter *p, const QColorGroup & cg, int column, int width, int alignment )
{
    QColorGroup group(cg);
    if (critical)
	group.setColor(QColorGroup::Text, red);
    QCheckListItem::paintCell(p, group, column, width, alignment);
}



void SetupWizardImpl::setStaticEnabled( bool se )
{
    bool enterprise = licenseInfo[ "PRODUCTS" ] == "qt-enterprise";
    if ( se ) {
	if ( accOn->isOn() ) {
	    accOn->setOn( false );
	    accOff->setOn( true );
	}
	if ( bigCodecsOff->isOn() ) {
	    bigCodecsOn->setOn( true );
	    bigCodecsOff->setOn( false );
	}
	if ( mngPlugin->isOn() ) {
	    mngDirect->setOn( true );
	    mngPlugin->setOn( false );
	    mngOff->setOn( false );
	}
	if ( pngPlugin->isOn() ) {
	    pngDirect->setOn( true );
	    pngPlugin->setOn( false );
	    pngOff->setOn( false );
	}
	if ( jpegPlugin->isOn() ) {
	    jpegDirect->setOn( true );
	    jpegPlugin->setOn( false );
	    jpegOff->setOn( false );
	}
	if ( sgiPlugin->isOn() ) {
	    sgiPlugin->setOn( false );
	    sgiDirect->setOn( true );
	}
	if ( cdePlugin->isOn() ) {
	    cdePlugin->setOn( false );
	    cdeDirect->setOn( true );
	}
	if ( motifplusPlugin->isOn() ) {
	    motifplusPlugin->setOn( false );
	    motifplusDirect->setOn( true );
	}
	if ( motifPlugin->isOn() ) {
	    motifPlugin->setOn( false );
	    motifDirect->setOn( true );
	}
	if ( platinumPlugin->isOn() ) {
	    platinumPlugin->setOn( false );
	    platinumDirect->setOn( true );
	}
	if ( xpPlugin->isOn() ) {
	    xpPlugin->setOn( false );
	    xpOff->setOn( true );
	}
	if ( enterprise ) {
	    if ( mysqlPlugin->isOn() ) {
		mysqlPlugin->setOn( false );
		mysqlDirect->setOn( true );
	    }
	    if ( ociPlugin->isOn() ) {
		ociPlugin->setOn( false );
		ociDirect->setOn( true );
	    }
	    if ( odbcPlugin->isOn() ) {
		odbcPlugin->setOn( false );
		odbcDirect->setOn( true );
	    }
	    if ( psqlPlugin->isOn() ) {
		psqlPlugin->setOn( false );
		psqlDirect->setOn( true );
	    }
	    if ( tdsPlugin->isOn() ) {
		tdsPlugin->setOn( false );
		tdsDirect->setOn( true );
	    }
	    if ( db2Plugin->isOn() ) {
		db2Plugin->setOn( false );
		db2Direct->setOn( true );
	    }
	    if ( sqlitePlugin->isOn() ) {
		sqlitePlugin->setOn( false );
		sqliteDirect->setOn( true );
	    }
	    if ( ibasePlugin->isOn() ) {
		ibasePlugin->setOn( false );
		ibaseDirect->setOn( true );
	    }
	}
	accOn->setEnabled( false );
	bigCodecsOff->setEnabled( false );
	mngPlugin->setEnabled( false );
	pngPlugin->setEnabled( false );
	jpegPlugin->setEnabled( false );
	sgiPlugin->setEnabled( false );
	cdePlugin->setEnabled( false );
	motifPlugin->setEnabled( false );
	motifplusPlugin->setEnabled( false );
	motifPlugin->setEnabled( false );
	platinumPlugin->setEnabled( false );
	xpPlugin->setEnabled( false );
	if ( enterprise ) {
	    mysqlPlugin->setEnabled( false );
	    ociPlugin->setEnabled( false );
	    odbcPlugin->setEnabled( false );
	    psqlPlugin->setEnabled( false );
	    tdsPlugin->setEnabled( false );
	    db2Plugin->setEnabled( false );
	    sqlitePlugin->setEnabled( false );
	    ibasePlugin->setEnabled( false );
	}
    } else {
	accOn->setEnabled( true );
	bigCodecsOff->setEnabled( true );
	mngPlugin->setEnabled( true );
	pngPlugin->setEnabled( true );
	jpegPlugin->setEnabled( true );
	sgiPlugin->setEnabled( true );
	cdePlugin->setEnabled( true );
	motifplusPlugin->setEnabled( true );
	motifPlugin->setEnabled( true );
	platinumPlugin->setEnabled( true );
	xpPlugin->setEnabled( true );
	if ( enterprise ) {
	    mysqlPlugin->setEnabled( true );
	    ociPlugin->setEnabled( true );
	    odbcPlugin->setEnabled( true );
	    psqlPlugin->setEnabled( true );
	    tdsPlugin->setEnabled( true );
	    db2Plugin->setEnabled( true );
	    sqlitePlugin->setEnabled( true );
	    ibasePlugin->setEnabled( true );
	}
    }
    setJpegDirect( mngDirect->isOn() );
}

void SetupWizardImpl::setJpegDirect( bool jd )
{
    // direct MNG support requires also direct JPEG support
    if ( jd ) {
	jpegOff->setOn( false );
	jpegPlugin->setOn( false );
	jpegDirect->setOn( true );

	jpegOff->setEnabled( false );
	jpegPlugin->setEnabled( false );
	jpegDirect->setEnabled( true );
    } else {
	jpegOff->setEnabled( true );
	if ( !staticItem->isOn() )
	    jpegPlugin->setEnabled( true );
	jpegDirect->setEnabled( true );
    }
}

void SetupWizardImpl::optionClicked( CheckListItem *item )
{
    if ( !item || item->type() != CheckListItem::RadioButton )
	return;

    if ( item->text(0) == "Static" && item->isOn() ) {
	setStaticEnabled( true );
	return;
    } else if ( item->text( 0 ) == "Shared" && item->isOn() ) {
	setStaticEnabled( false );
	return;
    } else if ( item==mngDirect || item==mngPlugin || item==mngOff ) {
	setJpegDirect( mngDirect->isOn() );
    } else if ( item==db2Direct && odbcDirect->isOn() ) {
	if ( odbcPlugin->isEnabled() )
	    odbcPlugin->QCheckListItem::setOn(true);
	else
	    odbcOff->QCheckListItem::setOn(true);
    } else if ( item==odbcDirect && db2Direct->isOn() ) {
	if ( db2Plugin->isEnabled() )
	    db2Plugin->QCheckListItem::setOn(true);
	else
	    db2Off->QCheckListItem::setOn(true);
    }
}


void SetupWizardImpl::configPageChanged()
{
    if ( configPage->configList->isVisible() ) {
	configPage->configList->setSelected( configPage->configList->currentItem(), true );
    } else if ( configPage->advancedList->isVisible() ) {
	configPage->advancedList->setSelected( configPage->advancedList->currentItem(), true );
    }
#if defined(EVAL) || defined(EDU) || defined(NON_COMMERCIAL)
    else if ( configPage->installList->isVisible() ) {
	configPage->installList->setSelected( configPage->installList->currentItem(), true );
    }
#endif
}

void SetupWizardImpl::cleanDone()
{
#if defined(EVAL) || defined(EDU) || defined(NON_COMMERCIAL)
    prepareEnvironment();
#  if defined(Q_OS_WIN32)
    QString qtdir = QEnvironment::getEnv( "QTDIR" );

    // adjust the .qmake.cache
    QFile qmakeCache( qtdir + "/.qmake.cache" );
    if ( qmakeCache.open( IO_ReadOnly ) ) {
	QString content = qmakeCache.readAll();
	qmakeCache.close();
	if ( globalInformation.sysId() == GlobalInformation::Borland )
	    content.replace( "C:\\QtEvaluation\\qtborland", qtdir );
	else
	    content.replace( "C:\\QtEvaluation\\qtmsvc", qtdir );

	if ( qmakeCache.open( IO_WriteOnly ) ) {
	    QTextStream ts( &qmakeCache );
	    ts << content;
	    qmakeCache.close();
	} else {
	    logOutput( QString("Warning: can't open the .qmake.cache file for writing: %1\n").arg( qmakeCache.errorString() ) );
	}
    } else {
	logOutput( QString("Warning: can't open the .qmake.cache file for reading: %1\n").arg( qmakeCache.errorString() ) );
    }

    QStringList args;
    args << ( qtdir + "\\bin\\configure.exe" );
    args << "-spec";
    args << globalInformation.text(GlobalInformation::Mkspec);
    if ( globalInformation.sysId() == GlobalInformation::MSVC )
	args << "-dsp";
    else if ( globalInformation.sysId() == GlobalInformation::MSVCNET )
	args << "-vcproj";

    if( qWinVersion() & WV_NT_based ) {
	logOutput( "Execute configure...\n" );
	logOutput( args.join( " " ) + "\n" );

	configure.setWorkingDirectory( qtdir );
	configure.setArguments( args );
	// Start the configure process
	buildPage->compileProgress->setTotalSteps( int(double(filesToCompile) * 2.6) );
	buildPage->restartBuild->setText( "Stop configure" );
	buildPage->restartBuild->setEnabled( true );
	buildPage->restartBuild->show();
	buildPage->compileProgress->show();
	if( !configure.start() ) {
	    logOutput( "Could not start configure process" );
	    emit wizardPageFailed( indexOf(currentPage()) );
	}
    } else { // no proper process handling on DOS based systems - create a batch file instead
	logOutput( "Generating batch file...\n" );
	QDir installDir;
	if ( optionsPage )
	    installDir.setPath( optionsPage->installPath->text() );
	else
	    installDir.setPath( qtdir );
	QFile outFile( installDir.filePath("build.bat") );
	QTextStream outStream( &outFile );

	if( outFile.open( IO_WriteOnly | IO_Translate ) ) {
	    if ( installDir.absPath()[1] == ':' )
		outStream << installDir.absPath().left(2) << endl;
	    outStream << "cd %QTDIR%" << endl;
	    outStream << args.join( " " ) << endl;
	    if( !globalInformation.reconfig() ) {
		outStream << globalInformation.text(GlobalInformation::MakeTool) << endl;
	    }
	    outFile.close();
	}
	logOutput( "Doing the final integration steps..." );
	doIDEIntegration();
	buildPage->compileProgress->setTotalSteps( buildPage->compileProgress->totalSteps() );
	showPage( finishPage );
    }
#  elif defined(Q_OS_UNIX)
    buildPage->compileProgress->show();
    buildPage->restartBuild->show();

    buildPage->compileProgress->setProgress( 0 );
    buildPage->compileProgress->setTotalSteps( int(double(filesToCompile) * 1.8) );
    configDone();
#  endif
#else
    QStringList args;
    QStringList entries;
    QSettings settings;
    QString entry;
    QStringList::Iterator it;
    QFile tmpFile;
    QTextStream tmpStream;
    bool settingsOK;

#  if defined(Q_OS_WIN32)
    args << ( QEnvironment::getEnv( "QTDIR" ) + "\\bin\\configure.exe" );
#  elif defined(Q_OS_UNIX)
    args << ( QEnvironment::getEnv( "QTDIR" ) + QDir::separator() + "configure" );
#  endif

    entry = settings.readEntry( "/Trolltech/Qt/Build", "Debug", &settingsOK );
    if ( entry == "Debug" )
	args += "-debug";
    else
	args += "-release";

    entry = settings.readEntry( "/Trolltech/Qt/Library", "Shared", &settingsOK );
    if ( entry == "Static" )
	args += "-static";
    else
	args += "-shared";

    entry = settings.readEntry( "/Trolltech/Qt/Threading", QString(), &settingsOK );
    if ( entry == "Threaded" )
	args += "-thread";
    else
	args += "-no-thread";

    entries = settings.readListEntry( "/Trolltech/Qt/Modules", ',', &settingsOK );
    for( it = allModules.begin(); it != allModules.end(); ++it ) {
	entry = *it;
	if ( entries.find( entry ) != entries.end() )
	    args += QString( "-enable-" ) + entry;
	else
	    args += QString( "-disable-") + entry;
    }

    entry = settings.readEntry( "/Trolltech/Qt/SQL Drivers/MySQL", "Off", &settingsOK );
    if ( entry == "Direct" )
	args += "-qt-sql-mysql";
    else if ( entry == "Plugin" )
	args += "-plugin-sql-mysql";
    else if ( entry == "Off" )
	args += "-no-sql-mysql";

    entry = settings.readEntry( "/Trolltech/Qt/SQL Drivers/OCI", "Off", &settingsOK );
    if ( entry == "Direct" )
	args += "-qt-sql-oci";
    else if ( entry == "Plugin" )
	args += "-plugin-sql-oci";
    else if ( entry == "Off" )
	args += "-no-sql-oci";

    entry = settings.readEntry( "/Trolltech/Qt/SQL Drivers/ODBC", "Off", &settingsOK );
    if ( entry == "Direct" )
	args += "-qt-sql-odbc";
    else if ( entry == "Plugin" )
	args += "-plugin-sql-odbc";
    else if ( entry == "Off" )
	args += "-no-sql-odbc";

    entry = settings.readEntry( "/Trolltech/Qt/SQL Drivers/PostgreSQL", "Off", &settingsOK );
    if ( entry == "Direct" )
	args += "-qt-sql-psql";
    else if ( entry == "Plugin" )
	args += "-plugin-sql-psql";
    else if ( entry == "Off" )
	args += "-no-sql-psql";

    entry = settings.readEntry( "/Trolltech/Qt/SQL Drivers/TDS", "Off", &settingsOK );
    if ( entry == "Direct" )
	args += "-qt-sql-tds";
    else if ( entry == "Plugin" )
	args += "-plugin-sql-tds";
    else if ( entry == "Off" )
	args += "-no-sql-tds";

    entry = settings.readEntry( "/Trolltech/Qt/SQL Drivers/DB2", "Off", &settingsOK );
    if ( entry == "Direct" )
	args += "-qt-sql-db2";
    else if ( entry == "Plugin" )
	args += "-plugin-sql-db2";
    else if ( entry == "Off" )
	args += "-no-sql-db2";

    entry = settings.readEntry( "/Trolltech/Qt/SQL Drivers/SQLite", "Off", &settingsOK );
    if ( entry == "Direct" )
	args += "-qt-sql-sqlite";
    else if ( entry == "Plugin" )
	args += "-plugin-sql-sqlite";
    else if ( entry == "Off" )
	args += "-no-sql-sqlite";

    entry = settings.readEntry( "/Trolltech/Qt/SQL Drivers/iBase", "Off", &settingsOK );
    if ( entry == "Direct" )
	args += "-qt-sql-ibase";
    else if ( entry == "Plugin" )
	args += "-plugin-sql-ibase";
    else if ( entry == "Off" )
	args += "-no-sql-ibase";

#  if defined(Q_OS_WIN32)
//TODO: Win only, remove these options from wizard on mac?
    entry = settings.readEntry( "/Trolltech/Qt/Accessibility", "On", &settingsOK );
    if ( entry == "On" )
	args += "-accessibility";
    else
	args += "-no-accessibility";
#  endif

    entry = settings.readEntry( "/Trolltech/Qt/Big Textcodecs", "On", &settingsOK );
    if ( entry == "On" )
	args += "-big-codecs";
    else
	args += "-no-big-codecs";

    entry = settings.readEntry( "/Trolltech/Qt/Tablet Support", "Off", &settingsOK );
    if ( entry == "On" )
	args += "-tablet";
    else
	args += "-no-tablet";

    entries = settings.readListEntry( "/Trolltech/Qt/Advanced C++", ',', &settingsOK );
    if ( entries.contains( "STL" ) )
	args += "-stl";
    else
	args += "-no-stl";
    if ( entries.contains( "Exceptions" ) )
	args += "-exceptions";
    else
	args += "-no-exceptions";
    if ( entries.contains( "RTTI" ) )
	args += "-rtti";
    else
	args += "-no-rtti";

#  if defined(Q_OS_WIN32)
//TODO: Win only, remove these options from wizard on mac?
    entry = settings.readEntry( "/Trolltech/Qt/Image Formats/PNG", "Direct", &settingsOK );
    if ( entry == "Plugin" )
	args += "-plugin-imgfmt-png";
    else if ( entry == "Direct" )
	args += "-qt-imgfmt-png";
    else if ( entry == "Off" )
	args += "-no-imgfmt-png";
    args += "-qt-png";

    entry = settings.readEntry( "/Trolltech/Qt/Image Formats/JPEG", "Direct", &settingsOK );
    if ( entry == "Plugin" )
	args += "-plugin-imgfmt-jpeg";
    else if ( entry == "Direct" )
	args += "-qt-imgfmt-jpeg";
    else if ( entry == "Off" )
	args += "-no-imgfmt-jpeg";
    args += "-qt-jpeg";

    entry = settings.readEntry( "/Trolltech/Qt/Image Formats/MNG", "Direct", &settingsOK );
    if ( entry == "Plugin" )
	args += "-plugin-imgfmt-mng";
    else if ( entry == "Direct" )
	args += "-qt-imgfmt-mng";
    else if ( entry == "Off" )
	args += "-no-imgfmt-mng";
    args += "-qt-mng";
#  endif

    entry = settings.readEntry( "/Trolltech/Qt/Image Formats/GIF", "Direct", &settingsOK );
    if ( entry == "Direct" )
	args += "-qt-gif";
    else if ( entry == "Off" )
	args += "-no-gif";

#  if defined(Q_OS_WIN32)
//TODO: Win only, remove these options from wizard on mac?
    entry = settings.readEntry( "/Trolltech/Qt/Styles/Windows", "Direct", &settingsOK );
    if ( entry == "Direct" )
	args += "-qt-style-windows";
    else if ( entry == "Plugin" )
	args += "-plugin-style-windows";
    else if ( entry == "Off" )
	args += "-no-style-windows";

    entry = settings.readEntry( "/Trolltech/Qt/Styles/Windows XP", "Off", &settingsOK );
    if ( entry == "Direct" )
	args += "-qt-style-windowsxp";
    else if ( entry == "Plugin" )
	args += "-plugin-style-windowsxp";
    else if ( entry == "Off" )
	args += "-no-style-windowsxp";

    entry = settings.readEntry( "/Trolltech/Qt/Styles/Motif", "Plugin", &settingsOK );
    if ( entry == "Direct" )
	args += "-qt-style-motif";
    else if ( entry == "Plugin" )
	args += "-plugin-style-motif";
    else if ( entry == "Off" )
	args += "-no-style-motif";

    entry = settings.readEntry( "/Trolltech/Qt/Styles/Platinum", "Plugin", &settingsOK );
    if ( entry == "Direct" )
	args += "-qt-style-platinum";
    else if ( entry == "Plugin" )
	args += "-plugin-style-platinum";
    else if ( entry == "Off" )
	args += "-no-style-platinum";

    entry = settings.readEntry( "/Trolltech/Qt/Styles/MotifPlus", "Plugin", &settingsOK );
    if ( entry == "Direct" )
	args += "-qt-style-motifplus";
    else if ( entry == "Plugin" )
	args += "-plugin-style-motifplus";
    else if ( entry == "Off" )
	args += "-no-style-motifplus";

    entry = settings.readEntry( "/Trolltech/Qt/Styles/CDE", "Plugin", &settingsOK );
    if ( entry == "Direct" )
	args += "-qt-style-cde";
    else if ( entry == "Plugin" )
	args += "-plugin-style-cde";
    else if ( entry == "Off" )
	args += "-no-style-cde";

    entry = settings.readEntry( "/Trolltech/Qt/Styles/SGI", "Plugin", &settingsOK );
    if ( entry == "Direct" )
	args += "-qt-style-sgi";
    else if ( entry == "Plugin" )
	args += "-plugin-style-sgi";
    else if ( entry == "Off" )
	args += "-no-style-sgi";
#  endif

    if ( globalInformation.sysId() == GlobalInformation::MSVC ) {
	entry = settings.readEntry( "/Trolltech/Qt/DSP Generation", "On", &settingsOK );
	if ( entry == "On" )
	    args += "-dsp";
	else if ( entry == "Off" )
	    args += "-no-dsp";
    } else if ( globalInformation.sysId() == GlobalInformation::MSVCNET ) {
	entry = settings.readEntry( "/Trolltech/Qt/VCPROJ Generation", "On", &settingsOK );
	if ( entry == "On" )
	    args += "-vcproj";
	else if ( entry == "Off" )
	    args += "-no-vcproj";
    } else if ( globalInformation.sysId() != GlobalInformation::MSVC && globalInformation.sysId() == GlobalInformation::MSVCNET ) {
	args += "-no-dsp";
	args += "-no-vcproj";
    }

    if ( ( ( !globalInformation.reconfig() && !optionsPage->skipBuild->isChecked() )
	    || ( globalInformation.reconfig() && configPage->rebuildInstallation->isChecked() ) )
#  if defined(Q_OS_WIN32)
    && qWinVersion() & WV_NT_based ) {
#  else
    ) {
#  endif
	logOutput( "Execute configure...\n" );
	logOutput( args.join( " " ) + "\n" );

	configure.setWorkingDirectory( QEnvironment::getEnv( "QTDIR" ) );
	configure.setArguments( args );
	// Start the configure process
	buildPage->compileProgress->setTotalSteps( int(double(filesToCompile) * 2.6) );
	buildPage->restartBuild->setText( "Stop configure" );
	buildPage->restartBuild->setEnabled( true );
	buildPage->restartBuild->show();
	buildPage->compileProgress->show();
	if( !configure.start() ) {
	    logOutput( "Could not start configure process" );
	    emit wizardPageFailed( indexOf(currentPage()) );
	}
    } else { // no proper process handling on DOS based systems - create a batch file instead
	logOutput( "Generating batch file...\n" );
	QDir installDir;
	if ( optionsPage )
	    installDir.setPath( optionsPage->installPath->text() );
	else
	    installDir.setPath( QEnvironment::getEnv( "QTDIR" ) );
	QFile outFile( installDir.filePath("build.bat") );
	QTextStream outStream( &outFile );

	if( outFile.open( IO_WriteOnly | IO_Translate ) ) {
	    if ( installDir.absPath()[1] == ':' )
		outStream << installDir.absPath().left(2) << endl;
	    outStream << "cd %QTDIR%" << endl;

	    if ( globalInformation.reconfig() )
		outStream << globalInformation.text(GlobalInformation::MakeTool) << " clean" << endl;

	    // There is a limitation on Windows 9x regarding the length of the
	    // command line. So rather use the configure.cache than specifying
	    // all configure options on the command line.
	    QFile configureCache( installDir.filePath("configure.cache") );
	    if( configureCache.open( IO_WriteOnly | IO_Translate ) ) {
		QTextStream confCacheStream( &configureCache );
		QStringList::Iterator it = args.begin();
		++it; // skip args[0] (configure)
		while ( it != args.end() ) {
		    confCacheStream << *it << endl;
		    ++it;
		}
		configureCache.close();
		outStream << args[0] << " -redo" << endl;
	    } else {
		outStream << args.join( " " ) << endl;
	    }

	    outStream << globalInformation.text(GlobalInformation::MakeTool) << endl;
	    outFile.close();
	}
	logOutput( "Doing the final integration steps..." );
	// No need to redo the integration step
	if ( !globalInformation.reconfig() )
	    doIDEIntegration();
	buildPage->compileProgress->setTotalSteps( buildPage->compileProgress->totalSteps() );
	showPage( finishPage );
    }
#endif
}

void SetupWizardImpl::prepareEnvironment()
{
    QByteArray pathBuffer;
    QStringList path;
    QString qtDir;
    int envSpec = QEnvironment::LocalEnv;

    if( globalInformation.reconfig() ) {
	qtDir = QEnvironment::getEnv( "QTDIR" );
	if ( configPage ) {
	    configPage->currentInstallation->setText( qtDir );
	}
    }
    else {
	qtDir = QDir::toNativeSeparators( QEnvironment::getFSFileName( optionsPage->installPath->text() ) );
    }

#if defined(Q_OS_WIN32)
    if( qWinVersion() & Qt::WV_NT_based ) {
	// under Windows 9x, we don't compile from the installer -- so there is
	// no need to set the local environment; and doing so, results in not
	// setting the persistent since qtDir\bin is already in the PATH
	path = QStringList::split( QRegExp("[;,]"), QEnvironment::getEnv( "PATH" ) );
	if( path.findIndex( qtDir + "\\bin" ) == -1 ) {
	    path.prepend( qtDir + "\\bin" );
	    QEnvironment::putEnv( "PATH", path.join( ";" ) );
	}
    }
#elif defined(Q_OS_UNIX)
    path = QStringList::split( QRegExp("[:]"), QEnvironment::getEnv( "PATH" ) );
    if( path.findIndex( qtDir + "/bin" ) == -1 ) {
	path.prepend( qtDir + "/bin" );
	QEnvironment::putEnv( "PATH", path.join( ":" ) );
    }
    QStringList dyld = QStringList::split( QRegExp("[:]"), QEnvironment::getEnv( "DYLD_LIBRARY_PATH" ) );
    if( dyld.findIndex( qtDir + "/lib" ) == -1 ) {
	dyld.prepend( qtDir + "/lib" );
	QEnvironment::putEnv( "DYLD_LIBRARY_PATH", dyld.join( ":" ) );
    }
#endif

#if defined(Q_OS_WIN32)
    if( foldersPage && foldersPage->qtDirCheck->isChecked() ) {
	envSpec |= QEnvironment::PersistentEnv;
/*
	if( folderGroups->currentItem() == 0 )
	    envSpec |= QEnvironment::GlobalEnv;
*/
	path.clear();

	if( int( qWinVersion() ) & int( Qt::WV_NT_based ) ) {
	    path = QStringList::split( ';', QEnvironment::getEnv( "PATH", QEnvironment::PersistentEnv ) );
	    if( path.findIndex( qtDir + "\\bin" ) == -1 ) {
		path.prepend( qtDir + "\\bin" );
		QEnvironment::putEnv( "PATH", path.join( ";" ), QEnvironment::PersistentEnv );
	    }
	} else {
	    if( path.findIndex( qtDir + "\\bin" ) == -1 ) {
		QEnvironment::putEnv( "PATH", qtDir + "\\bin;%PATH%", QEnvironment::PersistentEnv );
	    }
	}
    }
#elif defined(Q_OS_UNIX)
//Persistent environment not supported
#endif

    QEnvironment::putEnv( "QTDIR", qtDir, envSpec );
    if ( globalInformation.sysId() != GlobalInformation::Other )
	QEnvironment::putEnv( "QMAKESPEC", globalInformation.text(GlobalInformation::Mkspec), envSpec );
    else
	QEnvironment::putEnv( "QMAKESPEC", optionsPage->sysOtherCombo->currentText(), envSpec );
#if defined(Q_OS_WIN32)
    if( qWinVersion() & WV_NT_based ) {
	SendNotifyMessageA( HWND_BROADCAST, WM_WININICHANGE, 0, (LPARAM)"Environment" );
	SendNotifyMessageA( HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0 );
    }
#endif
}

void SetupWizardImpl::showPageConfig()
{
    if (autoContTimer.isActive()) {
	autoContTimer.stop();
	timeCounter = 30;
	nextButton()->setText("Next >");
    }
#if defined(EVAL) || defined(EDU) || defined(NON_COMMERCIAL)
    setBackEnabled( buildPage, false );

    static bool alreadyInitialized = false;
    if ( !alreadyInitialized ) {
	configPage->installList->setSorting( -1 );
	configPage->installList->setUpdatesEnabled(false);

	CheckListItem *item;
	CheckListItem *folder;

	folder = new CheckListItem( configPage->installList, "Database drivers" );

#if !defined(NON_COMMERCIAL)
#if !defined(Q_OS_MACX)
	item = new CheckListItem( folder, "DB2", QCheckListItem::CheckBox );
	item->addRequiredFiles("db2cli.dll");
	db2PluginInstall = item;
	db2PluginInstall->setHelpText( tr(
		    "Installs the DB2 driver. This driver can "
		    "be used to access DB2 databases."
		    "<p><font color=\"red\">Choosing this option requires "
		    "that the DB2 Client is installed and set up. "
		    "The driver depends on the db2cli.dll.</font></p>"
		    ), configPage->explainOption );

	item = new CheckListItem( folder, "TDS", QCheckListItem::CheckBox );
	item->addRequiredFiles("ntwdblib.dll");
	tdsPluginInstall = item;
	tdsPluginInstall->setHelpText( tr(
		    "Installs the TDS driver to access Sybase Adaptive "
		    "Server and Microsoft SQL Server (it is recommended "
		    "to rather use ODBC instead of TDS where applicable). "
		    "<p><font color=\"red\">Choosing this option requires "
		    "that the ntwdblib.dll is available.</font></p>"
		    ), configPage->explainOption );

	item = new CheckListItem( folder, "Oracle (OCI)", QCheckListItem::CheckBox );
	item->addRequiredFiles( "oci.dll" );
	ociPluginInstall = item;
	ociPluginInstall->setHelpText( tr(
		    "<p>Installs the Oracale Call Interface (OCI) driver.</p> "
		    "<p><font color=\"red\">Choosing this option requires "
		    "that the Oracle Client is installed and set up. "
		    "The driver depends on the oci.dll.</font></p>"
		    ), configPage->explainOption );
#endif

	if ( globalInformation.sysId() != GlobalInformation::Borland ) {
	    // I was not able to make Postgres work with Borland
	    item = new CheckListItem( folder, "PostgreSQL", QCheckListItem::CheckBox );
#ifndef Q_OS_MACX
	    item->addRequiredFiles( "libpq.dll" );
#endif
	    item->setOn( item->verify() );
	    psqlPluginInstall = item;
	    psqlPluginInstall->setHelpText( tr(
			"Installs the PostgreSQL 7.x driver. This driver can "
			"be used to access PostgreSQL 6 databases as well "
			"as PostgreSQL 7 databases."
#ifdef Q_OS_MACX
                        "\n\nRequires a proper PostgreSQL installation."
#endif
			), configPage->explainOption );
	} else {
	    psqlPluginInstall = 0;
	}

	item = new CheckListItem( folder, "MySQL", QCheckListItem::CheckBox );
#ifndef Q_OS_MACX
	item->addRequiredFiles( "libmySQL.dll" );
#endif
	item->setOn( item->verify() );
	mysqlPluginInstall = item;
	mysqlPluginInstall->setHelpText( tr(
		    "Installs the MySQL 3.x database driver."
#ifdef Q_OS_MACX
                        "\n\nRequires a proper MySQL installation."
#endif
		    ), configPage->explainOption );

#if !defined(Q_OS_MAC)
	item = new CheckListItem( folder, "ODBC", QCheckListItem::CheckBox );
	item->setOn( findFile( "odbc32.dll" ) );
	odbcPluginInstall = item;
	odbcPluginInstall->setHelpText( tr(
		    "Installs the Open Database Connectivity (ODBC) driver. "
		    "This driver depends on the odbc32.dll which should be "
		    "available on all modern Windows installations."
		    ), configPage->explainOption );
#endif
#else
	item = new CheckListItem( folder, "SQLite", QCheckListItem::CheckBox );
	item->setOn( true );
	sqlitePluginInstall = item;
    	sqlitePluginInstall->setHelpText( tr(
		    "Installs the SQLite driver.\n"
		    "This driver is an in-process SQL database "
		    "driver. It is needed for some of the "
		    "examples used in the book."
		    ), configPage->explainOption );
#endif

	configPage->installList->setUpdatesEnabled(true);
	alreadyInitialized = true;
    }
#else

    prepareEnvironment();

    bool enterprise = licenseInfo[ "PRODUCTS" ] == "qt-enterprise";
    configPage->configList->setUpdatesEnabled(false);
    configPage->advancedList->setUpdatesEnabled(false);

    if( configPage->configList->childCount() ) {
	QListViewItem* current = configPage->configList->firstChild();

	while( current ) {
	    QListViewItem* next = current->nextSibling();
	    delete current;
	    current = next;
	}

	current = configPage->advancedList->firstChild();
	while( current ) {
	    QListViewItem* next = current->nextSibling();
	    delete current;
	    current = next;
	}
    }
    QSettings settings;
    configPage->configList->setSorting( -1 );
    configPage->advancedList->setSorting( -1 );
    CheckListItem *item;
    CheckListItem *folder;
    QStringList::Iterator it;

    // general
    folder = new CheckListItem ( configPage->configList, "Modules" );
    folder->setHelpText(tr("<p>Some of these modules are optional."
			   "<p>You can deselect the modules that you "
			   "don't require for your development."
			   "<p>By default, all modules are selected."), configPage->explainOption);
    bool settingsOK;
    QStringList entries = settings.readListEntry( "/Trolltech/Qt/Modules", ',', &settingsOK );
    QStringList licensedModules = QStringList::split( " ", "network canvas table xml opengl sql" );
    for( it = licensedModules.begin(); it != licensedModules.end(); ++it ) {
	item = new CheckListItem( folder, (*it), QCheckListItem::CheckBox );
	bool on = entries.isEmpty() || entries.find( *it ) != entries.end();
	item->setOn( enterprise && on );
	item->setEnabled( enterprise );
	if ( enterprise )
	    allModules << *it;
    }

    licensedModules = QStringList::split( " ", "iconview workspace" );
    for( it = licensedModules.begin(); it != licensedModules.end(); ++it ) {
	item = new CheckListItem( folder, (*it), QCheckListItem::CheckBox );
	bool on = entries.isEmpty() || entries.find( *it ) != entries.end();
	item->setOn( on );
	allModules << *it;
    }

    QStringList requiredModules = QStringList::split( " ", "styles dialogs widgets tools kernel" );
    for( it = requiredModules.begin(); it != requiredModules.end(); ++it ) {
	item = new CheckListItem( folder, (*it), QCheckListItem::CheckBox );
	bool on = entries.isEmpty() || entries.find( *it ) != entries.end();
	item->setOn( on );
	item->setEnabled( false );
	allModules << *it;
    }

    folder = new CheckListItem ( configPage->configList, "Threading" );
    folder->setHelpText(tr("<p>Build the Qt library with or without thread support."
			   "<p>By default, threading is supported. Some classes will "
			   "not be available without thread support."), configPage->explainOption);
    QString entry = settings.readEntry( "/Trolltech/Qt/Threading", "Threaded", &settingsOK );
    item = new CheckListItem( folder, "Threaded", QCheckListItem::RadioButton );
    item->setOn( entry == "Threaded" );
    item = new CheckListItem( folder, "Non-threaded", QCheckListItem::RadioButton );
    item->setOn( entry == "Non-threaded" );
    CheckListItem *singleThreaded = item;

    folder = new CheckListItem ( configPage->configList, "Library" );
    folder->setHelpText(tr("<p>Build a shared or a static Qt library."
			   "<p>A shared Qt library makes it necessary to "
			   "distribute the Qt DLL together with your software. "
			   "Applications and libraries linked against a shared Qt library "
			   "are small and can make use of components and plugins."
			   "<p>All applications created with a static "
			   "library will be at least 1.5MB big. "
			   "<font color=\"red\">It is not possible to "
			   "build or use any components or plugins with a "
			   "static Qt library!</font>"), configPage->explainOption);
    entry = settings.readEntry( "/Trolltech/Qt/Library", "Shared", &settingsOK );
    staticItem = new CheckListItem( folder, "Static", QCheckListItem::RadioButton );
    staticItem->setOn( entry == "Static" );
    staticItem->setWarningText("<p>It will not be possible to build components "
			       "or plugins if you select the static build of the Qt library."
			       "<p>New features, e.g souce code editing in Qt Designer, will not "
			       "be available, and you or users of your software might not be able "
			       "to use all or new features, e.g. new styles.");

    item = new CheckListItem( folder, "Shared", QCheckListItem::RadioButton );
    item->setOn( entry == "Shared" );
    item->setWarningText("<p>Single-threaded, shared configurations "
			 "may cause instabilities because of runtime "
			 "library conflicts.", singleThreaded);
    singleThreaded->setWarningText("<p>Single-threaded, shared configurations "
			 "may cause instabilities because of runtime "
			 "library conflicts.", item);

    folder = new CheckListItem ( configPage->configList, "Build" );
    folder->setHelpText(tr("<p>Build a Qt library with or without debug symbols."
			   "<p>Use the debug build of the Qt library to enhance "
			   "debugging of your application. The release build "
			   "is both smaller and faster."), configPage->explainOption);
    entry = settings.readEntry( "/Trolltech/Qt/Build", "Release", &settingsOK );
    item = new CheckListItem( folder, "Debug", QCheckListItem::RadioButton );
    item->setOn( entry == "Debug" );
    item = new CheckListItem( folder, "Release", QCheckListItem::RadioButton );
    item->setOn( entry == "Release" );

    // Advanced options
    if ( globalInformation.sysId() == GlobalInformation::MSVC ) {
	entry = settings.readEntry( "/Trolltech/Qt/DSP Generation", "On", &settingsOK );
	folder = new CheckListItem( configPage->advancedList, "DSP Generation" );
	folder->setHelpText(tr("qmake can generate the Visual Studio 6 project files (dsp) as well "
			       "as makefiles when Qt is being configured."),
			       configPage->explainOption);
	dspOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
	dspOff->setOn( entry == "Off" );
	dspOn = new CheckListItem( folder, "On", QCheckListItem::RadioButton );
	dspOn->setOn( entry == "On" );
    } else if ( globalInformation.sysId() == GlobalInformation::MSVCNET ) {
	entry = settings.readEntry( "/Trolltech/Qt/VCPROJ Generation", "On", &settingsOK );
	folder = new CheckListItem( configPage->advancedList, "VCPROJ Generation" );
	folder->setHelpText(tr("qmake can generate the Visual Studio.NET project files (vcproj) as well "
			       "as makefiles when Qt is being configured."),
			       configPage->explainOption);
	vcprojOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
	vcprojOff->setOn( entry == "Off" );
	vcprojOn = new CheckListItem( folder, "On", QCheckListItem::RadioButton );
	vcprojOn->setOn( entry == "On" );
    }

    CheckListItem *imfolder = new CheckListItem( configPage->advancedList, "Image Formats" );
    imfolder->setHelpText(tr("<p>Qt ships with support for a wide range of common image formats. "
			     "<p>Standard formats are always included in Qt, and some more special formats "
			     "can be left out from the Qt library itself and provided by a plugin instead."),
			     configPage->explainOption);

    folder = new CheckListItem( imfolder, "GIF" );
    folder->setHelpText(tr("<p>Support for GIF images in Qt."
			   "<p><font color=\"red\">If you are in a country "
			   "which recognizes software patents and in which "
			   "Unisys holds a patent on LZW compression and/or "
			   "decompression and you want to use GIF, Unisys "
			   "may require you to license the technology. Such "
			   "countries include Canada, Japan, the USA, "
			   "France, Germany, Italy and the UK.</font>"),
			   configPage->explainOption);
    entry = settings.readEntry( "/Trolltech/Qt/Image Formats/GIF", "Off", &settingsOK );
    gifOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    gifOff->setOn( entry == "Off" );
    gifDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    gifDirect->setOn( entry == "Direct" );

    folder = new CheckListItem( imfolder, "MNG" );
    folder->setHelpText(tr("<p>Qt can support the \"Multiple-Image Network Graphics\" format."
			   "<p>MNG support can be compiled into Qt, provided by a plugin ",
			   "or turned off completely."),
			   configPage->explainOption);
    entry = settings.readEntry( "/Trolltech/Qt/Image Formats/MNG", "Plugin", &settingsOK );
#if 0
    // ### disable using system MNG for now -- please someone take a closer look
    entryPresent = settings.readEntry( "/Trolltech/Qt/Image Formats/MNG Present", "No", &settingsOK );
    mngPresent = new CheckListItem( folder, "Present", QCheckListItem::CheckBox );
    mngPresent->setOn( entry == "Yes" );
#endif
    mngOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    mngOff->setOn( entry == "Off" );
    mngPlugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    mngPlugin->setOn( entry == "Plugin" );
    mngDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    mngDirect->setOn( entry == "Direct" );

    folder = new CheckListItem( imfolder, "JPEG" );
    folder->setHelpText(tr("<p>Qt can support the \"Joint Photographic Experts Group\" format."
			   "<p>JPEG support can be compiled into Qt, provided by a plugin ",
			   "or turned off completely."),
			   configPage->explainOption);
    entry = settings.readEntry( "/Trolltech/Qt/Image Formats/JPEG", "Direct", &settingsOK );
#if 0
    // ### disable using system JPEG for now -- please someone take a closer look
    entryPresent = settings.readEntry( "/Trolltech/Qt/Image Formats/JPEG Present", "No", &settingsOK );
    jpegPresent = new CheckListItem( folder, "Present", QCheckListItem::CheckBox );
    jpegPresent->setOn( entry == "Yes" );
#endif
    jpegOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    jpegOff->setOn( entry == "Off" );
    jpegPlugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    jpegPlugin->setOn( entry == "Plugin" );
    jpegDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    jpegDirect->setOn( entry == "Direct" );

    folder = new CheckListItem( imfolder, "PNG" );
    folder->setHelpText(tr("<p>Qt can support the \"Portable Network Graphics\" format."
			   "<p>PNG support can be compiled into Qt, provided by a plugin ",
			   "or turned off completely."),
			   configPage->explainOption);
    entry = settings.readEntry( "/Trolltech/Qt/Image Formats/PNG", "Direct", &settingsOK );
#if 0
    // ### disable using system PNG for now -- please someone take a closer look
    entryPresent = settings.readEntry( "/Trolltech/Qt/Image Formats/PNG Present", "No", &settingsOK );
    pngPresent = new CheckListItem( folder, "Present", QCheckListItem::CheckBox );
    pngPresent->setOn( entry == "Yes" );
#endif
    pngOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    pngOff->setOn( entry == "Off" );
    // PNG is required by the build system (ie. we use PNG), so don't allow it to be turned off
    pngOff->setEnabled( false );
    pngPlugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    pngPlugin->setOn( entry == "Plugin" );
    pngDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    pngDirect->setOn( entry == "Direct" );

    CheckListItem *sqlfolder = new CheckListItem( configPage->advancedList, "Sql Drivers" );
    sqlfolder->setHelpText(tr("<p>Select the SQL Drivers you want to support."
			      "<p>SQL Drivers can be built into Qt or built as plugins to be more flexible "
			      "for later extensions."
			      "<p><font color=#FF0000>You must have the appropriate client libraries "
			      "and header files installed correctly before you can build the Qt SQL drivers.</font>"),
			   configPage->explainOption);

    folder = new CheckListItem( sqlfolder, "iBase" );
    folder->addRequiredFiles("ibase.h");
    entry = settings.readEntry( "/Trolltech/Qt/Sql Drivers/iBase", "Off", &settingsOK );
    ibaseOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    ibaseOff->setOn( true );
    ibaseOff->setEnabled( enterprise );
    ibasePlugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    ibasePlugin->setOn( entry == "Plugin" && folder->verify() && enterprise );
    ibasePlugin->setEnabled( enterprise );
    ibaseDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    ibaseDirect->setOn( entry == "Direct" && folder->verify() && enterprise );
    ibaseDirect->setEnabled( enterprise );
    if (globalInformation.sysId() == GlobalInformation::Borland)
	folder->addRequiredFiles("gds32.lib");
    else
	folder->addRequiredFiles("gds32_ms.lib");

    folder = new CheckListItem( sqlfolder, "DB2" );
    folder->addRequiredFiles("db2cli.lib,sqlcli1.h");
    entry = settings.readEntry( "/Trolltech/Qt/Sql Drivers/DB2", "Off", &settingsOK );
    db2Off = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    db2Off->setOn( true );
    db2Off->setEnabled( enterprise );
    db2Plugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    db2Plugin->setOn( entry == "Plugin" && folder->verify() && enterprise );
    db2Plugin->setEnabled( enterprise );
    db2Direct = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    db2Direct->setOn( entry == "Direct" && folder->verify() && enterprise );
    db2Direct->setEnabled( enterprise );

    folder = new CheckListItem( sqlfolder, "TDS" );
    folder->addRequiredFiles("ntwdblib.lib,sqldb.h");
    entry = settings.readEntry( "/Trolltech/Qt/Sql Drivers/TDS", "Off", &settingsOK );
    tdsOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    tdsOff->setOn( true );
    tdsOff->setEnabled( enterprise );
    tdsPlugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    tdsPlugin->setOn( entry == "Plugin" && folder->verify() && enterprise );
    tdsPlugin->setEnabled( enterprise );
    tdsDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    tdsDirect->setOn( entry == "Direct" && folder->verify() && enterprise );
    tdsDirect->setEnabled( enterprise );

    folder = new CheckListItem( sqlfolder, "PostgreSQL" );
    folder->addRequiredFiles("libpqdll.lib,libpq-fe.h");
    entry = settings.readEntry( "/Trolltech/Qt/Sql Drivers/PostgreSQL", "Off", &settingsOK );
    psqlOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    psqlOff->setOn( true );
    psqlOff->setEnabled( enterprise );
    psqlPlugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    psqlPlugin->setOn( entry == "Plugin" && folder->verify() && enterprise );
    psqlPlugin->setEnabled( enterprise );
    psqlDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    psqlDirect->setOn( entry == "Direct" && folder->verify() && enterprise );
    psqlDirect->setEnabled( enterprise );

    folder = new CheckListItem( sqlfolder, "OCI" );
    folder->addRequiredFiles("oci.lib,oci.h");
    entry = settings.readEntry( "/Trolltech/Qt/Sql Drivers/OCI", "Off", &settingsOK );
    ociOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    ociOff->setOn( true );
    ociOff->setEnabled( enterprise );
    ociPlugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    ociPlugin->setOn( entry == "Plugin" && folder->verify() && enterprise );
    ociPlugin->setEnabled( enterprise );
    ociDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    ociDirect->setOn( entry == "Direct" && folder->verify() && enterprise );
    ociDirect->setEnabled( enterprise );

    folder = new CheckListItem( sqlfolder, "MySQL" );
    folder->addRequiredFiles("libmysql.lib,mysql.h");
    entry = settings.readEntry( "/Trolltech/Qt/Sql Drivers/MySQL", "Off", &settingsOK );
    mysqlOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    mysqlOff->setOn( true );
    mysqlOff->setEnabled( enterprise );
    mysqlPlugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    mysqlPlugin->setOn( entry == "Plugin" && folder->verify() && enterprise );
    mysqlPlugin->setEnabled( enterprise );
    mysqlDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    mysqlDirect->setOn( entry == "Direct" && folder->verify() && enterprise );
    mysqlDirect->setEnabled( enterprise );

    folder = new CheckListItem( sqlfolder, "SQLite" );
    entry = settings.readEntry( "/Trolltech/Qt/Sql Drivers/SQLite", "Off", &settingsOK );
    sqliteOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    sqliteOff->setOn( true );
    sqliteOff->setEnabled( enterprise );
    sqlitePlugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    sqlitePlugin->setOn( entry == "Plugin" && folder->verify() && enterprise );
    sqlitePlugin->setEnabled( enterprise );
    sqliteDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    sqliteDirect->setOn( entry == "Direct" && folder->verify() && enterprise );
    sqliteDirect->setEnabled( enterprise );

    folder = new CheckListItem( sqlfolder, "ODBC" );
    folder->addRequiredFiles("odbc32.lib,sql.h");
    entry = settings.readEntry( "/Trolltech/Qt/Sql Drivers/ODBC", "Off", &settingsOK );
    odbcOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    odbcOff->setOn( true );
    odbcOff->setEnabled( enterprise );
    odbcPlugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    odbcPlugin->setOn( entry == "Plugin" && folder->verify() && enterprise );
    odbcPlugin->setEnabled( enterprise );
    odbcDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    odbcDirect->setOn( entry == "Direct" && folder->verify() && enterprise );
    odbcDirect->setEnabled( enterprise );

    CheckListItem *stfolder = new CheckListItem( configPage->advancedList, "Styles" );
    stfolder->setHelpText(tr("Select support for the various GUI styles that Qt supports." ),configPage->explainOption);

    folder = new CheckListItem( stfolder, "SGI" );
    entry = settings.readEntry( "/Trolltech/Qt/Styles/SGI", "Plugin", &settingsOK );
    sgiOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    sgiOff->setOn( entry == "Off" );
    sgiPlugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    sgiPlugin->setOn( entry == "Plugin" );
    sgiDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    sgiDirect->setOn( entry == "Direct" );

    folder = new CheckListItem( stfolder, "CDE" );
    entry = settings.readEntry( "/Trolltech/Qt/Styles/CDE", "Plugin", &settingsOK );
    cdeOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    cdeOff->setOn( entry == "Off" );
    cdePlugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    cdePlugin->setOn( entry == "Plugin" );
    cdeDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    cdeDirect->setOn( entry == "Direct" );

    folder = new CheckListItem( stfolder, "MotifPlus" );
    entry = settings.readEntry( "/Trolltech/Qt/Styles/MotifPlus", "Plugin", &settingsOK );
    motifplusOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    motifplusOff->setOn( entry == "Off" );
    motifplusPlugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    motifplusPlugin->setOn( entry == "Plugin" );
    motifplusDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    motifplusDirect->setOn( entry == "Direct" );

    folder = new CheckListItem( stfolder, "Platinum" );
    entry = settings.readEntry( "/Trolltech/Qt/Styles/Platinum", "Plugin", &settingsOK );
    platinumOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    platinumOff->setOn( entry == "Off" );
    platinumPlugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    platinumPlugin->setOn( entry == "Plugin" );
    platinumDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    platinumDirect->setOn( entry == "Direct" );

    folder = new CheckListItem( stfolder, "Motif" );
    entry = settings.readEntry( "/Trolltech/Qt/Styles/Motif", "Plugin", &settingsOK );
    motifOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    motifOff->setOn( entry == "Off" );
    motifPlugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    motifPlugin->setOn( entry == "Plugin" );
    motifDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    motifDirect->setOn( entry == "Direct" );

    folder = new CheckListItem( stfolder, "Windows XP" );
    folder->addRequiredFiles("uxtheme.h");
    folder->setRequiredFileLocation("part of the Microsoft Platform SDK, which is usually available for "
				    "download from the following location:"
				    "<p>http://www.microsoft.com/msdownload/platformsdk/sdkupdate/<p>");

    entry = settings.readEntry( "/Trolltech/Qt/Styles/Windows XP", "Direct", &settingsOK );
    xpOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    xpOff->setOn( true );
    xpPlugin = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    xpPlugin->setOn( entry == "Plugin" && folder->verify() );
    xpPlugin->setEnabled( folder->verify() );
    xpDirect = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    xpDirect->setOn( entry == "Direct" && folder->verify() );
    xpDirect->setEnabled( folder->verify() );

    folder = new CheckListItem( stfolder, "Windows" );
    entry = settings.readEntry( "/Trolltech/Qt/Styles/Windows", "Direct", &settingsOK );
    item = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    item->setEnabled( false );
    item->setOn( entry == "Off" );
    item = new CheckListItem( folder, "Plugin", QCheckListItem::RadioButton );
    item->setEnabled( false );
    item->setOn( entry == "Plugin" );
    item = new CheckListItem( folder, "Direct", QCheckListItem::RadioButton );
    item->setOn( entry == "Direct" );

    entries = settings.readListEntry( "/Trolltech/Qt/Advanced C++", ',', &settingsOK );
    folder = new CheckListItem( configPage->advancedList, "Advanced C++" );
    folder->setHelpText(tr("Qt can be built with exception handling, STL support and RTTI support "
			   "enabled or disabled. Qt itself doesn't use any of those features."
			   "The default is to disable all advanced C++ features."),
			   configPage->explainOption);
    advancedRTTI = new CheckListItem( folder, "RTTI", QCheckListItem::CheckBox );
    advancedRTTI->setOn( entries.contains( "RTTI" ) );
    advancedExceptions = new CheckListItem( folder, "Exceptions", QCheckListItem::CheckBox );
    advancedExceptions->setOn( entries.contains( "Exceptions" ) );
    advancedSTL = new CheckListItem( folder, "STL", QCheckListItem::CheckBox );
    advancedSTL->setOn( entries.contains( "STL" ) );

    folder = new CheckListItem( configPage->advancedList, "Tablet Support" );
    folder->addRequiredFiles("wintab.h,wintab.lib");
    folder->setRequiredFileLocation("available at http://www.pointing.com/FTP.HTM");
    folder->setHelpText(tr("Qt can support the Wacom brand tablet device."), configPage->explainOption);
    entry = settings.readEntry( "/Trolltech/Qt/Tablet Support", "Off", &settingsOK );
    tabletOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    tabletOff->setOn( true );
    tabletOn = new CheckListItem( folder, "On", QCheckListItem::RadioButton );
    tabletOn->setOn( entry == "On" && folder->verify() );

    folder = new CheckListItem( configPage->advancedList, "Accessibility" );
    folder->setHelpText(tr("<p>Accessibility means making software usable and accessible to a wide "
			   "range of users, including those with disabilities."
			   "This feature is only available with a shared Qt library."),
			   configPage->explainOption);
    entry = settings.readEntry( "/Trolltech/Qt/Accessibility", "On", &settingsOK );
    accOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    accOff->setOn( entry == "Off" );
    accOn = new CheckListItem( folder, "On", QCheckListItem::RadioButton );
    accOn->setOn( entry == "On" );

    entry = settings.readEntry( "/Trolltech/Qt/Big Textcodecs", "On", &settingsOK );
    folder = new CheckListItem( configPage->advancedList, "Big Textcodecs" );
    folder->setHelpText(tr("Textcodecs provide translations between text encodings. For "
			   "languages and script systems with many characters it is necessary "
			   "to have big data tables that provide the translation. Those codecs "
			   "can be left out of the Qt library and will be loaded on demand.\n"
			   "Having the codecs in a plugin is not available with a static Qt "
			   "library."), configPage->explainOption);
    bigCodecsOff = new CheckListItem( folder, "Off", QCheckListItem::RadioButton );
    bigCodecsOff->setOn( entry == "Off" );
    bigCodecsOn = new CheckListItem( folder, "On", QCheckListItem::RadioButton );
    bigCodecsOn->setOn( entry == "On" );

    setStaticEnabled( staticItem->isOn() );
    setJpegDirect( mngDirect->isOn() );

    configPage->configList->setUpdatesEnabled(true);
    configPage->advancedList->setUpdatesEnabled(true);

    // Needed to force the scrollbars to appear on Windows 9x...
    QListViewItem *dummy = new QListViewItem(configPage->configList, "Dummy Item");
    delete dummy;

    setBackEnabled( buildPage, false );
#endif
}

void SetupWizardImpl::showPageBuild()
{
    autoContTimer.stop();
    nextButton()->setText( "Next >" );
    saveSettings();

#if defined(Q_OS_WIN32)
    if( globalInformation.reconfig() && configPage->rebuildInstallation->isChecked() && qWinVersion() & WV_NT_based ) {
	QStringList args;

	buildPage->compileProgress->hide();
	buildPage->restartBuild->hide();

    	args << globalInformation.text(GlobalInformation::MakeTool) << "clean";
	logOutput( "Starting cleaning process" );
	connect( &cleaner, SIGNAL( processExited() ), this, SLOT( cleanDone() ) );
	connect( &cleaner, SIGNAL( readyReadStdout() ), this, SLOT( readCleanerOutput() ) );
	connect( &cleaner, SIGNAL( readyReadStderr() ), this, SLOT( readCleanerError() ) );
	cleaner.setWorkingDirectory( QEnvironment::getEnv( "QTDIR" ) );
	cleaner.setArguments( args );
	if( !cleaner.start() ) {
	    logOutput( "Could not start cleaning process" );
	    emit wizardPageFailed( indexOf(currentPage()) );
	}
    } else
#endif
	cleanDone();	// We're not doing a reconfig, so skip the clean step

}

static bool verifyHelper(QListView *listview, bool result)
{
    QListViewItemIterator it(listview);
    while (it.current()) {
	QListViewItem *item = it.current();
	++it;
	if (item->rtti() != CheckListItem::RTTI)
	    continue;

	CheckListItem *checkItem = (CheckListItem*)item;
	if (!checkItem->isOn())
	    continue;

	bool r = checkItem->verify();
	checkItem->setCritical(!r);
	if (result) result = r;
    }
    return result;
}

bool SetupWizardImpl::verifyConfig()
{
    bool result = true;
#if !defined(EVAL) && !defined(EDU) && !defined(NON_COMMERCIAL)
    result = verifyHelper(configPage->configList, result);
    result = verifyHelper(configPage->advancedList, result);
#endif
    return result;
}
