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

#ifndef __QtS60MainAPPLICATION_H__
#define __QtS60MainAPPLICATION_H__

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

static TUid ProcessUid()
    {
    RProcess me;
    TSecureId securId = me.SecureId();
    me.Close();
    return securId.operator TUid();
    }

/**
* CQtS60MainApplication application class.
* Provides factory to create concrete document object.
* An instance of CQtS60MainApplication is the application part of the
* AVKON application framework for the QtS60Main example application.
*/
class CQtS60MainApplication : public CAknApplication
    {
    public: // Functions from base classes

        /**
        * From CApaApplication, AppDllUid.
        * @return Application's UID (KUidQtS60MainApp).
        */
        TUid AppDllUid() const;

        /**
        * From CApaApplication, ResourceFileName
        * @return Application's resource filename (KUidQtS60MainApp).
        */
        TFileName ResourceFileName() const;

    protected: // Functions from base classes

        /**
        * From CApaApplication, CreateDocumentL.
        * Creates CQtS60MainDocument document object. The returned
        * pointer in not owned by the CQtS60MainApplication object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __QtS60MainAPPLICATION_H__

// End of File

