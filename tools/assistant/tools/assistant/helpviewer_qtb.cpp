/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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
#include "helpviewer_qtb.h"
#include "helpviewer_qwv.h"

#if defined(QT_NO_WEBKIT)

#include "centralwidget.h"
#include "helpenginewrapper.h"
#include "tracer.h"

#include <QtCore/QDir>
#include <QtCore/QStringBuilder>

#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>
#include <QtGui/QClipboard>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QDesktopServices>

QT_BEGIN_NAMESPACE

HelpViewer::HelpViewer(CentralWidget *parent, qreal zoom)
    : QTextBrowser(parent)
    , zoomCount(zoom)
    , controlPressed(false)
    , lastAnchor(QString())
    , parentWidget(parent)
    , helpEngine(HelpEngineWrapper::instance())
    , forceFont(false)
{
    TRACE_OBJ
    installEventFilter(this);
    document()->setDocumentMargin(8);

    QFont font = viewerFont();
    font.setPointSize(int(font.pointSize() + zoom));
    setViewerFont(font);
}

HelpViewer::~HelpViewer()
{
    TRACE_OBJ
}

QFont HelpViewer::viewerFont() const
{
    TRACE_OBJ
    if (HelpEngineWrapper::instance().usesBrowserFont())
        return helpEngine.browserFont();
    return qApp->font();
}

void HelpViewer::setViewerFont(const QFont &newFont)
{
    TRACE_OBJ
    if (font() != newFont) {
        forceFont = true;
        setFont(newFont);
        forceFont = false;
    }
}

void HelpViewer::scaleUp()
{
    TRACE_OBJ
    if (zoomCount < 10) {
        ++zoomCount;
        forceFont = true;
        zoomIn();
        forceFont = false;
    }
}

void HelpViewer::scaleDown()
{
    TRACE_OBJ
    if (zoomCount > -5) {
        --zoomCount;
        forceFont = true;
        zoomOut();
        forceFont = false;
    }
}

void HelpViewer::resetScale()
{
    TRACE_OBJ
    if (zoomCount != 0) {
        forceFont = true;
        zoomOut(zoomCount);
        forceFont = false;
    }
    zoomCount = 0;
}

void HelpViewer::setSource(const QUrl &url)
{
    TRACE_OBJ
    bool help = url.toString() == QLatin1String("help");
    if (url.isValid() && !help) {
        if (launchedWithExternalApp(url))
            return;

        QUrl u = helpEngine.findFile(url);
        if (u.isValid()) {
            QTextBrowser::setSource(u);
            return;
        }
    }

    if (help) {
        QTextBrowser::setSource(QUrl(QLatin1String("qthelp://com.trolltech.com."
            "assistantinternal-1.0.0/assistant/assistant.html")));
    } else {
        QTextBrowser::setSource(url);
        setHtml(PageNotFoundMessage.arg(url.toString()));
        emit sourceChanged(url);
    }
}

bool HelpViewer::launchedWithExternalApp(const QUrl &url)
{
    TRACE_OBJ
    const bool canOpen = canOpenPage(url.path());
    if (!isLocalUrl(url) || !canOpen) {
        bool launched = false;
        if (!canOpen && url.scheme() == QLatin1String("qthelp")) {
            const QString& path = url.path();
            const int lastDash = path.lastIndexOf(QChar('/'));
            QString fileName = QDir::tempPath() + QDir::separator();
            if (lastDash < 0)
                fileName += path;
            else
                fileName += path.mid(lastDash + 1, path.length());

            QFile tmpFile(QDir::cleanPath(fileName));
            if (tmpFile.open(QIODevice::ReadWrite)) {
                tmpFile.write(helpEngine.fileData(url));
                tmpFile.close();
            }
            launched = QDesktopServices::openUrl(QUrl(tmpFile.fileName()));
        } else {
            launched = QDesktopServices::openUrl(url);
        }

        if (!launched) {
            QMessageBox::information(this, tr("Help"),
                tr("Unable to launch external application.\n"), tr("OK"));
        }
        return true;
    }
    return false;
}

QVariant HelpViewer::loadResource(int type, const QUrl &name)
{
    TRACE_OBJ
    QByteArray ba;
    if (type < 4) {
        ba = helpEngine.fileData(name);
        if (name.toString().endsWith(QLatin1String(".svg"), Qt::CaseInsensitive)) {
            QImage image;
            image.loadFromData(ba, "svg");
            if (!image.isNull())
                return image;
        }
    }
    return ba;
}

void HelpViewer::openLinkInNewTab()
{
    TRACE_OBJ
    if(lastAnchor.isEmpty())
        return;

    parentWidget->setSourceInNewTab(QUrl(lastAnchor));
    lastAnchor.clear();
}

void HelpViewer::openLinkInNewTab(const QString &link)
{
    TRACE_OBJ
    lastAnchor = link;
    openLinkInNewTab();
}

bool HelpViewer::hasAnchorAt(const QPoint& pos)
{
    TRACE_OBJ
    lastAnchor = anchorAt(pos);
    if (lastAnchor.isEmpty())
        return false;

    lastAnchor = source().resolved(lastAnchor).toString();
    if (lastAnchor.at(0) == QLatin1Char('#')) {
        QString src = source().toString();
        int hsh = src.indexOf(QLatin1Char('#'));
        lastAnchor = (hsh>=0 ? src.left(hsh) : src) + lastAnchor;
    }

    return true;
}

void HelpViewer::contextMenuEvent(QContextMenuEvent *e)
{
    TRACE_OBJ
    QMenu menu(QLatin1String(""), 0);

    QUrl link;
    QAction *copyAnchorAction = 0;
    if (hasAnchorAt(e->pos())) {
        link = anchorAt(e->pos());
        if (link.isRelative())
            link = source().resolved(link);
        copyAnchorAction = menu.addAction(tr("Copy &Link Location"));
        copyAnchorAction->setEnabled(!link.isEmpty() && link.isValid());

        menu.addAction(tr("Open Link in New Tab\tCtrl+LMB"), this,
            SLOT(openLinkInNewTab()));
        menu.addSeparator();
    }
    menu.addActions(parentWidget->globalActions());
    QAction *action = menu.exec(e->globalPos());
    if (action == copyAnchorAction)
        QApplication::clipboard()->setText(link.toString());
}

void HelpViewer::mouseReleaseEvent(QMouseEvent *e)
{
    TRACE_OBJ
    if (e->button() == Qt::XButton1) {
        QTextBrowser::backward();
        return;
    }

    if (e->button() == Qt::XButton2) {
        QTextBrowser::forward();
        return;
    }

    controlPressed = e->modifiers() & Qt::ControlModifier;
    if ((controlPressed && hasAnchorAt(e->pos())) ||
        (e->button() == Qt::MidButton && hasAnchorAt(e->pos()))) {
        openLinkInNewTab();
        return;
    }

    QTextBrowser::mouseReleaseEvent(e);
}

void HelpViewer::keyPressEvent(QKeyEvent *e)
{
    TRACE_OBJ
    if ((e->key() == Qt::Key_Home && e->modifiers() != Qt::NoModifier)
        || (e->key() == Qt::Key_End && e->modifiers() != Qt::NoModifier)) {
        QKeyEvent* event = new QKeyEvent(e->type(), e->key(), Qt::NoModifier,
            e->text(), e->isAutoRepeat(), e->count());
        e = event;
    }
    QTextBrowser::keyPressEvent(e);
}

void HelpViewer::home()
{
    TRACE_OBJ
    setSource(helpEngine.homePage());
}

void HelpViewer::wheelEvent(QWheelEvent *e)
{
    TRACE_OBJ
    if (e->modifiers() == Qt::ControlModifier) {
        e->accept();
        e->delta() > 0 ? scaleUp() : scaleDown();
    } else {
        QTextBrowser::wheelEvent(e);
    }
}

bool HelpViewer::eventFilter(QObject *obj, QEvent *event)
{
    TRACE_OBJ
    if (event->type() == QEvent::FontChange && !forceFont)
        return true;
    return QTextBrowser::eventFilter(obj, event);
}

QT_END_NAMESPACE

#endif  // QT_NO_WEBKIT
