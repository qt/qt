/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Assistant of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef HELPVIEWERPRIVATE_H
#define HELPVIEWERPRIVATE_H

#include "centralwidget.h"
#include "helpviewer.h"
#include "openpagesmanager.h"

#include <QtCore/QObject>
#ifdef QT_NO_WEBKIT
#include <QtGui/QTextBrowser>
#endif

QT_BEGIN_NAMESPACE

class HelpViewer::HelpViewerPrivate : public QObject
{
    Q_OBJECT

public:
#ifdef QT_NO_WEBKIT
    HelpViewerPrivate(int zoom)
        : zoomCount(zoom)
        , forceFont(false)
        , lastAnchor(QString())
#else
    HelpViewerPrivate()
#endif
    {
        m_loadFinished = false;
    }

#ifdef QT_NO_WEBKIT
    bool hasAnchorAt(QTextBrowser *browser, const QPoint& pos)
    {
        lastAnchor = browser->anchorAt(pos);
        if (lastAnchor.isEmpty())
            return false;

        lastAnchor = browser->source().resolved(lastAnchor).toString();
        if (lastAnchor.at(0) == QLatin1Char('#')) {
            QString src = browser->source().toString();
            int hsh = src.indexOf(QLatin1Char('#'));
            lastAnchor = (hsh >= 0 ? src.left(hsh) : src) + lastAnchor;
        }
        return true;
    }

    void openLink(bool newPage)
    {
        if(lastAnchor.isEmpty())
            return;
        if (newPage)
            OpenPagesManager::instance()->createPage(lastAnchor);
        else
            CentralWidget::instance()->setSource(lastAnchor);
        lastAnchor.clear();
    }

public slots:
    void openLink()
    {
        openLink(false);
    }

    void openLinkInNewPage()
    {
        openLink(true);
    }

public:
    int zoomCount;
    bool forceFont;
    QString lastAnchor;
#endif // QT_NO_WEBKIT

public:
    bool m_loadFinished;
};

QT_END_NAMESPACE

#endif  // HELPVIEWERPRIVATE_H
