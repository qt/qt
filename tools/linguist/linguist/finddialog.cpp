/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
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

/*  TRANSLATOR FindDialog

    Choose Edit|Find from the menu bar or press Ctrl+F to pop up the
    Find dialog
*/

#include "finddialog.h"

QT_BEGIN_NAMESPACE

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    findNxt->setEnabled(false);

    connect(findNxt, SIGNAL(clicked()), this, SLOT(emitFindNext()));
    connect(led, SIGNAL(textChanged(QString)), this, SLOT(verifyText(QString)));

    led->setFocus();
}

void FindDialog::verifyText(const QString &text)
{
    findNxt->setEnabled(!text.isEmpty());
}

void FindDialog::emitFindNext()
{
    DataModel::FindLocation where;
    if (sourceText != 0)
        where =
            DataModel::FindLocation(
                (sourceText->isChecked() ? DataModel::SourceText : 0) |
                (translations->isChecked() ? DataModel::Translations : 0) |
                (comments->isChecked() ? DataModel::Comments : 0));
    else
        where = DataModel::Translations;
    emit findNext(led->text(), where, matchCase->isChecked(), ignoreAccelerators->isChecked());
    led->selectAll();
}

void FindDialog::find()
{
    led->setFocus();

    show();
    activateWindow();
    raise();
}

QT_END_NAMESPACE
