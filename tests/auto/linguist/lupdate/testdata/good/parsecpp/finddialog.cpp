/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the autotests of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "finddialog.h"
#include "mainwindow.h"
#include "tabbedbrowser.h"
#include "helpwindow.h"

#include <QTextBrowser>
#include <QTextCursor>
#include <QStatusBar>
#include <QLineEdit>
#include <QDateTime>
#include <QGridLayout>

CaseSensitiveModel::CaseSensitiveModel(int rows, int columns, QObject *parent)
    : QStandardItemModel(rows, columns, parent)
{}
QModelIndexList CaseSensitiveModel::match(const QModelIndex &start, int role, const QVariant &value,
                                          int hits, Qt::MatchFlags flags) const
{
    if (flags == Qt::MatchFlags(Qt::MatchStartsWith|Qt::MatchWrap))
        flags |= Qt::MatchCaseSensitive;

    return QStandardItemModel::match(start, role, value, hits, flags);
}

FindDialog::FindDialog(MainWindow *parent)
    : QDialog(parent)
{
    contentsWidget = new QWidget(this);
    ui.setupUi(contentsWidget);
    ui.comboFind->setModel(new CaseSensitiveModel(0, 1, ui.comboFind));

    QVBoxLayout *l = new QVBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);
    l->addWidget(contentsWidget);

    lastBrowser = 0;
    onceFound = false;
    findExpr.clear();

    sb = new QStatusBar(this);
    l->addWidget(sb);

    sb->showMessage(tr("Enter the text you are looking for."));

    connect(ui.findButton, SIGNAL(clicked()), this, SLOT(findButtonClicked()));
    connect(ui.closeButton, SIGNAL(clicked()), this, SLOT(reject()));
}

FindDialog::~FindDialog()
{
}

void FindDialog::findButtonClicked()
{
    doFind(ui.radioForward->isChecked());
}

void FindDialog::doFind(bool forward)
{
    QTextBrowser *browser = static_cast<QTextBrowser*>(mainWindow()->browsers()->currentBrowser());
    sb->clearMessage();

    if (ui.comboFind->currentText() != findExpr || lastBrowser != browser)
        onceFound = false;
    findExpr = ui.comboFind->currentText();

    QTextDocument::FindFlags flags = 0;

    if (ui.checkCase->isChecked())
        flags |= QTextDocument::FindCaseSensitively;

    if (ui.checkWords->isChecked())
        flags |= QTextDocument::FindWholeWords;

    QTextCursor c = browser->textCursor();
    if (!c.hasSelection()) {
        if (forward)
            c.movePosition(QTextCursor::Start);
        else
            c.movePosition(QTextCursor::End);

        browser->setTextCursor(c);
    }

    QTextDocument::FindFlags options;
    if (forward == false)
        flags |= QTextDocument::FindBackward;

    QTextCursor found = browser->document()->find(findExpr, c, flags);
    if (found.isNull()) {
        if (onceFound) {
            if (forward)
                statusMessage(tr("Search reached end of the document"));
            else
                statusMessage(tr("Search reached start of the document"));
        } else {
            statusMessage(tr( "Text not found" ));
        }
    } else {
        browser->setTextCursor(found);
    }
    onceFound |= !found.isNull();
    lastBrowser = browser;
}

bool FindDialog::hasFindExpression() const
{
    return !findExpr.isEmpty();
}

void FindDialog::statusMessage(const QString &message)
{
    if (isVisible())
        sb->showMessage(message);
    else
        static_cast<MainWindow*>(parent())->statusBar()->showMessage(message, 2000);
}

MainWindow *FindDialog::mainWindow() const
{
    return static_cast<MainWindow*>(parentWidget());
}

void FindDialog::reset()
{
    ui.comboFind->setFocus();
    ui.comboFind->lineEdit()->setSelection(
        0, ui.comboFind->lineEdit()->text().length());

    QString s = QApplication::translate("QCoreApplication", "with comment", "comment");
    QString s = QApplication::translate("QCoreApplication", "empty comment", "");
    QString s = QApplication::translate("QCoreApplication", "null comment", 0);
    QString s = tr("null comment");

    QString s = QApplication::translate("QCoreApplication", "encoding, using QCoreApplication", 0, QCoreApplication::UnicodeUTF8);
    QString s = QApplication::translate("QCoreApplication", "encoding, using QApplication", 0, QApplication::UnicodeUTF8);

    QString s = QApplication::translate("Kåntekst", "encoding, using QApplication", 0, QApplication::UnicodeUTF8);

}


