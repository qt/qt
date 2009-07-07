/****************************************************************************
**
** Copyright (C) 1992-$THISYEAR$ Trolltech AS. All rights reserved.
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $LICENSE$
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
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


    // Move it to another line and change the text,
    // then lupdate should add this one as a new one, and mark the old one as obsolete.
    sb->showMessage(tr("Enter the text you want to find."));

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
}
