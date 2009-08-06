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
#include "qts60mainappui.h"
#include "qts60maindocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CQtS60MainDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CQtS60MainDocument* CQtS60MainDocument::NewL(CEikApplication& aApp)
{
    CQtS60MainDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CQtS60MainDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CQtS60MainDocument* CQtS60MainDocument::NewLC(CEikApplication& aApp)
{
    CQtS60MainDocument* self = new(ELeave) CQtS60MainDocument(aApp);
    CleanupStack::PushL(self);
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
CQtS60MainDocument::CQtS60MainDocument(CEikApplication& aApp)
        : CAknDocument(aApp)
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
    return (static_cast <CEikAppUi*>(new(ELeave)CQtS60MainAppUi));
}

// End of File
