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
#include "pages.h"
#include "resource.h"
#include "../environment.h"
#include <qcombobox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qvalidator.h>
#include <qdir.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qmultilineedit.h>
#include <qfiledialog.h>
#include <qtabwidget.h>
#include <qmessagebox.h>
#include <setupwizardimpl.h>
#include <qtextbrowser.h>
#include <qtextview.h>
#include <qlayout.h>

#if defined(Q_OS_WIN32)
#include <windows.h>
#endif

extern SetupWizardImpl *wizard;

BuildPageImpl::BuildPageImpl( QWidget* parent, const char* name, WindowFlags fl )
    : BuildPage( parent, name, fl )
{
}

ConfigPageImpl::ConfigPageImpl( QWidget* parent, const char* name, WindowFlags fl )
    : ConfigPage( parent, name, fl )
{
    if( globalInformation.reconfig() ) {
	currentInstLabel->show();
	currentInstallation->show();
#if defined(Q_OS_WIN32)
	// Makes no sense to have the rebuild installation option on DOS based
	// Windows
	if ( qWinVersion() & WV_NT_based )
#endif
	    rebuildInstallation->show();
#if defined(Q_OS_WIN32)
	else {
	    rebuildInstallation->setChecked( false );
	    rebuildInstallation->hide();
	}
#endif
    } else {
	currentInstLabel->hide();
	currentInstallation->hide();
	rebuildInstallation->hide();
    }
#if defined(EVAL) || defined(EDU) || defined(NON_COMMERCIAL)
    // ### these pages should probably be included but all options should be
    // disabled so that the evaluation customer can see how he can configure Qt
    configTabs->removePage( generalTab );
    configTabs->removePage( advancedTab );
#else
    configTabs->removePage( installTab );
#endif
}

FinishPageImpl::FinishPageImpl( QWidget* parent, const char* name, WindowFlags fl )
    : FinishPage( parent, name, fl )
{
#if !defined(Q_OS_WIN32)
    showReadmeCheck->hide();
#endif
}

FoldersPageImpl::FoldersPageImpl( QWidget* parent, const char* name, WindowFlags fl )
    : FoldersPage( parent, name, fl )
{
#if defined(Q_OS_WIN32)
    QByteArray buffer( 256 );
    unsigned long buffSize( buffer.size() );
    GetUserNameA( buffer.data(), &buffSize );
    folderGroups->insertItem( "Anyone who uses this computer (all users)" );
    folderGroups->insertItem( QString( "Only for me (" ) + QString( buffer.data() ) + ")" );
#if defined(QSA)
    folderPath->setText( QString( "QSA " ) + globalInformation.qsaVersionStr() );
#else
    folderPath->setText( QString( "Qt " ) + globalInformation.qtVersionStr() );
#endif
    if( qWinVersion() & Qt::WV_NT_based )   // On NT we also have a common folder
	folderGroups->setEnabled( true );
    else
	folderGroups->setDisabled( true );
#elif defined(Q_OS_UNIX)
    folderGroups->setDisabled( true );
#endif
}

LicenseAgreementPageImpl::LicenseAgreementPageImpl( QWidget* parent, const char* name, WindowFlags fl )
    : LicenseAgreementPage( parent, name, fl ),
      titleStr("License agreement")
{
#if !defined(NON_COMMERCIAL)
    countryLabel->hide();
    countryCombo->hide();
    delete countryLayout;
#else
    connect( countryCombo, SIGNAL(activated(int)), SLOT(countryChanged(int)) );
#endif
    connect( licenceButtons, SIGNAL(clicked(int)), SLOT(licenseAction(int)));
}

void LicenseAgreementPageImpl::licenseAction(int act)
{
    if( act )
	wizard->setNextEnabled( this, false );
    else
	wizard->setNextEnabled( this, true );
}

void LicenseAgreementPageImpl::countryChanged(int index)
{
    ResourceLoader *rcLoader;
    if ( index == 0 )
	rcLoader = new ResourceLoader( "LICENSE-US" );
    else
	rcLoader = new ResourceLoader( "LICENSE" );

    if ( rcLoader->isValid() ) {
	introText->setText( rcLoader->data() );
    } else {
	QMessageBox::critical( this, tr("Package corrupted"),
		tr("Could not find the LICENSE file in the package.\nThe package might be corrupted.") );
    }
}

LicensePageImpl::LicensePageImpl( QWidget* parent, const char* name, WindowFlags fl )
    : LicensePage( parent, name, fl )
{
#if defined(Q_OS_MAC)
    // StyledPanel style looks very windowsish
    customerID->setFrameShape( QFrame::LineEditPanel );
#endif
    customerID->setFocus();
#if defined(EVAL)
    // ### improve text
#  if defined(QSA)
    licenseInfoHeader->setText( tr("Thank you for your interest in QSA.\n"
		"Please enter the license information you got for this evaluation version of QSA.") );
#  else
    licenseInfoHeader->setText( tr("Thank you for your interest in Qt.\n"
		"Please enter the license information you got for this evaluation version of Qt.") );
#  endif

    customerIDLabel->setText( tr("Name") );
    licenseIDLabel->setText( tr("Company name") );
    licenseeLabel->setText( tr("Serial number") );
    evalName = customerID;
    evalCompany = licenseID;
    serialNumber = licenseeName;

    expiryLabel->hide();
    expiryDate->hide();
    productsLabel->hide();
    productsString->hide();
    keyLabel->hide();
    key->hide();
    readLicenseButton->hide();
#elif defined(EDU)
    licenseInfoHeader->setText( tr("Please enter the license information for the educational edition of Qt.") );

    customerIDLabel->setText( tr("Educational institution") );
    licenseeLabel->setText( tr("Serial number") );
    university = customerID;
    serialNumber = licenseeName;

    licenseIDLabel->hide();
    licenseID->hide();
    expiryLabel->hide();
    expiryDate->hide();
    productsLabel->hide();
    productsString->hide();
    keyLabel->hide();
    key->hide();
    readLicenseButton->hide();
#else
    licenseID->setValidator( new QIntValidator( -1, 9999999, licenseID ) );

    // expiryDate and productsString comes from the license key
    expiryDate->setEnabled( false );
    productsString->setEnabled( false );
    keyLabel->setText( tr("License key") );
    licenseInfoHeader->setText( tr("Please enter your license information.\n"
		"The License key is required to be able to proceed with the installation process.") );
#endif
}

QValidator::State InstallPathValidator::validate( QString& input, int& ) const
{
    if ( ( globalInformation.sysId() == GlobalInformation::MSVC ||
	   globalInformation.sysId() == GlobalInformation::MSVCNET )
	   && input.contains( QRegExp("\\s") ) ) {
	QMessageBox::warning( 0, "Invalid directory", "No whitespace is allowed in the directory name due to a limitation with MSVC" );
	return Intermediate;
    } else if ( globalInformation.sysId() == GlobalInformation::Borland && input.contains( "-" ) ) {
	QMessageBox::warning( 0, "Invalid directory", "No '-' characters are allowed in the directory name due to a limitation in the "
			      "Borland linker" );
	return Intermediate;
    }
    return Acceptable;
}

OptionsPageImpl::OptionsPageImpl( QWidget* parent, const char* name, WindowFlags fl )
    : OptionsPage( parent, name, fl ),
      titleStr("Options"),
      shortTitleStr("Choose options")
{
    connect( installPathButton, SIGNAL(clicked()), SLOT(choosePath()));
    sysGroup->setButton(globalInformation.sysId());
#if defined(EVAL) || defined(EDU) || defined(NON_COMMERCIAL)
    sysOther->hide();
    sysOtherCombo->hide();
    noteLabel->hide();
#endif
#if defined(Q_OS_WIN32)
    installPath->setText(
	    QString( "C:\\Qt\\" ) +
	    QString( globalInformation.qtVersionStr() ).replace( QRegExp("\\s"), "" ).replace( QRegExp("-"), "" )
	    );
    installPath->setValidator( new InstallPathValidator( this ) );
#elif defined(Q_OS_MAC)
    // ### the replace for Windows is done because qmake has problems with
    // spaces and Borland has problems with "-" in the filenames -- I don't
    // think that there is a need for this on Mac (rms)
    QString base("QtMac-");
    base = QDir::homeDirPath() + QDir::separator() + base;
#if defined(EVAL)
    base += "Eval-";
#elif defined(EDU)
    base += "Edu-";
#endif
    installPath->setText(base + QString( globalInformation.qtVersionStr() ).replace( QRegExp("\\s"), "" ));
    sysGroup->hide();
#endif
}

void OptionsPageImpl::choosePath()
{
    QDir dir( installPath->text() );

#if defined(Q_OS_WIN32)
    if( !dir.exists() ) {
#  if defined(QSA)
	dir.setPath( "C:\\Qt_QSA" );
#  else
	dir.setPath( "C:\\Qt" );
#endif
    }

    QString dest = QFileDialog::getExistingDirectory( installPath->text(), this, NULL, "Select installation directory" );
    if ( dest.isNull() ) {
#  if defined(QSA)
	dest = "C:\\Qt_QSA";
#  else
	dest = "C:\\Qt";
#  endif
    }
    if ( dest.right(1) == "\\" ) {
#  if defined(QSA)
	dest += "Qt_QSA";
#  else
	dest += "Qt";
#  endif
    }
    if ( dest.contains( QRegExp( "\\s" ) ) && !sysBorland->isChecked() )
	QMessageBox::warning( 0, "Invalid directory", "No whitespace is allowed in the directory name due to a limitation with MSVC" );
    else if ( dest.contains( "-" ) && sysBorland->isChecked() )
	QMessageBox::warning( 0, "Invalid directory", "No '-' characters are allowed in the directory name due to a limitation with the Borland linker" );
    else {
	dir.setPath( dest );
	installPath->setText( QDir::toNativeSeparators(dir.absPath()) );
    }
#elif defined(Q_OS_MAC)
    if( !dir.exists() )
	dir.setPath( "/" );

    QString dest = QFileDialog::getExistingDirectory( installPath->text(), this, NULL, "Select installation directory" );
    if (!dest.isNull())
	installPath->setText( dest );
#endif
}

ProgressPageImpl::ProgressPageImpl( QWidget* parent, const char* name, WindowFlags fl )
    : ProgressPage( parent, name, fl )
{
    // ######### At the moment, we show only one line when unpacking. So the
    // horizontal scrollbar is never shown for now to avoid flickering.
    filesDisplay->setHScrollBarMode( QScrollView::AlwaysOff );
}

#if defined(Q_OS_WIN32)
WinIntroPageImpl::WinIntroPageImpl( QWidget* parent, const char* name, WindowFlags fl )
    : WinIntroPage( parent, name, fl )
{
#if defined(QSA)
    textBrowser->setText( "<p>This program installs Qt and QSA.</p>" + textBrowser->text() );
#else
    textBrowser->setText( "<p>This program installs Qt.</p>" + textBrowser->text() );
#endif
}
#endif
