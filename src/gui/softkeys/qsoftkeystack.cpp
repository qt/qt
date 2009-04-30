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

#include <eikenv.h>
#include <eikbtgpc.h> 
#include <eikappui.h>
#include <aknappui.h>
#include <avkon.rsg>

#include "qsoftkeystack.h"

QSoftKeyStackPrivate::QSoftKeyStackPrivate()
{

}

QSoftKeyStackPrivate::~QSoftKeyStackPrivate()
{

}

void QSoftKeyStackPrivate::setNativeSoftKeys()
{
    QSoftkeySet top = softKeyStack.top();
    CCoeAppUi* appui = CEikonEnv::Static()->AppUi();
    CAknAppUi* aknAppUi = static_cast <CAknAppUi*>(appui);
    CEikButtonGroupContainer* nativeContainer = aknAppUi->Cba();
    int role = top.at(0)->role();

    switch( top.at(0)->role() )
    {
        case(QSoftKeyAction::Back):
        {
        nativeContainer->SetCommandSetL(R_AVKON_SOFTKEYS_BACK);
        }
        break;
        default:
        {
        }
    }
}

void QSoftKeyStackPrivate::push(QSoftKeyAction *softKey)
{
    QSoftkeySet softkeys;
    softkeys.append( softKey );
    softKeyStack.push(softkeys);
    setNativeSoftKeys();

}
void QSoftKeyStackPrivate::push( QList<QSoftKeyAction*> softkeys)
{

}

void QSoftKeyStackPrivate::pop()
{
    softKeyStack.pop();
}

EXPORT_C QSoftKeyStack::QSoftKeyStack(QWidget *parent)
{
    d = new QSoftKeyStackPrivate();
}

QSoftKeyStack::~QSoftKeyStack()
{
    delete d;
}

void QSoftKeyStack::push(QSoftKeyAction *softKey)
{
    d->push(softKey);
}

void QSoftKeyStack::push(QList<QSoftKeyAction*> softKeys)
{
    d->push(softKeys);
}

void QSoftKeyStack::pop()
{
    d->pop();
}

