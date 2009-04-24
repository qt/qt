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
#include "qts60maindocument.h"
#include "qts60mainapplication.h"
#include <bautils.h>
#include <coemain.h>

// ============================ MEMBER FUNCTIONS ===============================


_LIT( KQtWrapperResourceFile,"\\resource\\apps\\s60main.rsc" );

// -----------------------------------------------------------------------------
// CQtS60MainApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CQtS60MainApplication::CreateDocumentL()
    {
    // Create an QtS60Main document, and return a pointer to it
    return (static_cast<CApaDocument*>( CQtS60MainDocument::NewL( *this ) ) );
    }

// -----------------------------------------------------------------------------
// CQtS60MainApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CQtS60MainApplication::AppDllUid() const
    {
    // Return the UID for the QtS60Main application
    return ProcessUid();
    }

// -----------------------------------------------------------------------------
// CQtS60MainApplication::ResourceFileName()
// Returns application resource filename
// -----------------------------------------------------------------------------
//
TFileName CQtS60MainApplication::ResourceFileName() const
    {
    TFindFile finder(iCoeEnv->FsSession());
    TInt err = finder.FindByDir(KQtWrapperResourceFile, KNullDesC);
    if (err == KErrNone)
        return finder.File();
    return KNullDesC();
    }


// End of File

