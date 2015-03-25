/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ABSTRACTFORMWINDOWMANAGER_H
#define ABSTRACTFORMWINDOWMANAGER_H

#include <QtDesigner/sdk_global.h>
#include <QtDesigner/abstractformwindow.h>

#include <QtCore/QObject>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QAction;
class QActionGroup;
class QDesignerFormEditorInterface;
class DomUI;
class QWidget;
class QDesignerDnDItemInterface;

class QDESIGNER_SDK_EXPORT QDesignerFormWindowManagerInterface: public QObject
{
    Q_OBJECT
public:
    QDesignerFormWindowManagerInterface(QObject *parent = 0);
    virtual ~QDesignerFormWindowManagerInterface();

    virtual QAction *actionCut() const;
    virtual QAction *actionCopy() const;
    virtual QAction *actionPaste() const;
    virtual QAction *actionDelete() const;
    virtual QAction *actionSelectAll() const;
    virtual QAction *actionLower() const;
    virtual QAction *actionRaise() const;
    virtual QAction *actionUndo() const;
    virtual QAction *actionRedo() const;

    virtual QAction *actionHorizontalLayout() const;
    virtual QAction *actionVerticalLayout() const;
    virtual QAction *actionSplitHorizontal() const;
    virtual QAction *actionSplitVertical() const;
    virtual QAction *actionGridLayout() const;
    QAction *actionFormLayout() const;
    virtual QAction *actionBreakLayout() const;
    virtual QAction *actionAdjustSize() const;
    QAction *actionSimplifyLayout() const;

    virtual QDesignerFormWindowInterface *activeFormWindow() const;

    virtual int formWindowCount() const;
    virtual QDesignerFormWindowInterface *formWindow(int index) const;

    virtual QDesignerFormWindowInterface *createFormWindow(QWidget *parentWidget = 0, Qt::WindowFlags flags = 0);

    virtual QDesignerFormEditorInterface *core() const;

    virtual void dragItems(const QList<QDesignerDnDItemInterface*> &item_list) = 0;

Q_SIGNALS:
    void formWindowAdded(QDesignerFormWindowInterface *formWindow);
    void formWindowRemoved(QDesignerFormWindowInterface *formWindow);
    void activeFormWindowChanged(QDesignerFormWindowInterface *formWindow);

public Q_SLOTS:
    virtual void addFormWindow(QDesignerFormWindowInterface *formWindow);
    virtual void removeFormWindow(QDesignerFormWindowInterface *formWindow);
    virtual void setActiveFormWindow(QDesignerFormWindowInterface *formWindow);

protected:
    void setActionFormLayout(QAction *action);
    void setActionSimplifyLayout(QAction *action);

private:
    QDesignerFormWindowManagerInterface(const QDesignerFormWindowManagerInterface &other);
    QDesignerFormWindowManagerInterface &operator = (const QDesignerFormWindowManagerInterface &other);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // ABSTRACTFORMWINDOWMANAGER_H
