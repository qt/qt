/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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
#ifndef HELPVIEWERQTB_H
#define HELPVIEWERQTB_H

#include "helpviewer.h"

#include <QtCore/QUrl>
#include <QtCore/QVariant>

#include <QtGui/QTextBrowser>

QT_BEGIN_NAMESPACE

class CentralWidget;
class HelpEngineWrapper;
class QContextMenuEvent;
class QKeyEvent;
class QMouseEvent;

class HelpViewer : public QTextBrowser, public AbstractHelpViewer
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
    qreal scale() const { return zoomCount; }

    bool handleForwardBackwardMouseButtons(QMouseEvent *e);

    void setSource(const QUrl &url);

    inline bool hasSelection() const
    { return textCursor().hasSelection(); }

public Q_SLOTS:
    void home();

protected:
    void wheelEvent(QWheelEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    QVariant loadResource(int type, const QUrl &name);
    void openLinkInNewTab(const QString &link);
    bool hasAnchorAt(const QPoint& pos);
    void contextMenuEvent(QContextMenuEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);

private slots:
    void openLinkInNewTab();

private:
    int zoomCount;
    bool controlPressed;
    QString lastAnchor;
    CentralWidget* parentWidget;
    HelpEngineWrapper &helpEngine;

    bool forceFont;
};

QT_END_NAMESPACE

#endif  // HELPVIEWERQTB_H
