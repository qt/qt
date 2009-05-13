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

#include "private/qmlcompiler_p.h"
#include <qfxperf.h>
#include "qmlparser_p.h"
#include "private/qmlscriptparser_p.h"
#include <qmlpropertyvaluesource.h>
#include <qmlcomponent.h>
#include "private/qmetaobjectbuilder_p.h"
#include <qmlbasicscript.h>
#include <QColor>
#include <QDebug>
#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <private/qmlcompiledcomponent_p.h>
#include <private/qmlstringconverters_p.h>
#include <private/qmlengine_p.h>
#include <qmlengine.h>
#include <qmlcontext.h>
#include <qmlmetatype.h>
#include <QtCore/qdebug.h>
#include "private/qmlcustomparser_p_p.h"
#include <private/qmlcontext_p.h>
#include <private/qmlcomponent_p.h>

#include "qmlscriptparser_p.h"

QT_BEGIN_NAMESPACE

using namespace QmlParser;

int QmlCompiledData::indexForString(const QString &data)
{
    int idx = primitives.indexOf(data);
    if (idx == -1) {
        idx = primitives.count();
        primitives << data;
    }
    return idx;
}

int QmlCompiledData::indexForByteArray(const QByteArray &data)
{
    int idx = datas.indexOf(data);
    if (idx == -1) {
        idx = datas.count();
        datas << data;
    }
    return idx;
}

int QmlCompiledData::indexForFloat(float *data, int count)
{
    Q_ASSERT(count > 0);

    for (int ii = 0; ii <= floatData.count() - count; ++ii) {
        bool found = true;
        for (int jj = 0; jj < count; ++jj) {
            if (floatData.at(ii + jj) != data[jj]) {
                found = false;
                break;
            }
        }

        if (found)
            return ii;
    }

    int idx = floatData.count();
    for (int ii = 0; ii < count; ++ii)
        floatData << data[ii];

    return idx;
}

int QmlCompiledData::indexForInt(int *data, int count)
{
    Q_ASSERT(count > 0);

    for (int ii = 0; ii <= intData.count() - count; ++ii) {
        bool found = true;
        for (int jj = 0; jj < count; ++jj) {
            if (intData.at(ii + jj) != data[jj]) {
                found = false;
                break;
            }
        }

        if (found)
            return ii;
    }

    int idx = intData.count();
    for (int ii = 0; ii < count; ++ii)
        intData << data[ii];

    return idx;
}

int QmlCompiledData::indexForLocation(const QmlParser::Location &l)
{
    // ### FIXME
    int rv = locations.count();
    locations << l;
    return rv;
}

int QmlCompiledData::indexForLocation(const QmlParser::LocationSpan &l)
{
    // ### FIXME
    int rv = locations.count();
    locations << l.start << l.end;
    return rv;
}

QmlCompiler::QmlCompiler()
: exceptionLine(-1), exceptionColumn(-1), output(0)
{
}

bool QmlCompiler::isError() const
{
    return exceptionLine != -1;
}

QList<QmlError> QmlCompiler::errors() const
{
    QList<QmlError> rv;

    if(isError()) {
        QmlError error;
        error.setDescription(exceptionDescription);
        error.setLine(exceptionLine);
        error.setColumn(exceptionColumn);
        rv << error;
    }

    return rv;
}

bool QmlCompiler::isValidId(const QString &val)
{
    if (val.isEmpty())
        return false;

    QChar u(QLatin1Char('_'));
    for (int ii = 0; ii < val.count(); ++ii) 
        if (val.at(ii) != u &&
           ((ii == 0 && !val.at(ii).isLetter()) ||
           (ii != 0 && !val.at(ii).isLetterOrNumber())) )
            return false;

    return true;
}

/*!
    Returns true if property name \a name refers to an attached property, false
    otherwise.

    Attached property names are those that start with a capital letter.
*/
bool QmlCompiler::isAttachedProperty(const QByteArray &name)
{
    return !name.isEmpty() && name.at(0) >= 'A' && name.at(0) <= 'Z';
}

QmlCompiler::StoreInstructionResult 
QmlCompiler::generateStoreInstruction(QmlCompiledData &cdata,
                                      QmlInstruction &instr, 
                                      const QMetaProperty &prop, 
                                      int coreIdx, int primitive,
                                      const QString *string)
{
    if (!prop.isWritable())
        return ReadOnly;
    if (prop.isEnumType()) {
        int value;
        if (prop.isFlagType()) {
            value = prop.enumerator().keysToValue(string->toLatin1().constData());
        } else
            value = prop.enumerator().keyToValue(string->toLatin1().constData());
        if (value == -1)
            return InvalidData;
        instr.type = QmlInstruction::StoreInteger;
        instr.storeInteger.propertyIndex = coreIdx;
        instr.storeInteger.value = value;
        return Ok;
    }
    int type = prop.type();
    switch(type) {
        case -1:
            instr.type = QmlInstruction::StoreVariant;
            instr.storeString.propertyIndex = coreIdx;
            if (primitive == -1)
                primitive = cdata.indexForString(*string);
            instr.storeString.value = primitive;
            break;
            break;
        case QVariant::String:
            {
            instr.type = QmlInstruction::StoreString;
            instr.storeString.propertyIndex = coreIdx;
            if (primitive == -1)
                primitive = cdata.indexForString(*string);
            instr.storeString.value = primitive;
            }
            break;
        case QVariant::UInt:
            {
            instr.type = QmlInstruction::StoreInteger;
            instr.storeInteger.propertyIndex = coreIdx;
            bool ok;
            int value = string->toUInt(&ok);
            if (!ok)
                return InvalidData;
            instr.storeInteger.value = value;
            }
            break;
        case QVariant::Int:
            {
            instr.type = QmlInstruction::StoreInteger;
            instr.storeInteger.propertyIndex = coreIdx;
            bool ok;
            int value = string->toInt(&ok);
            if (!ok)
                return InvalidData;
            instr.storeInteger.value = value;
            }
            break;
        case 135:
        case QVariant::Double:
            {
            instr.type = QmlInstruction::StoreReal;
            instr.storeReal.propertyIndex = coreIdx;
            bool ok;
            float value = string->toFloat(&ok);
            if (!ok)
                return InvalidData;
            instr.storeReal.value = value;
            }
            break;
        case QVariant::Color:
            {
            QColor c = QmlStringConverters::colorFromString(*string);
            if (!c.isValid())
                return InvalidData;
            instr.type = QmlInstruction::StoreColor;
            instr.storeColor.propertyIndex = coreIdx;
            instr.storeColor.value = c.rgba();
            }
            break;
        case QVariant::Date:
            {
            QDate d = QDate::fromString(*string, Qt::ISODate);
            if (!d.isValid())
                return InvalidData;
            instr.type = QmlInstruction::StoreDate;
            instr.storeDate.propertyIndex = coreIdx;
            instr.storeDate.value = d.toJulianDay();
            }
            break;
        case QVariant::Time:
            {
            QTime time = QTime::fromString(*string, Qt::ISODate);
            if (!time.isValid())
                return InvalidData;
            int data[] = { time.hour(), time.minute(), time.second(), time.msec() };
            int index = cdata.indexForInt(data, 4);
            instr.type = QmlInstruction::StoreTime;
            instr.storeTime.propertyIndex = coreIdx;
            instr.storeTime.valueIndex = index;
            }
            break;
        case QVariant::DateTime:
            {
            QDateTime dateTime = QDateTime::fromString(*string, Qt::ISODate);
            if (!dateTime.isValid())
                return InvalidData;
            int data[] = { dateTime.date().toJulianDay(),
                           dateTime.time().hour(),
                           dateTime.time().minute(),
                           dateTime.time().second(),
                           dateTime.time().msec() };
            int index = cdata.indexForInt(data, 5);
            instr.type = QmlInstruction::StoreDateTime;
            instr.storeDateTime.propertyIndex = coreIdx;
            instr.storeDateTime.valueIndex = index;
            }
            break;
        case QVariant::Point:
        case QVariant::PointF:
            {
                bool ok;
                QPointF point = QmlStringConverters::pointFFromString(*string, &ok);
                if (!ok)
                    return InvalidData;
                float data[] = { point.x(), point.y() };
                int index = cdata.indexForFloat(data, 2);
                if (type == QVariant::PointF)
                    instr.type = QmlInstruction::StorePointF;
                else
                    instr.type = QmlInstruction::StorePoint;
                instr.storeRealPair.propertyIndex = coreIdx;
                instr.storeRealPair.valueIndex = index;
            }
            break;
        case QVariant::Size:
        case QVariant::SizeF:
            {
                bool ok;
                QSizeF size = QmlStringConverters::sizeFFromString(*string, &ok);
                if (!ok)
                    return InvalidData;
                float data[] = { size.width(), size.height() };
                int index = cdata.indexForFloat(data, 2);
                if (type == QVariant::SizeF)
                    instr.type = QmlInstruction::StoreSizeF;
                else
                    instr.type = QmlInstruction::StoreSize;
                instr.storeRealPair.propertyIndex = coreIdx;
                instr.storeRealPair.valueIndex = index;
            }
            break;
        case QVariant::Rect:
        case QVariant::RectF:
            {
                bool ok;
                QRectF rect = QmlStringConverters::rectFFromString(*string, &ok);
                if (!ok)
                    return InvalidData;
                float data[] = { rect.x(), rect.y(), 
                                 rect.width(), rect.height() };
                int index = cdata.indexForFloat(data, 4);
                if (type == QVariant::RectF)
                    instr.type = QmlInstruction::StoreRectF;
                else
                    instr.type = QmlInstruction::StoreRect;
                instr.storeRect.propertyIndex = coreIdx;
                instr.storeRect.valueIndex = index;
            }
            break;
        case QVariant::Bool:
            {
                bool ok;
                bool b = QmlStringConverters::boolFromString(*string, &ok);
                if (!ok)
                    return InvalidData;
                instr.type = QmlInstruction::StoreBool;
                instr.storeBool.propertyIndex = coreIdx;
                instr.storeBool.value = b;
            }
            break;
        default:
            {
                int t = prop.type();
                if (t == QVariant::UserType)
                    t = prop.userType();
                QmlMetaType::StringConverter converter = 
                    QmlMetaType::customStringConverter(t);
                if (converter) {
                    int index = cdata.customTypeData.count();
                    instr.type = QmlInstruction::AssignCustomType;
                    instr.assignCustomType.propertyIndex = coreIdx;
                    instr.assignCustomType.valueIndex = index;

                    QmlCompiledData::CustomTypeData data;
                    if (primitive == -1)
                        primitive = cdata.indexForString(*string);
                    data.index = primitive;
                    data.type = t;
                    cdata.customTypeData << data;
                    break;
                }
            }
            return UnknownType;
            break;
    }
    return Ok;
}

void QmlCompiler::reset(QmlCompiledComponent *cc, bool deleteMemory)
{
    cc->types.clear();
    cc->primitives.clear();
    cc->floatData.clear();
    cc->intData.clear();
    cc->customTypeData.clear();
    cc->datas.clear();
    if (deleteMemory) {
        for (int ii = 0; ii < cc->synthesizedMetaObjects.count(); ++ii)
            qFree(cc->synthesizedMetaObjects.at(ii));
    }
    cc->synthesizedMetaObjects.clear();
    cc->bytecode.clear();
}

#define COMPILE_EXCEPTION2(token, desc) \
    {  \
        exceptionLine = token->location.start.line;  \
        exceptionColumn = token->location.start.column;  \
        QDebug d(&exceptionDescription); \
        d << desc;  \
        exceptionDescription = exceptionDescription.trimmed(); \
        return false; \
    } 

#define COMPILE_EXCEPTION(desc) \
    {  \
        exceptionLine = obj->location.start.line;  \
        exceptionColumn = obj->location.start.column;  \
        QDebug d(&exceptionDescription); \
        d << desc;  \
        exceptionDescription = exceptionDescription.trimmed(); \
        return false; \
    } 

#define COMPILE_CHECK(a) \
    { \
        if (!a) return false; \
    }

bool QmlCompiler::compile(QmlEngine *engine, 
                          QmlCompositeTypeData *unit,
                          QmlCompiledComponent *out)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::Compile> pc;
#endif
    exceptionLine = -1;

    Q_ASSERT(out);
    reset(out, true);

    output = out;

    // Compile types
    for (int ii = 0; ii < unit->types.count(); ++ii) {
        QmlCompositeTypeData::TypeReference &tref = unit->types[ii];
        QmlCompiledComponent::TypeReference ref;
        if (tref.type)
            ref.type = tref.type;
        else if (tref.unit) {
            ref.component = tref.unit->toComponent(engine);
            ref.ref = tref.unit;
            ref.ref->addref();
        } 
        ref.className = unit->data.types().at(ii).toLatin1();
        out->types << ref;
    }

    Object *root = unit->data.tree();
    Q_ASSERT(root);

    compileTree(root);

    if (!isError()) {
        out->dumpPre();
    } else {
        reset(out, true);
    }

    output = 0;
    return !isError();
}

void QmlCompiler::compileTree(Object *tree)
{
    QmlInstruction init;
    init.type = QmlInstruction::Init;
    init.line = 0;
    init.init.dataSize = 0;
    init.init.bindingsSize = 0;
    init.init.parserStatusSize = 0;
    output->bytecode << init;

    if (!compileObject(tree, 0)) // Compile failed
        return;

    if (tree->metatype)
        static_cast<QMetaObject &>(output->root) = *tree->metaObject();
    else
        static_cast<QMetaObject &>(output->root) = *output->types.at(tree->type).metaObject();

    QmlInstruction def;
    init.line = 0;
    def.type = QmlInstruction::SetDefault;
    output->bytecode << def;

    optimizeExpressions(0, output->bytecode.count() - 1, 0);
}

bool QmlCompiler::compileObject(Object *obj, int ctxt)
{    
    if (obj->type != -1) 
        obj->metatype = output->types.at(obj->type).metaObject();

    if (output->types.at(obj->type).className == "Component") {
        COMPILE_CHECK(compileComponent(obj, ctxt));
        return true;
    }

    ctxt = 0;

    int createInstrIdx = output->bytecode.count();
    // Create the object
    QmlInstruction create;
    create.type = QmlInstruction::CreateObject;
    create.line = obj->location.start.line;
    create.create.data = -1;
    create.create.type = obj->type;
    output->bytecode << create;

    COMPILE_CHECK(compileDynamicMeta(obj));

    if (obj->type != -1) {
        if (output->types.at(obj->type).component) {
            QmlInstruction begin;
            begin.type = QmlInstruction::TryBeginObject;
            begin.line = obj->location.start.line;
            output->bytecode << begin;
        } else {
            int cast = QmlMetaType::qmlParserStatusCast(QmlMetaType::type(output->types.at(obj->type).className));
            if (cast != -1) {
                QmlInstruction begin;
                begin.type = QmlInstruction::BeginObject;
                begin.begin.castValue = cast;
                begin.line = obj->location.start.line;
                output->bytecode << begin;
            }
        }
    } 

    bool isCustomParser = output->types.at(obj->type).type &&  
                          output->types.at(obj->type).type->customParser() != 0;
    QList<QmlCustomParserProperty> customProps;

    foreach(Property *prop, obj->properties) {
        if (prop->name.length() >= 3 && prop->name.startsWith("on") && 
           ('A' <= prop->name.at(2) && 'Z' >= prop->name.at(2))) {
            if (!isCustomParser) {
                COMPILE_CHECK(compileSignal(prop, obj));
            } else {
                customProps << QmlCustomParserNodePrivate::fromProperty(prop);
            }
        } else {
            if (!isCustomParser || (isCustomParser && testProperty(prop, obj))) {
                COMPILE_CHECK(compileProperty(prop, obj, ctxt));
            } else {
                customProps << QmlCustomParserNodePrivate::fromProperty(prop);
            }
        }
    }

    if (obj->defaultProperty)  {
        if(!isCustomParser || (isCustomParser && testProperty(obj->defaultProperty, obj))) {
            COMPILE_CHECK(compileProperty(obj->defaultProperty, obj, ctxt));
        } else {
            customProps << QmlCustomParserNodePrivate::fromProperty(obj->defaultProperty);
        }
    }

    if (isCustomParser && !customProps.isEmpty()) {
        // ### Check for failure
        bool ok = false;
        QmlCustomParser *cp = output->types.at(obj->type).type->customParser();
        QByteArray customData = cp->compile(customProps, &ok);
        if(!ok)
            COMPILE_EXCEPTION("Failure compiling custom type");
        if(!customData.isEmpty())
            output->bytecode[createInstrIdx].create.data = 
                output->indexForByteArray(customData);
    }

    if (obj->type != -1) {
        if (output->types.at(obj->type).component) {
            QmlInstruction complete;
            complete.type = QmlInstruction::TryCompleteObject;
            complete.line = obj->location.start.line;
            output->bytecode << complete;
        } else {
            int cast = QmlMetaType::qmlParserStatusCast(QmlMetaType::type(output->types.at(obj->type).className));
            if (cast != -1) {
                QmlInstruction complete;
                complete.type = QmlInstruction::CompleteObject;
                complete.complete.castValue = cast;
                complete.line = obj->location.start.line;
                output->bytecode << complete;
            }
        }
    } 

    return true;
}

bool QmlCompiler::compileComponent(Object *obj, int ctxt)
{
    Property *idProp = 0;
    if (obj->properties.count() > 1 ||
       (obj->properties.count() == 1 && obj->properties.begin().key() != "id"))
        COMPILE_EXCEPTION("Invalid component specification");
    if (obj->defaultProperty && 
       (obj->defaultProperty->value || obj->defaultProperty->values.count() > 1 ||
        (obj->defaultProperty->values.count() == 1 && !obj->defaultProperty->values.first()->object)))
        COMPILE_EXCEPTION("Invalid component body specification");
    if (obj->properties.count()) 
        idProp = *obj->properties.begin();
    if (idProp && (idProp->value || idProp->values.count() > 1))
        COMPILE_EXCEPTION("Invalid component id specification");

    Object *root = 0;
    if (obj->defaultProperty && obj->defaultProperty->values.count()) 
        root = obj->defaultProperty->values.first()->object;
    
    if (!root)
        COMPILE_EXCEPTION("Cannot create empty component specification");

    COMPILE_CHECK(compileComponentFromRoot(root, ctxt));

    if (idProp && idProp->values.count()) {
        QString val = idProp->values.at(0)->primitive();
        if (!isValidId(val))
            COMPILE_EXCEPTION("Invalid id property value");

        if (ids.contains(val))
            COMPILE_EXCEPTION("id is not unique");
        ids.insert(val);

        int pref = output->indexForString(val);
        QmlInstruction id;
        id.type = QmlInstruction::SetId;
        id.line = idProp->location.start.line;
        id.setId.value = pref;
        id.setId.save = -1;
        output->bytecode << id;
    }

    return true;
}

bool QmlCompiler::compileComponentFromRoot(Object *obj, int ctxt)
{
    output->bytecode.push_back(QmlInstruction());
    QmlInstruction &create = output->bytecode.last();
    create.type = QmlInstruction::CreateComponent;
    create.line = obj->location.start.line;
    create.createComponent.endLine = obj->location.end.line;
    int count = output->bytecode.count();

    QmlInstruction init;
    init.type = QmlInstruction::Init;
    init.init.dataSize = 0;
    init.init.bindingsSize = 0;
    init.init.parserStatusSize = 0;
    init.line = obj->location.start.line;
    output->bytecode << init;

    QSet<QString> oldIds = ids;
    ids.clear();
    if (obj) 
        COMPILE_CHECK(compileObject(obj, ctxt));
    ids = oldIds;

    create.createComponent.count = output->bytecode.count() - count;

    int inc = optimizeExpressions(count, count - 1 + create.createComponent.count, count);
    create.createComponent.count += inc;
    return true;
}


bool QmlCompiler::compileFetchedObject(Object *obj, int ctxt)
{
    if (obj->defaultProperty) 
        COMPILE_CHECK(compileProperty(obj->defaultProperty, obj, ctxt));

    foreach(Property *prop, obj->properties) {
        if (prop->name.length() >= 3 && prop->name.startsWith("on") && 
           ('A' <= prop->name.at(2) && 'Z' >= prop->name.at(2))) {
            COMPILE_CHECK(compileSignal(prop, obj));
        } else {
            COMPILE_CHECK(compileProperty(prop, obj, ctxt));
        }
    }

    return true;
}

bool QmlCompiler::compileSignal(Property *prop, Object *obj)
{
    if (prop->values.isEmpty() && !prop->value)
        return true;

    if (prop->value || prop->values.count() > 1)
        COMPILE_EXCEPTION("Incorrectly specified signal");

    if (prop->values.at(0)->object) {
        int pr = output->indexForByteArray(prop->name);

        bool rv = compileObject(prop->values.at(0)->object, 0);

        if (rv) {
            QmlInstruction assign;
            assign.type = QmlInstruction::AssignSignalObject;
            assign.line = prop->values.at(0)->location.start.line;
            assign.assignSignalObject.signal = pr;

            output->bytecode << assign;

            prop->values.at(0)->type = Value::SignalObject;
        }

        return rv;

    } else {
        QString script = prop->values.at(0)->value.asScript().trimmed();
        if (script.isEmpty())
            return true;

        int idx = output->indexForString(script); 
        int pr = output->indexForByteArray(prop->name);

        QmlInstruction assign;
        assign.type = QmlInstruction::AssignSignal;
        assign.line = prop->values.at(0)->location.start.line;
        assign.assignSignal.signal = pr;
        assign.assignSignal.value = idx;

        output->bytecode << assign;

        prop->values.at(0)->type = Value::SignalExpression;
    }

    return true;
}

// Returns true if prop exists on obj, false otherwise
bool QmlCompiler::testProperty(QmlParser::Property *prop, 
                               QmlParser::Object *obj)
{
    if(isAttachedProperty(prop->name) || prop->name == "id")
        return true;

    const QMetaObject *mo = obj->metaObject();
    if (mo) {
        if (prop->isDefault) {
            QMetaProperty p = QmlMetaType::defaultProperty(mo);
            return p.name() != 0;
        } else {
            int idx = mo->indexOfProperty(prop->name.constData());
            return idx != -1;
        }
    }
    
    return false;
}

bool QmlCompiler::compileProperty(Property *prop, Object *obj, int ctxt)
{
    if (prop->values.isEmpty() && !prop->value) 
        return true;

    // First we're going to need a reference to this property
    const QMetaObject *mo = obj->metaObject();
    if (mo && !isAttachedProperty(prop->name)) {
        if (prop->isDefault) {
            QMetaProperty p = QmlMetaType::defaultProperty(mo);
            // XXX
            // Currently we don't handle enums in the static analysis
            // so we let them drop through to generateStoreInstruction()
            if (p.name() && !p.isEnumType()) {
                prop->index = mo->indexOfProperty(p.name());
                prop->name = p.name();

                int t = p.type();
                if (t == QVariant::UserType)
                    t = p.userType();

                prop->type = t;
            }
        } else {
            prop->index = mo->indexOfProperty(prop->name.constData());
            QMetaProperty p = mo->property(prop->index);
            // XXX
            // Currently we don't handle enums in the static analysis
            // so we let them drop through to generateStoreInstruction()
            if (p.name() && !p.isEnumType()) {
                int t = p.type();
                if (t == QVariant::UserType)
                    t = p.userType();

                prop->type = t;
            }
        }
    }

    if (prop->name == "id") {

        COMPILE_CHECK(compileIdProperty(prop, obj));

    } else if (isAttachedProperty(prop->name)) {

        COMPILE_CHECK(compileAttachedProperty(prop, obj, ctxt));

    } else if (prop->value) {

        COMPILE_CHECK(compileNestedProperty(prop, ctxt));

    } else if (QmlMetaType::isQmlList(prop->type) || 
               QmlMetaType::isList(prop->type)) {

        COMPILE_CHECK(compileListProperty(prop, obj, ctxt));

    } else {

        COMPILE_CHECK(compilePropertyAssignment(prop, obj, ctxt));

    }

    return true;
}

bool QmlCompiler::compileIdProperty(QmlParser::Property *prop, 
                                    QmlParser::Object *obj)
{
    if (prop->value)
        COMPILE_EXCEPTION2(prop,"The id property cannot be fetched");
    if (prop->values.count() > 1)
        COMPILE_EXCEPTION2(prop, "The object id may only be set once");

    if (prop->values.count() == 1) {
        if (prop->values.at(0)->object)
            COMPILE_EXCEPTION("Cannot assign an object as an id");
        QString val = prop->values.at(0)->primitive();
        if (!isValidId(val))
            COMPILE_EXCEPTION(val << "is not a valid id");
        if (ids.contains(val))
            COMPILE_EXCEPTION("id is not unique");
        ids.insert(val);

        int pref = output->indexForString(val);

        if (prop->type == QVariant::String) {
            QmlInstruction assign;
            assign.type = QmlInstruction::StoreString;
            assign.storeString.propertyIndex = prop->index;
            assign.storeString.value = pref;
            assign.line = prop->values.at(0)->location.start.line;
            output->bytecode << assign;

            prop->values.at(0)->type = Value::Id;
        } else {
            prop->values.at(0)->type = Value::Literal;
        }

        QmlInstruction id;
        id.type = QmlInstruction::SetId;
        id.line = prop->values.at(0)->location.start.line;
        id.setId.value = pref;
        id.setId.save = -1;
        output->bytecode << id;

        obj->id = val.toLatin1();
    }

    return true;
}

bool QmlCompiler::compileAttachedProperty(QmlParser::Property *prop, 
                                          QmlParser::Object *obj,
                                          int ctxt)
{
    if (!prop->value) 
        COMPILE_EXCEPTION("Incorrect usage of an attached property");

    QmlInstruction fetch;
    fetch.type = QmlInstruction::FetchAttached;
    fetch.line = prop->location.start.line;
    int id = QmlMetaType::attachedPropertiesFuncId(prop->name);
    if (id == -1)
        COMPILE_EXCEPTION("Non-existant attached property object" << prop->name);
    fetch.fetchAttached.id = id;
    output->bytecode << fetch;

    COMPILE_CHECK(compileFetchedObject(prop->value, ctxt + 1));

    QmlInstruction pop;
    pop.type = QmlInstruction::PopFetchedObject;
    pop.line = prop->location.start.line;
    output->bytecode << pop;

    return true;
}

bool QmlCompiler::compileNestedProperty(QmlParser::Property *prop,
                                        int ctxt)
{
    if (prop->type != 0) 
        prop->value->metatype = QmlMetaType::metaObjectForType(prop->type);
    
    QmlInstruction fetch;
    if (prop->index != -1 && 
       QmlMetaType::isObject(prop->value->metatype)) {
        fetch.type = QmlInstruction::FetchObject;
        fetch.fetch.property = prop->index;
        fetch.fetch.isObject = true;
    } else {
        fetch.type = QmlInstruction::ResolveFetchObject;
        fetch.fetch.property = output->indexForByteArray(prop->name);
    }
    fetch.line = prop->location.start.line;
    output->bytecode << fetch;

    COMPILE_CHECK(compileFetchedObject(prop->value, ctxt + 1));

    QmlInstruction pop;
    pop.type = QmlInstruction::PopFetchedObject;
    pop.line = prop->location.start.line;
    output->bytecode << pop;

    return true;
}

bool QmlCompiler::compileListProperty(QmlParser::Property *prop,
                                      QmlParser::Object *obj,
                                      int ctxt)
{
    int t = prop->type;
    if (QmlMetaType::isQmlList(t)) {
        QmlInstruction fetch;
        fetch.line = prop->location.start.line;
        fetch.type = QmlInstruction::FetchQmlList;
        fetch.fetchQmlList.property = prop->index;
        fetch.fetchQmlList.type = QmlMetaType::qmlListType(t);
        output->bytecode << fetch;

        for (int ii = 0; ii < prop->values.count(); ++ii) {
            Value *v = prop->values.at(ii);
            if (v->object) {
                v->type = Value::CreatedObject;
                COMPILE_CHECK(compileObject(v->object, ctxt));
                QmlInstruction assign;
                assign.type = QmlInstruction::AssignObjectList;
                assign.line = prop->location.start.line;
                assign.assignObject.property = output->indexForByteArray(prop->name);
                assign.assignObject.castValue = 0;
                output->bytecode << assign;
            } else {
                COMPILE_EXCEPTION("Cannot assign primitives to lists");
            }
        }

        QmlInstruction pop;
        pop.type = QmlInstruction::PopQList;
        pop.line = prop->location.start.line;
        output->bytecode << pop;
    } else {
        Q_ASSERT(QmlMetaType::isList(t));

        QmlInstruction fetch;
        fetch.type = QmlInstruction::FetchQList;
        fetch.line = prop->location.start.line;
        fetch.fetch.property = prop->index;
        output->bytecode << fetch;

        bool assignedBinding = false;
        for (int ii = 0; ii < prop->values.count(); ++ii) {
            Value *v = prop->values.at(ii);
            if (v->object) {
                v->type = Value::CreatedObject;
                COMPILE_CHECK(compileObject(v->object, ctxt));
                QmlInstruction assign;
                assign.type = QmlInstruction::AssignObjectList;
                assign.line = v->location.start.line;
                assign.assignObject.property = output->indexForByteArray(prop->name);
                assign.assignObject.castValue = 0;
                output->bytecode << assign;
            } else if (v->value.isScript()) {
                if (assignedBinding)
                    COMPILE_EXCEPTION("Can only assign one binding to lists");

                assignedBinding = true;
                compileBinding(v->value.asScript(), prop, ctxt, 
                               obj->metaObject(), v->location.start.line);
                v->type = Value::PropertyBinding;
            } else {
                COMPILE_EXCEPTION("Cannot assign primitives to lists");
            }
        }

        QmlInstruction pop;
        pop.line = prop->location.start.line;
        pop.type = QmlInstruction::PopQList;
        output->bytecode << pop;
    }
    return true;
}

bool QmlCompiler::compilePropertyAssignment(QmlParser::Property *prop,
                                            QmlParser::Object *obj,
                                            int ctxt)
{
    for (int ii = 0; ii < prop->values.count(); ++ii) {
        Value *v = prop->values.at(ii);
        if (v->object) {

            COMPILE_CHECK(compilePropertyObjectAssignment(prop, obj, v, ctxt));

        } else {

            COMPILE_CHECK(compilePropertyLiteralAssignment(prop, obj, v, ctxt));

        }
    }

    return true;
}

bool QmlCompiler::compilePropertyObjectAssignment(QmlParser::Property *prop,
                                                  QmlParser::Object *obj,
                                                  QmlParser::Value *v,
                                                  int ctxt)
{
    if (v->object->type != -1) 
        v->object->metatype = output->types.at(v->object->type).metaObject();

    if (v->object->metaObject()) {

        const QMetaObject *propmo = 
            QmlMetaType::rawMetaObjectForType(prop->type);
        
        bool isPropertyValue = false;
        bool isAssignable = false;

        if (propmo) {
            // We want to raw metaObject here as the raw metaobject is the 
            // actual property type before we applied any extensions
            const QMetaObject *c = v->object->metatype;
            while(propmo && c) {
                isPropertyValue = isPropertyValue || (c == &QmlPropertyValueSource::staticMetaObject);
                isAssignable = isAssignable || (c == propmo);
                c = c->superClass();
            }
        } else {
            const QMetaObject *c = v->object->metatype;
            while(!isPropertyValue && c) {
                isPropertyValue = c == &QmlPropertyValueSource::staticMetaObject;
                c = c->superClass();
            }
        }

        if (!propmo && !isPropertyValue) {                            
            COMPILE_CHECK(compileObject(v->object, ctxt));

            QmlInstruction assign;
            assign.type = QmlInstruction::AssignObject;
            assign.line = v->object->location.start.line;
            assign.assignObject.castValue = 0;
            if (prop->isDefault)
                assign.assignObject.property = -1;
            else
                assign.assignObject.property = 
                    output->indexForByteArray(prop->name);
            output->bytecode << assign;

            v->type = Value::CreatedObject;
        } else if (isAssignable) {
            COMPILE_CHECK(compileObject(v->object, ctxt));

            QmlInstruction assign;
            assign.type = QmlInstruction::StoreObject;
            assign.line = v->object->location.start.line;
            assign.storeObject.propertyIndex = prop->index;
            // XXX - this cast may not be 0
            assign.storeObject.cast = 0;
            output->bytecode << assign;

            v->type = Value::CreatedObject;
        } else if (propmo == &QmlComponent::staticMetaObject) {

            COMPILE_CHECK(compileComponentFromRoot(v->object, ctxt));

            QmlInstruction assign;
            assign.type = QmlInstruction::StoreObject;
            assign.line = v->object->location.start.line;
            assign.storeObject.propertyIndex = prop->index;
            // XXX - this cast may not be 0
            assign.storeObject.cast = 0;
            output->bytecode << assign;

            v->type = Value::Component;
        } else if (isPropertyValue) {
            COMPILE_CHECK(compileObject(v->object, ctxt));

            if (prop->index != -1) {
                QmlInstruction assign;
                assign.type = QmlInstruction::StoreValueSource;
                assign.line = v->object->location.start.line;
                assign.assignValueSource.property = prop->index;
                output->bytecode << assign;
            } else {
                QmlInstruction assign;
                assign.type = QmlInstruction::AssignValueSource;
                assign.line = v->object->location.start.line;
                assign.assignValueSource.property = output->indexForByteArray(prop->name);;
                output->bytecode << assign;
            }

            v->type = Value::ValueSource;
        } else {
            COMPILE_EXCEPTION("Unassignable object");
        }

    } else {                        
        COMPILE_CHECK(compileObject(v->object, ctxt));

        QmlInstruction assign;
        assign.type = QmlInstruction::AssignObject;
        assign.line = v->object->location.start.line;
        assign.assignObject.property = output->indexForByteArray(prop->name);
        assign.assignObject.castValue = 0;
        output->bytecode << assign;

        v->type = Value::CreatedObject;
    }

    return true;
}

bool QmlCompiler::compilePropertyLiteralAssignment(QmlParser::Property *prop,
                                                   QmlParser::Object *obj,
                                                   QmlParser::Value *v,
                                                   int ctxt)
{
    if (v->value.isScript()) {

        compileBinding(v->value.asScript(), prop, ctxt, obj->metaObject(), 
                       v->location.start.line);

        v->type = Value::PropertyBinding;

    } else {

        QmlInstruction assign;
        assign.line = v->location.start.line;

        bool doassign = true;
        if (prop->index != -1) {
            QString value = v->primitive();
            StoreInstructionResult r = 
                generateStoreInstruction(*output, assign, obj->metaObject()->property(prop->index), prop->index, -1, &value);

            if (r == Ok) {
                doassign = false;
            } else if (r == InvalidData) {
                //### we are restricted to a rather generic message here. If we can find a way to move
                //    the exception into generateStoreInstruction we could potentially have better messages.
                //    (the problem is that both compile and run exceptions can be generated, though)
                COMPILE_EXCEPTION2(v, "Cannot assign value" << v->primitive() << "to property" << obj->metaObject()->property(prop->index).name());
                doassign = false;
            } else if (r == ReadOnly) {
                COMPILE_EXCEPTION2(v, "Cannot assign value" << v->primitive() << "to the read-only property" << obj->metaObject()->property(prop->index).name());
            } else {
                doassign = true;
            }
        }

        if (doassign) {
            assign.type = QmlInstruction::AssignConstant;
            if (prop->isDefault) {
                assign.assignConstant.property = -1;
            } else {
                assign.assignConstant.property = 
                    output->indexForByteArray(prop->name);
            }
            assign.assignConstant.constant = 
                output->indexForString(v->primitive());
        }

        output->bytecode << assign;

        v->type = Value::Literal;
    }
    return true;
}

bool QmlCompiler::compileDynamicMeta(QmlParser::Object *obj)
{
    // ### FIXME - Check that there is only one default property etc.
    if (obj->dynamicProperties.isEmpty() && 
        obj->dynamicSignals.isEmpty() &&
        obj->dynamicSlots.isEmpty())
        return true;

    QMetaObjectBuilder builder;
    if (obj->metatype)
        builder.setClassName(QByteArray(obj->metatype->className()) + "QML");

    builder.setFlags(QMetaObjectBuilder::DynamicMetaObject);
    for (int ii = 0; ii < obj->dynamicProperties.count(); ++ii) {
        const Object::DynamicProperty &p = obj->dynamicProperties.at(ii);

        if (p.isDefaultProperty)
            builder.addClassInfo("DefaultProperty", p.name);

        QByteArray type;
        switch(p.type) {
        case Object::DynamicProperty::Variant:
            type = "QVariant";
            break;
        case Object::DynamicProperty::Int:
            type = "int";
            break;
        case Object::DynamicProperty::Bool:
            type = "bool";
            break;
        case Object::DynamicProperty::Real:
            type = "double";
            break;
        case Object::DynamicProperty::String:
            type = "QString";
            break;
        case Object::DynamicProperty::Color:
            type = "QColor";
            break;
        case Object::DynamicProperty::Date:
            type = "QDate";
            break;
        }

        builder.addSignal(p.name + "Changed()");
        builder.addProperty(p.name, type, ii);
    }

    for (int ii = 0; ii < obj->dynamicSignals.count(); ++ii) {
        const Object::DynamicSignal &s = obj->dynamicSignals.at(ii);
        builder.addSignal(s.name + "()");
    }

    int slotStart = obj->dynamicSlots.isEmpty()?-1:output->primitives.count();

    for (int ii = 0; ii < obj->dynamicSlots.count(); ++ii) {
        const Object::DynamicSlot &s = obj->dynamicSlots.at(ii);
        builder.addSlot(s.name + "()");
        output->primitives << s.body;
    }

    if (obj->metatype)
        builder.setSuperClass(obj->metatype);

    obj->extObjectData = builder.toMetaObject();
    static_cast<QMetaObject &>(obj->extObject) = *obj->extObjectData;

    output->synthesizedMetaObjects << obj->extObjectData;
    QmlInstruction store;
    store.type = QmlInstruction::StoreMetaObject;
    store.storeMeta.data = output->synthesizedMetaObjects.count() - 1;
    store.storeMeta.slotData = slotStart;
    store.line = obj->location.start.line;
    output->bytecode << store;

    for (int ii = 0; ii < obj->dynamicProperties.count(); ++ii) {
        const Object::DynamicProperty &p = obj->dynamicProperties.at(ii);

        if (p.defaultValue) {
            p.defaultValue->name = p.name;
            p.defaultValue->isDefault = false;
            COMPILE_CHECK(compileProperty(p.defaultValue, obj, 0));
        }
    }

    return true;
}

void QmlCompiler::compileBinding(const QString &bind, QmlParser::Property *prop,
                                 int ctxt, const QMetaObject *mo, qint64 line)
{
    QmlBasicScript bs;
    bs.compile(bind.toLatin1());

    int bref;
    if (bs.isValid()) {
        bref = output->indexForByteArray(QByteArray(bs.compileData(), bs.compileDataSize()));
    } else {
        bref = output->indexForString(bind);
    }

    QmlInstruction assign;
    assign.assignBinding.context = ctxt;
    assign.line = line;
    if (prop->index != -1) {
        if (bs.isValid()) 
            assign.type = QmlInstruction::StoreCompiledBinding;
        else
            assign.type = QmlInstruction::StoreBinding;

        assign.assignBinding.property = prop->index;
        assign.assignBinding.value = bref;
        assign.assignBinding.category = QmlMetaProperty::Unknown;
        if (mo) {
            //XXX we should generate an exception if the property is read-only
            QMetaProperty mp = mo->property(assign.assignBinding.property);
            assign.assignBinding.category = QmlMetaProperty::propertyCategory(mp);
        } 
    } else {
        if (bs.isValid())
            assign.type = QmlInstruction::AssignCompiledBinding;
        else
            assign.type = QmlInstruction::AssignBinding;
        assign.assignBinding.property = output->indexForByteArray(prop->name);
        assign.assignBinding.value = bref;
        assign.assignBinding.category = QmlMetaProperty::Unknown;
    }
    output->bytecode << assign;
}

int QmlCompiler::optimizeExpressions(int start, int end, int patch)
{
    QHash<QString, int> ids;
    int saveCount = 0;
    int newInstrs = 0;
    int bindingsCount = 0;
    int parserStatusCount = 0;

    for (int ii = start; ii <= end; ++ii) {
        const QmlInstruction &instr = output->bytecode.at(ii);

        if (instr.type == QmlInstruction::CreateComponent) {
            ii += instr.createComponent.count - 1;
            continue;
        }

        if (instr.type == QmlInstruction::SetId) {
            QString id = output->primitives.at(instr.setId.value);
            ids.insert(id, ii);
        }
    }

    for (int ii = start; ii <= end; ++ii) {
        const QmlInstruction &instr = output->bytecode.at(ii);

        if (instr.type == QmlInstruction::CreateComponent) {
            ii += instr.createComponent.count - 1;
            continue;
        }
        
        if (instr.type == QmlInstruction::AssignBinding ||
            instr.type == QmlInstruction::AssignCompiledBinding ||
            instr.type == QmlInstruction::StoreBinding ||
            instr.type == QmlInstruction::StoreCompiledBinding) {
            ++bindingsCount;
        } else if (instr.type == QmlInstruction::TryBeginObject ||
                   instr.type == QmlInstruction::BeginObject) {
            ++parserStatusCount;
        }


        if (instr.type == QmlInstruction::StoreCompiledBinding) {
            QmlBasicScript s(output->datas.at(instr.assignBinding.value).constData());

            if (s.isSingleLoad()) {
                QString slt = QLatin1String(s.singleLoadTarget());
                if (!slt.at(0).isUpper())
                    continue;

                if (ids.contains(slt) && 
                   instr.assignBinding.category == QmlMetaProperty::Object) {
                    int id = ids[slt];
                    int saveId = -1;

                    if (output->bytecode.at(id).setId.save != -1) {
                        saveId = output->bytecode.at(id).setId.save;
                    } else {
                        output->bytecode[id].setId.save = saveCount;
                        saveId = saveCount;
                        ++saveCount;
                    }

                    int prop = instr.assignBinding.property;

                    QmlInstruction &rwinstr = output->bytecode[ii];
                    rwinstr.type = QmlInstruction::PushProperty;
                    rwinstr.pushProperty.property = prop;

                    QmlInstruction instr;
                    instr.type = QmlInstruction::AssignStackObject;
                    instr.line = 0;
                    instr.assignStackObject.property = newInstrs;
                    instr.assignStackObject.object = saveId;
                    output->bytecode << instr;
                    ++newInstrs;
                }
            }
        } 
    }

    output->bytecode[patch].init.dataSize = saveCount;
    output->bytecode[patch].init.bindingsSize = bindingsCount;
    output->bytecode[patch].init.parserStatusSize = parserStatusCount;;

    return newInstrs;
}

QmlCompiledData::QmlCompiledData()
{
}

QmlCompiledData::QmlCompiledData(const QmlCompiledData &other)
{
    *this = other;
}

QmlCompiledData::~QmlCompiledData()
{
    for (int ii = 0; ii < types.count(); ++ii) {
        if (types.at(ii).ref)
            types.at(ii).ref->release();
    }
}

QmlCompiledData &QmlCompiledData::operator=(const QmlCompiledData &other)
{
    types = other.types;
    root = other.root;
    primitives = other.primitives;
    floatData = other.floatData;
    intData = other.intData;
    customTypeData = other.customTypeData;
    datas = other.datas;
    synthesizedMetaObjects = other.synthesizedMetaObjects;
    bytecode = other.bytecode;
    return *this;
}

QObject *QmlCompiledData::TypeReference::createInstance(QmlContext *ctxt) const
{
    if (type) {
        QObject *rv = type->create();
        if (rv)
            QmlEngine::setContextForObject(rv, ctxt);
        return rv;
    } else {
        Q_ASSERT(component);
        QObject *rv = component->create(ctxt);
        QmlContext *ctxt = qmlContext(rv);
        if(ctxt) {
            static_cast<QmlContextPrivate *>(QObjectPrivate::get(ctxt))->typeName = className;
        }
        return rv;
    } 
}

const QMetaObject *QmlCompiledData::TypeReference::metaObject() const
{
    if (type) {
        return type->metaObject();
    } else {
        Q_ASSERT(component);
        return &static_cast<QmlComponentPrivate *>(QObjectPrivate::get(component))->cc->root;
    }
}

QT_END_NAMESPACE
