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

#include "listwidgeteditor.h"
#include <designerpropertymanager.h>
#include <abstractformbuilder.h>

#include <QtDesigner/private/abstractsettings_p.h>
#include <QtDesigner/QDesignerFormEditorInterface>

#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QDialogButtonBox>

QT_BEGIN_NAMESPACE

using namespace qdesigner_internal;

ListWidgetEditor::ListWidgetEditor(QDesignerFormWindowInterface *form,
                                   QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));

    m_itemsEditor = new ItemListEditor(form, 0);
    m_itemsEditor->layout()->setMargin(0);
    m_itemsEditor->setNewItemText(tr("New Item"));

    QFrame *sep = new QFrame;
    sep->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QBoxLayout *box = new QVBoxLayout(this);
    box->addWidget(m_itemsEditor);
    box->addWidget(sep);
    box->addWidget(buttonBox);

    // Numbers copied from itemlisteditor.ui
    // (Automatic resizing doesn't work because ui has parent).
    resize(550, 360);
}

static AbstractItemEditor::PropertyDefinition listBoxPropList[] = {
    { Qt::DisplayPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "text" },
    { Qt::DecorationPropertyRole, 0, DesignerPropertyManager::designerIconTypeId, "icon" },
    { Qt::ToolTipPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "toolTip" },
    { Qt::StatusTipPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "statusTip" },
    { Qt::WhatsThisPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "whatsThis" },
    { Qt::FontRole, QVariant::Font, 0, "font" },
    { Qt::TextAlignmentRole, 0, DesignerPropertyManager::designerAlignmentTypeId, "textAlignment" },
    { Qt::BackgroundRole, QVariant::Brush, 0, "background" },
    { Qt::ForegroundRole, QVariant::Brush, 0, "foreground" },
    { ItemFlagsShadowRole, 0, QtVariantPropertyManager::flagTypeId, "flags" },
    { Qt::CheckStateRole, 0, QtVariantPropertyManager::enumTypeId, "checkState" },
    { 0, 0, 0, 0 }
};

ListContents ListWidgetEditor::fillContentsFromListWidget(QListWidget *listWidget)
{
    setWindowTitle(tr("Edit List Widget"));

    ListContents retVal;
    retVal.createFromListWidget(listWidget, false);
    retVal.applyToListWidget(m_itemsEditor->listWidget(), m_itemsEditor->iconCache(), true);

    m_itemsEditor->setupEditor(listWidget, listBoxPropList);

    return retVal;
}

static AbstractItemEditor::PropertyDefinition comboBoxPropList[] = {
    { Qt::DisplayPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "text" },
    { Qt::DecorationPropertyRole, 0, DesignerPropertyManager::designerIconTypeId, "icon" },
    { 0, 0, 0, 0 }
};

ListContents ListWidgetEditor::fillContentsFromComboBox(QComboBox *comboBox)
{
    setWindowTitle(tr("Edit Combobox"));

    ListContents retVal;
    retVal.createFromComboBox(comboBox);
    retVal.applyToListWidget(m_itemsEditor->listWidget(), m_itemsEditor->iconCache(), true);

    m_itemsEditor->setupEditor(comboBox, comboBoxPropList);

    return retVal;
}

ListContents ListWidgetEditor::contents() const
{
    ListContents retVal;
    retVal.createFromListWidget(m_itemsEditor->listWidget(), true);
    return retVal;
}

QT_END_NAMESPACE
