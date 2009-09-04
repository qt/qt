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
** contained in the Technology Preview License Agreement accompanying
** this package.
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
** additional rights.  These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

// INCLUDE FILES
#include <exception>
#include "qs60mainappui_p.h"
#include "qs60maindocument_p.h"

QT_BEGIN_NAMESPACE

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QS60MainDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
QS60MainDocument* QS60MainDocument::NewL(CEikApplication& aApp)
{
    QS60MainDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// QS60MainDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
QS60MainDocument* QS60MainDocument::NewLC(CEikApplication& aApp)
{
    QS60MainDocument* self = new(ELeave) QS60MainDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// QS60MainDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void QS60MainDocument::ConstructL()
{
    // No implementation required
}

// -----------------------------------------------------------------------------
// QS60MainDocument::QS60MainDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
QS60MainDocument::QS60MainDocument(CEikApplication& aApp)
        : CAknDocument(aApp)
{
    // No implementation required
}

// ---------------------------------------------------------------------------
// QS60MainDocument::~QS60MainDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
QS60MainDocument::~QS60MainDocument()
{
    // No implementation required
}

// ---------------------------------------------------------------------------
// QS60MainDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* QS60MainDocument::CreateAppUiL()
{
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return (static_cast <CEikAppUi*>(new(ELeave)QS60MainAppUi));
}

QT_END_NAMESPACE

// End of File
