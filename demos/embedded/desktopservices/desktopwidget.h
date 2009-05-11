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

#ifndef DESKTOPWIDGET_H_
#define DESKTOPWIDGET_H_

// EXTERNAL INCLUDES
#include <QWidget>

// INTERNAL INCLUDES

// FORWARD DECLARATIONS
QT_BEGIN_NAMESPACE
class QTabWidget;
QT_END_NAMESPACE

// CLASS DECLARATION
/**
* DesktopWidget class.
* 
* Implements the main top level widget for QDesktopServices demo app.
*/
class DesktopWidget : public QWidget
{
    Q_OBJECT        
        
public:        // Constructors & Destructors  
    DesktopWidget(QWidget *parent);
    ~DesktopWidget();
   
};
    
#endif // DESKTOPWIDGET_H_    
    
// End of file
