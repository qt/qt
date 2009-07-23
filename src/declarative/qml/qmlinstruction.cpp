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

#include "qmlinstruction_p.h"
#include "qmlcompiler_p.h"
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

void QmlCompiledData::dump(QmlInstruction *instr, int idx)
{
    QByteArray lineNumber = QByteArray::number(instr->line);
    if (instr->line == (unsigned short)-1)
        lineNumber = "NA";
    const char *line = lineNumber.constData();

    switch(instr->type) {
    case QmlInstruction::Init:
        qWarning() << idx << "\t" << line << "\t" << "INIT";
        break;
    case QmlInstruction::CreateObject:
        qWarning() << idx << "\t" << line << "\t" << "CREATE\t\t\t" << instr->create.type << "\t\t\t" << types.at(instr->create.type).className;
        break;
    case QmlInstruction::SetId:
        qWarning() << idx << "\t" << line << "\t" << "SETID\t\t\t" << instr->setId.value << "\t\t\t\t" << primitives.at(instr->setId.value);
        break;
    case QmlInstruction::SetDefault:
        qWarning() << idx << "\t" << line << "\t" << "SET_DEFAULT";
        break;
    case QmlInstruction::CreateComponent:
        qWarning() << idx << "\t" << line << "\t" << "CREATE_COMPONENT\t" << instr->createComponent.count;
        break;
    case QmlInstruction::StoreMetaObject:
        qWarning() << idx << "\t" << line << "\t" << "STORE_META\t\t" << instr->storeMeta.data << "\t" << instr->storeMeta.slotData;
        break;
    case QmlInstruction::StoreFloat:
        qWarning() << idx << "\t" << line << "\t" << "STORE_FLOAT\t\t" << instr->storeFloat.propertyIndex << "\t" << instr->storeFloat.value;
        break;
    case QmlInstruction::StoreDouble:
        qWarning() << idx << "\t" << line << "\t" << "STORE_DOUBLE\t\t" << instr->storeDouble.propertyIndex << "\t" << instr->storeDouble.value;
        break;
    case QmlInstruction::StoreInteger:
        qWarning() << idx << "\t" << line << "\t" << "STORE_INTEGER\t\t" << instr->storeInteger.propertyIndex << "\t" << instr->storeInteger.value;
        break;
    case QmlInstruction::StoreBool:
        qWarning() << idx << "\t" << line << "\t" << "STORE_BOOL\t\t" << instr->storeBool.propertyIndex << "\t" << instr->storeBool.value;
        break;
    case QmlInstruction::StoreString:
        qWarning() << idx << "\t" << line << "\t" << "STORE_STRING\t\t" << instr->storeString.propertyIndex << "\t" << instr->storeString.value << "\t\t" << primitives.at(instr->storeString.value);
        break;
    case QmlInstruction::StoreUrl:
        qWarning() << idx << "\t" << line << "\t" << "STORE_URL\t\t" << instr->storeUrl.propertyIndex << "\t" << instr->storeUrl.value << "\t\t" << primitives.at(instr->storeUrl.value);
        break;
    case QmlInstruction::StoreColor:
        qWarning() << idx << "\t" << line << "\t" << "STORE_COLOR\t\t" << instr->storeColor.propertyIndex << "\t" << QString::number(instr->storeColor.value, 16);
        break;
    case QmlInstruction::StoreDate:
        qWarning() << idx << "\t" << line << "\t" << "STORE_DATE\t\t" << instr->storeDate.propertyIndex << "\t" << instr->storeDate.value;
        break;
    case QmlInstruction::StoreTime:
        qWarning() << idx << "\t" << line << "\t" << "STORE_TIME\t\t" << instr->storeTime.propertyIndex << "\t" << instr->storeTime.valueIndex;
        break;
    case QmlInstruction::StoreDateTime:
        qWarning() << idx << "\t" << line << "\t" << "STORE_DATETIME\t\t" << instr->storeDateTime.propertyIndex << "\t" << instr->storeDateTime.valueIndex;
        break;
    case QmlInstruction::StorePoint:
        qWarning() << idx << "\t" << line << "\t" << "STORE_POINT\t\t" << instr->storeRealPair.propertyIndex << "\t" << instr->storeRealPair.valueIndex;
        break;
    case QmlInstruction::StorePointF:
        qWarning() << idx << "\t" << line << "\t" << "STORE_POINTF\t\t" << instr->storeRealPair.propertyIndex << "\t" << instr->storeRealPair.valueIndex;
        break;
    case QmlInstruction::StoreSize:
        qWarning() << idx << "\t" << line << "\t" << "STORE_SIZE\t\t" << instr->storeRealPair.propertyIndex << "\t" << instr->storeRealPair.valueIndex;
        break;
    case QmlInstruction::StoreSizeF:
        qWarning() << idx << "\t" << line << "\t" << "STORE_SIZEF\t\t" << instr->storeRealPair.propertyIndex << "\t" << instr->storeRealPair.valueIndex;
        break;
    case QmlInstruction::StoreRect:
        qWarning() << idx << "\t" << line << "\t" << "STORE_RECT\t\t" << instr->storeRect.propertyIndex << "\t" << instr->storeRect.valueIndex;
        break;
    case QmlInstruction::StoreRectF:
        qWarning() << idx << "\t" << line << "\t" << "STORE_RECTF\t\t" << instr->storeRect.propertyIndex << "\t" << instr->storeRect.valueIndex;
        break;
    case QmlInstruction::StoreVariant:
        qWarning() << idx << "\t" << line << "\t" << "STORE_VARIANT\t\t" << instr->storeString.propertyIndex << "\t" << instr->storeString.value << "\t\t" << primitives.at(instr->storeString.value);
        break;
    case QmlInstruction::StoreObject:
        qWarning() << idx << "\t" << line << "\t" << "STORE_OBJECT\t\t" << instr->storeObject.propertyIndex;
        break;
    case QmlInstruction::StoreVariantObject:
        qWarning() << idx << "\t" << line << "\t" << "STORE_VARIANT_OBJECT\t" << instr->storeObject.propertyIndex;
        break;
    case QmlInstruction::StoreInterface:
        qWarning() << idx << "\t" << line << "\t" << "STORE_INTERFACE\t\t" << instr->storeObject.propertyIndex;
        break;
    case QmlInstruction::StoreSignal:
        qWarning() << idx << "\t" << line << "\t" << "STORE_SIGNAL\t\t" << instr->storeSignal.signalIndex << "\t" << instr->storeSignal.value << "\t\t" << primitives.at(instr->storeSignal.value);
        break;
    case QmlInstruction::AssignSignalObject:
        qWarning() << idx << "\t" << line << "\t" << "ASSIGN_SIGNAL_OBJECT\t" << instr->assignSignalObject.signal << "\t\t\t" << datas.at(instr->assignSignalObject.signal);
        break;
    case QmlInstruction::AssignCustomType:
        qWarning() << idx << "\t" << line << "\t" << "ASSIGN_CUSTOMTYPE\t\t" << instr->assignCustomType.propertyIndex << "\t" << instr->assignCustomType.valueIndex;
        break;
    case QmlInstruction::StoreBinding:
        qWarning() << idx << "\t" << line << "\t" << "STORE_BINDING\t\t" << instr->assignBinding.property << "\t" << instr->assignBinding.value << "\t\t" << instr->assignBinding.context << primitives.at(instr->assignBinding.value);
        break;
    case QmlInstruction::StoreCompiledBinding:
        qWarning() << idx << "\t" << line << "\t" << "STORE_COMPILED_BINDING\t" << instr->assignBinding.property << "\t" << instr->assignBinding.value << "\t\t" << instr->assignBinding.context;
        break;
    case QmlInstruction::StoreValueSource:
        qWarning() << idx << "\t" << line << "\t" << "STORE_VALUE_SOURCE\t" << instr->assignValueSource.property;
        break;
    case QmlInstruction::BeginObject:
        qWarning() << idx << "\t" << line << "\t" << "BEGIN\t\t\t" << instr->begin.castValue;
        break;
    case QmlInstruction::CompleteObject:
        qWarning() << idx << "\t" << line << "\t" << "COMPLETE\t\t" << instr->complete.castValue;
        break;
    case QmlInstruction::StoreObjectQmlList:
        qWarning() << idx << "\t" << line << "\t" << "STORE_OBJECT_QMLLIST";
        break;
    case QmlInstruction::StoreObjectQList:
        qWarning() << idx << "\t" << line << "\t" << "STORE_OBJECT_QLIST";
        break;
    case QmlInstruction::AssignObjectList:
        qWarning() << idx << "\t" << line << "\t" << "ASSIGN_OBJECT_LIST\t";
        break;
    case QmlInstruction::FetchAttached:
        qWarning() << idx << "\t" << line << "\t" << "FETCH_ATTACHED\t\t" << instr->fetchAttached.id;
        break;
    case QmlInstruction::FetchQmlList:
        qWarning() << idx << "\t" << line << "\t" << "FETCH_QMLLIST\t\t" << instr->fetchQmlList.property << "\t" << instr->fetchQmlList.type;
        break;
    case QmlInstruction::FetchQList:
        qWarning() << idx << "\t" << line << "\t" << "FETCH_QLIST\t\t" << instr->fetch.property;
        break;
    case QmlInstruction::FetchObject:
        qWarning() << idx << "\t" << line << "\t" << "FETCH\t\t\t" << instr->fetch.property;
        break;
    case QmlInstruction::FetchValueType:
        qWarning() << idx << "\t" << line << "\t" << "FETCH_VALUE\t\t" << instr->fetchValue.property << "\t" << instr->fetchValue.type;
        break;
    case QmlInstruction::PopFetchedObject:
        qWarning() << idx << "\t" << line << "\t" << "POP";
        break;
    case QmlInstruction::PopQList:
        qWarning() << idx << "\t" << line << "\t" << "POP_QLIST";
        break;
    case QmlInstruction::PopValueType:
        qWarning() << idx << "\t" << line << "\t" << "POP_VALUE\t\t" << instr->fetchValue.property << "\t" << instr->fetchValue.type;
        break;
    case QmlInstruction::Defer:
        qWarning() << idx << "\t" << line << "\t" << "DEFER" << "\t\t" << instr->defer.deferCount;
        break;
    default:
        qWarning() << idx << "\t" << line << "\t" << "XXX UNKOWN INSTRUCTION" << "\t" << instr->type;
        break;
    }
}

QT_END_NAMESPACE
