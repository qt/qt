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
#include "folderdlgimpl.h"
#include "../shell.h"
#include <qlineedit.h>
#include <qlistview.h>
#include <qpixmap.h>
#include <qdir.h>

FolderDlgImpl::FolderDlgImpl( QWidget* parent, const char* name, bool modal, WindowFlags f ) :
    FolderDlg( parent, name, modal, f )
{
}

void FolderDlgImpl::setup( QString programsFolder, QString folder )
{
    folderName->setText( folder );
    if( programsFolder.length() ) {
	QString topLevel = programsFolder.mid( programsFolder.findRev( '\\' ) + 1 );
	QListViewItem* topItem = new QListViewItem( folderTree, topLevel );
	topItem->setOpen( true );
	topItem->setPixmap( 0, *WinShell::getOpenFolderImage() );

	ScanFolder( programsFolder, topItem );

    }
}

void FolderDlgImpl::ScanFolder( QString folderPath, QListViewItem* parent )
{
    QDir folderDir( folderPath );
    folderDir.setFilter( QDir::Dirs );
    folderDir.setSorting( QDir::Name | QDir::IgnoreCase );
    const QFileInfoList* fiList = folderDir.entryInfoList();
    QFileInfoListIterator it( *fiList );
    QFileInfo* fi;

    while( ( fi = it.current() ) ) {
	if( fi->fileName()[0] != '.' ) { // Exclude dot-dirs
	    QListViewItem* item = new QListViewItem( parent, fi->fileName() );
	    item->setOpen( false );
	    item->setPixmap( 0, *WinShell::getClosedFolderImage() );
	    ScanFolder( fi->absFilePath(), item );
	}
	++it;
    }
}

void FolderDlgImpl::expandedDir( QListViewItem* item )
{
    item->setPixmap( 0, *WinShell::getOpenFolderImage() );
}

void FolderDlgImpl::collapsedDir( QListViewItem* item )
{
    item->setPixmap( 0, *WinShell::getClosedFolderImage() );
}

QString FolderDlgImpl::getFolderName()
{
    return folderName->text();
}

/*
** This will replace the contents of the folderName lineedit widget.
**
** The algoritm will traverse the item tree until it gets to the toplevel
** item, prepending each name to the folder name as it goes
*/
void FolderDlgImpl::selectedDir( QListViewItem* item )
{
    QListViewItem* currentItem = item;
    QString newFolder;

    while( currentItem->parent() ) {
	newFolder = currentItem->text( 0 ) + QString( "\\" ) + newFolder;
	currentItem = currentItem->parent();
    }
    newFolder.truncate( newFolder.length() - 1 );
    folderName->setText( newFolder );
}
