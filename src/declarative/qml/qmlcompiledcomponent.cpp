/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qmlcompiledcomponent_p.h"
#include "qmlparser_p.h"
#include <QtCore/qdebug.h>
#include <QmlComponent>
using namespace QmlParser;

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(compilerDump, QML_COMPILER_DUMP);

QmlCompiledComponent::QmlCompiledComponent()
: dumpStatus(NoDump)
{
}

QmlCompiledComponent::~QmlCompiledComponent()
{
    for (int ii = 0; ii < mos.count(); ++ii)
        qFree(mos.at(ii));
}


void QmlCompiledComponent::dumpInstructions()
{
    if (!compilerDump())
        return;

    if (!name.isEmpty())
        qWarning() << name;
    qWarning() << "Index\tLine\tOperation\t\tData1\tData2\t\tComments";
    qWarning() << "-------------------------------------------------------------------------------";
    for (int ii = 0; ii < bytecode.count(); ++ii) {
        dump(&bytecode[ii], ii);
    }
    qWarning() << "-------------------------------------------------------------------------------";
}

void QmlCompiledComponent::dump(int indent, Property *p)
{
    QByteArray ba(indent * 4, ' ');
    for (int ii = 0; ii < p->values.count(); ++ii)
        dump(indent, p->values.at(ii));
    if (p->value)
        dump(indent, p->value);
}

void QmlCompiledComponent::dump(int indent, Object *o)
{
    QByteArray ba(indent * 4, ' ');
    if (o->type != -1) {
        qWarning() << ba.constData() << "Object:" << types.at(o->type).className;
    } else {
        qWarning() << ba.constData() << "Object: fetched";
    }

    for (QHash<QByteArray, Property *>::ConstIterator iter = o->properties.begin();
            iter != o->properties.end();
            ++iter) {
        qWarning() << ba.constData() << " Property" << iter.key();
        dump(indent + 1, *iter);
    }

    if (o->defaultProperty) {
        qWarning() << ba.constData() << " Default property";
        dump(indent + 1, o->defaultProperty);
    }
}

void QmlCompiledComponent::dump(int indent, Value *v)
{
    QByteArray type;
    switch(v->type) {
    default:
    case Value::Unknown:
        type = "Unknown";
        break;
    case Value::Literal:
        type = "Literal";
        break;
    case Value::PropertyBinding:
        type = "PropertyBinding";
        break;
    case Value::ValueSource:
        type = "ValueSource";
        break;
    case Value::CreatedObject:
        type = "CreatedObject";
        break;
    case Value::SignalObject:
        type = "SignalObject";
        break;
    case Value::SignalExpression:
        type = "SignalExpression";
        break;
    case Value::Component:
        type = "Component";
        break;
    case Value::Id:
        type = "Id";
        break;
    };

    QByteArray ba(indent * 4, ' ');
    if (v->object) {
        qWarning() << ba.constData() << "Value (" << type << "):";
        dump(indent + 1, v->object);
    } else {
        qWarning() << ba.constData() << "Value (" << type << "):" << v->primitive;
    }
}

void QmlCompiledComponent::dumpPre()
{
    if (!(dumpStatus & DumpPre)) {
        dumpInstructions();
        dumpStatus = (DumpStatus)(dumpStatus | DumpPre);
    }
}

void QmlCompiledComponent::dumpPost()
{
    if (!(dumpStatus & DumpPost)) {
        dumpInstructions();
        dumpStatus = (DumpStatus)(dumpStatus | DumpPost);
    }

}

QT_END_NAMESPACE
