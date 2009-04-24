/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_EMBEDDED_LICENSE$
**
****************************************************************************/

// INCLUDE FILES
#include <avkon.hrh>
#include <eikmenub.h>
#include <eikmenup.h>
#include <barsread.h>
#include <s60main.rsg>
#include <avkon.rsg>

#include "qts60mainappui.h"
#include <QtGui/qapplication.h>
#include <QtGui/qmenu.h>
#include <QtGui/qmenubar.h>
#include <QtGui/private/qt_s60_p.h>

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CQtS60MainAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CQtS60MainAppUi::ConstructL()
{
    // Initialise app UI with standard value.
    // ENoAppResourceFile and ENonStandardResourceFile makes UI to work without
    // resource files in most SDKs. S60 3rd FP1 public seems to require resource file
    // even these flags are defined
    BaseConstructL(CAknAppUi::EAknEnableSkin);

    // Create async callback to call Qt main,
    // this is required to give S60 app FW to finish starting correctly
    TCallBack callBack( OpenCMainStaticCallBack, this );
    iAsyncCallBack = new(ELeave) CAsyncCallBack( callBack, CActive::EPriorityIdle );
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
void CQtS60MainAppUi::HandleCommandL( TInt aCommand )
{
    switch (aCommand) {
    case EEikCmdExit:
    case EAknSoftkeyBack:
    case EAknSoftkeyExit:
        if (qApp)
            qApp->exit();
        break;
    default:
        // For now assume all unknown menu items are Qt menu items
        QMenuBar::symbianCommands(aCommand);
        break;
    }
}

void CQtS60MainAppUi::HandleWsEventL(const TWsEvent& aEvent, CCoeControl *control)
{
    int result = 0;
    if (qApp)
        result = qApp->s60ProcessEvent(const_cast<TWsEvent*>(&aEvent));

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
TInt CQtS60MainAppUi::OpenCMainStaticCallBack(  TAny* aObject )
{
    CQtS60MainAppUi* myObj = static_cast<CQtS60MainAppUi*>( aObject );
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
    TInt ret = QtMainWrapper();
    Exit();
}

void CQtS60MainAppUi::DynInitMenuBarL(TInt, CEikMenuBar *)
{
}

void CQtS60MainAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane)
{
    if (aResourceId == R_QT_WRAPPERAPP_MENU){
        if (aMenuPane->NumberOfItemsInPane() <= 1)
            qt_symbian_show_toplevel(aMenuPane);
        
    }
    else if( aResourceId != R_AVKON_MENUPANE_FEP_DEFAULT && aResourceId != R_AVKON_MENUPANE_EDITTEXT_DEFAULT && aResourceId != R_AVKON_MENUPANE_LANGUAGE_DEFAULT  ){
        qt_symbian_show_submenu(aMenuPane, aResourceId);
    }
}

void CQtS60MainAppUi::RestoreMenuL(CCoeControl* aMenuWindow,TInt aMenuId,TMenuType aMenuType)
{
    if ((aMenuId==R_QT_WRAPPERAPP_MENUBAR) || (aMenuId == R_AVKON_MENUPANE_FEP_DEFAULT)) {
        TResourceReader reader;
        iCoeEnv->CreateResourceReaderLC(reader,aMenuId);
        aMenuWindow->ConstructFromResourceL(reader);
        CleanupStack::PopAndDestroy();
    }

    if (aMenuType==EMenuPane)
        DynInitMenuPaneL(aMenuId,(CEikMenuPane*)aMenuWindow);
    else
        DynInitMenuBarL(aMenuId,(CEikMenuBar*)aMenuWindow);
   }

// End of File

