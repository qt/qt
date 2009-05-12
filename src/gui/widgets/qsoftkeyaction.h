/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSOFTKEYACTION_H
#define QSOFTKEYACTION_H

#include <QtGui/qaction.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QSoftKeyActionPrivate;

class Q_GUI_EXPORT QSoftKeyAction : public QAction
{
    Q_DECLARE_PRIVATE(QSoftKeyAction)
public:
    enum StandardRole {
        Options,
        Select,
        Back,
        Next,
        Previous,
        Ok,
        Cancel,
        Edit,
        View,
        BackSpace,
        EndEdit,
        RevertEdit,
        Deselect,
        Finish,
        Menu,
        Custom
    };

    QSoftKeyAction(QObject *parent);
    QSoftKeyAction(StandardRole role, QObject *parent);
    QSoftKeyAction(const QString &text, QObject *parent);
    QSoftKeyAction(StandardRole role, const QString &text, QObject *parent);
    QSoftKeyAction(const QIcon &icon, const QString &text, QObject *parent);
    QSoftKeyAction(StandardRole role, const QIcon &icon, const QString &text, QObject *parent);
    ~QSoftKeyAction();

    StandardRole role() const;
    QString roleName() const;

    void setRole(StandardRole role);
    void setRole(const QString &role);
    int nativePosition() const;
    void setNativePosition(int position);
    int qtContextKey() const;
    void setQtContextKey(int position);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSOFTKEYACTION_H
