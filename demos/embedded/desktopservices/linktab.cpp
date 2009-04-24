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
#include <QUrl>
#include <QListWidgetItem>

// INTERNAL INCLUDES

// CLASS HEADER
#include "linktab.h"

LinkTab::LinkTab(QWidget *parent) : 
    ContentTab(parent)
{          
}
        
LinkTab::~LinkTab() 
{
}

void LinkTab::populateListWidget()
{
    m_WebItem = new QListWidgetItem(QIcon(":/resources/browser.png"), tr("Launch Browser"), this);
    m_MailToItem = new QListWidgetItem(QIcon(":/resources/message.png"), tr("New e-mail"), this);   
}

QUrl LinkTab::itemUrl(QListWidgetItem *item)
{
    if(m_WebItem == item) {
        return QUrl(tr("http://www.qtsoftware.com"));
    } else if(m_MailToItem == item) {
        return QUrl(tr("mailto:qts60-feedback@trolltech.com?subject=QtS60 feedback&body=Hello"));
    } else {
        // We should never endup here
        Q_ASSERT(false);
        return QUrl();
    }
}

// End of file
