/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** All rights reserved.
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
#ifndef HELPVIEWER_H
#define HELPVIEWER_H

#include <QtCore/QString>

#include <QtGui/QFont>

QT_BEGIN_NAMESPACE

class QMouseEvent;
class QUrl;

class AbstractHelpViewer
{
public:
    AbstractHelpViewer();
    virtual ~AbstractHelpViewer();

    virtual QFont viewerFont() const = 0;
    virtual void setViewerFont(const QFont &font) = 0;

    virtual void scaleUp() = 0;
    virtual void scaleDown() = 0;
    
    virtual void resetScale() = 0;
    virtual qreal scale() const = 0;

    virtual bool handleForwardBackwardMouseButtons(QMouseEvent *e) = 0;

    static const QLatin1String DocPath;
    static const QString AboutBlank;
    static const QString LocalHelpFile;
    static const QString PageNotFoundMessage;

    static bool isLocalUrl(const QUrl &url);
    static bool canOpenPage(const QString &url);
    static QString mimeFromUrl(const QUrl &url);
    static bool launchWithExternalApp(const QUrl &url);
};

QT_END_NAMESPACE

#endif  // HELPVIEWER_H
