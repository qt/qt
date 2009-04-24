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
#include "qts60mainappui.h"
#include "qts60maindocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CQtS60MainDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CQtS60MainDocument* CQtS60MainDocument::NewL( CEikApplication& aApp )
    {
    CQtS60MainDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CQtS60MainDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CQtS60MainDocument* CQtS60MainDocument::NewLC( CEikApplication& aApp )
    {
    CQtS60MainDocument* self = new ( ELeave ) CQtS60MainDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CQtS60MainDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CQtS60MainDocument::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CQtS60MainDocument::CQtS60MainDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CQtS60MainDocument::CQtS60MainDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CQtS60MainDocument::~CQtS60MainDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CQtS60MainDocument::~CQtS60MainDocument()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CQtS60MainDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CQtS60MainDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return ( static_cast <CEikAppUi*> ( new ( ELeave )CQtS60MainAppUi ) );
    }

// End of File

