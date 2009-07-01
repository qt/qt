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
#include "qmlbasicscript_p.h"
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
#include "parser/qmljsast_p.h"

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
: output(0)
{
}

bool QmlCompiler::isError() const
{
    return !exceptions.isEmpty(); 
}

QList<QmlError> QmlCompiler::errors() const
{
    return exceptions;
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
    Returns true if \a name refers to an attached property, false otherwise.

    Attached property names are those that start with a capital letter.
*/
bool QmlCompiler::isAttachedPropertyName(const QByteArray &name)
{
    return !name.isEmpty() && name.at(0) >= 'A' && name.at(0) <= 'Z';
}

/*!
    Returns true if \a name refers to a signal property, false otherwise.

    Signal property names are those that start with "on", followed by a capital
    letter.
*/
bool QmlCompiler::isSignalPropertyName(const QByteArray &name)
{
    return name.length() >= 3 && name.startsWith("on") && 
           'A' <= name.at(2) && 'Z' >= name.at(2);
}

#define COMPILE_EXCEPTION2(token, desc) \
    {  \
        QString exceptionDescription; \
        QmlError error; \
        error.setUrl(output->url); \
        error.setLine(token->location.start.line); \
        error.setColumn(token->location.start.column); \
        QDebug d(&exceptionDescription); \
        d << desc;  \
        error.setDescription(exceptionDescription.trimmed()); \
        exceptions << error; \
        return false; \
    } 

#define COMPILE_EXCEPTION(desc) \
    {  \
        QString exceptionDescription; \
        QmlError error; \
        error.setUrl(output->url); \
        error.setLine(obj->location.start.line); \
        error.setColumn(obj->location.start.column); \
        QDebug d(&exceptionDescription); \
        d << desc;  \
        error.setDescription(exceptionDescription.trimmed()); \
        exceptions << error; \
        return false; \
    } 

#define COMPILE_CHECK(a) \
    { \
        if (!a) return false; \
    }

// Compile a simple assignment of v to prop into instr
bool QmlCompiler::compileStoreInstruction(QmlInstruction &instr, 
                                          const QMetaProperty &prop, 
                                          QmlParser::Value *v)
{
    QString string = v->value.asScript();

    if (!prop.isWritable())
        COMPILE_EXCEPTION2(v, "Cannot assign literal value to read-only property" << prop.name());

    if (prop.isEnumType()) {
        int value;
        if (prop.isFlagType()) {
            value = prop.enumerator().keysToValue(string.toLatin1().constData());
        } else
            value = prop.enumerator().keyToValue(string.toLatin1().constData());
        if (value == -1)
            COMPILE_EXCEPTION2(v, "Cannot assign unknown enumeration to property" << prop.name());
        instr.type = QmlInstruction::StoreInteger;
        instr.storeInteger.propertyIndex = prop.propertyIndex();
        instr.storeInteger.value = value;
        return true;
    }
    int type = prop.userType();
    switch(type) {
        case -1:
            {
            instr.type = QmlInstruction::StoreVariant;
            instr.storeString.propertyIndex = prop.propertyIndex();
            instr.storeString.value = output->indexForString(string);
            }
            break;
        case QVariant::String:
            {
            instr.type = QmlInstruction::StoreString;
            instr.storeString.propertyIndex = prop.propertyIndex();
            instr.storeString.value = output->indexForString(string);
            }
            break;
        case QVariant::Url:
            {
            instr.type = QmlInstruction::StoreUrl;
            QUrl u = output->url.resolved(QUrl(string));
            instr.storeUrl.propertyIndex = prop.propertyIndex();
            instr.storeUrl.value = output->indexForString(u.toString());
            }
            break;
        case QVariant::UInt:
            {
            instr.type = QmlInstruction::StoreInteger;
            instr.storeInteger.propertyIndex = prop.propertyIndex();
            bool ok;
            int value = string.toUInt(&ok);
            if (!ok)
                COMPILE_EXCEPTION2(v, "Cannot convert value" << string << "to unsigned integer");
            instr.storeInteger.value = value;
            }
            break;
        case QVariant::Int:
            {
            instr.type = QmlInstruction::StoreInteger;
            instr.storeInteger.propertyIndex = prop.propertyIndex();
            bool ok;
            int value = string.toInt(&ok);
            if (!ok)
                COMPILE_EXCEPTION2(v, "Cannot convert value" << string << "to integer");
            instr.storeInteger.value = value;
            }
            break;
        case QMetaType::Float:
            {
            instr.type = QmlInstruction::StoreFloat;
            instr.storeFloat.propertyIndex = prop.propertyIndex();
            bool ok;
            float value = string.toFloat(&ok);
            if (!ok)
                COMPILE_EXCEPTION2(v, "Cannot convert value" << string << "to float number");
            instr.storeFloat.value = value;
            }
            break;
        case QVariant::Double:
            {
            instr.type = QmlInstruction::StoreDouble;
            instr.storeDouble.propertyIndex = prop.propertyIndex();
            bool ok;
            double value = string.toDouble(&ok);
            if (!ok)
                COMPILE_EXCEPTION2(v, "Cannot convert value" << string << "to double number");
            instr.storeDouble.value = value;
            }
            break;
        case QVariant::Color:
            {
            QColor c = QmlStringConverters::colorFromString(string);
            if (!c.isValid())
                COMPILE_EXCEPTION2(v, "Cannot convert value" << string << "to color");
            instr.type = QmlInstruction::StoreColor;
            instr.storeColor.propertyIndex = prop.propertyIndex();
            instr.storeColor.value = c.rgba();
            }
            break;
        case QVariant::Date:
            {
            QDate d = QDate::fromString(string, Qt::ISODate);
            if (!d.isValid())
                COMPILE_EXCEPTION2(v, "Cannot convert value" << string << "to date");
            instr.type = QmlInstruction::StoreDate;
            instr.storeDate.propertyIndex = prop.propertyIndex();
            instr.storeDate.value = d.toJulianDay();
            }
            break;
        case QVariant::Time:
            {
            QTime time = QTime::fromString(string, Qt::ISODate);
            if (!time.isValid())
                COMPILE_EXCEPTION2(v, "Cannot convert value" << string << "to time");
            int data[] = { time.hour(), time.minute(), time.second(), time.msec() };
            int index = output->indexForInt(data, 4);
            instr.type = QmlInstruction::StoreTime;
            instr.storeTime.propertyIndex = prop.propertyIndex();
            instr.storeTime.valueIndex = index;
            }
            break;
        case QVariant::DateTime:
            {
            QDateTime dateTime = QDateTime::fromString(string, Qt::ISODate);
            if (!dateTime.isValid())
                COMPILE_EXCEPTION2(v, "Cannot convert value" << string << "to date and time");
            int data[] = { dateTime.date().toJulianDay(),
                           dateTime.time().hour(),
                           dateTime.time().minute(),
                           dateTime.time().second(),
                           dateTime.time().msec() };
            int index = output->indexForInt(data, 5);
            instr.type = QmlInstruction::StoreDateTime;
            instr.storeDateTime.propertyIndex = prop.propertyIndex();
            instr.storeDateTime.valueIndex = index;
            }
            break;
        case QVariant::Point:
        case QVariant::PointF:
            {
                bool ok;
                QPointF point = QmlStringConverters::pointFFromString(string, &ok);
                if (!ok)
                    COMPILE_EXCEPTION2(v, "Cannot convert value" << string << "to point");
                float data[] = { point.x(), point.y() };
                int index = output->indexForFloat(data, 2);
                if (type == QVariant::PointF)
                    instr.type = QmlInstruction::StorePointF;
                else
                    instr.type = QmlInstruction::StorePoint;
                instr.storeRealPair.propertyIndex = prop.propertyIndex();
                instr.storeRealPair.valueIndex = index;
            }
            break;
        case QVariant::Size:
        case QVariant::SizeF:
            {
                bool ok;
                QSizeF size = QmlStringConverters::sizeFFromString(string, &ok);
                if (!ok)
                    COMPILE_EXCEPTION2(v, "Cannot convert value" << string << "to size");
                float data[] = { size.width(), size.height() };
                int index = output->indexForFloat(data, 2);
                if (type == QVariant::SizeF)
                    instr.type = QmlInstruction::StoreSizeF;
                else
                    instr.type = QmlInstruction::StoreSize;
                instr.storeRealPair.propertyIndex = prop.propertyIndex();
                instr.storeRealPair.valueIndex = index;
            }
            break;
        case QVariant::Rect:
        case QVariant::RectF:
            {
                bool ok;
                QRectF rect = QmlStringConverters::rectFFromString(string, &ok);
                if (!ok)
                    COMPILE_EXCEPTION2(v, "Cannot convert value" << string << "to rect");
                float data[] = { rect.x(), rect.y(), 
                                 rect.width(), rect.height() };
                int index = output->indexForFloat(data, 4);
                if (type == QVariant::RectF)
                    instr.type = QmlInstruction::StoreRectF;
                else
                    instr.type = QmlInstruction::StoreRect;
                instr.storeRect.propertyIndex = prop.propertyIndex();
                instr.storeRect.valueIndex = index;
            }
            break;
        case QVariant::Bool:
            {
                bool ok;
                bool b = QmlStringConverters::boolFromString(string, &ok);
                if (!ok)
                    COMPILE_EXCEPTION2(v, "Cannot convert value" << string << "to boolean");
                instr.type = QmlInstruction::StoreBool;
                instr.storeBool.propertyIndex = prop.propertyIndex();
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
                    int index = output->customTypeData.count();
                    instr.type = QmlInstruction::AssignCustomType;
                    instr.assignCustomType.propertyIndex = prop.propertyIndex();
                    instr.assignCustomType.valueIndex = index;

                    QmlCompiledData::CustomTypeData data;
                    data.index = output->indexForString(string);
                    data.type = t;
                    output->customTypeData << data;
                    break;
                }
            }
            COMPILE_EXCEPTION2(v, "Cannot assign to property" << prop.name() << "of unknown type" << prop.type());
            break;
    }
    return true;
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

bool QmlCompiler::compile(QmlEngine *engine, 
                          QmlCompositeTypeData *unit,
                          QmlCompiledComponent *out)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::Compilation> pc;
#endif
    exceptions.clear();

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

            if (ref.component->isError()) {
                QmlError error;
                error.setUrl(output->url); 
                error.setDescription(QLatin1String("Unable to create type ") + 
                                     unit->data.types().at(ii));
                exceptions << error;
                exceptions << ref.component->errors();
                reset(out, true);
                return false;
            }
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
    compileState.root = tree;

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

    finalizeComponent(0);
}

bool QmlCompiler::compileObject(Object *obj, const BindingContext &ctxt)
{    
    Q_ASSERT (obj->type != -1);
    const QmlCompiledData::TypeReference &tr = output->types.at(obj->type);
    obj->metatype = tr.metaObject();
    if (tr.component) 
        obj->url = tr.component->url();

    if (output->types.at(obj->type).className == "Component") {
        COMPILE_CHECK(compileComponent(obj, ctxt));
        return true;
    }

    BindingContext objCtxt(obj);

    int createInstrIdx = output->bytecode.count();
    // Create the object
    QmlInstruction create;
    create.type = QmlInstruction::CreateObject;
    create.line = obj->location.start.line;
    create.create.data = -1;
    create.create.type = obj->type;
    output->bytecode << create;

    // Create the synthesized meta object
    COMPILE_CHECK(compileDynamicMeta(obj));

    // Find the native type and check for the QmlParserStatus interface
    // ### Optimize
    const QMetaObject *mo = obj->metatype; 
    QmlType *type = 0;
    while (!type && mo) {
        type = QmlMetaType::qmlType(mo);
        mo = mo->superClass();
    }
    Q_ASSERT(type);
    int parserStatusCast = type->parserStatusCast();

    // If the type support the QmlParserStatusInterface we need to invoke
    // classBegin()
    if (parserStatusCast != -1) {
        QmlInstruction begin;
        begin.type = QmlInstruction::BeginObject;
        begin.begin.castValue = parserStatusCast;
        begin.line = obj->location.start.line;
        output->bytecode << begin;

        compileState.parserStatusCount++;
    }

    // Check if this is a custom parser type.  Custom parser types allow 
    // assignments to non-existant properties.  These assignments are then
    // compiled by the type.
    bool isCustomParser = output->types.at(obj->type).type &&  
                          output->types.at(obj->type).type->customParser() != 0;
    QList<QmlCustomParserProperty> customProps;

    QStringList deferred = deferredProperties(obj);
    QList<Property *> deferredProps;

    // Compile all explicit properties specified
    foreach(Property *prop, obj->properties) {

        if (isCustomParser) {
            // Custom parser types don't support signal properties
            if (testProperty(prop, obj)) {
                if (deferred.contains(QString::fromLatin1(prop->name.constData())))
                    deferredProps << prop;
                else 
                    COMPILE_CHECK(compileProperty(prop, obj, objCtxt));
            } else {
                customProps << QmlCustomParserNodePrivate::fromProperty(prop);
            }
        } else {
            if (isSignalPropertyName(prop->name))  {
                COMPILE_CHECK(compileSignal(prop,obj));
            } else {
                if (deferred.contains(QString::fromLatin1(prop->name.constData())))
                    deferredProps << prop;
                else 
                    COMPILE_CHECK(compileProperty(prop, obj, objCtxt));
            }
        }

    }

    // Compile the default property
    if (obj->defaultProperty)  {
        Property *prop = obj->defaultProperty;

        if (isCustomParser) {
            if (testProperty(prop, obj)) {
                QMetaProperty p = deferred.isEmpty()?QMetaProperty():QmlMetaType::defaultProperty(obj->metaObject());
                if (deferred.contains(QString::fromLatin1(p.name())))
                    deferredProps << prop;
                else
                    COMPILE_CHECK(compileProperty(prop, obj, objCtxt));
            } else {
                customProps << QmlCustomParserNodePrivate::fromProperty(prop);
            }
        } else {
            QMetaProperty p = deferred.isEmpty()?QMetaProperty():QmlMetaType::defaultProperty(obj->metaObject());
            if (deferred.contains(QString::fromLatin1(p.name())))
                deferredProps << prop;
            else
                COMPILE_CHECK(compileProperty(prop, obj, objCtxt));
        }

    }

    // Compile custom parser parts
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

    // Build the deferred block 
    if (!deferredProps.isEmpty()) {
        QmlInstruction defer;
        defer.type = QmlInstruction::Defer;
        defer.line = 0;
        defer.defer.deferCount = 0;
        int deferIdx = output->bytecode.count();
        output->bytecode << defer;

        // ### This is lame, we should check if individual properties have
        // ids defined within them
        int idCount = compileState.ids.count();
        foreach (Property *prop, deferredProps) {
            COMPILE_CHECK(compileProperty(prop, obj, objCtxt));
        }
        if (idCount == compileState.ids.count()) 
            output->bytecode[deferIdx].defer.deferCount = 
                output->bytecode.count() - deferIdx - 1;
    }

    // If the type support the QmlParserStatusInterface we need to invoke
    // classComplete()
    if (parserStatusCast != -1) {
        QmlInstruction complete;
        complete.type = QmlInstruction::CompleteObject;
        complete.complete.castValue = parserStatusCast;
        complete.line = obj->location.start.line;
        output->bytecode << complete;
    } 

    return true;
}

bool QmlCompiler::compileComponent(Object *obj, const BindingContext &ctxt)
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

        if (compileState.ids.contains(val))
            COMPILE_EXCEPTION("id is not unique");

        IdReference reference;
        reference.id = val;
        reference.object = obj;
        reference.instructionIdx = output->bytecode.count();
        reference.idx = compileState.ids.count();
        compileState.ids.insert(val, reference);

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

bool QmlCompiler::compileComponentFromRoot(Object *obj, const BindingContext &ctxt)
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

    ComponentCompileState oldComponentCompileState = compileState;
    compileState = ComponentCompileState();
    compileState.root = obj;
    if (obj) 
        COMPILE_CHECK(compileObject(obj, ctxt));

    finalizeComponent(count);
    create.createComponent.count = output->bytecode.count() - count;
    compileState = oldComponentCompileState;
    return true;
}


bool QmlCompiler::compileFetchedObject(Object *obj, const BindingContext &ctxt)
{
    Q_ASSERT(obj->metatype);

    if (obj->defaultProperty) 
        COMPILE_CHECK(compileProperty(obj->defaultProperty, obj, ctxt));

    foreach(Property *prop, obj->properties) {
        if (isSignalPropertyName(prop->name)) {
            COMPILE_CHECK(compileSignal(prop, obj));
        } else {
            COMPILE_CHECK(compileProperty(prop, obj, ctxt));
        }
    }

    return true;
}

int QmlCompiler::signalByName(const QMetaObject *mo, const QByteArray &name)
{
    int methods = mo->methodCount();
    for (int ii = methods - 1; ii >= 0; --ii) {
        QMetaMethod method = mo->method(ii);
        QByteArray methodName = method.signature();
        int idx = methodName.indexOf('(');
        methodName = methodName.left(idx);

        if (methodName == name) 
            return ii;
    }
    return -1;
}

bool QmlCompiler::compileSignal(Property *prop, Object *obj)
{
    Q_ASSERT(obj->metaObject());

    if (prop->values.isEmpty() && !prop->value)
        return true;

    if (prop->value || prop->values.count() > 1)
        COMPILE_EXCEPTION("Incorrectly specified signal");

    QByteArray name = prop->name;
    Q_ASSERT(name.startsWith("on"));
    name = name.mid(2);
    if(name[0] >= 'A' && name[0] <= 'Z')
        name[0] = name[0] - 'A' + 'a';

    int sigIdx = signalByName(obj->metaObject(), name);

    if (sigIdx == -1) {

        COMPILE_CHECK(compileProperty(prop, obj, 0));

    }  else {

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

            QmlInstruction store;
            store.line = prop->values.at(0)->location.start.line;
            store.type = QmlInstruction::StoreSignal;
            store.storeSignal.signalIndex = sigIdx;
            store.storeSignal.value = idx;

            output->bytecode << store;

            prop->values.at(0)->type = Value::SignalExpression;
        }
    }
    
    return true;
}

// Returns true if prop exists on obj, false otherwise
bool QmlCompiler::testProperty(QmlParser::Property *prop, 
                               QmlParser::Object *obj)
{
    if(isAttachedPropertyName(prop->name) || prop->name == "id")
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

bool QmlCompiler::compileProperty(Property *prop, Object *obj, const BindingContext &ctxt)
{
    if (prop->values.isEmpty() && !prop->value) 
        COMPILE_EXCEPTION2(prop, "Empty property assignment");

    const QMetaObject *metaObject = obj->metaObject();
    Q_ASSERT(metaObject);

    if (isAttachedPropertyName(prop->name)) {
        // Setup attached property data
        QmlType *type = QmlMetaType::qmlType(prop->name);

        if (!type || !type->attachedPropertiesType())
            COMPILE_EXCEPTION2(prop, "Non-existant attached object");

        if (!prop->value)
            COMPILE_EXCEPTION2(prop, "Cannot assign directly to attached object");

        prop->value->metatype = type->attachedPropertiesType();
    } else {
        // Setup regular property data
        QMetaProperty p;

        if (prop->isDefault) {
            p = QmlMetaType::defaultProperty(metaObject);

            if (p.name()) {
                prop->index = p.propertyIndex();
                prop->name = p.name();
            }

        } else {
            prop->index = metaObject->indexOfProperty(prop->name.constData());

            if (prop->index != -1) {
                p = metaObject->property(prop->index);
                Q_ASSERT(p.name());
            }
        }

        // We can't error here as the "id" property does not require a 
        // successful index resolution
        if (p.name()) {
            int t = p.type();

            if (t == QVariant::UserType)
                t = p.userType();

            prop->type = t;
        }
    }

    if (!prop->isDefault && prop->name == "id") {

        COMPILE_CHECK(compileIdProperty(prop, obj));

    } else if (isAttachedPropertyName(prop->name)) {

        COMPILE_CHECK(compileAttachedProperty(prop, ctxt));

    } else if (prop->index == -1) {

        if (prop->isDefault) {
            COMPILE_EXCEPTION2(prop, "Cannot assign to non-existant default property");
        } else {
            COMPILE_EXCEPTION2(prop, "Cannot assign to non-existant property" << prop->name);
        }

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

    if (prop->values.at(0)->object)
        COMPILE_EXCEPTION("Cannot assign an object as an id");
    QString val = prop->values.at(0)->primitive();
    if (!isValidId(val))
        COMPILE_EXCEPTION(val << "is not a valid id");
    if (compileState.ids.contains(val))
        COMPILE_EXCEPTION("id is not unique");

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

    IdReference reference;
    reference.id = val;
    reference.object = obj;
    reference.instructionIdx = output->bytecode.count();
    reference.idx = compileState.ids.count();
    compileState.ids.insert(val, reference);

    QmlInstruction id;
    id.type = QmlInstruction::SetId;
    id.line = prop->values.at(0)->location.start.line;
    id.setId.value = pref;
    id.setId.save = -1;
    output->bytecode << id;

    obj->id = val.toLatin1();

    return true;
}

// Compile attached property object.  In this example,
// Text {
//    GridView.row: 10
// }
// GridView is an attached property object.
bool QmlCompiler::compileAttachedProperty(QmlParser::Property *prop, 
                                          const BindingContext &ctxt)
{
    Q_ASSERT(prop->value);
    int id = QmlMetaType::attachedPropertiesFuncId(prop->name);
    Q_ASSERT(id != -1); // This is checked in compileProperty()

    QmlInstruction fetch;
    fetch.type = QmlInstruction::FetchAttached;
    fetch.line = prop->location.start.line;
    fetch.fetchAttached.id = id;
    output->bytecode << fetch;

    COMPILE_CHECK(compileFetchedObject(prop->value, ctxt.incr()));

    QmlInstruction pop;
    pop.type = QmlInstruction::PopFetchedObject;
    pop.line = prop->location.start.line;
    output->bytecode << pop;

    return true;
}

// Compile "nested" properties. In this example:
// Text {
//     font.size: 12
// }
// font is a nested property.  size is not.
bool QmlCompiler::compileNestedProperty(QmlParser::Property *prop,
                                        const BindingContext &ctxt)
{
    Q_ASSERT(prop->type != 0);
    Q_ASSERT(prop->index != -1);

    // Load the nested property's meta type
    prop->value->metatype = QmlMetaType::metaObjectForType(prop->type);
    if (!prop->value->metatype)
        COMPILE_EXCEPTION2(prop, "Cannot nest non-QObject property" << prop->name);

    QmlInstruction fetch;
    fetch.type = QmlInstruction::FetchObject;
    fetch.fetch.property = prop->index;
    fetch.fetch.isObject = true;
    fetch.line = prop->location.start.line;
    output->bytecode << fetch;

    COMPILE_CHECK(compileFetchedObject(prop->value, ctxt.incr()));

    QmlInstruction pop;
    pop.type = QmlInstruction::PopFetchedObject;
    pop.line = prop->location.start.line;
    output->bytecode << pop;

    return true;
}

// Compile assignments to QML lists.  QML lists are properties of type
// QList<T *> * and QmlList<T *> *.
//
// QList<T *> * types can accept a list of objects, or a single binding
// QmlList<T *> * types can accept a list of objects
bool QmlCompiler::compileListProperty(QmlParser::Property *prop,
                                      QmlParser::Object *obj,
                                      const BindingContext &ctxt)
{
    Q_ASSERT(QmlMetaType::isList(prop->type) || 
             QmlMetaType::isQmlList(prop->type));

    int t = prop->type;

    if (QmlMetaType::isQmlList(t)) {
        QmlInstruction fetch;
        fetch.line = prop->location.start.line;
        fetch.type = QmlInstruction::FetchQmlList;
        fetch.fetchQmlList.property = prop->index;
        int listType = QmlMetaType::qmlListType(t);
        bool listTypeIsInterface = QmlMetaType::isInterface(listType);
        fetch.fetchQmlList.type = listType;
        output->bytecode << fetch;

        for (int ii = 0; ii < prop->values.count(); ++ii) {
            Value *v = prop->values.at(ii);
            if (v->object) {
                v->type = Value::CreatedObject;
                COMPILE_CHECK(compileObject(v->object, ctxt));

                if (!listTypeIsInterface) {
                    if (canConvert(listType, v->object)) {
                        QmlInstruction store;
                        store.type = QmlInstruction::StoreObjectQmlList;
                        store.line = prop->location.start.line;
                        output->bytecode << store;
                    } else {
                        COMPILE_EXCEPTION("Cannot assign object to list");
                    }

                } else {
                    QmlInstruction assign;
                    assign.type = QmlInstruction::AssignObjectList;
                    assign.line = prop->location.start.line;
                    output->bytecode << assign;
                }
            } else {
                COMPILE_EXCEPTION("Cannot assign primitives to lists");
            }
        }

        QmlInstruction pop;
        pop.type = QmlInstruction::PopQList;
        pop.line = prop->location.start.line;
        output->bytecode << pop;
    } else {
        QmlInstruction fetch;
        fetch.type = QmlInstruction::FetchQList;
        fetch.line = prop->location.start.line;
        fetch.fetchQmlList.property = prop->index;
        int listType = QmlMetaType::listType(t);
        bool listTypeIsInterface = QmlMetaType::isInterface(listType);
        fetch.fetchQmlList.type = listType;
        output->bytecode << fetch;

        bool assignedBinding = false;
        for (int ii = 0; ii < prop->values.count(); ++ii) {
            Value *v = prop->values.at(ii);
            if (v->object) {
                v->type = Value::CreatedObject;
                COMPILE_CHECK(compileObject(v->object, ctxt));

                if (!listTypeIsInterface) {
                    if (canConvert(listType, v->object)) {
                        QmlInstruction store;
                        store.type = QmlInstruction::StoreObjectQList;
                        store.line = prop->location.start.line;
                        output->bytecode << store;
                    } else {
                        COMPILE_EXCEPTION("Cannot assign object to list");
                    }
                } else {
                    QmlInstruction assign;
                    assign.type = QmlInstruction::AssignObjectList;
                    assign.line = v->location.start.line;
                    output->bytecode << assign;
                }
            } else if (v->value.isScript()) {
                if (assignedBinding)
                    COMPILE_EXCEPTION("Can only assign one binding to lists");

                assignedBinding = true;
                COMPILE_CHECK(compileBinding(v, prop, ctxt));
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

// Compile regular property assignments of the form property: <value>
//
// ### The following problems exist
//
// There is no distinction between how "lists" of values are specified.  This
//    Item {
//        children: Item {}
//        children: Item {}
//    }
// is identical to
//    Item {
//        children: [ Item {}, Item {} ]
//    }
//
// We allow assignming multiple values to single value properties
bool QmlCompiler::compilePropertyAssignment(QmlParser::Property *prop,
                                            QmlParser::Object *obj,
                                            const BindingContext &ctxt)
{
    for (int ii = 0; ii < prop->values.count(); ++ii) {
        Value *v = prop->values.at(ii);
        if (v->object) {

            COMPILE_CHECK(compilePropertyObjectAssignment(prop, v, ctxt));

        } else {

            COMPILE_CHECK(compilePropertyLiteralAssignment(prop, obj, v, ctxt));

        }
    }

    return true;
}

// Compile assigning a single object instance to a regular property 
bool QmlCompiler::compilePropertyObjectAssignment(QmlParser::Property *prop,
                                                  QmlParser::Value *v,
                                                  const BindingContext &ctxt)
{
    Q_ASSERT(prop->index != -1);
    Q_ASSERT(v->object->type != -1);

    if (QmlMetaType::isInterface(prop->type)) {

        // Assigning an object to an interface ptr property
        COMPILE_CHECK(compileObject(v->object, ctxt));

        QmlInstruction assign;
        assign.type = QmlInstruction::StoreInterface;
        assign.line = v->object->location.start.line;
        assign.storeObject.propertyIndex = prop->index;
        output->bytecode << assign;

        v->type = Value::CreatedObject;

    } else if (prop->type == -1) {

        // Assigning an object to a QVariant
        COMPILE_CHECK(compileObject(v->object, ctxt));

        QmlInstruction assign;
        assign.type = QmlInstruction::StoreVariantObject;
        assign.line = v->object->location.start.line;
        assign.storeObject.propertyIndex = prop->index;
        output->bytecode << assign;

        v->type = Value::CreatedObject;
    } else {
        // Normally compileObject() will set this up, but we need the static 
        // meta object earlier to test for assignability.  It doesn't matter
        // that there may still be outstanding synthesized meta object changes
        // on this type, as they are not relevant for assignability testing
        v->object->metatype = output->types.at(v->object->type).metaObject();
        Q_ASSERT(v->object->metaObject());

        // We want to raw metaObject here as the raw metaobject is the 
        // actual property type before we applied any extensions that might 
        // effect the properties on the type, but don't effect assignability
        const QMetaObject *propertyMetaObject = 
            QmlMetaType::rawMetaObjectForType(prop->type);
        
        // Will be true if the assigned type inherits QmlPropertyValueSource
        bool isPropertyValue = false; 
        // Will be true if the assgned type inherits propertyMetaObject
        bool isAssignable = false;
        // Determine isPropertyValue and isAssignable values
        if (propertyMetaObject) {
            const QMetaObject *c = v->object->metatype;
            while(c) {
                isPropertyValue |= (c == &QmlPropertyValueSource::staticMetaObject);
                isAssignable |= (c == propertyMetaObject);
                c = c->superClass();
            }
        } else {
            const QMetaObject *c = v->object->metatype;
            while(!isPropertyValue && c) {
                isPropertyValue |= (c == &QmlPropertyValueSource::staticMetaObject);
                c = c->superClass();
            }
        }

        if (isAssignable) {
            // Simple assignment
            COMPILE_CHECK(compileObject(v->object, ctxt));

            QmlInstruction assign;
            assign.type = QmlInstruction::StoreObject;
            assign.line = v->object->location.start.line;
            assign.storeObject.propertyIndex = prop->index;
            output->bytecode << assign;

            v->type = Value::CreatedObject;
        } else if (propertyMetaObject == &QmlComponent::staticMetaObject) {
            // Automatic "Component" insertion
            COMPILE_CHECK(compileComponentFromRoot(v->object, ctxt));

            QmlInstruction assign;
            assign.type = QmlInstruction::StoreObject;
            assign.line = v->object->location.start.line;
            assign.storeObject.propertyIndex = prop->index;
            output->bytecode << assign;

            v->type = Value::Component;
        } else if (isPropertyValue) {
            // Assign as a property value source
            COMPILE_CHECK(compileObject(v->object, ctxt));

            QmlInstruction assign;
            assign.type = QmlInstruction::StoreValueSource;
            assign.line = v->object->location.start.line;
            assign.assignValueSource.property = prop->index;
            output->bytecode << assign;

            v->type = Value::ValueSource;
        } else {
            COMPILE_EXCEPTION2(v->object, "Unassignable object");
        }
    }

    return true;
}

// Compile assigning a literal or binding to a regular property 
bool QmlCompiler::compilePropertyLiteralAssignment(QmlParser::Property *prop,
                                                   QmlParser::Object *obj,
                                                   QmlParser::Value *v,
                                                   const BindingContext &ctxt)
{
    Q_ASSERT(prop->index != -1);

    if (v->value.isScript()) {

        COMPILE_CHECK(compileBinding(v, prop, ctxt));

        v->type = Value::PropertyBinding;

    } else {

        QmlInstruction assign;
        assign.line = v->location.start.line;
        COMPILE_CHECK(compileStoreInstruction(assign, obj->metaObject()->property(prop->index), v));
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
        case Object::DynamicProperty::Url:
            type = "QUrl";
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

bool QmlCompiler::compileBinding(QmlParser::Value *value, 
                                 QmlParser::Property *prop,
                                 const BindingContext &ctxt)
{
    Q_ASSERT(prop->index);
    Q_ASSERT(prop->parent);
    Q_ASSERT(prop->parent->metaObject());

    QMetaProperty mp = prop->parent->metaObject()->property(prop->index);
    if (!mp.isWritable() && !QmlMetaType::isList(prop->type))
        COMPILE_EXCEPTION2(prop, "Cannot assign binding to read-only property");

    BindingReference reference;
    reference.expression = value->value;
    reference.property = prop;
    reference.value = value;
    reference.instructionIdx = output->bytecode.count();
    reference.bindingContext = ctxt;
    compileState.bindings << reference;

    output->bytecode << QmlInstruction();;

    return true;
}

#if 0

#include <iostream> 
#ifdef Q_CC_GNU 
#include <cxxabi.h> 
#endif 
 
//////////////////////////////////////////////////////////////////////////////// 
// AST Dump 
//////////////////////////////////////////////////////////////////////////////// 
class Dump: protected QmlJS::AST::Visitor 
{ 
    std::ostream &out; 
    int depth; 
 
public: 
    Dump(std::ostream &out) 
        : out(out), depth(-1) 
     { } 
 
    void operator()(QmlJS::AST::Node *node) 
    { QmlJS::AST::Node::acceptChild(node, this); } 
 
protected: 
    virtual bool preVisit(QmlJS::AST::Node *node) 
    { 
        const char *name = typeid(*node).name(); 
#ifdef Q_CC_GNU 
        name = abi::__cxa_demangle(name, 0, 0, 0) + 17; 
#endif 
        std::cout << std::string(++depth, ' ') << name << std::endl; 
        return true; 
    } 
 
    virtual void postVisit(QmlJS::AST::Node *) 
    { 
        --depth; 
    } 
}; 
#endif

// Update the init instruction with final data, and optimize some simple 
// bindings
void QmlCompiler::finalizeComponent(int patch)
{
    for (int ii = 0; ii < compileState.bindings.count(); ++ii) {
        const BindingReference &binding = compileState.bindings.at(ii);
        finalizeBinding(binding);
    }

    output->bytecode[patch].init.dataSize = compileState.savedObjects;;
    output->bytecode[patch].init.bindingsSize = compileState.bindings.count();
    output->bytecode[patch].init.parserStatusSize = 
        compileState.parserStatusCount;
}

void QmlCompiler::finalizeBinding(const BindingReference &binding)
{
    QmlBasicScript bs;
    QmlBasicScript::Expression expr;
    expr.component = compileState.root;
    expr.context = binding.bindingContext.object;
    expr.property = binding.property;
    expr.expression = binding.expression;
    foreach (const IdReference &id, compileState.ids)
        expr.ids.insert(id.id, qMakePair(id.object, id.idx));

    bs.compile(expr);

    QmlInstruction &instr = output->bytecode[binding.instructionIdx];
    instr.line = binding.value->location.start.line;

    // Single load optimization 
    if (bs.isValid() && bs.isSingleLoad()) {

        QString singleLoadTarget = QLatin1String(bs.singleLoadTarget());

        if (singleLoadTarget.at(0).isUpper() && 
            compileState.ids.contains(singleLoadTarget) &&
            QmlMetaType::isObject(binding.property->type)) {

            IdReference reference = compileState.ids[singleLoadTarget];

            const QMetaObject *idMo = reference.object->metaObject();
            const QMetaObject *storeMo = 
                QmlMetaType::rawMetaObjectForType(binding.property->type);

            Q_ASSERT(idMo);
            Q_ASSERT(storeMo);

            bool canAssign = false;
            while (!canAssign && idMo) {
                if (idMo == storeMo)
                    canAssign = true;
                else
                    idMo = idMo->superClass();
            }

            if (canAssign) {
                int instructionIdx = reference.instructionIdx;
                if (output->bytecode.at(instructionIdx).setId.save == -1) {
                    output->bytecode[instructionIdx].setId.save = 
                        compileState.savedObjects++;
                }
                int saveId = output->bytecode.at(instructionIdx).setId.save;

                instr.type = QmlInstruction::PushProperty;
                instr.pushProperty.property = binding.property->index;

                QmlInstruction store;
                store.type = QmlInstruction::StoreStackObject;
                store.line = 0;
                store.assignStackObject.property = 
                    compileState.pushedProperties++;
                store.assignStackObject.object = saveId;
                output->bytecode << store;
                return;
            }
        }
    }

    // General binding fallback
    int bref;
    if (bs.isValid()) {
        bref = output->indexForByteArray(QByteArray(bs.compileData(), bs.compileDataSize()));
    } else {
        bref = output->indexForString(binding.expression.asScript());
    }

    instr.assignBinding.context = binding.bindingContext.stack;

    if (bs.isValid()) 
        instr.type = QmlInstruction::StoreCompiledBinding;
    else
        instr.type = QmlInstruction::StoreBinding;

    instr.assignBinding.property = binding.property->index;
    instr.assignBinding.value = bref;
    QMetaProperty mp = binding.property->parent->metaObject()->property(binding.property->index);
    instr.assignBinding.category = QmlMetaProperty::propertyCategory(mp);
}

/*!
    Returns true if object can be assigned to a (QObject) property of type 
    convertType.
*/
bool QmlCompiler::canConvert(int convertType, QmlParser::Object *object)
{
    const QMetaObject *convertTypeMo = 
        QmlMetaType::rawMetaObjectForType(convertType);
    const QMetaObject *objectMo = object->metaObject();

    while (objectMo) {
        if (objectMo == convertTypeMo)
            return true;
        objectMo = objectMo->superClass();
    }
    return false;
}

QStringList QmlCompiler::deferredProperties(QmlParser::Object *obj)
{
    const QMetaObject *mo = obj->metatype;

    int idx = mo->indexOfClassInfo("DeferredPropertyNames");
    if (idx == -1)
        return QStringList();

    QMetaClassInfo classInfo = mo->classInfo(idx);
    QStringList rv = QString(QLatin1String(classInfo.value())).split(QLatin1Char(','));
    return rv;
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
