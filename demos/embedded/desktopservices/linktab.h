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

#ifndef LINKTAB_H_
#define LINKTAB_H_

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "contenttab.h"

// FORWARD DECLARATIONS
class QWidget;
class QListWidgetItem;

// CLASS DECLARATION

/**
* LinkTab class.
* 
* This class implements tab for opening http and mailto links.
*/
class LinkTab : public ContentTab
{
    Q_OBJECT
    
public:     // Constructors & Destructors   
    LinkTab(QWidget *parent);
    ~LinkTab();    
    
protected:  // Derived Methods  
    virtual void populateListWidget();
    virtual QUrl itemUrl(QListWidgetItem *item);    
    
private:    // Used variables  
    QListWidgetItem *m_WebItem;
    QListWidgetItem *m_MailToItem; 
    
private:    // Owned variables      
    
};

#endif // CONTENTTAB_H_    
    
// End of File
