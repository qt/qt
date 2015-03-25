/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

#include "qxsdattribute_p.h"
#include "qxsdcomplextype_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;


void XsdAttribute::Scope::setVariety(Variety variety)
{
    m_variety = variety;
}

XsdAttribute::Scope::Variety XsdAttribute::Scope::variety() const
{
    return m_variety;
}

void XsdAttribute::Scope::setParent(const NamedSchemaComponent::Ptr &parent)
{
    m_parent = parent.data();
}

NamedSchemaComponent::Ptr XsdAttribute::Scope::parent() const
{
    return NamedSchemaComponent::Ptr(m_parent);
}

void XsdAttribute::ValueConstraint::setVariety(Variety variety)
{
    m_variety = variety;
}

XsdAttribute::ValueConstraint::Variety XsdAttribute::ValueConstraint::variety() const
{
    return m_variety;
}

void XsdAttribute::ValueConstraint::setValue(const QString &value)
{
    m_value = value;
}

QString XsdAttribute::ValueConstraint::value() const
{
    return m_value;
}

void XsdAttribute::ValueConstraint::setLexicalForm(const QString &form)
{
    m_lexicalForm = form;
}

QString XsdAttribute::ValueConstraint::lexicalForm() const
{
    return m_lexicalForm;
}

void XsdAttribute::setType(const AnySimpleType::Ptr &type)
{
    m_type = type;
}

AnySimpleType::Ptr XsdAttribute::type() const
{
    return m_type;
}

void XsdAttribute::setScope(const Scope::Ptr &scope)
{
    m_scope = scope;
}

XsdAttribute::Scope::Ptr XsdAttribute::scope() const
{
    return m_scope;
}

void XsdAttribute::setValueConstraint(const ValueConstraint::Ptr &constraint)
{
    m_valueConstraint = constraint;
}

XsdAttribute::ValueConstraint::Ptr XsdAttribute::valueConstraint() const
{
    return m_valueConstraint;
}

QT_END_NAMESPACE
