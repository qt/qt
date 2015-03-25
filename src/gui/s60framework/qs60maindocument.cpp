/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Symbian application wrapper of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qs60mainappui.h"
#include "qs60maindocument.h"
#include "qcoreapplication.h"
#include "qevent.h"
#include "private/qcore_symbian_p.h"

#include <exception>

QT_BEGIN_NAMESPACE

/*!
  \class QS60MainDocument
  \since 4.6
  \brief The QS60MainDocument class is a helper class for S60 migration.

  \warning This class is provided only to get access to S60 specific
  functionality in the application framework classes. It is not
  portable. We strongly recommend against using it in new applications.

  The QS60MainDocument provides a helper class for use in migrating
  from existing S60 based applications to Qt based applications. It is
  used in the exact same way as the \c CEikDocument class from
  Symbian, but internally provides extensions used by Qt.

  When modifying old S60 applications that rely on implementing
  functions in \c CEikDocument, the class should be modified to
  inherit from this class instead of \c CEikDocument. Then the
  application can choose to override only certain functions.

  For more information on \c CEikDocument, please see the S60
  documentation.

  Unlike other Qt classes, QS60MainDocument behaves like an S60 class,
  and can throw Symbian leaves.

  \sa QS60MainApplication, QS60MainAppUi
 */

/*!
 * \brief Constructs an instance of QS60MainDocument.
 *
 * \a mainApplication should contain a pointer to a QS60MainApplication instance.
 */
QS60MainDocument::QS60MainDocument(CEikApplication &mainApplication)
    : QS60MainDocumentBase(mainApplication)
{
    // No implementation required
}

/*!
 * \brief Destroys the QS60MainDocument.
 */
QS60MainDocument::~QS60MainDocument()
{
    // No implementation required
}

/*!
 * \brief Creates an instance of QS60MainAppUi.
 *
 * \sa QS60MainAppUi
 */
CEikAppUi *QS60MainDocument::CreateAppUiL()
{
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return (static_cast <CEikAppUi*>(new(ELeave)QS60MainAppUi));
}

/*!
  \internal
 */
CFileStore *QS60MainDocument::OpenFileL(TBool /*aDoOpen*/, const TDesC &aFilename, RFs &/*aFs*/)
{
    QT_TRYCATCH_LEAVING( {
        QCoreApplication* app = QCoreApplication::instance();
        QString qname = qt_TDesC2QString(aFilename);
        QFileOpenEvent* event = new QFileOpenEvent(qname);
        app->postEvent(app, event);
    })
    return 0;
}

/*!
  \internal
 */
void QS60MainDocument::OpenFileL(CFileStore *&aFileStore, RFile &aFile)
{
    QT_TRYCATCH_LEAVING( {
        QCoreApplication* app = QCoreApplication::instance();
        QFileOpenEvent* event = new QFileOpenEvent(aFile);
        app->postEvent(app, event);
        aFileStore = 0;
    })
}

QT_END_NAMESPACE
