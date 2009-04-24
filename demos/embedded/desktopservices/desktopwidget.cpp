/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

// EXTERNAL INCLUDES
#include <QTabWidget>
#include <QVBoxLayout>
#include <QDesktopServices>

// INTERNAL INCLUDES
#include "linktab.h"
#include "contenttab.h"

// CLASS HEADER
#include "desktopwidget.h"

// CONSTRUCTORS & DESTRUCTORS  
DesktopWidget::DesktopWidget(QWidget *parent) : QWidget(parent)
    
{
    QTabWidget *tabWidget = new QTabWidget(this);

    // Images
    ContentTab* imageTab = new ContentTab(tabWidget);
    imageTab->init(QDesktopServices::PicturesLocation, ":/resources/photo.png");
    tabWidget->addTab(imageTab, tr("Images")); 
    
    // Music
    ContentTab* musicTab = new ContentTab(tabWidget);     
    musicTab->init(QDesktopServices::MusicLocation, ":/resources/music.png");        
    tabWidget->addTab(musicTab, tr("Music"));       
    
    // Links
    LinkTab* othersTab = new LinkTab(tabWidget);;   
    // Given icon file will be overriden by LinkTab
    othersTab->init(QDesktopServices::PicturesLocation, "");    
    tabWidget->addTab(othersTab, tr("Links"));       
    
    // Layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tabWidget);
    setLayout(layout);     
}
      
DesktopWidget::~DesktopWidget()
{
}
    
// End of file
