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

#ifndef __QtS60MainAPPUI_H__
#define __QtS60MainAPPUI_H__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* CQtS60MainAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CQtS60MainAppUi : public CAknAppUi
    {
    public: // Constructors and destructor

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CQtS60MainAppUi.
        * C++ default constructor. This needs to be public due to
        * the way the framework constructs the AppUi
        */
        CQtS60MainAppUi();

        /**
        * ~CQtS60MainAppUi.
        * Virtual Destructor.
        */
        virtual ~CQtS60MainAppUi();

    protected:
        void RestoreMenuL(CCoeControl* aMenuWindow,TInt aMenuId,TMenuType aMenuType);
        void DynInitMenuBarL(TInt aResourceId, CEikMenuBar *aMenuBar);
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane);

    private:  // Functions from base classes

        /**
        * From CEikAppUi, HandleCommandL.
        * Takes care of command handling.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CAknAppUi, HandleResourceChangeL
        * Handles resource change events such as layout switches in global level.
        * @param aType event type.
        */
        void HandleResourceChangeL(TInt aType);
        
        /**
        *  HandleStatusPaneSizeChange.
        *  Called by the framework when the application status pane
		*  size is changed.
        */
		void HandleStatusPaneSizeChange();

        /**
        *  Static callback method for invoking Qt main.
        *  Called asynchronously from ConstructL() - passes call to non static method.
        */
        static TInt OpenCMainStaticCallBack(  TAny* aObject );

        /**
        *  Callback method for invoking Qt main.
        *  Called from static OpenCMainStaticCallBack.
        */
        void OpenCMainCallBack();

    protected:
        void HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination);


    private: // Data

        /**
        * Async callback object to call Qt main
        * Owned by CQtS60MainAppUi
        */
        CAsyncCallBack* iAsyncCallBack;

    };

#endif // __QtS60MainAPPUI_H__

// End of File

