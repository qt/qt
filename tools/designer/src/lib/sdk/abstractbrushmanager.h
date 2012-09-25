/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#ifndef ABSTRACTBRUSHMANAGER_H
#define ABSTRACTBRUSHMANAGER_H

#include <QtDesigner/sdk_global.h>

#include <QtCore/qobject.h>
#include <QtCore/qmap.h>
#include <QtGui/qbrush.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QObject;

class QDESIGNER_SDK_EXPORT QDesignerBrushManagerInterface : public QObject
{
    Q_OBJECT
public:
    QDesignerBrushManagerInterface(QObject *parentObject = 0) : QObject(parentObject) {}

    virtual QBrush brush(const QString &name) const = 0;
    virtual QMap<QString, QBrush> brushes() const = 0;
    virtual QString currentBrush() const = 0;

    virtual QString addBrush(const QString &name, const QBrush &brush) = 0;
    virtual void removeBrush(const QString &name) = 0;
    virtual void setCurrentBrush(const QString &name) = 0;

    virtual QPixmap brushPixmap(const QBrush &brush) const = 0;
Q_SIGNALS:
    void brushAdded(const QString &name, const QBrush &brush);
    void brushRemoved(const QString &name);
    void currentBrushChanged(const QString &name, const QBrush &brush);

};

QT_END_NAMESPACE

QT_END_HEADER

#endif
