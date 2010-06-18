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

#ifndef HELPVIEWERQWV_H
#define HELPVIEWERQWV_H

#include "helpviewer.h"

#include <QtGui/QAction>
#include <QtWebKit/QWebView>

QT_BEGIN_NAMESPACE

class CentralWidget;
class HelpEngineWrapper;
class QMouseEvent;

class HelpViewer : public QWebView, public AbstractHelpViewer
{
    Q_OBJECT

public:
    HelpViewer(CentralWidget *parent, qreal zoom = 0.0);
    ~HelpViewer();

    QFont viewerFont() const;
    void setViewerFont(const QFont &font);

    void scaleUp();
    void scaleDown();
    void resetScale();
    qreal scale() const { return textSizeMultiplier(); }

    bool handleForwardBackwardMouseButtons(QMouseEvent *e);

    QUrl source() const;
    void setSource(const QUrl &url);

    inline QString documentTitle() const
    { return title(); }

    inline bool hasSelection() const
    { return !selectedText().isEmpty(); } // ### this is suboptimal

    inline void copy()
    { return triggerPageAction(QWebPage::Copy); }

    inline bool isForwardAvailable() const
    { return pageAction(QWebPage::Forward)->isEnabled(); }
    inline bool isBackwardAvailable() const
    { return pageAction(QWebPage::Back)->isEnabled(); }
    inline bool hasLoadFinished() const
    { return loadFinished; }

public Q_SLOTS:
    void home();
    void backward() { back(); }

Q_SIGNALS:
    void copyAvailable(bool enabled);
    void forwardAvailable(bool enabled);
    void backwardAvailable(bool enabled);
    void highlighted(const QString &);
    void sourceChanged(const QUrl &);
    void printRequested();

protected:
    virtual void wheelEvent(QWheelEvent *);
    void mouseReleaseEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *event);

private Q_SLOTS:
    void actionChanged();
    void setLoadStarted();
    void setLoadFinished(bool ok);

private:
    CentralWidget* parentWidget;
    bool loadFinished;
    HelpEngineWrapper &helpEngine;
};

QT_END_NAMESPACE

#endif  // HELPVIEWERQWV_H
