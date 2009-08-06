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
** contact the sales department at http://www.qtsoftware.com/contact.
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

#include "qts60mainappui_p.h"
#include <QtGui/qapplication.h>
#include <QtGui/qmenu.h>
#include <QtGui/private/qt_s60_p.h>

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CQtS60MainAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CQtS60MainAppUi::ConstructL()
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

    // Create async callback to call Qt main,
    // this is required to give S60 app FW to finish starting correctly
    TCallBack callBack(OpenCMainStaticCallBack, this);
    iAsyncCallBack = new(ELeave) CAsyncCallBack(callBack, CActive::EPriorityIdle);
    iAsyncCallBack->Call();
}

// -----------------------------------------------------------------------------
// CQtS60MainAppUi::CQtS60MainAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CQtS60MainAppUi::CQtS60MainAppUi()
{
    // No implementation required
}

// -----------------------------------------------------------------------------
// CQtS60MainAppUi::~CQtS60MainAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CQtS60MainAppUi::~CQtS60MainAppUi()
{
    delete iAsyncCallBack;
}

// -----------------------------------------------------------------------------
// CQtS60MainAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CQtS60MainAppUi::HandleCommandL(TInt aCommand)
{
    if (qApp)
        qApp->symbianHandleCommand(aCommand);
}

// -----------------------------------------------------------------------------
// CQtS60MainAppUi::HandleResourceChangeL()
// Takes care of event handling.
// -----------------------------------------------------------------------------
//
void CQtS60MainAppUi::HandleResourceChangeL(TInt aType)
{
    CAknAppUi::HandleResourceChangeL(aType);

    if (qApp)
        qApp->symbianResourceChange(aType);
}

void CQtS60MainAppUi::HandleWsEventL(const TWsEvent& aEvent, CCoeControl *control)
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
void CQtS60MainAppUi::HandleStatusPaneSizeChange()
{
    HandleResourceChangeL(KInternalStatusPaneChange);
    HandleStackedControlsResourceChange(KInternalStatusPaneChange);
}

// -----------------------------------------------------------------------------
//  Called asynchronously from ConstructL() - passes call to nan static method
// -----------------------------------------------------------------------------
//
TInt CQtS60MainAppUi::OpenCMainStaticCallBack(TAny* aObject)
{
    CQtS60MainAppUi* myObj = static_cast<CQtS60MainAppUi*>(aObject);
    myObj->OpenCMainCallBack();
    return 0;
}

#include "qtS60main_mcrt0.cpp"

// -----------------------------------------------------------------------------
//  Invokes Qt main, the Qt main will block and when we return from there
//  application should be closed. -> Call Exit();
// -----------------------------------------------------------------------------
//
void CQtS60MainAppUi::OpenCMainCallBack()
{
    TInt ret;
    TRAPD(err, ret = QtMainWrapper());
    Q_UNUSED(ret);
    Q_UNUSED(err);
    Exit();
}

void CQtS60MainAppUi::DynInitMenuBarL(TInt, CEikMenuBar *)
{
}

void CQtS60MainAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane)
{
    if (aResourceId == R_QT_WRAPPERAPP_MENU) {
        if (aMenuPane->NumberOfItemsInPane() <= 1)
            qt_symbian_show_toplevel(aMenuPane);

    } else if (aResourceId != R_AVKON_MENUPANE_FEP_DEFAULT && aResourceId != R_AVKON_MENUPANE_EDITTEXT_DEFAULT && aResourceId != R_AVKON_MENUPANE_LANGUAGE_DEFAULT) {
        qt_symbian_show_submenu(aMenuPane, aResourceId);
    }
}

void CQtS60MainAppUi::RestoreMenuL(CCoeControl* aMenuWindow, TInt aMenuId, TMenuType aMenuType)
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

// End of File
