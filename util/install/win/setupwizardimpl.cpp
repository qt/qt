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
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qtabwidget.h>
#include <qarchive.h>
#include <qvalidator.h>
#include <qdatetime.h>
#include <qlayout.h>

#include <keyinfo.h>
#if defined(Q_OS_WIN32)

#include <process.h>
#endif

#if defined(EVAL) || defined(EDU) || defined(NON_COMMERCIAL)
#include <check-and-patch.h>
#endif

#if defined(EVAL)
#  define LICENSE_DEST "LICENSE.EVAL"
#elif defined(EDU)
#  define LICENSE_DEST "LICENSE.EDU"
#elif defined(NON_COMMERCIAL)
#  define LICENSE_DEST "LICENSE.NON_COMMERCIAL"
#else
#  define LICENSE_DEST "LICENSE"
#endif

#include "resource.h"
#include "pages/sidedecorationimpl.h"

#define FILESTOCOPY 4582

static const char* const logo_data[] = {
"32 32 238 2",
"Qt c None",
"#u c #000000",
".# c #020204",
"a. c #102322",
"af c #282500",
"as c #292e26",
"a8 c #2c686a",
"ae c #307072",
"#C c #322a0c",
"#s c #36320c",
"am c #3b3d3f",
"#3 c #3c8082",
"#f c #3e3a0c",
"## c #423e0c",
"#9 c #434341",
"ad c #438888",
"aU c #458d8e",
"#g c #46420c",
"aM c #46494a",
"ay c #474948",
"#D c #4a4328",
".W c #4a4611",
"az c #4a4641",
"a1 c #4a4a49",
"aH c #4b9e9e",
"au c #4d9a9f",
"aS c #4e9a9a",
"an c #4f4e4a",
".X c #504e0c",
"a7 c #51a4a9",
"#0 c #525250",
"aT c #55a6a3",
".Y c #56520c",
"#a c #5a5604",
".Z c #5e5a0c",
".V c #5e5e5c",
"a0 c #5e5e60",
"a6 c #5ea0a6",
".J c #625e0c",
"bB c #64aaa9",
"#m c #665e2c",
"aL c #686867",
"bw c #68acb2",
"bo c #696928",
"ba c #696967",
"aE c #69aeb2",
"#z c #6a5614",
".K c #6a660c",
"aZ c #6a6a65",
"bG c #6db4b4",
".9 c #6e5e24",
"#. c #6e6a5c",
"bv c #6fb6b9",
"bC c #706d28",
"br c #70bcc5",
"aQ c #71b7ba",
".I c #726234",
".L c #726e0c",
".0 c #72720c",
"#w c #746d44",
"be c #747028",
"bH c #747428",
".M c #76720a",
"aR c #78c1c2",
"#Z c #797977",
"a2 c #7a5d3d",
"#H c #7a6614",
"#I c #7a760a",
"#l c #7a7634",
".1 c #7a7a0c",
"#e c #7a7a5c",
"bL c #7bc0c2",
"b. c #7c7d82",
"#d c #7e6e34",
".N c #7e7a0a",
"bP c #816c20",
".8 c #82763c",
"#h c #827a3c",
".x c #827e0c",
"#t c #827f4b",
".O c #828204",
"#v c #828384",
".P c #868604",
"bq c #87d4d9",
"#k c #89864b",
"#c c #8a8244",
".y c #8a8604",
"#j c #8d8652",
"al c #8d8d8a",
"#b c #8e8644",
".z c #8e8e04",
"aW c #8f9094",
"#i c #908952",
"#Q c #909021",
"ag c #90d0d2",
"bO c #916f34",
"bQ c #91cdd3",
".7 c #928a44",
"#p c #928e6c",
"#P c #947f2f",
".A c #949204",
"bh c #949495",
".6 c #968e4c",
"aC c #999721",
".w c #9a8a44",
"#M c #9a9a99",
"ap c #9b9b21",
".5 c #9c924c",
"#R c #9c9a04",
"#7 c #9d9d9b",
"ao c #9e7641",
".4 c #9e964c",
"#J c #9e9b21",
".B c #9e9e04",
"ac c #9e9e9d",
"#S c #a09e21",
"ax c #a0a0a3",
"aK c #a1a1a2",
"aX c #a1a1a4",
".r c #a2a204",
"#1 c #a2a221",
"aF c #a2e1dd",
".3 c #a49a54",
".2 c #a69e54",
"bR c #a78446",
"#6 c #a9a9a8",
".T c #aaa254",
".s c #aaaa04",
"#W c #abaaa6",
"aN c #ac8861",
".S c #aea25c",
".R c #aea65c",
".t c #aeae04",
"#L c #b0b0b0",
"#o c #b2ae94",
".u c #b2b204",
"aI c #b2b2b4",
"b# c #b3b3b2",
"#X c #b4b4b6",
"#V c #b5b4b4",
".Q c #b6aa5c",
".n c #b6b604",
"aY c #b6b6b7",
"bN c #b79658",
"ah c #b7e5e3",
"aG c #b7ebe9",
"ar c #b9d9dc",
"#8 c #bcbcbe",
"ab c #bdbdbe",
".m c #beae5c",
".F c #beb264",
"aq c #bef6f6",
"aB c #c1a470",
"#F c #c1c1c3",
".E c #c2b664",
"at c #c2e9eb",
"bI c #c39c6a",
"bs c #c3a366",
"#U c #c3c3c0",
"aw c #c3c3c1",
"#G c #c3c3c7",
"aD c #c3f1f2",
"a# c #c6c6c3",
"#2 c #c7edf3",
".D c #c8ba6c",
"bM c #c9a470",
"#N c #c9c9c4",
".C c #cabe6c",
"ak c #cacaca",
"bx c #cbb076",
"aa c #cbcbc9",
"a3 c #ccac7f",
".H c #ceba54",
"#E c #ceced0",
"bi c #cfaf7e",
"#Y c #cfcfcb",
"bK c #d1ac80",
"#5 c #d1d1cf",
"bu c #d2ae83",
"bm c #d3b180",
"bD c #d3b384",
"bF c #d4b589",
"aJ c #d4d4d3",
".j c #d6c664",
".v c #d6c674",
"#K c #d6d6d5",
"bJ c #d7b588",
"bd c #d8b289",
"bz c #d8b78d",
".q c #d8ca74",
"aj c #d8d8d9",
"bb c #dabd97",
"a5 c #dcba91",
"bE c #dcc097",
"aA c #ddc292",
"aP c #dec491",
".p c #dece75",
"bk c #dfc79c",
"av c #e0e0e0",
"#A c #e2dabc",
"#O c #e2e2e4",
"aO c #e3c898",
"by c #e4c7a1",
".l c #e6da84",
"a4 c #e7c7a2",
"bt c #eacaa5",
".o c #eede84",
".G c #eee284",
".i c #eee294",
"bn c #efd7b4",
".k c #f2e69c",
".e c #f2eaa4",
"bc c #f3d8b8",
"bj c #f5e2c8",
"#r c #f6eea4",
".f c #f6eeb8",
".g c #f6f2cc",
".c c #faf6d4",
".d c #fafae4",
".U c #feee95",
"bA c #fef26c",
"#q c #fef2ac",
"#x c #fef2b8",
"bp c #fef684",
"bl c #fef690",
"bg c #fef69c",
"bf c #fef6a4",
".a c #fef6b4",
"#B c #fef6c4",
"#y c #fef6ce",
"a9 c #fefaac",
"aV c #fefab7",
"ai c #fefac4",
"#4 c #fefad1",
"#n c #fefadf",
".h c #fefaec",
"#T c #fefee6",
".b c #fefefa",
"QtQtQtQtQtQtQtQtQtQtQtQt.#QtQtQtQtQtQt.#QtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQt.#.#.a.#QtQtQtQt.#.b.#.#QtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQt.#.#.a.a.a.a.#QtQt.#.c.d.b.b.#.#QtQtQtQtQtQtQtQt",
"QtQtQtQtQtQt.#.#.e.e.e.e.e.e.e.#.#.f.f.g.g.c.h.b.#.#QtQtQtQtQtQt",
"QtQtQtQt.#.#.i.i.i.i.i.i.i.i.j.j.b.b.k.e.f.f.g.g.c.d.#.#QtQtQtQt",
"QtQt.#.#.l.l.l.l.l.l.l.l.m.m.n.n.o.o.b.b.k.k.e.f.f.g.g.c.#.#QtQt",
"Qt.#.p.p.q.p.p.p.p.p.m.m.r.s.t.u.p.q.v.v.b.b.i.i.k.e.f.f.g.g.#Qt",
"QtQt.#.j.j.j.j.j.w.w.x.y.z.A.B.r.C.C.D.E.E.F.b.b.o.G.k.k.e.#QtQt",
"QtQtQt.#.H.H.I.I.J.K.L.M.N.O.P.z.Q.Q.Q.R.R.R.S.T.b.b.G.G.#QtQtQt",
"Qt.#.#.U.V.W.W.W.X.Y.Z.J.K.L.0.1.2.3.3.4.5.5.6.6.7.8.9#..b.#.#Qt",
"QtQtQt.#.U.U.U.W.W##.W.X.Y#a.J.K.7.7#b#b#b#c#c#d#d.h.h.b.#QtQtQt",
"QtQtQtQt.##e.U.U.U.W.W#f#g.W.X.Y#h#i#j#k#l#m#m#n#n.h#g.#QtQtQtQt",
"QtQtQtQt.##o#p#e#q#q#r#f#f#s#f#g#t#u#v#u#w#x#y#y#g#g#z.#QtQtQtQt",
"QtQtQtQt.#.b#A#o#p#e#B#B#B#C#C#D#u#E#F#G#u#y#g#g#H#I#J#uQtQtQtQt",
"QtQtQtQt.#.b.h.b#A#o#p#e.d.d.d#u#K#L#M#N#O#u#P#Q#R#S#u#u#uQtQtQt",
"QtQtQtQt.#.b#T.h#T#n#A#o#p#e#u#U#V#W#X#Y#Z#0#u#1#S#u#2#3#uQtQtQt",
"QtQtQtQt.##T.h#T#n#T#n#4#A#u#5#6#7#6#8#Z#0#9#u#S#u#2#3a.Qt#u#uQt",
"QtQtQtQt.##T#n#T#4#4#4#4#u#Oa#aaabac#Z#9#9#u#S#u#2adaeaf#uagah#u",
"QtQtQtQt.##n#n#4#4#4ai#u#Oajak#Yalamanao#u#Sap#uaqar#3asagatau#u",
"QtQtQtQt.##T#4#4#4ai#uav#O#OawaxayazaAaB#uapaC#uaDaEaFaGataH#uQt",
"QtQtQtQt.##4#4aiaiai#uaIaJ#OaKaLaMaNaOaPao#u#uaDaQaRaSaTaU#uQtQt",
"QtQtQtQt.##4aiaV.aaV#uaWaXaYaZa0a1a2a3a4a5ao#ua6a7a8#u#u#uQtQtQt",
"QtQtQtQt.#aiaiaiaV.aa9#ub.b#baa0#u#1#ubbbcbdao#ua8#ube.#.#.#.#Qt",
"QtQtQtQt.#aV.aaVaVbfbfbg#ubhba#u#1.AaC#ubibjbkao#ube#a.#.#.#.#.#",
"QtQtQtQt.#.aa9.abfa9bgbgbg#u#ubl.AaC#uaD#ubmbna5ao#ubo.#.#.#.#Qt",
"QtQtQtQt.#.aa9a9bgbgblblblblbpbpaC#uaDbqbr#ubsbtbuao#u.#.#QtQtQt",
"QtQtQtQtQt.#.#bgbgbgblblbpbpbpbp#uatbvbwa8#u#ubxbybzao#uQtQtQtQt",
"QtQtQtQtQtQtQt.#.#blblbpbpbAbp#uaDbBbwa8#ubebC#ubDbEbFao#uQtQtQt",
"QtQtQtQtQtQtQtQtQt.#.#bAbpbA#uaDbGbwa8#ubH.#.#Qt#ubIbJbKao#uQtQt",
"QtQtQtQtQtQtQtQtQtQtQt.#.##uaDbLbwa8#u.#.#QtQtQtQt#ubMbNbObP#uQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQt#ubQbwa8#u.#QtQtQtQtQtQtQt#ubRbO#uQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQt#u#u#uQtQtQtQtQtQtQtQtQtQt#u#uQtQtQt"};

static bool findFileInPaths( const QString &fileName, const QStringList &paths )
{
	QDir d;
	for( QStringList::ConstIterator it = paths.begin(); it != paths.end(); ++it ) {
	    // Remove any leading or trailing ", this is commonly used in the environment
	    // variables
	    QString path = (*it);
	    if ( path.startsWith( "\"" ) )
		path = path.right( path.length() - 1 );
	    if ( path.endsWith( "\"" ) )
		path = path.left( path.length() - 1 );
	    if( d.exists( path + QDir::separator() + fileName ) )
		return true;
	}
	return false;
}

bool findFile( const QString &fileName )
{
    QString file = fileName.lower();
    QStringList paths;
#if defined(Q_OS_WIN32)
    QRegExp split( "[;,]" );
#else
    QRegExp split( "[:]" );
#endif
    if ( file.endsWith( ".h" ) ) {
	if ( globalInformation.sysId() == GlobalInformation::Borland )
	    return true;
	paths = QStringList::split( split, QEnvironment::getEnv( "INCLUDE" ) );
    } else if ( file.endsWith( ".lib" ) ) {
	if ( globalInformation.sysId() == GlobalInformation::Borland )
	    return true;
	paths = QStringList::split( split, QEnvironment::getEnv( "LIB" ) );
    } else {
	paths = QStringList::split( split, QEnvironment::getEnv( "PATH" ) );
    }
    return findFileInPaths( file, paths );
}

static bool createDir( const QString& fullPath )
{
    QStringList hierarchy = QStringList::split( QDir::separator(), fullPath );
    QString pathComponent, tmpPath;
    QDir dirTmp;
    bool success = true;

    for( QStringList::Iterator it = hierarchy.begin(); it != hierarchy.end(); ++it ) {
	pathComponent = *it + QDir::separator();
	tmpPath += pathComponent;
#if defined(Q_OS_WIN32)
	success = dirTmp.mkdir( tmpPath );
#else
	success = dirTmp.mkdir( QDir::separator() + tmpPath );
#endif
    }
    return success;
}

SetupWizardImpl::SetupWizardImpl( QWidget* parent, const char* name, bool modal, WindowFlags flag ) :
    QWizard( parent, name, modal, flag ),
    tmpPath( QEnvironment::getTempPath() ),
    fixedPath(false),
    filesCopied( false ),
    filesToCompile( 0 ),
    filesCompiled( 0 ),
    licensePage( 0 ),
    licenseAgreementPage( 0 ),
    licenseAgreementPageQsa( 0 ),
    optionsPage( 0 ),
    optionsPageQsa( 0 ),
    foldersPage( 0 ),
    configPage( 0 ),
    progressPage( 0 ),
    buildPage( 0 ),
    finishPage( 0 )
{
    // initialize
    if ( !name )
	setName( "SetupWizard" );
    resize( 600, 390 );
#if defined(QSA)
    setCaption( trUtf8( "QSA Installation Wizard" ) );
#else
    setCaption( trUtf8( "Qt Installation Wizard" ) );
#endif
    QPixmap logo( ( const char** ) logo_data );
    setIcon( logo );
#if defined(QSA)
    setIconText( trUtf8( "QSA Installation Wizard" ) );
#else
    setIconText( trUtf8( "Qt Installation Wizard" ) );
#endif
    QFont f( font() );
    f.setFamily( "Arial" );
    f.setPointSize( 12 );
    f.setBold( true );
    setTitleFont( f );

    totalFiles = 0;

    // try to read the archive header information and use them instead of
    // QT_VERSION_STR if possible
    QArchiveHeader *archiveHeader = 0;
    ResourceLoader rcLoader( "QT_ARQ", 500 );
    if ( rcLoader.isValid() ) {
	// First, try to find qt.arq as a binary resource to the file.
	QArchive ar;
	QDataStream ds( rcLoader.data(), IO_ReadOnly );
	archiveHeader = ar.readArchiveHeader( &ds );
    } else {
	// If the resource could not be loaded or is smaller than 500
	// bytes, we have the dummy qt.arq: try to find and install
	// from qt.arq in the current directory instead.
	QArchive ar;
	QString archiveName = "qt.arq";
# if defined(Q_OS_MAC)
	QString appDir = qApp->argv()[0];
	int truncpos = appDir.findRev( "/Contents/MacOS/" );
	if (truncpos != -1)
	    appDir.truncate( truncpos );
	archiveName  = appDir + "/Contents/Qt/qtmac.arq";
# endif
	ar.setPath( archiveName );
	if( ar.open( IO_ReadOnly ) )  {
	    archiveHeader = ar.readArchiveHeader();
	}
    }

#if defined(QSA)
    ResourceLoader rcLoaderQsa( "QSA_ARQ", 500 );
    if ( rcLoaderQsa.isValid() ) {
	// First, try to find qt.arq as a binary resource to the file.
	QArchive ar;
	QDataStream ds( rcLoaderQsa.data(), IO_ReadOnly );
	QArchiveHeader *archiveHeaderQsa = ar.readArchiveHeader( &ds );
	if ( archiveHeaderQsa ) {
	    QString qsa_version_str = archiveHeaderQsa->description();
	    if ( !qsa_version_str.isEmpty() )
		globalInformation.setQsaVersionStr( qsa_version_str );
	    delete archiveHeaderQsa;
	}
    }
#endif

#if defined(Q_OS_WIN32)
    // First check for MSVC 6.0
    QString regValue = QEnvironment::getRegistryString( "Software\\Microsoft\\VisualStudio\\6.0\\Setup\\Microsoft Visual C++", "ProductDir", QEnvironment::LocalMachine );
    if (!regValue.isEmpty())
	globalInformation.setSysId(GlobalInformation::MSVC);

    // MSVC.NET 7.0 & 7.1 takes presedence over 6.0
    regValue = QEnvironment::getRegistryString( "Software\\Microsoft\\VisualStudio\\7.0", "InstallDir", QEnvironment::LocalMachine );
    if (regValue.isEmpty())
	regValue = QEnvironment::getRegistryString( "Software\\Microsoft\\VisualStudio\\7.1", "InstallDir", QEnvironment::LocalMachine );
    if (!regValue.isEmpty())
	globalInformation.setSysId(GlobalInformation::MSVCNET);

    while (globalInformation.sysId() == GlobalInformation::Other) {
 	globalInformation.setSysId(GlobalInformation::Borland);
 	if (findFile(globalInformation.text(GlobalInformation::MakeTool)))
 	    break;
 	globalInformation.setSysId(GlobalInformation::MSVCNET);
 	if (findFile(globalInformation.text(GlobalInformation::MakeTool)))
 	    break;
 	globalInformation.setSysId(GlobalInformation::MinGW);
 	if (findFile(globalInformation.text(GlobalInformation::MakeTool)))
 	    break;
 	globalInformation.setSysId(GlobalInformation::Watcom);
 	if (findFile(globalInformation.text(GlobalInformation::MakeTool)))
 	    break;
     }
#endif

    if ( archiveHeader ) {
	QString qt_version_str = archiveHeader->description();
	if ( !qt_version_str.isEmpty() )
	    globalInformation.setQtVersionStr( qt_version_str );

#if defined(EVAL) || defined(EDU) || defined(NON_COMMERCIAL)
	if ( archiveHeader->findExtraData( "compiler" ) == "borland" )
	    globalInformation.setSysId(GlobalInformation::Borland);
#endif
	delete archiveHeader;
    }

    initPages();
    initConnections();

    if (optionsPage) {
#if defined(QSA)
	optionsPage->installPath->setText(
	    QString( "C:\\Qt_QSA\\Qt" ) +
	    QString( globalInformation.qtVersionStr() ).replace( QRegExp("\\s"), "" ).replace( QRegExp("-"), "" )
	    );
#endif
    }
    if ( optionsPageQsa ) {
#if defined(QSA)
	optionsPageQsa->installPath->setText(
	    QString( "C:\\Qt_QSA\\QSA" ) +
	    QString( globalInformation.qsaVersionStr() ).replace( QRegExp("\\s"), "" ).replace( QRegExp("-"), "" )
	    );
#endif
    }
    readLicense( QDir::homeDirPath() + "/.qt-license" );
}

static bool copyFile( const QString& src, const QString& dest )
{
#ifdef Q_WS_WIN
    QT_WA( {
	return CopyFileW( (const wchar_t*)src.ucs2(), (const wchar_t*)dest.ucs2(), false );
    }, {
	return CopyFileA( src.local8Bit(), dest.local8Bit(), false );
    } );
#else
    int len;
    const int buflen = 4096;
    char buf[buflen];
    QFileInfo info( src );
    QFile srcFile( src ), destFile( dest );
    if (!srcFile.open( IO_ReadOnly ))
	return false;
    destFile.remove();
    if (!destFile.open( IO_WriteOnly ))
	return false;

    while (!srcFile.atEnd()) {
	len = srcFile.readBlock( buf, buflen );
	if (len <= 0)
	    break;
	if (destFile.writeBlock( buf, len ) != len)
	    return false;
    }
    destFile.flush();
    return true;
#endif
}

void SetupWizardImpl::initPages()
{
#define ADD_PAGE( var, Class ) \
    { \
    var = new Class( this, #var ); \
    SideDecorationImpl *sideDeco = new SideDecorationImpl( var ); \
    \
    Q_ASSERT( var->layout() != 0 ); \
    if ( var->layout()->inherits("QBoxLayout") ) { \
	((QBoxLayout*)var->layout())->insertWidget( 0, sideDeco ); \
	((QBoxLayout*)var->layout())->insertSpacing( 1, 10 ); \
    } \
    \
    pages.append( var ); \
    addPage( var, var->title() ); \
    setHelpEnabled( var, false ); \
    \
    connect( this, SIGNAL(wizardPages(const QPtrList<Page>&)), \
	    sideDeco, SLOT(wizardPages(const QPtrList<Page>&)) ); \
    connect( this, SIGNAL(wizardPageShowed(int)), \
	    sideDeco, SLOT(wizardPageShowed(int)) ); \
    connect( this, SIGNAL(wizardPageFailed(int)), \
	    sideDeco, SLOT(wizardPageFailed(int)) ); \
    connect( this, SIGNAL(editionString(const QString&)), \
	    sideDeco->editionLabel, SLOT(setText(const QString&)) ); \
    }

    QPtrList<Page> pages;
    if( globalInformation.reconfig() ) {
	ADD_PAGE( configPage,	ConfigPageImpl  )
	ADD_PAGE( buildPage,	BuildPageImpl   )
	ADD_PAGE( finishPage,	FinishPageImpl  )
    } else {
#if defined(Q_OS_WIN32)
	ADD_PAGE( winIntroPage,		WinIntroPageImpl	)
#endif
#if !defined(EVAL_CD) && !defined(NON_COMMERCIAL)
	ADD_PAGE( licensePage,		LicensePageImpl		)
#endif
	ADD_PAGE( licenseAgreementPage, LicenseAgreementPageImpl)
#if defined(QSA)
	ADD_PAGE( licenseAgreementPageQsa, LicenseAgreementPageImpl)
#endif
	ADD_PAGE( optionsPage,		OptionsPageImpl		)
#if defined(QSA)
	ADD_PAGE( optionsPageQsa,	OptionsPageImpl		)
#endif
#if !defined(Q_OS_UNIX)
	ADD_PAGE( foldersPage,		FoldersPageImpl		)
#endif
	ADD_PAGE( configPage,		ConfigPageImpl		)
	ADD_PAGE( progressPage,		ProgressPageImpl	)
	ADD_PAGE( buildPage,		BuildPageImpl		)
	ADD_PAGE( finishPage,		FinishPageImpl		)
    }
#undef ADD_PAGE

    if ( licensePage ) {
	setNextEnabled( licensePage, false );
    }
    if ( licenseAgreementPage ) {
	setNextEnabled( licenseAgreementPage, false );
    }
    if ( licenseAgreementPageQsa ) {
	setNextEnabled( licenseAgreementPageQsa, false );
	licenseAgreementPage->titleStr = "License agreement Qt";
	licenseAgreementPageQsa->titleStr = "License agreement QSA";
    }
    if ( optionsPage ) {
	setBackEnabled( optionsPage, false );
    }
    if ( optionsPageQsa ) {
	optionsPageQsa->installExamples->hide();
	optionsPageQsa->installTools->hide();
	optionsPageQsa->installExtensions->hide();
	optionsPageQsa->installTutorials->hide();
	optionsPageQsa->skipBuild->hide();
	optionsPageQsa->installDocs->hide();
	optionsPageQsa->sysGroup->hide();
	optionsPageQsa->pathLabel->setText("QSA destination &path");

	optionsPage->titleStr = "Options for Qt";
	optionsPage->shortTitleStr = "Choose options for Qt";
	optionsPageQsa->titleStr = "Options for QSA";
	optionsPageQsa->shortTitleStr = "Choose options for QSA";
    }
    if ( configPage )
	setBackEnabled( configPage, false );
    if ( progressPage ) {
	setBackEnabled( progressPage, false );
	setNextEnabled( progressPage, false );
    }
    if ( buildPage ) {
	setBackEnabled( buildPage, false );
	setNextEnabled( buildPage, false );
    }
    if ( finishPage ) {
	setBackEnabled( finishPage, false );
	setFinishEnabled( finishPage, true );
    }
    emit wizardPages( pages );
}

void SetupWizardImpl::initConnections()
{
    connect( &autoContTimer, SIGNAL( timeout() ), this, SLOT( timerFired() ) );

    if ( optionsPage ) {
	connect( optionsPage->sysGroup, SIGNAL(clicked(int)), SLOT(clickedSystem(int)));
	connect( optionsPage->sysOtherCombo, SIGNAL(activated(int)), SLOT(sysOtherComboChanged(int)));
    }
    if ( foldersPage ) {
	connect( foldersPage->folderPathButton, SIGNAL(clicked()), SLOT(clickedFolderPath()));
	connect( foldersPage->devSysPathButton, SIGNAL(clicked()), SLOT(clickedDevSysPath()));
    }
    if ( licensePage ) {
	connect( licensePage->readLicenseButton, SIGNAL(clicked()), SLOT(clickedLicenseFile()));
	connect( licensePage->customerID, SIGNAL(textChanged(const QString&)), SLOT(licenseChanged()));
	connect( licensePage->licenseID, SIGNAL(textChanged(const QString&)), SLOT(licenseChanged()));
	connect( licensePage->licenseeName, SIGNAL(textChanged(const QString&)), SLOT(licenseChanged()));
	connect( licensePage->expiryDate, SIGNAL(textChanged(const QString&)), SLOT(licenseChanged()));
	connect( licensePage->productsString, SIGNAL(activated(int)), SLOT(licenseChanged()));
	connect( licensePage->key, SIGNAL(textChanged(const QString&)), SLOT(licenseChanged()));
    }
    if ( configPage ) {
	connect( configPage->configTabs, SIGNAL(currentChanged(QWidget*)), SLOT(configPageChanged()));
    }
    if ( buildPage ) {
	connect( &configure, SIGNAL( processExited() ), this, SLOT( configDone() ) );
	connect( &configure, SIGNAL( readyReadStdout() ), this, SLOT( readConfigureOutput() ) );
	connect( &configure, SIGNAL( readyReadStderr() ), this, SLOT( readConfigureError() ) );
	connect( &make, SIGNAL( processExited() ), this, SLOT( makeDone() ) );
	connect( &make, SIGNAL( readyReadStdout() ), this, SLOT( readMakeOutput() ) );
	connect( &make, SIGNAL( readyReadStderr() ), this, SLOT( readMakeError() ) );
	connect( buildPage->restartBuild, SIGNAL(clicked()), this, SLOT(restartBuild()) );
    }
}

void SetupWizardImpl::stopProcesses()
{
    if( cleaner.isRunning() )
	cleaner.kill();
    if( configure.isRunning() )
	configure.kill();
    if( make.isRunning() )
	make.kill();
}

void SetupWizardImpl::clickedFolderPath()
{
    foldersPage->folderPath->setText( shell.selectFolder( foldersPage->folderPath->text(), ( foldersPage->folderGroups->currentItem() == 0 ) ) );
}

void SetupWizardImpl::clickedDevSysPath()
{
    QDir dir( foldersPage->devSysPath->text() );
    if( !dir.exists() )
	dir.setPath( devSysFolder );

    QString dest = QFileDialog::getExistingDirectory( dir.absPath(), this, 0, "Select the path to Microsoft Visual Studio" );
    if (!dest.isNull())
	foldersPage->devSysPath->setText( dest );
}

void SetupWizardImpl::sysOtherComboChanged(int)
{
    clickedSystem(GlobalInformation::Other);
}

static QString getDirectoryList(const char *envvar)
{
    QString environment;
    const char *cpath = getenv(envvar);
    if (cpath) {
	environment = QString::fromLocal8Bit(cpath);
	environment = QStringList::split(QRegExp("[;,]"), environment).join("\n");
    } else {
	environment = "<Environment variable empty>";
    }
    return environment;
}

void SetupWizardImpl::clickedSystem( int sys )
{
#ifndef Q_OS_MACX
    if (sys == 99) // This is the Integrate with IDE checkbox
	return;
    globalInformation.setSysId( GlobalInformation::SysId(sys) );
    if (sys == GlobalInformation::Other) {
	if (optionsPage->sysOtherCombo->currentText() == "win32-watcom")
	    globalInformation.setSysId(GlobalInformation::Watcom);
    }
    if (!isVisible())
	return;
    QString makeCmd = globalInformation.text(GlobalInformation::MakeTool);
    QString environment;
    fixedPath = false;
    if ( !optionsPage->skipBuild->isChecked() && optionsPage->skipBuild->isEnabled() ) {
        QString commandTool;
	environment = getenv("COMSPEC");
        if( qWinVersion() & WV_DOS_based )
            commandTool = "command.com";
        else
            commandTool = "cmd.exe";
        if (!environment.isEmpty() && !environment.endsWith(commandTool, false)) {
	    if (QMessageBox::critical(this, "Environment problems",
                                        "The 'COMSPEC' environment variable is not set to use\n"
                                        "'" + commandTool + "'. This could cause some problems when building.\n"
                                        "If you have difficulty then change it to use '" + commandTool + "'\n"
                                        "and restart the installation\n\n"
                                        "Please contact your local system administration if you have\n"
                                        "difficulties finding the file, or if you don't know how to\n"
                                        "modify the environment settings on your system.\n\n"
					"Alternatively, by clicking yes, the installer will try to set\n"
					"these for you.",
					 QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
		fixEnvironment("COMSPEC", commandTool);
        }
        if( !findFile( makeCmd ) ) {
	    environment = getDirectoryList("PATH");
	    // ### try to adjust environment
	    if (QMessageBox::critical(this, "Environment problems",
					 "The make tool '" + makeCmd + "' could not be located in any\n"
					 "directory listed in the 'PATH' environment variable:"
					 "\n\n" + environment + "\n\n"
					 "Make sure the path to this file is present in the PATH environment\n"
					 "variable and restart the installation.\n"
					 "\n"
					 "You can find the path to the tool using the 'Find' tool\n"
					 "and add the location to the environment settings of your\n"
					 "system. Please contact your local system administration if\n"
					 "you have difficulties finding the files, or if you don't\n"
					 "know how to modifiy the environment settings of your system.\n\n"
					 "Alternatively, by clicking yes, the installer will try to set\n"
					 "these for you.",
					 QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
		fixEnvironment("PATH", makeCmd);
	}
	if (globalInformation.sysId() != GlobalInformation::Borland && globalInformation.sysId() != GlobalInformation::MinGW) {
	    if (!findFile( "string.h" ) ) {
		environment = getDirectoryList("INCLUDE");
		// ### try to adjust environment
		if (QMessageBox::critical(this, "Environment problems",
				      "The file 'string.h' could not be located in any directory\n"
				      "listed in the 'INCLUDE' environment variable:\n\n" + environment + "\n\n"
				      "You might have to install the platform headers, or adjust\n"
				      "the environment variables of your system, and restart the\n"
				      "installation.\n\n"
				      "Please contact your local system administration if you have\n"
				      "difficulties finding the file, or if you don't know how to\n"
				      "modify the environment settings on your system.\n\n"
				      "Alternatively, by clicking yes, the installer will try to set\n"
				      "these for you.",
    				      QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
		fixEnvironment("INCLUDE", "string.h");

	    }
	    if (!findFile("ole32.lib")) {
		environment = getDirectoryList("LIB");
		// ### try to adjust environment
		if (QMessageBox::critical(this, "Environment problems",
				      "The file 'ole32.lib' could not be located in any directory\n"
				      "listed in the 'LIB' environment variable:\n\n" + environment + "\n\n"
				      "You might have to install the platform libraries, or adjust\n"
				      "the environment variables of your system, and restart the\n"
				      "installation.\n\n"
				      "Please contact your local system administration if you have\n"
				      "difficulties finding the file, or if you don't know how to\n"
				      "modify the environment settings on your system.\n\n"
				      "Alternatively, by clicking yes, the installer will try to set\n"
				      "these for you.",
  				      QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
		fixEnvironment("LIB", "ole32.lib");

	    }
	    bool foundCommonDll = false;
	    QString commonDll;
	    QString commonDllText;
	    QString presentFileText = "Make sure the path to this file is present in the PATH environment\n";
	    if (globalInformation.sysId() == GlobalInformation::MSVC) {
		commonDll = "mspdb60.dll";
		foundCommonDll = findFile(commonDll);
		commonDllText = "The file 'mspdb60.dll' ";
	    } else if(globalInformation.sysId() == GlobalInformation::MSVCNET) {
		commonDll = "mspdb70.dll";
		foundCommonDll = findFile(commonDll);
		if (!foundCommonDll) {
		    commonDll = "mspdb71.dll";
		    foundCommonDll = findFile(commonDll);
		    commonDllText = "The files 'mspdb70.dll' and 'mspdb71.dll' "; // VC 7.0 or VC 7.1
		    presentFileText = "Make sure the path to one of these files is present in the PATH environment\n";
		}
	    } else {
		foundCommonDll = true;
	    }
	    if(!foundCommonDll && !fixedPath) {
		environment = getDirectoryList("PATH");
		// ### try to adjust environment
		if (QMessageBox::critical(this, "Environment problems",
				       commonDllText + "could not be located in any\n"
				       "directory listed in the 'PATH' environment variable:"
				       "\n\n" + environment + "\n\n"
				       + presentFileText +
				       "variable and restart the installation.\n"
				       "\n"
				       "You can find the path to the tool using the 'Find' tool\n"
				       "and add the location to the environment settings of your\n"
				       "system. Please contact your local system administration if\n"
				       "you have difficulties finding the files, or if you don't\n"
				       "know how to modifiy the environment settings of your system.\n\n"
				       "Alternatively, by clicking yes, the installer will try to set\n"
				       "these for you.",
    				       QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
		fixEnvironment("PATH", commonDllText);

	    }
	}
	if (globalInformation.sysId() == GlobalInformation::Intel && !findFile("icl.exe")) {
	    environment = getDirectoryList("PATH");
	    if (QMessageBox::critical(this, "Environment problems",
				  "The Intel C++ compiler (icl.exe) could not be found\n"
				  "in your PATH:\n\n" + environment + "\n\n"
				  "Make sure the path to this file is present in the PATH environment\n"
				  "variable and restart the installation.\n"
				  "\n"
				  "You can find the path to the tool using the 'Find' tool\n"
				  "and add the location to the environment settings of your\n"
				  "system. Please contact your local system administration if\n"
				  "you have difficulties finding the files, or if you don't\n"
				  "know how to modifiy the environment settings of your system.\n\n"
				  "Alternatively, by clicking yes, the installer will try to set\n"
				  "these for you.",
 				  QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
		fixEnvironment("PATH", "icl.exe");

	}
    }
#endif
}

void SetupWizardImpl::fixEnvironment(const QString &var, const QString &file)
{
    if (var == "COMSPEC" || !(globalInformation.sysId() == GlobalInformation::MSVC || 
	globalInformation.sysId() == GlobalInformation::MSVCNET)) {
	QString fn = QDir::toNativeSeparators(QFileDialog::getOpenFileName(QString(), QString(), this, 0,
						  "Please find " + file));
	QString envs = getenv(var);
	if (var != "COMSPEC") {
	    fn.truncate(fn.findRev("\\") - 1);
	    fn += ";" + envs;
	}
	if (!fn.isEmpty())
	    QEnvironment::putEnv(var, fn, QEnvironment::PersistentEnv | QEnvironment::LocalEnv);
    } else if (globalInformation.sysId() == GlobalInformation::MSVC) {
	QString visualStudio = 
	    QEnvironment::getRegistryString("Software\\Microsoft\\VisualStudio\\6.0\\Setup\\Microsoft Visual Studio", 
	 				    "ProductDir", QEnvironment::LocalMachine);
	if (var == "PATH" && !fixedPath) {
	    QString newPaths = visualStudio + "\\vc98\\bin;";
	    newPaths += visualStudio + "\\Common\\MSDev98\\Bin;";
	    if (qWinVersion() & Qt::WV_NT_based)
		newPaths += visualStudio + "\\Common\\Tools\\WinNT;";
	    else
		newPaths += visualStudio + "\\Common\\Tools\\Win95;";
	    QEnvironment::putEnv("PATH", newPaths + getenv("PATH"), 
				 QEnvironment::PersistentEnv | QEnvironment::LocalEnv);
	    fixedPath = true;
	} else if (var == "LIB") {
	    QString newPaths = visualStudio + "\\vc98\\lib;";
	    newPaths += visualStudio + "\\vc98\\mfc\\lib;";
	    QEnvironment::putEnv("LIB", newPaths + getenv("LIB"), 
				 QEnvironment::PersistentEnv | QEnvironment::LocalEnv);
	} else if (var == "INCLUDE") {
	    QString newPaths = visualStudio + "\\vc98\\atl\\include;";
	    newPaths += visualStudio + "\\vc98\\include;";
	    newPaths += visualStudio + "\\vc98\\mfc\\include;";
	    QEnvironment::putEnv("INCLUDE", newPaths + getenv("INCLUDE"), 
				 QEnvironment::PersistentEnv | QEnvironment::LocalEnv);
	}
    } else if (globalInformation.sysId() == GlobalInformation::MSVCNET) {
	QString visualStudio = QEnvironment::getRegistryString("Software\\Microsoft\\VisualStudio\\7.1\\Setup\\VS", 
							       "ProductDir", QEnvironment::LocalMachine);
	if (visualStudio.isEmpty())
	    visualStudio = QEnvironment::getRegistryString("Software\\Microsoft\\VisualStudio\\7.0\\Setup\\VS", 
							   "ProductDir", QEnvironment::LocalMachine);
	// With .NET this isn't so easily done, we need to read in the vsvars32.bat file 
	// to get this right
	QFile f(visualStudio + "\\Common7\\Tools\\vsvars32.bat");
	QString contents;
	if (f.open(IO_ReadOnly)) {
	    contents = QString(f.readAll());
	}
	int vsinstall = contents.find("VSINSTALLDIR=")+13;
	QString VSINSTALLDIR = contents.mid(vsinstall, contents.find("\n", vsinstall) - vsinstall);
	int vcinstall = contents.find("VCINSTALLDIR=")+13;
	QString VCINSTALLDIR = contents.mid(vcinstall, contents.find("\n", vcinstall) - vcinstall);
	int framework = contents.find("FrameworkDir=")+13;
	QString FrameworkDir = contents.mid(framework, contents.find("\n", framework) - framework);
	int frameworkVer = contents.find("FrameworkVersion=")+17;
	QString FrameworkVer = contents.mid(frameworkVer, contents.find("\n", frameworkVer) - frameworkVer);
	int frameworkSDK = contents.find("FrameworkSDKDir=")+16;
	QString FrameworkSDK = contents.mid(frameworkSDK, contents.find("\n", frameworkSDK) - frameworkSDK);
	if (var == "PATH" && !fixedPath) {
	    QString newPaths = VSINSTALLDIR + ";";
	    newPaths += VCINSTALLDIR + "\\Bin;";
	    newPaths += VCINSTALLDIR + "\\Common7\\Tools;";
	    newPaths += VCINSTALLDIR + "\\Common7\\Tools\\bin\\prerelease;";
	    newPaths += VCINSTALLDIR + "\\Common7\\Tools\\bin;";
	    newPaths += FrameworkSDK + "\\bin;";
	    newPaths += FrameworkSDK + "\\" + FrameworkVer + ";";
	    QEnvironment::putEnv("PATH", newPaths + getenv("PATH"), 
				 QEnvironment::PersistentEnv | QEnvironment::LocalEnv);
	    fixedPath = true;
	} else if (var == "LIB") {
	    QString newPaths = VCINSTALLDIR + "\\ATLMFC\\LIB;";
	    newPaths += VCINSTALLDIR + "\\LIB;";
	    newPaths += VCINSTALLDIR + "\\PlatformSDK\\lib\\prerelease;";
	    newPaths += VCINSTALLDIR + "\\PlatformSDK\\lib;";
	    newPaths += FrameworkSDK + "\\lib;";
	    QEnvironment::putEnv("LIB", newPaths + getenv("LIB"), 
				 QEnvironment::PersistentEnv | QEnvironment::LocalEnv);
	} else if (var == "INCLUDE") {
	    QString newPaths = VCINSTALLDIR + "\\ATLMFC\\INCLUDE;";
	    newPaths += VCINSTALLDIR + "\\INCLUDE;";
	    newPaths += VCINSTALLDIR + "\\PlatformSDK\\include\\prerelease;";
	    newPaths += VCINSTALLDIR + "\\PlatformSDK\\include;";
	    newPaths += FrameworkSDK + "\\include;";
	    QEnvironment::putEnv("INCLUDE", newPaths + getenv("INCLUDE"), 
				 QEnvironment::PersistentEnv | QEnvironment::LocalEnv);
	}

    }
}

void SetupWizardImpl::readCleanerOutput()
{
    updateDisplay( cleaner.readStdout(), currentOLine );
}

void SetupWizardImpl::readConfigureOutput()
{
    updateDisplay( configure.readStdout(), currentOLine );
}

void SetupWizardImpl::readMakeOutput()
{
    updateDisplay( make.readStdout(), currentOLine );
}

void SetupWizardImpl::readAssistantOutput()
{
#if defined(QSA)
    updateDisplay( assistant.readStdout(), currentOLine );
#endif
}

void SetupWizardImpl::readCleanerError()
{
    updateDisplay( cleaner.readStderr(), currentELine  );
}

void SetupWizardImpl::readConfigureError()
{
    updateDisplay( configure.readStderr(), currentELine );
}

void SetupWizardImpl::readMakeError()
{
    updateDisplay( make.readStderr(), currentELine  );
}

void SetupWizardImpl::readAssistantError()
{
#if defined(QSA)
    updateDisplay( assistant.readStderr(), currentELine );
#endif
}

void SetupWizardImpl::updateDisplay( const QString &input, QString &output)
{
    const QChar *c = input.unicode();
    for( int i = 0; i < (int)input.length(); ++i, ++c ) {
	switch( char( *c ) ) {
	case '\r':
	case 0x00:
	    break;
	case '\t':
	    currentOLine += "    ";  // Simulate a TAB by using 4 spaces
	    break;
	case '\n':
	    if( output.length() ) {
		if ( !globalInformation.reconfig() ) {
		    if ( output.right( 4 ) == ".cpp" ||
			 output.right( 2 ) == ".c" ||
			 output.right( 4 ) == ".pro" ||
			 output.right( 3 ) == ".ui" ) {
			buildPage->compileProgress->setProgress( ++filesCompiled );
		    }
		}
		logOutput( output );
		output = "";
	    }
	    break;
	default:
	    output += *c;
	    break;
	}
    }
}

#if defined(Q_OS_WIN32)
void SetupWizardImpl::installIcons( const QString& iconFolder, const QString& dirName, bool common )
{
    QDir dir( dirName );

    dir.setSorting( QDir::Name | QDir::IgnoreCase );
    const QFileInfoList* filist = dir.entryInfoList();
    if ( !filist )
	return;
    QFileInfoListIterator it( *filist );
    QFileInfo* fi;
    while( ( fi = it.current() ) ) {
	if( fi->fileName()[0] != '.' && // Exclude dot-dirs
	    fi->fileName() != "sql" ) { // Exclude SQL-dir
	    if( fi->isDir() ) {
		installIcons( iconFolder, fi->absFilePath(), common );
	    } else if( fi->fileName().right( 4 ) == ".exe" ) {
		shell.createShortcut( iconFolder, common, fi->baseName(), fi->absFilePath() );
	    }
	}
	++it;
    }
}
#endif

void SetupWizardImpl::assistantDone()
{
#if defined(QSA)
    QString contentFile;
    static int count = 0;
    if ( count == 0 ) {
	connect( &assistant, SIGNAL( processExited() ), this, SLOT( assistantDone() ) );
	connect( &assistant, SIGNAL( readyReadStdout() ), this, SLOT( readAssistantOutput() ) );
	connect( &assistant, SIGNAL( readyReadStderr() ), this, SLOT( readAssistantError() ) );
	contentFile = "qsa.xml";
    } else if ( count == 1 ) {
	contentFile = "qt-script-for-applications.xml";
    } else {
	doIDEIntegration();
	return;
    }
    ++count;

    // install documentation
    QDir html( optionsPageQsa->installPath->text() );
    html.cd( "doc/html/" );

    QStringList lst;
    lst << "assistant";
    lst << "-addContentFile";
    lst << QDir::toNativeSeparators( html.filePath( contentFile ) );
    assistant.setArguments( lst );
    if( !assistant.start() ) {
	logOutput( "Installing QSA documentation failed\n" );
	assistantDone();
    }
#else
    doIDEIntegration();
#endif
}

void SetupWizardImpl::doIDEIntegration()
{
#if defined(Q_OS_WIN32)
    QDir installDir( optionsPage->installPath->text() );
    if ( optionsPage->installIDEIntegration->isChecked() && optionsPage->installIDEIntegration->isEnabled()
	 && !foldersPage->devSysPath->text().isEmpty() ) {
	// install the precompiled MS integration
	if ( globalInformation.sysId() == GlobalInformation::MSVC ) {
	    QDir addinsDir( foldersPage->devSysPath->text() );
	    addinsDir.cd( "Common/MSDev98/Addins" );
	    if ( copyFile( installDir.filePath("qmsdev.dll"), addinsDir.filePath("qmsdev.dll") ) ) {
		installDir.remove( "qmsdev.dll" );
	    }
	} else if ( globalInformation.sysId() == GlobalInformation::MSVCNET 
	            || globalInformation.sysId() == GlobalInformation::Intel){
	    QString filepath = installDir.filePath("QMsNetSetup.msi");
	    filepath = filepath.replace( '/', '\\' );

	    int res = _spawnlp( _P_NOWAIT, "msiexec.exe", "msiexec.exe", "-i", filepath.latin1(), NULL );
	    if ( res == -1 ) {
		//MSIExec is not in path, look up in registry (only works for NT machines)
		QString msiexec = QEnvironment::getRegistryString( "SYSTEM\\CurrentControlSet\\Services\\MSIServer",
								   "ImagePath",
								   QEnvironment::LocalMachine );
		if ( !msiexec.isEmpty() )
		    msiexec.remove( " /V" );
		res = _spawnl( _P_NOWAIT, msiexec.latin1(), msiexec.latin1(), "-i", filepath.latin1(), NULL );
	    }

	    if ( res == -1 ) {
		QMessageBox::warning( this, "Couldn't execute .NET addin installer script",
				      "Microsoft Installer (MSI) was not found on your system.\n"
				      "Please install MSI, then execute the .NET addin installer "
				      "script,\nlocated at " + filepath );
	    }
	}

	QFile *autoexp  = 0;
	QFile *usertype = 0;
	switch( globalInformation.sysId() ) {
	    case GlobalInformation::MSVC:
		autoexp = new QFile( foldersPage->devSysPath->text() + "\\Common\\MsDev98\\bin\\autoexp.dat" );
		usertype = new QFile( foldersPage->devSysPath->text() + "\\Common\\MsDev98\\bin\\usertype.dat" );
		break;
	    case GlobalInformation::MSVCNET:
		autoexp = new QFile( foldersPage->devSysPath->text() + "\\Common7\\Packages\\Debugger\\autoexp.dat" );
		usertype = new QFile( foldersPage->devSysPath->text() + "\\Common7\\Packages\\Debugger\\usertype.dat" );
		break;
	}

	if ( autoexp ) {
	    QString autoExpContents;
	    if ( !autoexp->exists() ) {
		autoexp->open( IO_WriteOnly );
	    } else {
		// First try to open the file to search for existing installations
		autoexp->open( IO_ReadOnly );
		QByteArray bytes = autoexp->readAll();
		autoExpContents = QString::fromLatin1(bytes.data(), bytes.size());
		autoexp->close();
		if ( autoExpContents.find( "; Trolltech Qt" ) == -1 )
		    autoexp->open(IO_WriteOnly | IO_Translate);
	    }
	    if( autoexp->isOpen() ) {
		bool written = false;
		QTextStream outstream( autoexp );
		QStringList entries = QStringList::split("\r\n", autoExpContents, true);
		for (QStringList::Iterator entry = entries.begin(); entry != entries.end(); ++entry) {
		    QString e(*entry);
		    outstream << e << endl;
		    if (!written && e.startsWith("[AutoExpand]")) {
			outstream << endl;
			outstream << "; Trolltech Qt" << endl;
			outstream << "QString=<d->unicode,su> len=<d->len,u>" << endl;
			outstream << "QCString =<shd->data, s>" << endl;
			outstream << "QPoint =x=<xp> y=<yp>" << endl;
			outstream << "QRect =x1=<x1> y1=<y1> x2=<x2> y2=<y2>" << endl;
			outstream << "QSize =width=<wd> height=<ht>" << endl;
			outstream << "QWMatrix =m11=<_m11> m12=<_m12> m21=<_m21> m22=<_m22> dx=<_dx> dy=<_dy>" << endl;
			outstream << "QVariant =Type=<d->typ> value=<d->value>" << endl;
			outstream << "QValueList<*> =Count=<sh->nodes>" << endl;
			outstream << "QPtrList<*> =Count=<numNodes>" << endl;
			outstream << "QGuardedPtr<*> =ptr=<priv->obj>" << endl;
			outstream << "QEvent =type=<t>" << endl;
			outstream << "QObject =class=<metaObj->classname,s> name=<objname,s>" << endl;
			written = true;
		    }
		}
		autoexp->close();
	    }
	    delete autoexp;
	}

	if ( usertype ) {
	    if ( !usertype->exists() ) {
		usertype->open( IO_WriteOnly | IO_Translate );
	    } else {
		usertype->open( IO_ReadOnly );
		QString existingUserType = usertype->readAll();
		usertype->close();
		if ( existingUserType.find( "Q_OBJECT" ) == -1 )
		    usertype->open(IO_WriteOnly | IO_Append | IO_Translate);
	    }
	    if ( usertype->isOpen() ) {
		QTextStream outstream( usertype );
		outstream << endl;
		outstream << "Q_OBJECT" << endl;
		outstream << "Q_PROPERTY" << endl;
		outstream << "Q_ENUMS" << endl;
		outstream << "Q_SETS" << endl;
		outstream << "Q_CLASSINFO" << endl;
		outstream << "emit" << endl;
		outstream << "TRUE" << endl;
		outstream << "FALSE" << endl;
		outstream << "SIGNAL" << endl;
		outstream << "SLOT" << endl;
		outstream << "signals:" << endl;
		outstream << "slots:" << endl;
		usertype->close();
	    }
	    delete usertype;
	}
    }

    if ( globalInformation.sysId() != GlobalInformation::MinGW )
	installDir.remove( "Makefile.win32-g++" );
    if (globalInformation.sysId() != GlobalInformation::MSVC)
	installDir.remove( "qmsdev.dll" );
    if (globalInformation.sysId() != GlobalInformation::MSVCNET)
	installDir.remove( "QMsNetSetup.msi" );
#endif

    doStartMenuIntegration();
}

void SetupWizardImpl::doStartMenuIntegration()
{
#if defined(Q_OS_WIN32)
    /*
    ** Set up our icon folder and populate it with shortcuts.
    ** Then move to the next page.
    */
    QString dirName, examplesName, tutorialsName;
    bool common( foldersPage->folderGroups->currentItem() == 0 );
    QString qtDir = QEnvironment::getEnv( "QTDIR" );

    dirName = shell.createFolder( foldersPage->folderPath->text(), common );
    shell.createShortcut( dirName, common, "Qt Designer", qtDir + "\\bin\\designer.exe", "GUI designer", "", qtDir );
#if !defined(EVAL) && !defined(EDU) && !defined(NON_COMMERCIAL)
    shell.createShortcut( dirName, common, "Reconfigure Qt",
	    qtDir + "\\bin\\install.exe",
	    "Reconfigure the Qt library",
	    QString("-reconfig \"%1\"").arg(globalInformation.qtVersionStr()),
	    qtDir );
#endif
#if defined(QSA)
    shell.createShortcut( dirName, common, "License agreement for Qt", "notepad.exe", "Review the license agreement",
	    "\"" + qtDir + "\\" LICENSE_DEST "\"" );
    shell.createShortcut( dirName, common, "Readme for Qt", "notepad.exe", "Important information",
	    "\"" + qtDir + "\\README\"" );
    shell.createShortcut( dirName, common, "License agreement for QSA", "notepad.exe", "Review the license agreement",
	    "\"" + optionsPageQsa->installPath->text() + "\\" LICENSE_DEST "\"" );
    shell.createShortcut( dirName, common, "Readme for QSA", "notepad.exe", "Important information",
	    "\"" + optionsPageQsa->installPath->text() + "\\README\"" );
#else
    shell.createShortcut( dirName, common, "License agreement", "notepad.exe", "Review the license agreement", "\"" + qtDir + "\\" LICENSE_DEST "\"" );
    shell.createShortcut( dirName, common, "Readme", "notepad.exe", "Important information", QString( "\"" ) + qtDir + "\\README\"" );
#endif
    shell.createShortcut( dirName, common, "Qt Assistant", qtDir + "\\bin\\assistant.exe", "Browse the On-line documentation", "", qtDir );
    shell.createShortcut( dirName, common, "Qt Linguist", qtDir + "\\bin\\linguist.exe", "Qt translation utility", "", qtDir );
    shell.createInternetShortcut( dirName, common, "Trolltech.com", "http://qtsoftware.com/" );
#if defined(EVAL_CD)
    shell.createInternetShortcut( dirName, common, "Register for Support", "http://qtsoftware.com/products/qt/evaluate.html" );
#endif

    if ( ( ( !globalInformation.reconfig() && optionsPage->skipBuild->isChecked() )
	 || ( globalInformation.reconfig() && !configPage->rebuildInstallation->isChecked() ) )
	|| qWinVersion() & WV_DOS_based ) {
	QString description;
#if defined(EVAL) || defined(EDU) || defined(NON_COMMERCIAL)
	buildQtShortcutText = "Build Qt Examples and Tutorials";
	description = "Build the Qt Examples and Tutorials";
#else
	buildQtShortcutText = "Build Qt " + globalInformation.qtVersionStr();
	description = "Build the Qt library";
#endif
	shell.createShortcut( dirName, common,
	    buildQtShortcutText,
	    QEnvironment::getEnv( "QTDIR" ) + "\\build.bat",
	    description );
    }

#if defined(QSA)
    QString qsaExamplesName;
    if( qWinVersion() & WV_DOS_based ) {
	shell.createShortcut( dirName, common,
		"QSA Examples",
		optionsPageQsa->installPath->text() + "\\examples" );
    } else {
	qsaExamplesName = shell.createFolder( foldersPage->folderPath->text() + "\\QSA Examples", common );
	installIcons( qsaExamplesName, optionsPageQsa->installPath->text() + "\\examples", common );
    }
#endif
    if( optionsPage->installTutorials->isChecked() ) {
	if( qWinVersion() & WV_DOS_based ) {
	    shell.createShortcut( dirName, common,
#if defined(QSA)
		"Qt Tutorials",
#else
		"Tutorials",
#endif
		QEnvironment::getEnv( "QTDIR" ) + "\\tutorial" );
	} else {
#if defined(QSA)
	    tutorialsName = shell.createFolder( foldersPage->folderPath->text() + "\\Qt Tutorials", common );
#else
	    tutorialsName = shell.createFolder( foldersPage->folderPath->text() + "\\Tutorials", common );
#endif
	    installIcons( tutorialsName, QEnvironment::getEnv( "QTDIR" ) + "\\tutorial", common );
	}
    }
    if( optionsPage->installExamples->isChecked() ) {
	if( qWinVersion() & WV_DOS_based ) {
	    shell.createShortcut( dirName, common,
#if defined(QSA)
		"Qt Examples",
#else
		"Examples",
#endif
		QEnvironment::getEnv( "QTDIR" ) + "\\examples" );
	} else {
#if defined(QSA)
	    examplesName = shell.createFolder( foldersPage->folderPath->text() + "\\Qt Examples", common );
#else
	    examplesName = shell.createFolder( foldersPage->folderPath->text() + "\\Examples", common );
#endif
	    installIcons( examplesName, QEnvironment::getEnv( "QTDIR" ) + "\\examples", common );
	}
    }
#endif
#if defined(QSA)
#endif
    buildPage->compileProgress->setProgress( buildPage->compileProgress->totalSteps() );
    setNextEnabled( buildPage, true );
    logOutput( "The build was successful", true );
}

void SetupWizardImpl::makeDone()
{
    makeDone( !make.normalExit() || make.exitStatus() );
}

void SetupWizardImpl::makeDone( bool error )
{
    if( error ) {
	if (!backButton()->isEnabled()) {
	    logOutput( "The build process failed!\n" );
	    emit wizardPageFailed( indexOf(currentPage()) );
	    QMessageBox::critical( this, "Error", "The build process failed!\nSee the log for details." );
	    buildPage->restartBuild->setText( "Restart compile" );
	    backButton()->setEnabled(true);
	}
	setAppropriate( progressPage, false );
#if defined(QSA)
    } else if ( make.workingDirectory() == QEnvironment::getEnv( "QTDIR" ) ) {
	QStringList args;
	args << globalInformation.text(GlobalInformation::MakeTool);
	args << "sub-examples";

	make.setWorkingDirectory( optionsPageQsa->installPath->text() );
	make.setArguments( args );

	if( !make.start() ) {
	    logOutput( "Could not start make process.\n"
		       "Make sure that your compiler tools are installed\n"
		       "and registered correctly in your PATH environment." );
	    emit wizardPageFailed( indexOf(currentPage()) );
	    backButton()->setEnabled( true );
	}
#endif
    } else {
	// We still have some more items to do in order to finish all the
	// integration stuff.
	if ( !globalInformation.reconfig() ) {
	    logOutput( "Doing the final integration steps..." );
	    assistantDone();
	} else {
	    setNextEnabled( buildPage, true );
	    logOutput( "The build was successful", true );
	}
	buildPage->restartBuild->setText( "Success" );
	buildPage->restartBuild->setEnabled( false );
    }
}

void SetupWizardImpl::configDone()
{
    QStringList args;

    if( globalInformation.reconfig() && !configPage->rebuildInstallation->isChecked() )
	showPage( finishPage );

#if !defined(EVAL) && !defined(EDU) && !defined(NON_COMMERCIAL)
    if( !configure.normalExit() || configure.exitStatus() ) {
	logOutput( "The configure process failed.\n" );
	emit wizardPageFailed( indexOf(currentPage()) );
	buildPage->restartBuild->setText( "Restart configure" );
	setAppropriate( progressPage, false );
	backButton()->setEnabled(true);
    } else
#endif
    {
	args << globalInformation.text(GlobalInformation::MakeTool);
#if !defined(EVAL) && !defined(EDU) && !defined(NON_COMMERCIAL)
	args << "sub-src";
	args << "sub-plugins";
	if ( optionsPage ) {
	    if ( optionsPage->installTools->isChecked() )
		args << "sub-tools";
	    if ( optionsPage->installTutorials->isChecked() )
		args << "sub-tutorial";
	    if ( optionsPage->installExamples->isChecked() )
		args << "sub-examples";
	    if ( optionsPage->installExtensions->isChecked() )
		args << "sub-extensions";
	} else if (globalInformation.reconfig()) {
	    args << "sub-tools"; // We want to make sure it rebuilds uic etc
	}
#elif defined(Q_OS_WIN32)
	if ( optionsPage ) {
	    if ( optionsPage->installTutorials->isChecked() )
		args << "sub-tutorial";
	    if ( optionsPage->installExamples->isChecked() )
		args << "sub-examples";
#if !defined(NON_COMMERCIAL)
	    if ( optionsPage->installExtensions->isChecked() )
		args << "sub-extensions";
#endif
	}
	if ( args.count() == 1 ) {
	    make.setWorkingDirectory( QEnvironment::getEnv( "QTDIR" ) );
	    makeDone( false );
	    return;
	}
#endif
	if ( globalInformation.sysId() == GlobalInformation::MinGW ) {
	    args << "-fMakefile.win32-g++";
	}

	make.setWorkingDirectory( QEnvironment::getEnv( "QTDIR" ) );
	make.setArguments( args );

	if( !make.start() ) {
	    logOutput( "Could not start make process.\n"
		       "Make sure that your compiler tools are installed\n"
		       "and registered correctly in your PATH environment." );
	    emit wizardPageFailed( indexOf(currentPage()) );
	    backButton()->setEnabled( true );
	} else {
	    buildPage->restartBuild->setText( "Stop compilation" );
	}
    }
}

void SetupWizardImpl::restartBuild()
{
    if ( configure.isRunning() ||
       (!configure.isRunning() && (!configure.normalExit() || configure.exitStatus())) ) {
	if ( configure.isRunning() ) {	// Stop configure
	    configure.kill();
	    buildPage->restartBuild->setText( "Restart configure" );
	    logOutput( "\n*** Configure stopped by user...\n" );
	    backButton()->setEnabled( true );
	} else {			// Restart configure
	    emit wizardPageShowed( indexOf(currentPage()) );
	    backButton()->setEnabled( false );
	    cleanDone();
	    buildPage->restartBuild->setText( "Stop configure" );
	    logOutput( "\n*** Configure restarted by user...\n" );
	}
    } else if ( make.isRunning() ||
	      (!make.isRunning() && (!make.normalExit() || make.exitStatus())) ) {
	if ( make.isRunning() ) {	// Stop compile
	    buildPage->restartBuild->setText( "Restart compile" );
	    logOutput( "\n*** Compilation stopped by user...\n" );
	    backButton()->setEnabled( true );
	    make.kill();
	} else {			// Restart compile
	    wizardPageShowed( indexOf(currentPage()) );
	    backButton()->setEnabled( false );
	    configDone();
	    buildPage->restartBuild->setText( "Stop compile" );
	    logOutput( "\n*** Compilation restarted by user...\n" );
	}
    }
}

void SetupWizardImpl::saveSettings()
{
#if !defined(EVAL) && !defined(EDU) && !defined(NON_COMMERCIAL)
    QApplication::setOverrideCursor( Qt::waitCursor );
    saveSet( configPage->configList );
    saveSet( configPage->advancedList );
    QApplication::restoreOverrideCursor();
#endif
}

void SetupWizardImpl::saveSet( QListView* list )
{
    QSettings settings;
    settings.writeEntry( "/Trolltech/Qt/ResetDefaults", "FALSE" );

    QListViewItemIterator it( list );
    while ( it.current() ) {
	QListViewItem *itm = it.current();
	++it;
	if ( itm->rtti() != CheckListItem::RTTI )
	    continue;
	CheckListItem *item = (CheckListItem*)itm;
	if ( item->type() == QCheckListItem::RadioButton ) {
	    if ( item->isOn() ) {
		QString folder;
		QListViewItem *pItem = item;
		while ( (pItem = pItem->parent() ) ) {
		    if ( folder.isEmpty() )
			folder = pItem->text( 0 );
		    else
			folder = pItem->text(0) + "/" + folder;
		}

		settings.writeEntry( "/Trolltech/Qt/" + folder, item->text() );
	    }
	} else if ( item->type() == QCheckListItem::CheckBox ) {
	    QStringList lst;
	    QListViewItem *p = item->parent();
	    if ( p )
		--it;
	    QString c = p->text( 0 );
	    while ( ( itm = it.current() ) &&
		itm->rtti() == CheckListItem::RTTI &&
		item->type() == CheckListItem::CheckBox ) {
		item = (CheckListItem*)itm;
		++it;
		if ( item->isOn() )
		    lst << item->text( 0 );
	    }
	    if ( lst.count() )
		settings.writeEntry( "/Trolltech/Qt/" + p->text(0), lst, ',' );
	    else
		settings.writeEntry( "/Trolltech/Qt/" + p->text(0), "Nothing selected" );
	}
    }
}

void SetupWizardImpl::showPage( QWidget* newPage )
{
    if ( currentPage() == configPage && newPage == progressPage && !verifyConfig() ) {
	if (QMessageBox::warning( this, "Configuration with Warnings",
			      "One or more of the selected options could not be verified by the installer.\n"
			      "Do you want to continue?", "Yes", "No" ))
	    return;
    }

    QWizard::showPage( newPage );
    setInstallStep( indexOf(newPage) + 1 );

    if( newPage == licensePage ) {
	showPageLicense();
    } else if( newPage == licenseAgreementPage ) {
	readLicenseAgreement();
    } else if( newPage == licenseAgreementPageQsa ) {
	readLicenseAgreement();
    } else if( newPage == optionsPage ) {
	showPageOptions();
    } else if( newPage == foldersPage ) {
	showPageFolders();
    } else if( newPage == configPage ) {
	showPageConfig();
    } else if( newPage == progressPage ) {
	showPageProgress();
    } else if( newPage == buildPage ) {
	showPageBuild();
    } else if( newPage == finishPage ) {
	showPageFinish();
    }
}

void SetupWizardImpl::showPageLicense()
{
    licenseChanged();
}

void SetupWizardImpl::showPageOptions()
{
    static bool done = false;
    if (done)
	return;

    done = true;

    // First make sure that the current license information is saved
    if( !globalInformation.reconfig() )
	writeLicense( QDir::homeDirPath() + "/.qt-license" );

    // ### unsupported
    optionsPage->installDocs->hide();

    bool enterprise = licenseInfo[ "PRODUCTS" ] == "qt-enterprise";
    optionsPage->installExtensions->setChecked( enterprise );
    optionsPage->installExtensions->setEnabled( enterprise );

#if defined(EVAL) || defined(EDU) || defined(NON_COMMERCIAL)
    optionsPage->installDocs->setEnabled( false );
    optionsPage->skipBuild->setEnabled( false );
    if ( globalInformation.sysId()==GlobalInformation::Borland ) {
	optionsPage->sysMsvcNet->setEnabled( false );
	optionsPage->sysMsvc->setEnabled( false );
	optionsPage->sysBorland->setEnabled( true );
	optionsPage->sysMinGW->setEnabled( false );
	optionsPage->sysIntel->setEnabled( false );
	optionsPage->sysOther->setEnabled( false );
    } else {
	optionsPage->sysMsvcNet->setEnabled( true );
	optionsPage->sysMsvc->setEnabled( true );
	optionsPage->sysBorland->setEnabled( false );
	optionsPage->sysOther->setEnabled( false );
	optionsPage->sysIntel->setEnabled( false );
	optionsPage->sysMinGW->setEnabled( false );
    }
#  if defined(Q_OS_WIN32)
    optionsPage->installExamples->setEnabled( true );
    optionsPage->installTutorials->setEnabled( true );
    optionsPage->installTools->setEnabled( false );
#    if defined(NON_COMMERCIAL)
    optionsPage->installExtensions->hide();
#    else
    optionsPage->installExtensions->setChecked( true );
    optionsPage->installExtensions->setEnabled( true );
#    endif
#  else
    optionsPage->installExamples->setEnabled( false );
    optionsPage->installTutorials->setEnabled( false );
    optionsPage->installExtensions->setChecked( false );
    optionsPage->installExtensions->setEnabled( false );
#  endif
#else
#  if defined(Q_OS_WIN32)
    // No need to offer the option of skipping the build on 9x, it's skipped anyway
    if ( qWinVersion() & WV_DOS_based )
	optionsPage->skipBuild->setEnabled( false );
#  endif
#endif

    // trigger environment test
    clickedSystem(globalInformation.sysId());
}

void SetupWizardImpl::showPageFolders()
{
    QString ideName = globalInformation.text(GlobalInformation::IDE);
    foldersPage->devSysLabel->setText( ideName + " path");
    foldersPage->devSysLabel->setShown(!ideName.isEmpty());
    foldersPage->devSysPath->setShown(!ideName.isEmpty());
    foldersPage->devSysPathButton->setShown(!ideName.isEmpty());
#if defined(Q_OS_WIN32)
    if( globalInformation.sysId() == GlobalInformation::MSVC ) {
	QString devPath = QEnvironment::getRegistryString( "Software\\Microsoft\\VisualStudio\\6.0\\Setup\\Microsoft Visual Studio", "ProductDir", QEnvironment::LocalMachine );
	if ( devPath.isEmpty() ) {
	    // fallback for Windows 9x
	    QDir msdevDir( QEnvironment::getEnv("MSDEVDIR") );
	    msdevDir.cdUp();
	    msdevDir.cdUp();
	    devPath = QDir::toNativeSeparators( msdevDir.absPath() );
	}
	foldersPage->devSysPath->setText( devPath );
    } else if ( globalInformation.sysId() == GlobalInformation::MSVCNET ) {
	QString devPath = QEnvironment::getRegistryString( "Software\\Microsoft\\VisualStudio\\7.1\\Setup\\VS", "ProductDir", QEnvironment::LocalMachine );
	if ( !devPath.length() )
	    devPath = QEnvironment::getRegistryString( "Software\\Microsoft\\VisualStudio\\7.0\\Setup\\VS", "ProductDir", QEnvironment::LocalMachine );
	foldersPage->devSysPath->setText( devPath );
    }
#endif
}

void SetupWizardImpl::showPageProgress()
{
    saveSettings();
    int totalSize = 0;
    QFileInfo fi;
    totalRead = 0;
    bool copySuccessful = true;

    if( !filesCopied ) {
	createDir( optionsPage->installPath->text() );
	if (optionsPageQsa)
	    createDir( optionsPageQsa->installPath->text() );
	progressPage->filesDisplay->append( "Installing files...\n" );

	// install the right LICENSE file
	QDir installDir( optionsPage->installPath->text() );
	QFile licenseFile( installDir.filePath( LICENSE_DEST ) );
	if ( licenseFile.open( IO_WriteOnly ) ) {
	    ResourceLoader *rcLoader;
#if defined(EVAL) || defined(EDU)
	    rcLoader = new ResourceLoader( "LICENSE" );
#elif defined(NON_COMMERCIAL)
	    if ( licenseAgreementPage->countryCombo->currentItem() == 0 )
		rcLoader = new ResourceLoader( "LICENSE-US" );
	    else
		rcLoader = new ResourceLoader( "LICENSE" );
#else
	    if ( usLicense ) {
		rcLoader = new ResourceLoader( "LICENSE-US" );
	    } else {
		rcLoader = new ResourceLoader( "LICENSE" );
	    }
#endif
	    if ( rcLoader->isValid() ) {
		licenseFile.writeBlock( rcLoader->data() );
	    } else {
		emit wizardPageFailed( indexOf(currentPage()) );
		QMessageBox::critical( this, tr("Package corrupted"),
			tr("Could not find the LICENSE file in the package.\nThe package might be corrupted.") );
	    }
	    delete rcLoader;
	    licenseFile.close();
	} else {
	    // ### error handling -- we could not write the LICENSE file
	}
#if defined(QSA)
	QDir installDirQsa( optionsPageQsa->installPath->text() );
	QFile licenseFileQsa( installDirQsa.filePath( LICENSE_DEST ) );
	if ( licenseFileQsa.open( IO_WriteOnly ) ) {
	    ResourceLoader *rcLoader;
	    rcLoader = new ResourceLoader( "LICENSE_QSA" );
	    if ( rcLoader->isValid() ) {
		licenseFileQsa.writeBlock( rcLoader->data() );
	    } else {
		emit wizardPageFailed( indexOf(currentPage()) );
		QMessageBox::critical( this, tr("Package corrupted"),
			tr("Could not find the LICENSE file in the package.\nThe package might be corrupted.") );
	    }
	    delete rcLoader;
	    licenseFileQsa.close();
	} else {
	    // ### error handling -- we could not write the LICENSE file
	}
#endif

	// Install the files -- use different fallbacks if one method failed.
	QArchive ar;
	QString licenseKey;
#if !defined(EVAL_CD) && !defined(NON_COMMERCIAL)
	licenseKey = licensePage->key->text();
#endif
	ar.setVerbosity( QArchive::Destination | QArchive::Verbose | QArchive::Progress );
	connect( &ar, SIGNAL( operationFeedback( const QString& ) ), this, SLOT( archiveMsg( const QString& ) ) );
	connect( &ar, SIGNAL( operationFeedback( int ) ), progressPage->operationProgress, SLOT( setProgress( int ) ) );
	// First, try to find qt.arq as a binary resource to the file.
	ResourceLoader rcLoader( "QT_ARQ", 500 );
	if ( rcLoader.isValid() ) {
	    progressPage->operationProgress->setTotalSteps( rcLoader.data().count() );
	    QDataStream ds( rcLoader.data(), IO_ReadOnly );
	    ar.readArchive( &ds, optionsPage->installPath->text(), licenseKey );
	} else {
	    // If the resource could not be loaded or is smaller than 500
	    // bytes, we have the dummy qt.arq: try to find and install
	    // from qt.arq in the current directory instead.
	    QString archiveName = "qt.arq";
#if defined(Q_OS_MAC)
	    QString appDir = qApp->argv()[0];
	    int truncpos = appDir.findRev( "/Contents/MacOS/" );
	    if (truncpos != -1)
	    appDir.truncate( truncpos );
	    archiveName  = appDir + "/Contents/Qt/qtmac.arq";
#endif
	    fi.setFile( archiveName );
	    if( fi.exists() )
		totalSize = fi.size();
	    progressPage->operationProgress->setTotalSteps( totalSize );

	    ar.setPath( archiveName );
	    if( ar.open( IO_ReadOnly ) )  {
		ar.readArchive( optionsPage->installPath->text(), licenseKey );
	    } else {
		// We were not able to find any qt.arq -- so assume we have
		// the old fashioned zip archive and simply copy the files
		// instead.
		progressPage->operationProgress->setTotalSteps( FILESTOCOPY );
		copySuccessful = copyFiles( QDir::currentDirPath(), optionsPage->installPath->text(), true );

		/*These lines are only to be used when changing the filecount estimate
		  QString tmp( "%1" );
		  tmp = tmp.arg( totalFiles );
		  QMessageBox::information( this, tmp, tmp );
		 */
		progressPage->operationProgress->setProgress( FILESTOCOPY );
	    }
	}

#if defined(QSA)
	QArchive arQsa;
	arQsa.setVerbosity( QArchive::Destination | QArchive::Verbose | QArchive::Progress );
	connect( &arQsa, SIGNAL( operationFeedback( const QString& ) ), this, SLOT( archiveMsg( const QString& ) ) );
	connect( &arQsa, SIGNAL( operationFeedback( int ) ), progressPage->operationProgress, SLOT( setProgress( int ) ) );
	ResourceLoader rcLoaderQsa( "QSA_ARQ", 500 );
	if ( rcLoaderQsa.isValid() ) {
	    progressPage->operationProgress->setTotalSteps( rcLoaderQsa.data().count() );
	    QDataStream ds( rcLoaderQsa.data(), IO_ReadOnly );
	    arQsa.readArchive( &ds, optionsPageQsa->installPath->text(), licenseKey );
	} else {
	    // ### error handling
	}
#endif
	filesCopied = copySuccessful;

	timeCounter = 30;
	if( copySuccessful ) {
	    QDir installDir( optionsPage->installPath->text() );
#if defined(Q_OS_WIN32)
	    QDir windowsFolderDir( shell.windowsFolderName );
#  if !defined(EVAL) && !defined(EDU) && !defined(NON_COMMERCIAL)
	    {
		// move $QTDIR/install.exe to $QTDIR/bin/install.exe
		// This is done because install.exe is also used to reconfigure Qt
		// (and this expects install.exe in bin). We can't move install.exe
		// to bin in first place, since for the snapshots, we don't have
		// the .arq archives.
		QString inFile( installDir.filePath("install.exe") );
		copyFile(inFile, installDir.filePath("bin/install.exe"));
		QFile::remove(inFile);
	    }
#  endif
	    {
		// move the uninstaller to the Windows directory
		// This is necessary since the uninstaller deletes all files in
		// the installation directory (and therefore can't delete
		// itself).
		QString inFile( installDir.filePath("bin/quninstall.exe") );
		copyFile(inFile, windowsFolderDir.filePath("quninstall.exe"));
		QFile::remove(inFile);
	    }
#endif
#if defined(EVAL) || defined(EDU) || defined(NON_COMMERCIAL)
	    QStringList::Iterator it;
	    QDir lib( optionsPage->installPath->text() );
	    lib.cd( "lib" );
#  if !defined(EVAL_CD)
	    // patch qt*.dll
#	if !defined(Q_OS_MAC)
	    QStringList qtDlls = lib.entryList( "qt*.dll" );
#	else
	    QStringList qtDlls = lib.entryList( "libqt-mt-eval.dylib" );
#	endif
	    if ( qtDlls.count() == 0 ) {
		copySuccessful = false;
		QMessageBox::critical( this,
			tr( "Error patching Qt library" ),
#    if defined(EVAL)
			tr( "Could not patch the Qt library with the evaluation\n"
			    "license information - no Qt DLL was found." )
#    elif defined(EDU)
			tr( "Could not patch the Qt library with the educational\n"
			    "edition license information - no Qt DLL was found." )
#    else
			tr( "Could not patch the Qt library the installation\n"
			    "path information - no Qt DLL was found." )
#    endif
			);
	    }
	    for ( it=qtDlls.begin(); it!=qtDlls.end(); ++it ) {
		//### add serial number etc. to log
		logFiles( tr("Patching the Qt library %1.").arg(*it) );
		int ret = trDoIt( lib.absFilePath(*it),
#    if defined(EVAL)
			licensePage->evalName->text().latin1(),
			licensePage->evalCompany->text().latin1(),
			licensePage->serialNumber->text().latin1(),
#    elif defined(EDU)
			"",
			licensePage->university->text().latin1(),
			licensePage->serialNumber->text().latin1(),
#    endif
			installDir.absPath()
			);
		if ( ret != 0 ) {
		    copySuccessful = false;
		    QMessageBox::critical( this,
			    tr( "Error patching Qt library" ),
#    if defined(EVAL)
			    tr( "Could not patch the Qt library with the evaluation\n"
				"license information. You will not be able to execute\n"
				"any program linked against %1. Error %2" ).arg( *it ).arg(ret)
#    elif defined(EDU)
			    tr( "Could not patch the Qt library with the educational\n"
				"edition license information. You will not be able to\n"
				"execute any program linked against %1." ).arg( *it )
#    else
			    tr( "Could not patch the Qt library with the installation\n"
				"path information. You will not be able to execute\n"
				"some programs linked against %1." ).arg( *it )
#    endif
			    );
		}
	    }
#  endif
#  if !defined(Q_OS_MAC)
	    // copy lib/*.dll bin/
	    QStringList dlls = lib.entryList( "*.dll" );
	    for ( it=dlls.begin(); it!=dlls.end(); ++it ) {
		copyFile( lib.absFilePath(*it), QDir::cleanDirPath(lib.absFilePath("../bin/"+*it)) );
	    }
	    // delete the non-wanted database drivers
	    QDir plugins( optionsPage->installPath->text() );
	    plugins.cd( "plugins" );
	    plugins.cd( "sqldrivers" );
	    QDir bin( optionsPage->installPath->text() );
	    bin.cd( "bin" );
#if defined(NON_COMMERCIAL)
	    if ( sqlitePluginInstall && !sqlitePluginInstall->isOn() ) {
		plugins.remove( "qsqlite.dll" );
	    }
#else
	    if ( mysqlPluginInstall && !mysqlPluginInstall->isOn() ) {
		plugins.remove( "qsqlmysql.dll" );
		bin.remove( "libmySQL.dll" );
	    }
	    if ( ociPluginInstall && !ociPluginInstall->isOn() ) {
		plugins.remove( "qsqloci.dll" );
	    }
	    if ( odbcPluginInstall && !odbcPluginInstall->isOn() ) {
		plugins.remove( "qsqlodbc.dll" );
	    }
	    if ( psqlPluginInstall && !psqlPluginInstall->isOn() ) {
		plugins.remove( "qsqlpsql.dll" );
		bin.remove( "libpq.dll" );
	    }
	    if ( tdsPluginInstall && !tdsPluginInstall->isOn() ) {
		plugins.remove( "qsqltds.dll" );
	    }
	    if ( db2PluginInstall && !db2PluginInstall->isOn() ) {
		plugins.remove( "qsqldb2.dll" );
	    }
#endif
	    // patch the .qmake.cache with the correct paths
	    QFile cacheFile( installDir.filePath(".qmake.cache") );
	    if ( cacheFile.open( IO_ReadOnly | IO_Translate ) ) {
		QTextStream tsIn( &cacheFile );
		QString cache = tsIn.read();
		cacheFile.close();
		if ( cacheFile.open( IO_WriteOnly | IO_Translate ) ) {
		    QTextStream tsOut( &cacheFile );
		    if ( globalInformation.sysId() == GlobalInformation::Borland )
			tsOut << cache.replace( "C:/QtEvaluation/qtborland", installDir.absPath() );
		    else
			tsOut << cache.replace( "C:/QtEvaluation/qtmsvc", installDir.absPath() );
		    cacheFile.close();
		}
	    }
#  endif
#endif
	    logFiles( tr("All files have been installed.\n"
			 "This log has been saved to the installation directory.\n"
			 "The build will start automatically in 30 seconds."), true );
	} else {
	    logFiles( tr("One or more errors occurred during file installation.\n"
			 "Please review the log and try to amend the situation.\n"), true );
	}
    }
    if ( copySuccessful ) {
#if defined(Q_OS_WIN32)
	/*
	** Then record the installation in the registry, and set up the uninstallation
	*/
	QStringList uninstaller;
	uninstaller << ( QString("\"") + shell.windowsFolderName + "\\quninstall.exe" + QString("\"") );
	uninstaller << optionsPage->installPath->text();

	if( foldersPage->folderGroups->currentItem() == 0 )
	    uninstaller << ( QString("\"") + shell.commonProgramsFolderName + QString("\\") + foldersPage->folderPath->text() + QString("\"") );
	else
	    uninstaller << ( QString("\"") + shell.localProgramsFolderName + QString("\\") + foldersPage->folderPath->text() + QString("\"") );

	uninstaller << ( QString("\"") + globalInformation.qtVersionStr() + QString("\"") );

	QEnvironment::recordUninstall( QString( "Qt " ) + globalInformation.qtVersionStr(), uninstaller.join( " " ) );
#endif
	autoContTimer.start( 1000 );
    }
    else
	emit wizardPageFailed( indexOf(currentPage()) );
    setNextEnabled( progressPage, copySuccessful );
}

void SetupWizardImpl::showPageFinish()
{
    autoContTimer.stop();
    nextButton()->setText( "Next >" );
    QString finishMsg;
    if ( ( ( !globalInformation.reconfig() && !optionsPage->skipBuild->isChecked() )
	    || ( globalInformation.reconfig() && configPage->rebuildInstallation->isChecked() ) )
#if defined(Q_OS_WIN32)
    && qWinVersion() & WV_NT_based ) {
#else
    ) {
#endif
	if( globalInformation.reconfig() ) {
	    finishMsg = "Qt has been reconfigured and rebuilt, and is ready for use.";
	} else {
#if defined(Q_OS_MAC)
            finishMsg = QString( "Qt has been installed to " ) + optionsPage->installPath->text() +
                        " and is ready to use.\n\nPlease try out the developer tools in the bin folder and example "
                        "programs in the examples folder.\n\nFor further information please consult the "
			"README.txt file included in the installation folder.";
#else
	    finishMsg = QString( "Qt has been installed to %1 and is ready to use.\n"
		        "You might have to logoff and logon for changes to the environment to have an effect.").
			arg(optionsPage->installPath->text());
#  if defined(QSA)
	    finishMsg = QString( "\nQSA has been installed to " ) + optionsPageQsa->installPath->text() + " and is ready to use.";
#  endif
#endif
	}
    } else {
	if( globalInformation.reconfig() ) {
		finishMsg = "The new configuration has been written.\nThe library needs to be rebuilt to activate the ";
		finishMsg += "new configuration.";
#if defined(Q_OS_WIN32)
                finishMsg += "To rebuild it, use the \"Build Qt ";
		finishMsg += globalInformation.qtVersionStr();
		finishMsg += "\" icon in the Qt program group in the start menu.";
#endif
	} else {
	    finishMsg = QString( "The Qt files have been installed to " ) + optionsPage->installPath->text() + " and is ready to be compiled.\n";
#if defined(Q_OS_WIN32)
	    if( persistentEnv && qWinVersion() & WV_DOS_based ) {
		finishMsg += "The environment variables needed to use Qt have been recorded into your AUTOEXEC.BAT file.\n";
		finishMsg += "Please review this file, and take action as appropriate depending on your operating system to get them into the persistent environment. (Windows Me users, run MsConfig)\n\n";
	    }
#  if defined(EVAL) || defined(EDU) || defined(NON_COMMERCIAL)
	    finishMsg += QString( "To build the examples and tutorials, use the "
				  "\"Build the Qt Examples and Tutorials\""
				  " icon which has been installed into your Start-Menu." );
#  else
	    finishMsg += QString( "To build Qt, use the "
				  "\"Build Qt " ) + globalInformation.qtVersionStr() + "\""
				  " icon which has been installed into your Start-Menu.";
#  endif
#endif
	}
    }
#if defined(EVAL_CD)
    finishMsg += "\n\n"
		 "The Trolltech technical support service is available to "
		 "Qt Professional and Enterprise Edition licensees. As an "
		 "evaluation user, you can register for 30 days of evaluation "
		 "support at\n"
		 "http://qtsoftware.com/products/qt/evaluate.html";
#endif
    finishPage->finishText->setText( finishMsg );
}

void SetupWizardImpl::licenseChanged()
{
#if defined(EVAL) || defined(EDU) || defined(NON_COMMERCIAL)
    int ret = trCheckIt(
#  if defined(EVAL)
	    licensePage->evalName->text().latin1(),
	    licensePage->evalCompany->text().latin1(),
	    licensePage->serialNumber->text().latin1()
#  elif defined(EDU)
	    "",
	    licensePage->university->text().latin1(),
	    licensePage->serialNumber->text().latin1()
#  else
	    "",
	    "",
	    ""
#  endif
	    );

    if ( ret == 0 )
	setNextEnabled( licensePage, true );
    else
	setNextEnabled( licensePage, false );
    return;
#else
    QDate date;
    uint features;
    uint testFeature;
    QString platformString;
    QString licenseKey = licensePage->key->text().stripWhiteSpace();
    if ( licenseKey.length() != 14 ) {
	goto rejectLicense;
    }
    features = featuresForKey( licenseKey.upper() );
    date = decodedExpiryDate( licenseKey.mid(9) );
    if ( !date.isValid() ) {
	goto rejectLicense;
    }
#  if defined(Q_OS_MAC)
    testFeature = Feature_Mac;
    platformString = "Mac OS X";
#  elif defined(Q_OS_WIN32)
    testFeature = Feature_Windows;
    platformString = "Windows";
#  else
    testFeature = Feature_Unix;
    platformString = "UNIX";
#    ifdef Q_CC_GNU
#    warning "What about Qtopia Core?"
#    endif
#  endif
    if ( !(features&testFeature) && currentPage() == licensePage ) {
	if ( features & (Feature_Unix|Feature_Windows|Feature_Mac|Feature_Embedded) ) {
	    int ret = QMessageBox::information( this,
		    tr("No %1 license").arg(platformString),
		    tr("You are not licensed for the %1 platform.\n"
		       "Please contact sales@trolltech.com to upgrade\n"
		       "your license to include the Windows platform.").arg(platformString),
		    tr("Try again"),
		    tr("Abort installation")
		    );
	    if ( ret == 1 ) {
		QApplication::exit();
	    } else {
		licensePage->key->setText( "" );
	    }
	}
	goto rejectLicense;
    }
    if ( date < QDate::currentDate() && currentPage() == licensePage ) {
	static bool alreadyShown = false;
	if ( !alreadyShown ) {
	    QMessageBox::warning( this,
		    tr("Support and upgrade period has expired"),
		    tr("Your support and upgrade period has expired.\n"
			"\n"
			"You may continue to use your last licensed release\n"
			"of Qt under the terms of your existing license\n"
			"agreement. But you are not entitled to technical\n"
			"support, nor are you entitled to use any more recent\n"
			"Qt releases.\n"
			"\n"
			"Please contact sales@trolltech.com to renew your\n"
			"support and upgrades for this license.")
		    );
	    alreadyShown = true;
	}
    }
    if ( features & Feature_US )
	usLicense = true;
    else
	usLicense = false;

    licensePage->expiryDate->setText( date.toString( Qt::ISODate ) );
    if( features & Feature_Enterprise ) {
	licensePage->productsString->setCurrentItem( 1 );
	emit editionString( "Enterprise Edition" );
    } else {
	licensePage->productsString->setCurrentItem( 0 );
	emit editionString( "Professional Edition" );
    }
    setNextEnabled( licensePage, true );
    return;

rejectLicense:
    licensePage->expiryDate->setText( "" );
#  if defined(Q_OS_WIN32)
    //TODO: Is this a bug? It bus errors on MACX, ask rms.
    // it should work -- if it doesn't this seems to be a bug in the MACX code,
    // I guess (rms)
    licensePage->productsString->setCurrentItem( -1 );
#  endif
    emit editionString( "" );
    setNextEnabled( licensePage, false );
    return;
#endif
}

void SetupWizardImpl::logFiles( const QString& entry, bool close )
{
    if( !fileLog.isOpen() ) {
	fileLog.setName( optionsPage->installPath->text() + QDir::separator() + "install.log" );
	if( !fileLog.open( IO_WriteOnly | IO_Translate ) )
	    return;
    }

#if 1
    progressPage->filesDisplay->append(entry);
#else
    progressPage->filesDisplay->setText( "Installing files...\n" + entry + "\n" );
#endif

    static QTextStream outstream;
    outstream.setDevice( &fileLog );
    outstream << ( entry + "\n" );

    if( close )
	fileLog.close();
}

void SetupWizardImpl::logOutput( const QString& entry, bool close )
{
    if( !outputLog.isOpen() ) {
	QDir installDir;
	if ( optionsPage )
	    installDir.setPath( optionsPage->installPath->text() );
	else
	    installDir.setPath( QEnvironment::getEnv( "QTDIR" ) );
	outputLog.setName( installDir.filePath("build.log") );
	if( !outputLog.open( IO_WriteOnly | IO_Translate ) )
	    return;
    }

    buildPage->outputDisplay->append(entry);

    static QTextStream outstream;
    outstream.setDevice( &outputLog );
    outstream << ( entry + "\n" );

    if( close )
	outputLog.close();
}

void SetupWizardImpl::archiveMsg( const QString& msg )
{
    if( msg.right( 7 ) == ".cpp..." || msg.right( 5 ) == ".c..." || msg.right( 7 ) == ".pro..." || msg.right( 6 ) == ".ui..." )
	filesToCompile++;
    qApp->processEvents();
    if ( msg.startsWith("Expanding") )
	// only show the "Expanding" entries to avoid flickering
	logFiles( msg );
}

#ifdef Q_WS_WIN
static HANDLE createFile( const QString &entryName, DWORD attr1, DWORD attr2 )
{
    QT_WA({
    	return ::CreateFileW( entryName.ucs2(), attr1, attr2, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    }, {
	return ::CreateFileA( entryName.local8Bit(), attr1, attr2, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    })
}
#endif

bool SetupWizardImpl::copyFiles( const QString& sourcePath, const QString& destPath, bool topLevel )
{
    QDir dir( sourcePath );
    const QFileInfoList* list = dir.entryInfoList();
    QFileInfoListIterator it( *list );
    QFileInfo* fi;
    bool doCopy;

    while( ( fi = it.current() ) ) {
	if( fi->fileName()[ 0 ] != '.' ) {
	    QString entryName = sourcePath + QDir::separator() + fi->fileName();
	    QString targetName = destPath + QDir::separator() + fi->fileName();
	    doCopy = true;
	    if( fi->isDir() ) {
		if( !dir.exists( targetName ) )
		    createDir( targetName );
		if( topLevel ) {
		    if ( fi->fileName() == "doc" )
			doCopy = optionsPage->installDocs->isChecked();
		    else if ( fi->fileName() == "tools" )
			doCopy = optionsPage->installTools->isChecked();
		    else if ( fi->fileName() == "tutorial" )
			doCopy = optionsPage->installTutorials->isChecked();
		    else if ( fi->fileName() == "examples" )
			doCopy = optionsPage->installExamples->isChecked();
		}
		if( doCopy )
		    if( !copyFiles( entryName, targetName, false ) )
			return false;
	    } else {
		if( qApp && isShown() ) {
		    qApp->processEvents();
		    progressPage->operationProgress->setProgress( totalFiles );
		    logFiles( targetName );
		} else {
		    return false;
		}
		if( entryName.right( 4 ) == ".cpp" ||
		    entryName.right( 2 ) == ".c" ||
		    entryName.right( 4 ) == ".pro" ||
		    entryName.right( 3 ) == ".ui" )
		    filesToCompile++;
		bool res = true;
		if ( !QFile::exists( targetName ) )
		    res = copyFile( entryName, targetName );
#if defined(Q_OS_WIN32)
		if ( res ) {
		    totalFiles++;
		    HANDLE inFile, outFile;
		    if( inFile = createFile( entryName, GENERIC_READ, FILE_SHARE_READ ) ){
			if( outFile = createFile( targetName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE ) ){
			    FILETIME createTime, accessTime, writeTime;
			    ::GetFileTime( inFile, &createTime, &accessTime, &writeTime );
			    ::SetFileTime( outFile, &createTime, &accessTime, &writeTime );
			    ::CloseHandle( outFile );
			}
			::CloseHandle( inFile );
		    }
		} else {
		    QString error = QEnvironment::getLastError();
		    logFiles( QString( "   ERROR: " ) + error + "\n" );
		    if( QMessageBox::warning( this, "File copy error", entryName + ": " + error, "Continue", "Cancel", QString(), 0 ) )
			return false;
		}
#elif defined( Q_OS_UNIX )
		// ### TODO: keep file date the same, handle errors
#endif
	    }
	}
	++it;
    }
    return true;
}

void SetupWizardImpl::setInstallStep( int step )
{
    QString captionTxt;
#if defined(QSA)
    captionTxt = tr("QSA Evaluation Version Installation Wizard");
#elif defined(EVAL)
    captionTxt = tr("Qt Evaluation Version Installation Wizard");
#elif defined(EDU)
    captionTxt = tr("Qt Educational Edition Installation Wizard");
#elif defined(NON_COMMERCIAL)
    captionTxt = tr("Qt Non-Commercial Edition Installation Wizard");
#else
    if( globalInformation.reconfig() )
	captionTxt = tr("Qt Configuration Wizard");
    else
	captionTxt = tr("Qt Installation Wizard");
#endif
    setCaption( tr("%1 - Step %2 of %3").arg( captionTxt ).arg( step ).arg( pageCount() ) );
    emit wizardPageShowed( step-1 );
}

void SetupWizardImpl::timerFired()
{
    QString tmp( "Next %1 >" );

    timeCounter--;

    if( timeCounter )
	nextButton()->setText( tmp.arg( timeCounter ) );
    else {
	next();
	autoContTimer.stop();
    }
}

void SetupWizardImpl::readLicense( QString filePath)
{
#if !defined(EVAL) && !defined(EDU) && !defined(NON_COMMERCIAL)
    QFile licenseFile( filePath );

    if( licenseFile.open( IO_ReadOnly ) ) {
	QString buffer;

	while( licenseFile.readLine( buffer, 1024 ) != -1 ) {
	    if( buffer[ 0 ] != '#' ) {
		QStringList components = QStringList::split( '=', buffer );
		QStringList::Iterator it = components.begin();
		QString keyString = (*it++).stripWhiteSpace().replace( QRegExp( QString( "\"" ) ), QString() ).upper();
		QString value = (*it++).stripWhiteSpace().replace( QRegExp( QString( "\"" ) ), QString() );

		licenseInfo[ keyString ] = value;
	    }
	}
	licenseFile.close();

	if ( licensePage ) {
	    licensePage->customerID->setText( licenseInfo[ "CUSTOMERID" ] );
	    licensePage->licenseID->setText( licenseInfo[ "LICENSEID" ] );
	    licensePage->licenseeName->setText( licenseInfo[ "LICENSEE" ] );
	    if( licenseInfo[ "PRODUCTS" ] == "qt-enterprise" ) {
		licensePage->productsString->setCurrentItem( 1 );
		emit editionString( "Enterprise Edition" );
	    } else {
		licensePage->productsString->setCurrentItem( 0 );
		emit editionString( "Professional Edition" );
	    }
	    licensePage->expiryDate->setText( licenseInfo[ "EXPIRYDATE" ] );
	    licensePage->key->setText( licenseInfo[ "LICENSEKEY" ] );
	}
    }
#endif
}

void SetupWizardImpl::writeLicense( QString filePath )
{
#if !defined(EVAL) && !defined(EDU) && !defined(NON_COMMERCIAL)
    QFile licenseFile( filePath );

    if( licenseFile.open( IO_WriteOnly | IO_Translate ) ) {
	QTextStream licStream( &licenseFile );

	licenseInfo[ "CUSTOMERID" ] = licensePage->customerID->text();
	licenseInfo[ "LICENSEID" ] = licensePage->licenseID->text();
	licenseInfo[ "LICENSEE" ] = licensePage->licenseeName->text();
	if( licensePage->productsString->currentItem() == 0 ) {
	    licenseInfo[ "PRODUCTS" ] = "qt-professional";
	    emit editionString( "Professional Edition" );
	} else {
	    licenseInfo[ "PRODUCTS" ] = "qt-enterprise";
	    emit editionString( "Enterprise Edition" );
	}

	licenseInfo[ "EXPIRYDATE" ] = licensePage->expiryDate->text();
	licenseInfo[ "LICENSEKEY" ] = licensePage->key->text();

	licStream << "# Toolkit license file" << endl;
	licStream << "CustomerID=\"" << licenseInfo[ "CUSTOMERID" ].latin1() << "\"" << endl;
	licStream << "LicenseID=\"" << licenseInfo[ "LICENSEID" ].latin1() << "\"" << endl;
	licStream << "Licensee=\"" << licenseInfo[ "LICENSEE" ].latin1() << "\"" << endl;
	licStream << "Products=\"" << licenseInfo[ "PRODUCTS" ].latin1() << "\"" << endl;
	licStream << "ExpiryDate=" << licenseInfo[ "EXPIRYDATE" ].latin1() << endl;
	licStream << "LicenseKey=" << licenseInfo[ "LICENSEKEY" ].latin1() << endl;

	licenseFile.close();
    }
#endif
}

void SetupWizardImpl::clickedLicenseFile()
{
    QString licensePath = QFileDialog::getOpenFileName( optionsPage->installPath->text(), QString(), this, NULL, "Browse for license file" );

    if( !licensePath.isEmpty() )
	readLicense( licensePath );

}

void SetupWizardImpl::readLicenseAgreement()
{
    // Intropage
    ResourceLoader *rcLoader;
#if defined(QSA)
    LicenseAgreementPageImpl *lap;
    if ( currentPage() == licenseAgreementPageQsa ) {
	lap = licenseAgreementPageQsa;
	rcLoader = new ResourceLoader( "LICENSE_QSA" );
    } else {
	lap = licenseAgreementPage;
	rcLoader = new ResourceLoader( "LICENSE" );
    }
#elif defined(EVAL) || defined(EDU)
    LicenseAgreementPageImpl *lap = licenseAgreementPage;
    rcLoader = new ResourceLoader( "LICENSE" );
#elif defined(NON_COMMERCIAL)
    LicenseAgreementPageImpl *lap = licenseAgreementPage;
    if ( lap->countryCombo->currentItem() == 0 )
	rcLoader = new ResourceLoader( "LICENSE-US" );
    else
	rcLoader = new ResourceLoader( "LICENSE" );
#else
    LicenseAgreementPageImpl *lap = licenseAgreementPage;
    if ( usLicense ) {
	rcLoader = new ResourceLoader( "LICENSE-US" );
    } else {
	rcLoader = new ResourceLoader( "LICENSE" );
    }
#endif
    if ( rcLoader->isValid() ) {
	lap->introText->setText( rcLoader->data() );
	lap->acceptLicense->setEnabled( true );
    } else {
	emit wizardPageFailed( indexOf(currentPage()) );
	QMessageBox::critical( this, tr("Package corrupted"),
		tr("Could not find the LICENSE file in the package.\nThe package might be corrupted.") );
	lap->acceptLicense->setEnabled( false );
    }
    delete rcLoader;
}

void SetupWizardImpl::accept()
{
#if defined(Q_OS_WIN32)
    if ( finishPage->showReadmeCheck->isChecked() ) {
	QProcess proc( QString("notepad.exe") );
#if defined(QSA)
	QString qsaDir = optionsPageQsa->installPath->text();
	proc.addArgument( qsaDir + "\\README" );
#else
	QString qtDir = QEnvironment::getEnv( "QTDIR" );
	proc.addArgument( qtDir + "\\README" );
#endif
	proc.start();
    }
#endif
    QDialog::accept();
}
