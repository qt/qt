/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Symbian application wrapper of the Qt Toolkit.
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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

// INCLUDE FILES
#include <exception>
#include <avkon.hrh>
#include <eikmenub.h>
#include <eikmenup.h>
#include <barsread.h>
#include <s60main.rsg>
#include <avkon.rsg>

#include "qs60mainappui_p.h"
#include <QtGui/qapplication.h>
#include <QtGui/qmenu.h>
#include <QtGui/private/qt_s60_p.h>

QT_BEGIN_NAMESPACE

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// QS60MainAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void QS60MainAppUi::ConstructL()
{
    // Cone's heap and handle checks on app destruction are not suitable for Qt apps, as many
    // objects can still exist in static data at that point. Instead we will print relevant information
    // so that comparative checks may be made for memory leaks, using ~SPrintExitInfo in corelib.
    iEikonEnv->DisableExitChecks(ETrue);

    // Initialise app UI with standard value.
    // ENoAppResourceFile and ENonStandardResourceFile makes UI to work without
    // resource files in most SDKs. S60 3rd FP1 public seems to require resource file
    // even these flags are defined
    BaseConstructL(CAknAppUi::EAknEnableSkin);

    CEikButtonGroupContainer* nativeContainer = Cba();
    nativeContainer->SetCommandSetL(R_AVKON_SOFTKEYS_EMPTY_WITH_IDS);
}

// -----------------------------------------------------------------------------
// QS60MainAppUi::QS60MainAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
QS60MainAppUi::QS60MainAppUi()
{
    // No implementation required
}

// -----------------------------------------------------------------------------
// QS60MainAppUi::~QS60MainAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
QS60MainAppUi::~QS60MainAppUi()
{
}

// -----------------------------------------------------------------------------
// QS60MainAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void QS60MainAppUi::HandleCommandL(TInt aCommand)
{
    if (qApp)
        qApp->symbianHandleCommand(aCommand);
}

// -----------------------------------------------------------------------------
// QS60MainAppUi::HandleResourceChangeL()
// Takes care of event handling.
// -----------------------------------------------------------------------------
//
void QS60MainAppUi::HandleResourceChangeL(TInt aType)
{
    CAknAppUi::HandleResourceChangeL(aType);

    if (qApp)
        qApp->symbianResourceChange(aType);
}

void QS60MainAppUi::HandleWsEventL(const TWsEvent& aEvent, CCoeControl *control)
{
    int result = 0;
    if (qApp)
        QT_TRYCATCH_LEAVING(
            result = qApp->s60ProcessEvent(const_cast<TWsEvent*>(&aEvent))
        );

    if (result <= 0)
        CAknAppUi::HandleWsEventL(aEvent, control);
}


// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void QS60MainAppUi::HandleStatusPaneSizeChange()
{
    HandleResourceChangeL(KInternalStatusPaneChange);
    HandleStackedControlsResourceChange(KInternalStatusPaneChange);
}

void QS60MainAppUi::DynInitMenuBarL(TInt, CEikMenuBar *)
{
}

void QS60MainAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane)
{
    if (aResourceId == R_QT_WRAPPERAPP_MENU) {
        if (aMenuPane->NumberOfItemsInPane() <= 1)
            qt_symbian_show_toplevel(aMenuPane);

    } else if (aResourceId != R_AVKON_MENUPANE_FEP_DEFAULT && aResourceId != R_AVKON_MENUPANE_EDITTEXT_DEFAULT && aResourceId != R_AVKON_MENUPANE_LANGUAGE_DEFAULT) {
        qt_symbian_show_submenu(aMenuPane, aResourceId);
    }
}

void QS60MainAppUi::RestoreMenuL(CCoeControl* aMenuWindow, TInt aMenuId, TMenuType aMenuType)
{
    if ((aMenuId == R_QT_WRAPPERAPP_MENUBAR) || (aMenuId == R_AVKON_MENUPANE_FEP_DEFAULT)) {
        TResourceReader reader;
        iCoeEnv->CreateResourceReaderLC(reader, aMenuId);
        aMenuWindow->ConstructFromResourceL(reader);
        CleanupStack::PopAndDestroy();
    }

    if (aMenuType == EMenuPane)
        DynInitMenuPaneL(aMenuId, (CEikMenuPane*)aMenuWindow);
    else
        DynInitMenuBarL(aMenuId, (CEikMenuBar*)aMenuWindow);
}

QT_END_NAMESPACE

// End of File
