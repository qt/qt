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

#include "scriptcommand_p.h"
#include "metadatabase_p.h"

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>

#include <QtCore/QCoreApplication>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

ScriptCommand::ScriptCommand(QDesignerFormWindowInterface *formWindow) :
    QDesignerFormWindowCommand(QCoreApplication::translate("Command", "Change script"), formWindow)
{
}

bool ScriptCommand::init(const ObjectList &list, const QString &script)
{
    MetaDataBase *metaDataBase = qobject_cast<MetaDataBase*>(formWindow()->core()->metaDataBase());
    if (!metaDataBase)
        return false;

    // Save old values
    m_oldValues.clear();
    foreach (QObject *obj, list) {
        const MetaDataBaseItem* item = metaDataBase->metaDataBaseItem(obj);
        if (!item)
            return false;
        m_oldValues.push_back(ObjectScriptPair(obj, item->script()));
    }
    m_script = script;
    return true;
}

void ScriptCommand::redo()
{
    MetaDataBase *metaDataBase = qobject_cast<MetaDataBase*>(formWindow()->core()->metaDataBase());
    Q_ASSERT(metaDataBase);

    ObjectScriptList::const_iterator cend = m_oldValues.constEnd();
    for (ObjectScriptList::const_iterator it = m_oldValues.constBegin();it != cend; ++it )  {
        if (it->first)
            metaDataBase->metaDataBaseItem(it->first)->setScript(m_script);
    }
}

void ScriptCommand::undo()
{
    MetaDataBase *metaDataBase = qobject_cast<MetaDataBase*>(formWindow()->core()->metaDataBase());
    Q_ASSERT(metaDataBase);

    ObjectScriptList::const_iterator cend = m_oldValues.constEnd();
    for (ObjectScriptList::const_iterator it = m_oldValues.constBegin();it != cend; ++it )  {
        if (it->first)
            metaDataBase->metaDataBaseItem(it->first)->setScript(it->second);
    }
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
