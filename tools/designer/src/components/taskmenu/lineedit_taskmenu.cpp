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

#include "lineedit_taskmenu.h"
#include "inplace_editor.h"

#include <QtDesigner/QDesignerFormWindowInterface>

#include <QtGui/QAction>
#include <QtGui/QStyle>
#include <QtGui/QStyleOption>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

// -------- LineEditTaskMenuInlineEditor
class LineEditTaskMenuInlineEditor : public  TaskMenuInlineEditor
{
public:
    LineEditTaskMenuInlineEditor(QLineEdit *button, QObject *parent);

protected:
    virtual QRect editRectangle() const;
};

LineEditTaskMenuInlineEditor::LineEditTaskMenuInlineEditor(QLineEdit *w, QObject *parent) :
      TaskMenuInlineEditor(w, ValidationSingleLine, QLatin1String("text"), parent)
{
}

QRect LineEditTaskMenuInlineEditor::editRectangle() const
{
    QStyleOption opt;
    opt.init(widget());
    return opt.rect;
}

// --------------- LineEditTaskMenu
LineEditTaskMenu::LineEditTaskMenu(QLineEdit *lineEdit, QObject *parent) :
    QDesignerTaskMenu(lineEdit, parent),
    m_editTextAction(new QAction(tr("Change text..."), this))
{
    TaskMenuInlineEditor *editor = new LineEditTaskMenuInlineEditor(lineEdit, this);
    connect(m_editTextAction, SIGNAL(triggered()), editor, SLOT(editText()));
    m_taskActions.append(m_editTextAction);

    QAction *sep = new QAction(this);
    sep->setSeparator(true);
    m_taskActions.append(sep);
}

QAction *LineEditTaskMenu::preferredEditAction() const
{
    return m_editTextAction;
}

QList<QAction*> LineEditTaskMenu::taskActions() const
{
    return m_taskActions + QDesignerTaskMenu::taskActions();
}

}

QT_END_NAMESPACE
