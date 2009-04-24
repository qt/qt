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
#include <QKeyEvent>
#include <QListWidget>
#include <QVBoxLayout>
#include <QFileInfoList>
#include <QListWidgetItem>

// INTERNAL INCLUDES

// CLASS HEADER
#include "contenttab.h"


// CONSTRUCTORS & DESTRUCTORS  
ContentTab::ContentTab(QWidget *parent) : 
    QListWidget(parent)
{             
    setDragEnabled(false);
    setIconSize(QSize(45, 45));
}
        
ContentTab::~ContentTab() 
{
}

// NEW PUBLIC METHODS
void ContentTab::init(const QDesktopServices::StandardLocation &location, 
                      const QString &icon)
{
    setContentDir(location);
    setIcon(icon);    
    
    connect(this, SIGNAL(itemClicked(QListWidgetItem *)),
            this, SLOT(openItem(QListWidgetItem *)));            
    
    populateListWidget();
}

// NEW PROTECTED METHODS
void ContentTab::setContentDir(const QDesktopServices::StandardLocation &location)
{
    m_ContentDir.setPath(QDesktopServices::storageLocation(location));
}

void ContentTab::setIcon(const QString &icon)
{
    m_Icon = QIcon(icon);
}

void ContentTab::populateListWidget()
{
    QFileInfoList fileList = m_ContentDir.entryInfoList(QStringList(), QDir::Files, QDir::Time);
    foreach(QFileInfo item, fileList) {
        new QListWidgetItem(m_Icon, itemName(item), this);
    }   
}

QString ContentTab::itemName(const QFileInfo &item)
{
    return QString(item.baseName() + "." + item.completeSuffix());
}

QUrl ContentTab::itemUrl(QListWidgetItem *item)
{
    return QUrl("file:///" + m_ContentDir.absolutePath() + "/" + item->text());
}

void ContentTab::keyPressEvent(QKeyEvent *event)  
{
    switch(event->key()) {
    case Qt::Key_Up:
        if(currentRow() == 0) {
            setCurrentRow(count()-1);
        } else {
            setCurrentRow(currentRow()-1);      
        }
        break;
    case Qt::Key_Down:
        if(currentRow() == (count()-1)) {
            setCurrentRow(0);   
        } else {
            setCurrentRow(currentRow()+1);      
        }
        break;
    case Qt::Key_Select:
        openItem(currentItem());
    default:
        QListWidget::keyPressEvent(event);
        break;
    }
}

// NEW SLOTS
void ContentTab::openItem(QListWidgetItem *item)
{
    QDesktopServices::openUrl(itemUrl(item));
}

// End of File
