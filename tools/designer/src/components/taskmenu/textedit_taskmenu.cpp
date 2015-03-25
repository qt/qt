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

#include "textedit_taskmenu.h"

#include <QtDesigner/QDesignerFormWindowInterface>

#include <QtGui/QAction>
#include <QtCore/QEvent>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

TextEditTaskMenu::TextEditTaskMenu(QTextEdit *textEdit, QObject *parent) :
    QDesignerTaskMenu(textEdit, parent),
    m_format(Qt::RichText),
    m_property(QLatin1String("html")),
    m_windowTitle(tr("Edit HTML")),
    m_editTextAction(new QAction(tr("Change HTML..."), this))
{
    initialize();
}

TextEditTaskMenu::TextEditTaskMenu(QPlainTextEdit *textEdit, QObject *parent) :
    QDesignerTaskMenu(textEdit, parent),
    m_format(Qt::PlainText),
    m_property(QLatin1String("plainText")),
    m_windowTitle(tr("Edit Text")),
    m_editTextAction(new QAction(tr("Change Plain Text..."), this))
{
    initialize();
}


void TextEditTaskMenu::initialize()
{
    connect(m_editTextAction, SIGNAL(triggered()), this, SLOT(editText()));
    m_taskActions.append(m_editTextAction);

    QAction *sep = new QAction(this);
    sep->setSeparator(true);
    m_taskActions.append(sep);
}

TextEditTaskMenu::~TextEditTaskMenu()
{
}

QAction *TextEditTaskMenu::preferredEditAction() const
{
    return m_editTextAction;
}

QList<QAction*> TextEditTaskMenu::taskActions() const
{
    return m_taskActions + QDesignerTaskMenu::taskActions();
}

void TextEditTaskMenu::editText()
{
    changeTextProperty(m_property, m_windowTitle, MultiSelectionMode, m_format);
}

}
QT_END_NAMESPACE
