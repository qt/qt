/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QT_NO_PRINTDIALOG

#include "qprintdialog.h"

#include <qwidget.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <private/qapplication_p.h>

#include <private/qabstractprintdialog_p.h>
#include <private/qprintengine_win_p.h>
#include <private/qprinter_p.h>

QT_BEGIN_NAMESPACE

extern void qt_win_eatMouseMove();

class QPrintDialogPrivate : public QAbstractPrintDialogPrivate
{
    Q_DECLARE_PUBLIC(QPrintDialog)
public:
    QPrintDialogPrivate()
        : ep(0)
    {
    }

    inline void _q_printToFileChanged(int) {}
    inline void _q_rbPrintRangeToggled(bool) {}
    inline void _q_printerChanged(int) {}
    inline void _q_chbPrintLastFirstToggled(bool) {}
    inline void _q_paperSizeChanged(int) {}
    inline void _q_btnBrowseClicked() {}
    inline void _q_btnPropertiesClicked() {}
    int openWindowsPrintDialogModally();

    QWin32PrintEnginePrivate *ep;
};

static PRINTDLG* qt_win_make_PRINTDLG(QWidget *parent,
                                      QPrintDialog *pdlg,
                                      QPrintDialogPrivate *d, HGLOBAL *tempDevNames)
{
    PRINTDLG *pd = new PRINTDLG;
    memset(pd, 0, sizeof(PRINTDLG));
    pd->lStructSize = sizeof(PRINTDLG);

    DEVMODE *devMode = d->ep->devMode;

    if (devMode) {
        int size = sizeof(DEVMODE) + devMode->dmDriverExtra;
        pd->hDevMode = GlobalAlloc(GHND, size);
        {
            void *dest = GlobalLock(pd->hDevMode);
            memcpy(dest, devMode, size);
            GlobalUnlock(pd->hDevMode);
        }
    } else {
        pd->hDevMode = NULL;
    }
    pd->hDevNames  = tempDevNames;

    pd->Flags = PD_RETURNDC;
    pd->Flags |= PD_USEDEVMODECOPIESANDCOLLATE;

    if (!pdlg->isOptionEnabled(QPrintDialog::PrintSelection))
        pd->Flags |= PD_NOSELECTION;
    if (pdlg->isOptionEnabled(QPrintDialog::PrintPageRange)) {
        pd->nMinPage = pdlg->minPage();
        pd->nMaxPage = pdlg->maxPage();
    }

    if(!pdlg->isOptionEnabled(QPrintDialog::PrintToFile))
        pd->Flags |= PD_DISABLEPRINTTOFILE;

    if (pdlg->printRange() == QPrintDialog::Selection)
        pd->Flags |= PD_SELECTION;
    else if (pdlg->printRange() == QPrintDialog::PageRange)
        pd->Flags |= PD_PAGENUMS;
    else
        pd->Flags |= PD_ALLPAGES;

    // As stated by MSDN, to enable collate option when minpage==maxpage==0
    // set the PD_NOPAGENUMS flag
    if (pd->nMinPage==0 && pd->nMaxPage==0)
        pd->Flags |= PD_NOPAGENUMS;

    if (d->ep->printToFile)
        pd->Flags |= PD_PRINTTOFILE;
    Q_ASSERT(!parent ||parent->testAttribute(Qt::WA_WState_Created));
    pd->hwndOwner = parent ? parent->winId() : 0;
    pd->nFromPage = qMax(pdlg->fromPage(), pdlg->minPage());
    pd->nToPage   = (pdlg->toPage() > 0) ? qMin(pdlg->toPage(), pdlg->maxPage()) : 1;
    pd->nCopies = d->ep->num_copies;

    return pd;
}

static void qt_win_clean_up_PRINTDLG(PRINTDLG **pd)
{
    delete *pd;
    *pd = 0;
}

static void qt_win_read_back_PRINTDLG(PRINTDLG *pd, QPrintDialog *pdlg, QPrintDialogPrivate *d)
{
    if (pd->Flags & PD_SELECTION) {
        pdlg->setPrintRange(QPrintDialog::Selection);
        pdlg->setFromTo(0, 0);
    } else if (pd->Flags & PD_PAGENUMS) {
        pdlg->setPrintRange(QPrintDialog::PageRange);
        pdlg->setFromTo(pd->nFromPage, pd->nToPage);
    } else {
        pdlg->setPrintRange(QPrintDialog::AllPages);
        pdlg->setFromTo(0, 0);
    }

    d->ep->printToFile = (pd->Flags & PD_PRINTTOFILE) != 0;

    d->ep->readDevnames(pd->hDevNames);
    d->ep->readDevmode(pd->hDevMode);
    d->ep->updateCustomPaperSize();

    if (d->ep->printToFile && d->ep->fileName.isEmpty())
        d->ep->fileName = d->ep->port;
    else if (!d->ep->printToFile && d->ep->fileName == QLatin1String("FILE:"))
        d->ep->fileName.clear();
}

static bool warnIfNotNative(QPrinter *printer)
{
    if (printer->outputFormat() != QPrinter::NativeFormat) {
        qWarning("QPrintDialog: Cannot be used on non-native printers");
        return false;
    }
    return true;
}

QPrintDialog::QPrintDialog(QPrinter *printer, QWidget *parent)
    : QAbstractPrintDialog( *(new QPrintDialogPrivate), printer, parent)
{
    Q_D(QPrintDialog);
    if (!warnIfNotNative(d->printer))
        return;
    d->ep = static_cast<QWin32PrintEngine *>(d->printer->paintEngine())->d_func();
}

QPrintDialog::QPrintDialog(QWidget *parent)
    : QAbstractPrintDialog( *(new QPrintDialogPrivate), 0, parent)
{
    Q_D(QPrintDialog);
    if (!warnIfNotNative(d->printer))
        return;
    d->ep = static_cast<QWin32PrintEngine *>(d->printer->paintEngine())->d_func();
}

QPrintDialog::~QPrintDialog()
{
}

int QPrintDialog::exec()
{
    if (!warnIfNotNative(printer()))
        return 0;

    Q_D(QPrintDialog);
    return d->openWindowsPrintDialogModally();
}

int QPrintDialogPrivate::openWindowsPrintDialogModally()
{
    Q_Q(QPrintDialog);
    QWidget *parent = q->parentWidget();
    if (parent)
        parent = parent->window();
    else
        parent = QApplication::activeWindow();

    QWidget modal_widget;
    modal_widget.setAttribute(Qt::WA_NoChildEventsForParent, true);
    modal_widget.setParent(parent, Qt::Window);
    QApplicationPrivate::enterModal(&modal_widget);

    HGLOBAL *tempDevNames = ep->createDevNames();

    bool result;
    bool done;
    PRINTDLG *pd = qt_win_make_PRINTDLG(parent, q, this, tempDevNames);

    do {
        done = true;
        result = PrintDlg(pd);
        if ((pd->Flags & PD_PAGENUMS) && (pd->nFromPage > pd->nToPage))
            done = false;
        if (result && pd->hDC == 0)
            result = false;
        else if (!result)
            done = true;

        if (!done) {
            QMessageBox::warning(0, QPrintDialog::tr("Print"),
                                 QPrintDialog::tr("The 'From' value cannot be greater than the 'To' value."),
                                 QPrintDialog::tr("OK"));
        }
    } while (!done);

    QApplicationPrivate::leaveModal(&modal_widget);

    qt_win_eatMouseMove();

    // write values back...
    if (result) {
        qt_win_read_back_PRINTDLG(pd, q, this);
        qt_win_clean_up_PRINTDLG(&pd);
        // update printer validity
        printer->d_func()->validPrinter = !ep->name.isEmpty();
    }

    // Cleanup...
    GlobalFree(tempDevNames);

    q->done(result);

    return result;
}

void QPrintDialog::setVisible(bool visible)
{
    Q_D(QPrintDialog);

    // its always modal, so we cannot hide a native print dialog
    if (!visible)
        return;

    if (!warnIfNotNative(d->printer))
        return;

    (void)d->openWindowsPrintDialogModally();
    return;
}

QT_END_NAMESPACE

#include "moc_qprintdialog.cpp"

#endif // QT_NO_PRINTDIALOG
