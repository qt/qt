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

FindDialog::FindDialog(MainWindow *parent)
    : QDialog(parent)
{
    // Move it to another line and change the text,
    // then lupdate should add this one as a new one, and mark the old one as obsolete.
    sb->showMessage(tr("Enter the text you want to find."));

}

void FindDialog::doFind(bool forward)
{
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
}
