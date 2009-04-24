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

#ifndef CONTENTTAB_H_
#define CONTENTTAB_H_

// EXTERNAL INCLUDES
#include <QDir>
#include <QUrl>
#include <QIcon>
#include <QFileInfo>
#include <QListWidget>
#include <QDesktopServices>

// INTERNAL INCLUDES

// FORWARD DECLARATIONS
class QListWidgetItem;

// CLASS DECLARATION

/**
* ContentTab class.
* 
* This class implements general purpose tab for media files.
*/
class ContentTab : public QListWidget
{
    Q_OBJECT

public:         // Constructors & Destructors          
    ContentTab(QWidget *parent);
    virtual ~ContentTab();
    
public:         // New Methods
    virtual void init(const QDesktopServices::StandardLocation &location, 
                      const QString &icon);    
    
protected:      // New Methods
    virtual void setContentDir(const QDesktopServices::StandardLocation &location);
    virtual void setIcon(const QString &icon);    
    virtual void populateListWidget();
    virtual QString itemName(const QFileInfo &item);
    virtual QUrl itemUrl(QListWidgetItem *item);
    
protected:
    void keyPressEvent(QKeyEvent *event);   
    
public slots:   // New Slots
    virtual void openItem(QListWidgetItem *item);  

protected:     // Owned variables   
    QDir m_ContentDir;
    QIcon m_Icon;
};


#endif // CONTENTTAB_H_    

// End of File
