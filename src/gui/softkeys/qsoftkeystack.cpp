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

#include "private/qcore_symbian_p.h"


#include "qsoftkeystack.h"

QSoftKeyStackPrivate::QSoftKeyStackPrivate()
{

}

QSoftKeyStackPrivate::~QSoftKeyStackPrivate()
{

}

void QSoftKeyStackPrivate::mapSoftKeys(QSoftkeySet& top)
{
    if( top.count() == 1)
    {
        top.at(0)->setNativePosition(2);
        top.at(0)->setQtContextKey(Qt::Key_Context2);
    }
    else
    {
    // FIX THIS
    // veryWeirdMagic is needes as s60 5th edition sdk always panics if cba is set with index 1, this hops over it
    // This needs further investigation why so that the hack can be removed
        int veryWeirdMagic = 0;
        for (int index=0;index<top.count();index++)
        {
            top.at(index)->setNativePosition(index+veryWeirdMagic);
            top.at(index)->setQtContextKey(Qt::Key_Context1+index);
            if (veryWeirdMagic==0)
                veryWeirdMagic=1;
        }
    }
}

void QSoftKeyStackPrivate::setNativeSoftKeys()
{
    CCoeAppUi* appui = CEikonEnv::Static()->AppUi();
    CAknAppUi* aknAppUi = static_cast <CAknAppUi*>(appui);
    CEikButtonGroupContainer* nativeContainer = aknAppUi->Cba();
    nativeContainer->SetCommandSetL(R_AVKON_SOFTKEYS_EMPTY_WITH_IDS);
    if(softKeyStack.isEmpty())
        return;

    QSoftkeySet top = softKeyStack.top();
    mapSoftKeys(top);

    for (int index=0;index<top.count();index++)
    {
        QSoftKeyAction* softKeyAction = top.at(index);
        HBufC* text = qt_QString2HBufCNewL(softKeyAction->text());
        CleanupStack::PushL(text);
        if (softKeyAction->role() == QSoftKeyAction::Menu) {
            nativeContainer->SetCommandL(softKeyAction->nativePosition(), EAknSoftkeyOptions, *text);
            }
        else {
            nativeContainer->SetCommandL(softKeyAction->nativePosition(), SOFTKEYSTART+softKeyAction->qtContextKey(), *text);
        }
        CleanupStack::PopAndDestroy();
    }
}

void QSoftKeyStackPrivate::push(QSoftKeyAction *softKey)
{
    QSoftkeySet softKeySet;
    softKeySet.append( softKey );
    softKeyStack.push(softKeySet);
    setNativeSoftKeys();

}
void QSoftKeyStackPrivate::push( QList<QSoftKeyAction*> softkeys)
{
    QSoftkeySet softKeySet(softkeys);
    softKeyStack.push(softKeySet);
    setNativeSoftKeys();
}

void QSoftKeyStackPrivate::pop()
{
    softKeyStack.pop();
    setNativeSoftKeys();
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

