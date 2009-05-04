/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QSOFTKEYSTACK_H
#define QSOFTKEYSTACK_H

#include <QtGui/qwidget.h>
#include <QtGui/qaction.h>
#include <QStack>

#include "qsoftkeyaction.h"

#define QSoftkeySet QList <QSoftKeyAction*>
#define SOFTKEYSTART 5000
#define SOFTKEYEND 5005
class QSoftKeyStackPrivate : public QObject
    {
    Q_OBJECT
    public:
    IMPORT_C QSoftKeyStackPrivate();
    ~QSoftKeyStackPrivate();    

public:
    void push(QSoftKeyAction *softKey);
    void push(QList<QSoftKeyAction*> softKeys);
    void pop();   
    
private:
    void mapSoftKeys(QSoftkeySet& top);
    void setNativeSoftKeys();
    
private:
    QStack <QSoftkeySet> softKeyStack;
    };



/*class QSoftkeySet
    {
    const QList<QSoftKeyAction*> softkeys;    
    };


class QSoftkeySet
    {
    const QList<QSoftKeyAction*> softkeys;    
    };
*/
class QSoftKeyStack : public QObject
{
    Q_OBJECT
public:
    IMPORT_C QSoftKeyStack(QWidget *parent);
    ~QSoftKeyStack();
public:
    void push(QSoftKeyAction *softKey);
    void push(QList<QSoftKeyAction*> softkeys);
    void pop();
protected:
    
private Q_SLOTS:

private:
    QSoftKeyStackPrivate *d;

};

#endif // QSOFTKEYSTACK_H
