/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include <eikenv.h>
#include <eikbtgpc.h> 
#include <eikappui.h>
#include <aknappui.h>
#include <avkon.rsg>

#include "private/qcore_symbian_p.h"

#include "qsoftkeystack_p.h"
#include "qapplication.h"
#include "qmainwindow.h"

void QSoftKeyStackPrivate::mapSoftKeys(const QSoftkeySet &top)
{
    if (top.count() == 1) {
        top.at(0)->setNativePosition(2);
        top.at(0)->setQtContextKey(Qt::Key_Context1);
    }
    else {
    // FIX THIS
    // veryWeirdMagic is needes as s60 5th edition sdk always panics if cba is set with index 1, this hops over it
    // This needs further investigation why so that the hack can be removed
        int veryWeirdMagic = 0;
        for (int index = 0; index < top.count(); index++) {
            top.at(index)->setNativePosition(index + veryWeirdMagic);
            top.at(index)->setQtContextKey(Qt::Key_Context1 + index);
            if (veryWeirdMagic == 0)
                veryWeirdMagic = 1;
        }
    }
}

void QSoftKeyStackPrivate::setNativeSoftKeys()
{
    CCoeAppUi* appui = CEikonEnv::Static()->AppUi();
    CAknAppUi* aknAppUi = static_cast <CAknAppUi*>(appui);
    CEikButtonGroupContainer* nativeContainer = aknAppUi->Cba();
    nativeContainer->SetCommandSetL(R_AVKON_SOFTKEYS_EMPTY_WITH_IDS);
    if (softKeyStack.isEmpty())
        return;

    const QSoftkeySet top = softKeyStack.top();
    mapSoftKeys(top);

    for (int index = 0; index < top.count(); index++) {
        const QSoftKeyAction* softKeyAction = top.at(index);
        if (softKeyAction->role() != QSoftKeyAction::ContextMenu) {

            HBufC* text = qt_QString2HBufCNewL(softKeyAction->text());
            CleanupStack::PushL(text);
            if (softKeyAction->role() == QSoftKeyAction::Menu) {
                nativeContainer->SetCommandL(softKeyAction->nativePosition(), EAknSoftkeyOptions, *text);
            } else {
                nativeContainer->SetCommandL(softKeyAction->nativePosition(), SOFTKEYSTART + softKeyAction->qtContextKey()-Qt::Key_Context1, *text);
            }
            CleanupStack::PopAndDestroy();
        }
    }
}

void QSoftKeyStackPrivate::handleSoftKeyPress(int command)
{
    const QMainWindow *activeMainWindow =
        qobject_cast<const QMainWindow*>(QApplication::activeWindow());
    if (!activeMainWindow)
        return;
    QSoftKeyStackPrivate *d_ptr = activeMainWindow->softKeyStack()->d_func();

    const QSoftkeySet top = d_ptr->softKeyStack.top();
    int index = command-SOFTKEYSTART;
    if (index < 0 || index >= top.count()) {
        // ### FIX THIS, add proper error handling, now fail quietly
        return;
    }

    top.at(index)->activate(QAction::Trigger);
}

