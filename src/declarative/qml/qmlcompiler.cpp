/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmlcompiler_p.h"

#include "qmlcompositetypedata_p.h"
#include "qmlparser_p.h"
#include "qmlscriptparser_p.h"
#include "qmlpropertyvaluesource.h"
#include "qmlcomponent.h"
#include "qmetaobjectbuilder_p.h"
#include "qmlstringconverters_p.h"
#include "qmlengine_p.h"
#include "qmlengine.h"
#include "qmlcontext.h"
#include "qmlmetatype.h"
#include "qmlcustomparser_p_p.h"
#include "qmlcontext_p.h"
#include "qmlcomponent_p.h"
#include "parser/qmljsast_p.h"
#include "qmlvmemetaobject_p.h"
#include "qmlexpression_p.h"
#include "qmlmetaproperty_p.h"
#include "qmlrewrite_p.h"
#include "qmlscriptstring.h"
#include "qmlglobal_p.h"
#include "qmlscriptparser_p.h"
#include "qmlbinding.h"
#include "qmlcompiledbindings_p.h"

#include <qfxperf_p_p.h>

#include <QCoreApplication>
#include <QColor>
#include <QDebug>
#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <QAtomicInt>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(compilerDump, QML_COMPILER_DUMP);
DEFINE_BOOL_CONFIG_OPTION(compilerStatDump, QML_COMPILER_STATISTICS_DUMP);
DEFINE_BOOL_CONFIG_OPTION(qmlExperimental, QML_EXPERIMENTAL);

using namespace QmlParser;

/*!
    Instantiate a new QmlCompiler.
*/
QmlCompiler::QmlCompiler()
: output(0), engine(0), unitRoot(0), unit(0)
{
}

/*!
    Returns true if the last call to compile() caused errors.

    \sa errors()
*/
bool QmlCompiler::isError() const
{
    return !exceptions.isEmpty();
}

/*!
    Return the list of errors from the last call to compile(), or an empty list
    if there were no errors.
*/
QList<QmlError> QmlCompiler::errors() const
{
    return exceptions;
}

/*!
    Returns true if \a val is a legal object id, false otherwise.

    Legal ids must start with a lower-case letter or underscore, and contain only
    letters, numbers and underscores.
*/
bool QmlCompiler::isValidId(const QString &val)
{
    if (val.isEmpty())
        return false;

    // TODO this will be enforced and return false
    if (val.at(0).isLetter() && !val.at(0).isLower()) {
        //return false;
        qWarning().nospace() << "id '" << val << "' is invalid: ids cannot start with uppercase letters. This will be enforced in an upcoming version of QML.";
    }

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

/*!
    \macro COMPILE_EXCEPTION
    \internal
    Inserts an error into the QmlCompiler error list, and returns false
    (failure).

    \a token is used to source the error line and column, and \a desc is the
    error itself.  \a desc can be an expression that can be piped into QDebug.

    For example:

    \code
    COMPILE_EXCEPTION(property, QCoreApplication::translate("QmlCompiler","Error for property \"%1\"").arg(QString::fromUtf8(property->name)));
    \endcode
*/
#define COMPILE_EXCEPTION(token, desc) \
    {  \
        QString exceptionDescription; \
        QmlError error; \
        error.setUrl(output->url); \
        error.setLine((token)->location.start.line); \
        error.setColumn((token)->location.start.column); \
        error.setDescription(desc.trimmed()); \
        exceptions << error; \
        return false; \
    }

/*!
    \macro COMPILE_CHECK
    \internal
    Returns false if \a is false, otherwise does nothing.
*/
#define COMPILE_CHECK(a) \
    { \
        if (!a) return false; \
    }

/*!
    Returns true if literal \a v can be assigned to property \a prop, otherwise
    false.

    This test corresponds to action taken by genLiteralAssignment().  Any change
    made here, must have a corresponding action in genLiteralAssigment().
*/
bool QmlCompiler::testLiteralAssignment(const QMetaProperty &prop,
                                        QmlParser::Value *v)
{
    QString string = v->value.asScript();

    if (!prop.isWritable())
        COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: \"%1\" is a read-only property").arg(QString::fromUtf8(prop.name())));

    if (prop.isEnumType()) {
        int value;
        if (prop.isFlagType()) {
            value = prop.enumerator().keysToValue(string.toUtf8().constData());
        } else
            value = prop.enumerator().keyToValue(string.toUtf8().constData());
        if (value == -1)
            COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: unknown enumeration"));
        return true;
    }
    int type = prop.userType();
    switch(type) {
        case -1:
            break;
        case QVariant::String:
            if (!v->value.isString()) COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: string expected"));
            break;
        case QVariant::Url:
            if (!v->value.isString()) COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: url expected"));
            break;
        case QVariant::UInt:
            {
            bool ok;
            string.toUInt(&ok);
            if (!v->value.isNumber() || !ok) COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: unsigned int expected"));
            }
            break;
        case QVariant::Int:
            {
            bool ok;
            string.toInt(&ok);
            if (!v->value.isNumber() || !ok) COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: int expected"));
            }
            break;
        case QMetaType::Float:
            {
            bool ok;
            string.toFloat(&ok);
            if (!v->value.isNumber() || !ok) COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: float expected"));
            }
            break;
        case QVariant::Double:
            {
            bool ok;
            string.toDouble(&ok);
            if (!v->value.isNumber() || !ok) COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: double expected"));
            }
            break;
        case QVariant::Color:
            {
            bool ok;
            QmlStringConverters::colorFromString(string, &ok);
            if (!ok) COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: color expected"));
            }
            break;
        case QVariant::Date:
            {
            bool ok;
            QmlStringConverters::dateFromString(string, &ok);
            if (!ok) COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: date expected"));
            }
            break;
        case QVariant::Time:
            {
            bool ok;
            QmlStringConverters::timeFromString(string, &ok);
            if (!ok) COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: time expected"));
            }
            break;
        case QVariant::DateTime:
            {
            bool ok;
            QmlStringConverters::dateTimeFromString(string, &ok);
            if (!ok) COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: datetime expected"));
            }
            break;
        case QVariant::Point:
        case QVariant::PointF:
            {
            bool ok;
            QPointF point = QmlStringConverters::pointFFromString(string, &ok);
            if (!ok) COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: point expected"));
            }
            break;
        case QVariant::Size:
        case QVariant::SizeF:
            {
            bool ok;
            QSizeF size = QmlStringConverters::sizeFFromString(string, &ok);
            if (!ok) COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: size expected"));
            }
            break;
        case QVariant::Rect:
        case QVariant::RectF:
            {
            bool ok;
            QRectF rect = QmlStringConverters::rectFFromString(string, &ok);
            if (!ok) COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: rect expected"));
            }
            break;
        case QVariant::Bool:
            {
            if (!v->value.isBoolean()) COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: boolean expected"));
            }
            break;
        case QVariant::Vector3D:
            {
            bool ok;
            QmlStringConverters::vector3DFromString(string, &ok);
            if (!ok) COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: 3D vector expected"));
            }
            break;
        default:
            {
            int t = prop.userType();
            QmlMetaType::StringConverter converter =
                QmlMetaType::customStringConverter(t);
            if (!converter)
                COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: unsupported type \"%1\"").arg(QString::fromLatin1(QVariant::typeToName(prop.type()))));
            }
            break;
    }
    return true;
}

/*!
    Generate a store instruction for assigning literal \a v to property \a prop.

    Any literal assignment that is approved in testLiteralAssignment() must have
    a corresponding action in this method.
*/
void QmlCompiler::genLiteralAssignment(const QMetaProperty &prop,
                                       QmlParser::Value *v)
{
    QString string = v->value.asScript();

    QmlInstruction instr;
    instr.line = v->location.start.line;
    if (prop.isEnumType()) {
        int value;
        if (prop.isFlagType()) {
            value = prop.enumerator().keysToValue(string.toUtf8().constData());
        } else
            value = prop.enumerator().keyToValue(string.toUtf8().constData());

        instr.type = QmlInstruction::StoreInteger;
        instr.storeInteger.propertyIndex = prop.propertyIndex();
        instr.storeInteger.value = value;
        output->bytecode << instr;
        return;
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
            QUrl u = string.isEmpty() ? QUrl() : output->url.resolved(QUrl(string));
            instr.storeUrl.propertyIndex = prop.propertyIndex();
            instr.storeUrl.value = output->indexForUrl(u);
            }
            break;
        case QVariant::UInt:
            {
            instr.type = QmlInstruction::StoreInteger;
            instr.storeInteger.propertyIndex = prop.propertyIndex();
            instr.storeInteger.value = string.toUInt();
            }
            break;
        case QVariant::Int:
            {
            instr.type = QmlInstruction::StoreInteger;
            instr.storeInteger.propertyIndex = prop.propertyIndex();
            instr.storeInteger.value = string.toInt();
            }
            break;
        case QMetaType::Float:
            {
            instr.type = QmlInstruction::StoreFloat;
            instr.storeFloat.propertyIndex = prop.propertyIndex();
            instr.storeFloat.value = string.toFloat();
            }
            break;
        case QVariant::Double:
            {
            instr.type = QmlInstruction::StoreDouble;
            instr.storeDouble.propertyIndex = prop.propertyIndex();
            instr.storeDouble.value = string.toDouble();
            }
            break;
        case QVariant::Color:
            {
            QColor c = QmlStringConverters::colorFromString(string);
            instr.type = QmlInstruction::StoreColor;
            instr.storeColor.propertyIndex = prop.propertyIndex();
            instr.storeColor.value = c.rgba();
            }
            break;
        case QVariant::Date:
            {
            QDate d = QmlStringConverters::dateFromString(string);
            instr.type = QmlInstruction::StoreDate;
            instr.storeDate.propertyIndex = prop.propertyIndex();
            instr.storeDate.value = d.toJulianDay();
            }
            break;
        case QVariant::Time:
            {
            QTime time = QmlStringConverters::timeFromString(string);
            int data[] = { time.hour(), time.minute(),
                           time.second(), time.msec() };
            int index = output->indexForInt(data, 4);
            instr.type = QmlInstruction::StoreTime;
            instr.storeTime.propertyIndex = prop.propertyIndex();
            instr.storeTime.valueIndex = index;
            }
            break;
        case QVariant::DateTime:
            {
            QDateTime dateTime = QmlStringConverters::dateTimeFromString(string);
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
            QPointF point =
                QmlStringConverters::pointFFromString(string, &ok);
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
            bool b = v->value.asBoolean();
            instr.type = QmlInstruction::StoreBool;
            instr.storeBool.propertyIndex = prop.propertyIndex();
            instr.storeBool.value = b;
            }
            break;
        case QVariant::Vector3D:
            {
            bool ok;
            QVector3D vector =
                QmlStringConverters::vector3DFromString(string, &ok);
            float data[] = { vector.x(), vector.y(), vector.z() };
            int index = output->indexForFloat(data, 3);
            instr.type = QmlInstruction::StoreVector3D;
            instr.storeRealPair.propertyIndex = prop.propertyIndex();
            instr.storeRealPair.valueIndex = index;
            }
            break;
        default:
            {
            int t = prop.userType();
            int index = output->customTypeData.count();
            instr.type = QmlInstruction::AssignCustomType;
            instr.assignCustomType.propertyIndex = prop.propertyIndex();
            instr.assignCustomType.valueIndex = index;

            QmlCompiledData::CustomTypeData data;
            data.index = output->indexForString(string);
            data.type = t;
            output->customTypeData << data;
            }
            break;
    }
    output->bytecode << instr;
}

/*!
    Resets data by clearing the lists that the QmlCompiler modifies.
*/
void QmlCompiler::reset(QmlCompiledData *data)
{
    data->types.clear();
    data->primitives.clear();
    data->floatData.clear();
    data->intData.clear();
    data->customTypeData.clear();
    data->datas.clear();
    data->bytecode.clear();
}

/*!
    Compile \a unit, and store the output in \a out.  \a engine is the QmlEngine
    with which the QmlCompiledData will be associated.

    Returns true on success, false on failure.  On failure, the compile errors
    are available from errors().

    If the environment variant QML_COMPILER_DUMP is set
    (eg. QML_COMPILER_DUMP=1) the compiled instructions will be dumped to stderr
    on a successful compiler.
*/
bool QmlCompiler::compile(QmlEngine *engine,
                          QmlCompositeTypeData *unit,
                          QmlCompiledData *out)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QmlPerfTimer<QmlPerf::Compilation> pc;
#endif
    exceptions.clear();

    Q_ASSERT(out);
    reset(out);

    output = out;

    // Compile types
    for (int ii = 0; ii < unit->types.count(); ++ii) {
        QmlCompositeTypeData::TypeReference &tref = unit->types[ii];
        QmlCompiledData::TypeReference ref;
        QmlScriptParser::TypeReference *parserRef = unit->data.referencedTypes().at(ii);
        if (tref.type)
            ref.type = tref.type;
        else if (tref.unit) {
            ref.component = tref.unit->toComponent(engine);

            if (ref.component->isError()) {
                QmlError error;
                error.setUrl(output->url);
                error.setDescription(QLatin1String("Unable to create type ") +
                                     parserRef->name);
                if (!parserRef->refObjects.isEmpty()) {
                    QmlParser::Object *parserObject = parserRef->refObjects.first();
                    error.setLine(parserObject->location.start.line);
                    error.setColumn(parserObject->location.start.column);
                }

                exceptions << error;
                exceptions << ref.component->errors();
                reset(out);
                return false;
            }
            ref.ref = tref.unit;
            ref.ref->addref();
        }
        ref.className = parserRef->name.toUtf8();
        out->types << ref;
    }

    Object *root = unit->data.tree();
    Q_ASSERT(root);

    this->engine = engine;
    this->unit = unit;
    this->unitRoot = root;
    compileTree(root);

    if (!isError()) {
        if (compilerDump())
            out->dumpInstructions();
        if (compilerStatDump())
            dumpStats();
    } else {
        reset(out);
    }

    compileState = ComponentCompileState();
    savedCompileStates.clear();
    output = 0;
    this->engine = 0;
    this->unit = 0;
    this->unitRoot = 0;

    return !isError();
}

void QmlCompiler::compileTree(Object *tree)
{
    compileState.root = tree;

    if (!buildObject(tree, BindingContext()) || !completeComponentBuild())
        return;

    QmlInstruction init;
    init.type = QmlInstruction::Init;
    init.line = 0;
    init.init.bindingsSize = compileState.bindings.count();
    init.init.parserStatusSize = compileState.parserStatusCount;
    init.init.contextCache = genContextCache();
    if (compileState.compiledBindingData.isEmpty())
        init.init.compiledBinding = -1;
    else
        init.init.compiledBinding = output->indexForByteArray(compileState.compiledBindingData);
    output->bytecode << init;

    genObject(tree);

    QmlInstruction def;
    init.line = 0;
    def.type = QmlInstruction::SetDefault;
    output->bytecode << def;

    output->imports = unit->imports;
    output->importCache = output->imports.cache(engine);

    Q_ASSERT(tree->metatype);

    if (tree->metadata.isEmpty()) {
        output->root = tree->metatype;
    } else {
        static_cast<QMetaObject &>(output->rootData) = *tree->metaObject();
        output->root = &output->rootData;
    }
    if (!tree->metadata.isEmpty()) 
        QmlEnginePrivate::get(engine)->registerCompositeType(output);
}

static bool ValuePtrLessThan(const Value *t1, const Value *t2) 
{
    return t1->location.start.line < t2->location.start.line ||
           (t1->location.start.line == t2->location.start.line &&
            t1->location.start.column < t2->location.start.column);
}

bool QmlCompiler::buildObject(Object *obj, const BindingContext &ctxt)
{
    componentStat.objects++;

    Q_ASSERT (obj->type != -1);
    const QmlCompiledData::TypeReference &tr =
        output->types.at(obj->type);
    obj->metatype = tr.metaObject();

    if (tr.component)
        obj->url = tr.component->url();
    if (tr.type)
        obj->typeName = tr.type->qmlTypeName();
    obj->className = tr.className;

    // This object is a "Component" element
    if (tr.type && obj->metatype == &QmlComponent::staticMetaObject) {
        COMPILE_CHECK(buildComponent(obj, ctxt));
        return true;
    } 

    // Build any script blocks for this type
    for (int ii = 0; ii < obj->scriptBlockObjects.count(); ++ii)
        COMPILE_CHECK(buildScript(obj, obj->scriptBlockObjects.at(ii)));

    // Object instantiations reset the binding context
    BindingContext objCtxt(obj);

    // Create the synthesized meta object, ignoring aliases
    COMPILE_CHECK(mergeDynamicMetaProperties(obj));
    COMPILE_CHECK(buildDynamicMeta(obj, IgnoreAliases));

    // Find the native type and check for the QmlParserStatus interface
    QmlType *type = toQmlType(obj);
    Q_ASSERT(type);
    obj->parserStatusCast = type->parserStatusCast();
    if (obj->parserStatusCast != -1)
        compileState.parserStatusCount++;

    // Check if this is a custom parser type.  Custom parser types allow
    // assignments to non-existant properties.  These assignments are then
    // compiled by the type.
    bool isCustomParser = output->types.at(obj->type).type &&
                          output->types.at(obj->type).type->customParser() != 0;
    QList<QmlCustomParserProperty> customProps;

    // Fetch the list of deferred properties
    QStringList deferredList = deferredProperties(obj);

    // Must do id property first.  This is to ensure that the id given to any
    // id reference created matches the order in which the objects are
    // instantiated
    foreach(Property *prop, obj->properties) {
        if (prop->name == "id") {
            COMPILE_CHECK(buildProperty(prop, obj, objCtxt));
            break;
        }
    }

    // Merge 
    Property *defaultProperty = 0;
    Property *skipProperty = 0;
    if (obj->defaultProperty) {
        const QMetaObject *metaObject = obj->metaObject();
        Q_ASSERT(metaObject);
        QMetaProperty p = QmlMetaType::defaultProperty(metaObject);
        if (p.name()) {
            Property *explicitProperty = obj->getProperty(p.name(), false);
            if (explicitProperty && !explicitProperty->value) {
                skipProperty = explicitProperty;

                defaultProperty = new Property;
                defaultProperty->parent = obj;
                defaultProperty->isDefault = true;
                defaultProperty->location = obj->defaultProperty->location;
                defaultProperty->listValueRange = obj->defaultProperty->listValueRange;
                defaultProperty->listCommaPositions = obj->defaultProperty->listCommaPositions;

                defaultProperty->values  = obj->defaultProperty->values;
                defaultProperty->values += explicitProperty->values;
                foreach(Value *value, defaultProperty->values)
                    value->addref();
                qSort(defaultProperty->values.begin(), defaultProperty->values.end(), ValuePtrLessThan);

            } else {
                defaultProperty = obj->defaultProperty;
                defaultProperty->addref();
            }
        } else {
            defaultProperty = obj->defaultProperty;
            defaultProperty->addref();
        }
    }

    // Build all explicit properties specified
    foreach(Property *prop, obj->properties) {

        if (prop == skipProperty)
            continue;
        if (prop->name == "id")
            continue;

        bool canDefer = false;
        if (isCustomParser) {
            if (doesPropertyExist(prop, obj)) {
                int ids = compileState.ids.count();
                COMPILE_CHECK(buildProperty(prop, obj, objCtxt));
                canDefer = ids == compileState.ids.count();
            } else {
                customProps << QmlCustomParserNodePrivate::fromProperty(prop);
            }
        } else {
            if (isSignalPropertyName(prop->name)) {
                COMPILE_CHECK(buildSignal(prop,obj,objCtxt));
            } else {
                int ids = compileState.ids.count();
                COMPILE_CHECK(buildProperty(prop, obj, objCtxt));
                canDefer = ids == compileState.ids.count();
            }
        }

        if (canDefer && !deferredList.isEmpty() &&
            deferredList.contains(QString::fromUtf8(prop->name)))
            prop->isDeferred = true;

    }

    // Build the default property
    if (defaultProperty)  {
        Property *prop = defaultProperty;

        bool canDefer = false;
        if (isCustomParser) {
            if (doesPropertyExist(prop, obj)) {
                int ids = compileState.ids.count();
                COMPILE_CHECK(buildProperty(prop, obj, objCtxt));
                canDefer = ids == compileState.ids.count();
            } else {
                customProps << QmlCustomParserNodePrivate::fromProperty(prop);
            }
        } else {
            int ids = compileState.ids.count();
            COMPILE_CHECK(buildProperty(prop, obj, objCtxt));
            canDefer = ids == compileState.ids.count();
        }

        if (canDefer && !deferredList.isEmpty() &&
            deferredList.contains(QString::fromUtf8(prop->name)))
            prop->isDeferred = true;
    }

    if (defaultProperty) 
        defaultProperty->release();

    // Compile custom parser parts
    if (isCustomParser && !customProps.isEmpty()) {
        QmlCustomParser *cp = output->types.at(obj->type).type->customParser();
        cp->clearErrors();
        obj->custom = cp->compile(customProps);
        foreach (QmlError err, cp->errors()) {
            err.setUrl(output->url);
            exceptions << err;
        }
    }

    return true;
}

void QmlCompiler::genObject(QmlParser::Object *obj)
{
    const QmlCompiledData::TypeReference &tr =
        output->types.at(obj->type);
    if (tr.type && obj->metatype == &QmlComponent::staticMetaObject) {
        genComponent(obj);
        return;
    }

    // Create the object
    QmlInstruction create;
    create.type = QmlInstruction::CreateObject;
    create.line = obj->location.start.line;
    create.create.column = obj->location.start.column;
    create.create.data = -1;
    if (!obj->custom.isEmpty())
        create.create.data = output->indexForByteArray(obj->custom);
    create.create.type = obj->type;
    if (!output->types.at(create.create.type).type && 
        !obj->bindingBitmask.isEmpty()) {
        Q_ASSERT(obj->bindingBitmask.size() % 4 == 0);
        create.create.bindingBits = 
            output->indexForByteArray(obj->bindingBitmask);
    } else {
        create.create.bindingBits = -1;
    }
    output->bytecode << create;

    // Setup the synthesized meta object if necessary
    if (!obj->metadata.isEmpty()) {
        QmlInstruction meta;
        meta.type = QmlInstruction::StoreMetaObject;
        meta.line = 0;
        meta.storeMeta.data = output->indexForByteArray(obj->metadata);
        meta.storeMeta.aliasData = output->indexForByteArray(obj->synthdata);
        meta.storeMeta.propertyCache = output->propertyCaches.count();
        // ### Surely the creation of this property cache could be more efficient
        QmlPropertyCache *propertyCache = 0;
        if (tr.component && QmlComponentPrivate::get(tr.component)->cc->rootPropertyCache) {
            propertyCache = QmlComponentPrivate::get(tr.component)->cc->rootPropertyCache->copy();
        } else {
            propertyCache = QmlPropertyCache::create(engine, obj->metaObject()->superClass());
        }
        propertyCache->append(engine, obj->metaObject(), QmlPropertyCache::Data::NoFlags,
                              QmlPropertyCache::Data::IsVMEFunction);
        if (obj == unitRoot) {
            propertyCache->addref();
            output->rootPropertyCache = propertyCache;
        }
        output->propertyCaches << propertyCache;
        output->bytecode << meta;
    }

    // Set the object id
    if (!obj->id.isEmpty()) {
        QmlInstruction id;
        id.type = QmlInstruction::SetId;
        id.line = 0;
        id.setId.value = output->indexForString(obj->id);
        id.setId.index = obj->idIndex;
        output->bytecode << id;
    }

    // Set any script blocks
    for (int ii = 0; ii < obj->scripts.count(); ++ii) {
        QmlInstruction script;
        script.type = QmlInstruction::StoreScript;
        script.line = 0; // ###
        int idx = output->scripts.count();
        output->scripts << obj->scripts.at(ii);
        script.storeScript.value = idx;
        output->bytecode << script;
    }

    // Begin the class
    if (obj->parserStatusCast != -1) {
        QmlInstruction begin;
        begin.type = QmlInstruction::BeginObject;
        begin.begin.castValue = obj->parserStatusCast;
        begin.line = obj->location.start.line;
        output->bytecode << begin;
    }

    genObjectBody(obj);
}

void QmlCompiler::genObjectBody(QmlParser::Object *obj)
{
    typedef QPair<Property *, int> PropPair;
    foreach(const PropPair &prop, obj->scriptStringProperties) {
        QmlInstruction ss;
        ss.type = QmlInstruction::StoreScriptString;
        ss.storeScriptString.propertyIndex = prop.first->index;
        ss.storeScriptString.value = 
            output->indexForString(prop.first->values.at(0)->value.asScript());
        ss.storeScriptString.scope = prop.second;
        output->bytecode << ss;
    }

    bool seenDefer = false;
    foreach(Property *prop, obj->valueProperties) {
        if (prop->isDeferred) {
            seenDefer = true;
            continue;
        }
        genValueProperty(prop, obj);
    }
    if (seenDefer) {
        QmlInstruction defer;
        defer.type = QmlInstruction::Defer;
        defer.line = 0;
        defer.defer.deferCount = 0;
        int deferIdx = output->bytecode.count();
        output->bytecode << defer;

        QmlInstruction init;
        init.type = QmlInstruction::Init;
        init.init.bindingsSize = compileState.bindings.count(); // XXX - bigger than necessary
        init.init.parserStatusSize = compileState.parserStatusCount; // XXX - bigger than necessary
        init.init.contextCache = -1;
        init.init.compiledBinding = -1;
        output->bytecode << init;

        foreach(Property *prop, obj->valueProperties) {
            if (!prop->isDeferred)
                continue;
            genValueProperty(prop, obj);
        }

        output->bytecode[deferIdx].defer.deferCount =
            output->bytecode.count() - deferIdx - 1;
    }

    foreach(Property *prop, obj->signalProperties) {

        QmlParser::Value *v = prop->values.at(0);

        if (v->type == Value::SignalObject) {

            genObject(v->object);

            QmlInstruction assign;
            assign.type = QmlInstruction::AssignSignalObject;
            assign.line = v->location.start.line;
            assign.assignSignalObject.signal =
                output->indexForByteArray(prop->name);
            output->bytecode << assign;

        } else if (v->type == Value::SignalExpression) {

            QmlInstruction store;
            store.type = QmlInstruction::StoreSignal;
            store.line = v->location.start.line;
            store.storeSignal.signalIndex = prop->index;
            store.storeSignal.value =
                output->indexForString(v->value.asScript().trimmed());
            output->bytecode << store;

        }

    }

    foreach(Property *prop, obj->attachedProperties) {
        QmlInstruction fetch;
        fetch.type = QmlInstruction::FetchAttached;
        fetch.line = prop->location.start.line;
        fetch.fetchAttached.id = prop->index;
        output->bytecode << fetch;

        genObjectBody(prop->value);

        QmlInstruction pop;
        pop.type = QmlInstruction::PopFetchedObject;
        pop.line = prop->location.start.line;
        output->bytecode << pop;
    }

    foreach(Property *prop, obj->groupedProperties) {
        QmlInstruction fetch;
        fetch.type = QmlInstruction::FetchObject;
        fetch.fetch.property = prop->index;
        fetch.line = prop->location.start.line;
        output->bytecode << fetch;

        genObjectBody(prop->value);

        QmlInstruction pop;
        pop.type = QmlInstruction::PopFetchedObject;
        pop.line = prop->location.start.line;
        output->bytecode << pop;
    }

    foreach(Property *prop, obj->valueTypeProperties) {
        QmlInstruction fetch;
        fetch.type = QmlInstruction::FetchValueType;
        fetch.fetchValue.property = prop->index;
        fetch.fetchValue.type = prop->type;
        fetch.line = prop->location.start.line;

        output->bytecode << fetch;

        foreach(Property *vprop, prop->value->valueProperties) {
            genPropertyAssignment(vprop, prop->value, prop);
        }

        QmlInstruction pop;
        pop.type = QmlInstruction::PopValueType;
        pop.fetchValue.property = prop->index;
        pop.fetchValue.type = prop->type;
        pop.line = prop->location.start.line;
        output->bytecode << pop;
    }
}

void QmlCompiler::genComponent(QmlParser::Object *obj)
{
    QmlParser::Object *root = obj->defaultProperty->values.at(0)->object;
    Q_ASSERT(root);

    QmlInstruction create;
    create.type = QmlInstruction::CreateComponent;
    create.line = root->location.start.line;
    create.createComponent.column = root->location.start.column;
    create.createComponent.endLine = root->location.end.line;
    output->bytecode << create;
    int count = output->bytecode.count();

    ComponentCompileState oldCompileState = compileState;
    compileState = componentState(root);

    QmlInstruction init;
    init.type = QmlInstruction::Init;
    init.init.bindingsSize = compileState.bindings.count();
    init.init.parserStatusSize = compileState.parserStatusCount;
    init.init.contextCache = genContextCache();
    if (compileState.compiledBindingData.isEmpty())
        init.init.compiledBinding = -1;
    else
        init.init.compiledBinding = output->indexForByteArray(compileState.compiledBindingData);
    init.line = obj->location.start.line;
    output->bytecode << init;

    genObject(root);

    QmlInstruction def;
    init.line = 0;
    def.type = QmlInstruction::SetDefault;
    output->bytecode << def;

    output->bytecode[count - 1].createComponent.count =
        output->bytecode.count() - count;

    compileState = oldCompileState;

    if (!obj->id.isEmpty()) {
        QmlInstruction id;
        id.type = QmlInstruction::SetId;
        id.line = 0;
        id.setId.value = output->indexForString(obj->id);
        id.setId.index = obj->idIndex;
        output->bytecode << id;
    }
}

bool QmlCompiler::buildComponent(QmlParser::Object *obj,
                                 const BindingContext &ctxt)
{
    // The special "Component" element can only have the id property and a
    // default property, that actually defines the component's tree

    // Find, check and set the "id" property (if any)
    Property *idProp = 0;
    if (obj->properties.count() > 1 ||
       (obj->properties.count() == 1 && obj->properties.begin().key() != "id"))
        COMPILE_EXCEPTION(*obj->properties.begin(), QCoreApplication::translate("QmlCompiler","Invalid component specification"));
       
    if (!obj->scriptBlockObjects.isEmpty())
        COMPILE_EXCEPTION(obj->scriptBlockObjects.first(), QCoreApplication::translate("QmlCompiler","Invalid component specification"));

    if (obj->properties.count())
        idProp = *obj->properties.begin();

    if (idProp && (idProp->value || idProp->values.count() > 1 || !isValidId(idProp->values.first()->primitive())))
        COMPILE_EXCEPTION(idProp, QCoreApplication::translate("QmlCompiler","Invalid component id specification"));

    if (idProp) {
        QString idVal = idProp->values.first()->primitive();

        if (compileState.ids.contains(idVal))
            COMPILE_EXCEPTION(idProp, QCoreApplication::translate("QmlCompiler","id is not unique"));

        obj->id = idVal;
        addId(idVal, obj);
    }

    // Check the Component tree is well formed
    if (obj->defaultProperty &&
       (obj->defaultProperty->value || obj->defaultProperty->values.count() > 1 ||
        (obj->defaultProperty->values.count() == 1 && !obj->defaultProperty->values.first()->object)))
        COMPILE_EXCEPTION(obj, QCoreApplication::translate("QmlCompiler","Invalid component body specification"));

    Object *root = 0;
    if (obj->defaultProperty && obj->defaultProperty->values.count())
        root = obj->defaultProperty->values.first()->object;

    if (!root)
        COMPILE_EXCEPTION(obj, QCoreApplication::translate("QmlCompiler","Cannot create empty component specification"));

    // Build the component tree
    COMPILE_CHECK(buildComponentFromRoot(root, ctxt));

    return true;
}

bool QmlCompiler::buildScript(QmlParser::Object *obj, QmlParser::Object *script)
{
    Object::ScriptBlock scriptBlock;

    if (script->properties.count() == 1 && 
        script->properties.begin().key() == QByteArray("source")) {

        Property *source = *script->properties.begin();
        if (script->defaultProperty)
            COMPILE_EXCEPTION(source, QCoreApplication::translate("QmlCompiler","Invalid Script block.  Specify either the source property or inline script"));

        if (source->value || source->values.count() != 1 ||
            source->values.at(0)->object || !source->values.at(0)->value.isStringList())
            COMPILE_EXCEPTION(source, QCoreApplication::translate("QmlCompiler","Invalid Script source value"));

        QStringList sources = source->values.at(0)->value.asStringList();

        for (int jj = 0; jj < sources.count(); ++jj) {
            QString sourceUrl = output->url.resolved(QUrl(sources.at(jj))).toString();
            QString scriptCode;
            int lineNumber = 1;

            for (int ii = 0; ii < unit->resources.count(); ++ii) {
                if (unit->resources.at(ii)->url == sourceUrl) {
                    scriptCode = QString::fromUtf8(unit->resources.at(ii)->data);
                    break;
                }
            }

            if (!scriptCode.isEmpty()) {
                scriptBlock.codes.append(scriptCode);
                scriptBlock.files.append(sourceUrl);
                scriptBlock.lineNumbers.append(lineNumber);
            }
        }

    } else if (!script->properties.isEmpty()) {
        COMPILE_EXCEPTION(*script->properties.begin(), QCoreApplication::translate("QmlCompiler","Properties cannot be set on Script block"));
    } else if (script->defaultProperty) {

        QString scriptCode;
        int lineNumber = 1;
        QString sourceUrl = output->url.toString();

        QmlParser::Location currentLocation;

        for (int ii = 0; ii < script->defaultProperty->values.count(); ++ii) {
            Value *v = script->defaultProperty->values.at(ii);
            if (lineNumber == 1)
                lineNumber = v->location.start.line;
            if (v->object || !v->value.isString())
                COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid Script block"));

            if (ii == 0) {
                currentLocation = v->location.start;
                scriptCode.append(QString(currentLocation.column, QLatin1Char(' ')));
            }

            while (currentLocation.line < v->location.start.line) {
                scriptCode.append(QLatin1Char('\n'));
                currentLocation.line++;
                currentLocation.column = 0;
            }

            scriptCode.append(QString(v->location.start.column - currentLocation.column, QLatin1Char(' ')));

            scriptCode += v->value.asString();
            currentLocation = v->location.end;
            currentLocation.column++;
        }

        if (!scriptCode.isEmpty()) {
            scriptBlock.codes.append(scriptCode);
            scriptBlock.files.append(sourceUrl);
            scriptBlock.lineNumbers.append(lineNumber);
        }
    }

    if (!scriptBlock.codes.isEmpty())
        obj->scripts << scriptBlock;

    return true;
}

bool QmlCompiler::buildComponentFromRoot(QmlParser::Object *obj,
                                         const BindingContext &ctxt)
{
    ComponentCompileState oldComponentCompileState = compileState;
    ComponentStat oldComponentStat = componentStat;

    compileState = ComponentCompileState();
    compileState.root = obj;

    componentStat = ComponentStat();
    componentStat.lineNumber = obj->location.start.line;

    if (obj)
        COMPILE_CHECK(buildObject(obj, ctxt));

    COMPILE_CHECK(completeComponentBuild());

    compileState = oldComponentCompileState;
    componentStat = oldComponentStat;

    return true;
}


// Build a sub-object.  A sub-object is one that was not created directly by
// QML - such as a grouped property object, or an attached object.  Sub-object's
// can't have an id, involve a custom parser, have attached properties etc.
bool QmlCompiler::buildSubObject(Object *obj, const BindingContext &ctxt)
{
    Q_ASSERT(obj->metatype);
    Q_ASSERT(!obj->defaultProperty);
    Q_ASSERT(ctxt.isSubContext()); // sub-objects must always be in a binding
                                   // sub-context

    foreach(Property *prop, obj->properties) {
        if (isSignalPropertyName(prop->name)) {
            COMPILE_CHECK(buildSignal(prop, obj, ctxt));
        } else {
            COMPILE_CHECK(buildProperty(prop, obj, ctxt));
        }
    }

    return true;
}

int QmlCompiler::componentTypeRef()
{
    QmlType *t = QmlMetaType::qmlType("Qt/Component",4,6);
    for (int ii = output->types.count() - 1; ii >= 0; --ii) {
        if (output->types.at(ii).type == t)
            return ii;
    }
    QmlCompiledData::TypeReference ref;
    ref.className = "Component";
    ref.type = t;
    output->types << ref;
    return output->types.count() - 1;
}

int QmlCompiler::findSignalByName(const QMetaObject *mo, const QByteArray &name)
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

bool QmlCompiler::buildSignal(QmlParser::Property *prop, QmlParser::Object *obj,
                              const BindingContext &ctxt)
{
    Q_ASSERT(obj->metaObject());
    Q_ASSERT(!prop->isEmpty());

    QByteArray name = prop->name;
    Q_ASSERT(name.startsWith("on"));
    name = name.mid(2);
    if(name[0] >= 'A' && name[0] <= 'Z')
        name[0] = name[0] - 'A' + 'a';

    int sigIdx = findSignalByName(obj->metaObject(), name);

    if (sigIdx == -1) {

        // If the "on<Signal>" name doesn't resolve into a signal, try it as a
        // property.
        COMPILE_CHECK(buildProperty(prop, obj, ctxt));

    }  else {

        if (prop->value || prop->values.count() > 1)
            COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Incorrectly specified signal"));

        prop->index = sigIdx;
        obj->addSignalProperty(prop);

        if (prop->values.at(0)->object) {
            COMPILE_CHECK(buildObject(prop->values.at(0)->object, ctxt));
            prop->values.at(0)->type = Value::SignalObject;
        } else {
            prop->values.at(0)->type = Value::SignalExpression;

            QString script = prop->values.at(0)->value.asScript().trimmed();
            if (script.isEmpty())
                COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Empty signal assignment"));
        }
    }

    return true;
}


/*!
    Returns true if (value) property \a prop exists on obj, false otherwise.
*/
bool QmlCompiler::doesPropertyExist(QmlParser::Property *prop,
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

bool QmlCompiler::buildProperty(QmlParser::Property *prop,
                                QmlParser::Object *obj,
                                const BindingContext &ctxt)
{
    if (prop->isEmpty())
        COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Empty property assignment"));

    const QMetaObject *metaObject = obj->metaObject();
    Q_ASSERT(metaObject);

    if (isAttachedPropertyName(prop->name)) {
        // Setup attached property data

        if (ctxt.isSubContext()) {
            // Attached properties cannot be used on sub-objects.  Sub-objects
            // always exist in a binding sub-context, which is what we test
            // for here.
            COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Attached properties cannot be used here"));
        }

        QmlType *type = 0;
        QmlEnginePrivate::ImportedNamespace *typeNamespace = 0;
        QmlEnginePrivate::get(engine)->resolveType(unit->imports, prop->name, 
                                                   &type, 0, 0, 0, &typeNamespace);

        if (typeNamespace) {
            // ### We might need to indicate that this property is a namespace 
            // for the DOM API
            COMPILE_CHECK(buildPropertyInNamespace(typeNamespace, prop, obj, 
                                                   ctxt));
            return true;
        } else if (!type || !type->attachedPropertiesType())  {
            COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Non-existant attached object"));
        }

        if (!prop->value)
            COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Invalid attached object assignment"));

        Q_ASSERT(type->attachedPropertiesFunction());
        prop->index = type->index();
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
            prop->type = p.userType();
        }
    }

    if (prop->index != -1) 
        prop->parent->setBindingBit(prop->index);

    if (!prop->isDefault && prop->name == "id" && !ctxt.isSubContext()) {

        // The magic "id" behavior doesn't apply when "id" is resolved as a
        // default property or to sub-objects (which are always in binding
        // sub-contexts)
        COMPILE_CHECK(buildIdProperty(prop, obj));
        if (prop->type == QVariant::String &&
            prop->values.at(0)->value.isString())
            COMPILE_CHECK(buildPropertyAssignment(prop, obj, ctxt));

    } else if (isAttachedPropertyName(prop->name)) {

        COMPILE_CHECK(buildAttachedProperty(prop, obj, ctxt));

    } else if (prop->index == -1) {

        if (prop->isDefault) {
            COMPILE_EXCEPTION(prop->values.first(), QCoreApplication::translate("QmlCompiler","Cannot assign to non-existant default property"));
        } else {
            COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Cannot assign to non-existant property \"%1\"").arg(QString::fromUtf8(prop->name)));
        }

    } else if (prop->value) {

        COMPILE_CHECK(buildGroupedProperty(prop, obj, ctxt));

    } else if (QmlEnginePrivate::get(engine)->isQmlList(prop->type) ||
               QmlMetaType::isList(prop->type)) {

        COMPILE_CHECK(buildListProperty(prop, obj, ctxt));

    } else if (prop->type == qMetaTypeId<QmlScriptString>()) {

        COMPILE_CHECK(buildScriptStringProperty(prop, obj, ctxt));

    } else {

        COMPILE_CHECK(buildPropertyAssignment(prop, obj, ctxt));

    }

    return true;
}

bool 
QmlCompiler::buildPropertyInNamespace(QmlEnginePrivate::ImportedNamespace *ns,
                                      QmlParser::Property *nsProp, 
                                      QmlParser::Object *obj, 
                                      const BindingContext &ctxt)
{
    if (!nsProp->value)
        COMPILE_EXCEPTION(nsProp, QCoreApplication::translate("QmlCompiler","Invalid use of namespace"));

    foreach (Property *prop, nsProp->value->properties) {

        if (!isAttachedPropertyName(prop->name))
            COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Not an attached property name"));

        // Setup attached property data

        QmlType *type = 0;
        QmlEnginePrivate::get(engine)->resolveTypeInNamespace(ns, prop->name,
                                                              &type, 0, 0, 0);

        if (!type || !type->attachedPropertiesType()) 
            COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Non-existant attached object"));

        if (!prop->value)
            COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Invalid attached object assignment"));

        Q_ASSERT(type->attachedPropertiesFunction());
        prop->index = type->index();
        prop->value->metatype = type->attachedPropertiesType();

        COMPILE_CHECK(buildAttachedProperty(prop, obj, ctxt));
    }

    return true;
}

void QmlCompiler::genValueProperty(QmlParser::Property *prop,
                                   QmlParser::Object *obj)
{
    if (QmlEnginePrivate::get(engine)->isQmlList(prop->type) || 
        QmlMetaType::isList(prop->type)) {
        genListProperty(prop, obj);
    } else {
        genPropertyAssignment(prop, obj);
    }
}

void QmlCompiler::genListProperty(QmlParser::Property *prop,
                                  QmlParser::Object *obj)
{
    QmlInstruction::Type fetchType;
    QmlInstruction::Type storeType;
    int listType;

    if (QmlEnginePrivate::get(engine)->isQmlList(prop->type)) {
        fetchType = QmlInstruction::FetchQmlList;
        storeType = QmlInstruction::StoreObjectQmlList;
        listType = QmlEnginePrivate::get(engine)->qmlListType(prop->type);
    } else {
        fetchType = QmlInstruction::FetchQList;
        storeType = QmlInstruction::StoreObjectQList;
        listType = QmlMetaType::listType(prop->type);
    }

    QmlInstruction fetch;
    fetch.type = fetchType;
    fetch.line = prop->location.start.line;
    fetch.fetchQmlList.property = prop->index;
    bool listTypeIsInterface = QmlMetaType::isInterface(listType);
    fetch.fetchQmlList.type = listType;
    output->bytecode << fetch;

    for (int ii = 0; ii < prop->values.count(); ++ii) {
        Value *v = prop->values.at(ii);

        if (v->type == Value::CreatedObject) {

            genObject(v->object);
            if (listTypeIsInterface) {
                QmlInstruction assign;
                assign.type = QmlInstruction::AssignObjectList;
                assign.line = prop->location.start.line;
                output->bytecode << assign;
            } else {
                QmlInstruction store;
                store.type = storeType;
                store.line = prop->location.start.line;
                output->bytecode << store;
            }

        } else if (v->type == Value::PropertyBinding) {

            genBindingAssignment(v, prop, obj);

        }

    }

    QmlInstruction pop;
    pop.type = QmlInstruction::PopQList;
    pop.line = prop->location.start.line;
    output->bytecode << pop;
}

void QmlCompiler::genPropertyAssignment(QmlParser::Property *prop,
                                        QmlParser::Object *obj,
                                        QmlParser::Property *valueTypeProperty)
{
    for (int ii = 0; ii < prop->values.count(); ++ii) {
        QmlParser::Value *v = prop->values.at(ii);

        if (v->type == Value::CreatedObject) {

            genObject(v->object);

            if (QmlMetaType::isInterface(prop->type)) {

                QmlInstruction store;
                store.type = QmlInstruction::StoreInterface;
                store.line = v->object->location.start.line;
                store.storeObject.propertyIndex = prop->index;
                output->bytecode << store;

            } else if (prop->type == -1) {

                QmlInstruction store;
                store.type = QmlInstruction::StoreVariantObject;
                store.line = v->object->location.start.line;
                store.storeObject.propertyIndex = prop->index;
                output->bytecode << store;

            } else {

                QmlInstruction store;
                store.type = QmlInstruction::StoreObject;
                store.line = v->object->location.start.line;
                store.storeObject.propertyIndex = prop->index;
                output->bytecode << store;

            }
        } else if (v->type == Value::ValueSource) {
            genObject(v->object);

            QmlInstruction store;
            store.type = QmlInstruction::StoreValueSource;
            store.line = v->object->location.start.line;
            if (valueTypeProperty) {
                store.assignValueSource.property = genValueTypeData(prop, valueTypeProperty);
                store.assignValueSource.owner = 1;
            } else {
                store.assignValueSource.property = genPropertyData(prop);
                store.assignValueSource.owner = 0;
            }
            QmlType *valueType = toQmlType(v->object);
            store.assignValueSource.castValue = valueType->propertyValueSourceCast();
            output->bytecode << store;

        } else if (v->type == Value::ValueInterceptor) {
            genObject(v->object);

            QmlInstruction store;
            store.type = QmlInstruction::StoreValueInterceptor;
            store.line = v->object->location.start.line;
            if (valueTypeProperty) {
                store.assignValueInterceptor.property = genValueTypeData(prop, valueTypeProperty);
                store.assignValueInterceptor.owner = 1;
            } else {
                store.assignValueInterceptor.property = genPropertyData(prop);
                store.assignValueInterceptor.owner = 0;
            }
            QmlType *valueType = toQmlType(v->object);
            store.assignValueInterceptor.castValue = valueType->propertyValueInterceptorCast();
            output->bytecode << store;

        } else if (v->type == Value::PropertyBinding) {

            genBindingAssignment(v, prop, obj, valueTypeProperty);

        } else if (v->type == Value::Literal) {

            QMetaProperty mp = obj->metaObject()->property(prop->index);
            genLiteralAssignment(mp, v);

        }

    }
}

bool QmlCompiler::buildIdProperty(QmlParser::Property *prop,
                                  QmlParser::Object *obj)
{
    if (prop->value ||
        prop->values.count() > 1 ||
        prop->values.at(0)->object)
        COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Invalid use of id property"));

    QmlParser::Value *idValue = prop->values.at(0);
    QString val = idValue->primitive();

    if (!isValidId(val))
        COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","\"%1\" is not a valid object id").arg(val));

    // We disallow id's that conflict with import prefixes and types
    QmlEnginePrivate::ImportedNamespace *ns = 0;
    QmlType *type = 0;
    QmlEnginePrivate::get(engine)->resolveType(unit->imports, val.toUtf8(), 
                                               &type, 0, 0, 0, &ns);
    if (type)
        COMPILE_EXCEPTION(idValue, QCoreApplication::translate("QmlCompiler","id conflicts with type name"));
    if (ns)
        COMPILE_EXCEPTION(idValue, QCoreApplication::translate("QmlCompiler","id conflicts with namespace prefix"));

    if (compileState.ids.contains(val))
        COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","id is not unique"));

    prop->values.at(0)->type = Value::Id;

    obj->id = val;
    addId(val, obj);

    return true;
}

void QmlCompiler::addId(const QString &id, QmlParser::Object *obj)
{
    Q_ASSERT(!compileState.ids.contains(id));
    Q_ASSERT(obj->id == id);
    obj->idIndex = compileState.ids.count();
    compileState.ids.insert(id, obj);
    compileState.idIndexes.insert(obj->idIndex, obj);
}

void QmlCompiler::addBindingReference(const BindingReference &ref)
{
    Q_ASSERT(ref.value && !compileState.bindings.contains(ref.value));
    compileState.bindings.insert(ref.value, ref);
}

void QmlCompiler::saveComponentState()
{
    Q_ASSERT(compileState.root);
    Q_ASSERT(!savedCompileStates.contains(compileState.root));

    savedCompileStates.insert(compileState.root, compileState);
    savedComponentStats.append(componentStat);
}

QmlCompiler::ComponentCompileState
QmlCompiler::componentState(QmlParser::Object *obj)
{
    Q_ASSERT(savedCompileStates.contains(obj));
    return savedCompileStates.value(obj);
}

// Build attached property object.  In this example,
// Text {
//    GridView.row: 10
// }
// GridView is an attached property object.
bool QmlCompiler::buildAttachedProperty(QmlParser::Property *prop,
                                        QmlParser::Object *obj,
                                        const BindingContext &ctxt)
{
    Q_ASSERT(prop->value);
    Q_ASSERT(prop->index != -1); // This is set in buildProperty()

    obj->addAttachedProperty(prop);

    COMPILE_CHECK(buildSubObject(prop->value, ctxt.incr()));

    return true;
}


// Build "grouped" properties. In this example:
// Text {
//     font.pointSize: 12
//     font.family: "Helvetica"
// }
// font is a nested property.  pointSize and family are not.
bool QmlCompiler::buildGroupedProperty(QmlParser::Property *prop,
                                       QmlParser::Object *obj,
                                       const BindingContext &ctxt)
{
    Q_ASSERT(prop->type != 0);
    Q_ASSERT(prop->index != -1);

    if (prop->values.count())
        COMPILE_EXCEPTION(prop->values.first(), QCoreApplication::translate("QmlCompiler", "Invalid value in grouped property"));

    if (prop->type < (int)QVariant::UserType) {
        QmlEnginePrivate *ep =
            static_cast<QmlEnginePrivate *>(QObjectPrivate::get(engine));
        if (prop->type >= 0 /* QVariant == -1 */ && ep->valueTypes[prop->type]) {
            COMPILE_CHECK(buildValueTypeProperty(ep->valueTypes[prop->type],
                                                 prop->value, obj, ctxt.incr()));
            obj->addValueTypeProperty(prop);
        } else {
            COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Invalid grouped property access"));
        }

    } else {
        // Load the nested property's meta type
        prop->value->metatype = 
            QmlEnginePrivate::get(engine)->metaObjectForType(prop->type);
        if (!prop->value->metatype)
            COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Invalid grouped property access"));

        obj->addGroupedProperty(prop);

        COMPILE_CHECK(buildSubObject(prop->value, ctxt.incr()));
    }

    return true;
}

bool QmlCompiler::buildValueTypeProperty(QObject *type,
                                         QmlParser::Object *obj,
                                         QmlParser::Object *baseObj,
                                         const BindingContext &ctxt)
{
    if (obj->defaultProperty)
        COMPILE_EXCEPTION(obj, QCoreApplication::translate("QmlCompiler","Invalid property use"));
    obj->metatype = type->metaObject();

    foreach (Property *prop, obj->properties) {
        int idx = type->metaObject()->indexOfProperty(prop->name.constData());
        if (idx == -1)
            COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Cannot assign to non-existant property \"%1\"").arg(QString::fromUtf8(prop->name)));
        QMetaProperty p = type->metaObject()->property(idx);
        prop->index = idx;
        prop->type = p.userType();

        if (prop->value)
            COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Property assignment expected"));

        if (prop->values.count() != 1)
            COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Single property assignment expected"));

        Value *value = prop->values.at(0);

        if (value->object) {
            bool isPropertyValue = output->types.at(value->object->type).type->propertyValueSourceCast() != -1;
            bool isPropertyInterceptor = output->types.at(value->object->type).type->propertyValueInterceptorCast() != -1;
            if (!isPropertyValue && !isPropertyInterceptor) {
                COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Unexpected object assignment"));
            } else {
                COMPILE_CHECK(buildObject(value->object, ctxt));

                if (isPropertyInterceptor && baseObj->synthdata.isEmpty())
                    buildDynamicMeta(baseObj, ForceCreation);
                value->type = isPropertyValue ? Value::ValueSource : Value::ValueInterceptor;
            }
        } else if (value->value.isScript()) {
            // ### Check for writability
            BindingReference reference;
            reference.expression = value->value;
            reference.property = prop;
            reference.value = value;
            reference.bindingContext = ctxt;
            reference.bindingContext.owner++;
            addBindingReference(reference);
            value->type = Value::PropertyBinding;
        } else  {
            COMPILE_CHECK(testLiteralAssignment(p, value));
            value->type = Value::Literal;
        }
        obj->addValueProperty(prop);
    }

    return true;
}

// Build assignments to QML lists.  QML lists are properties of type
// QList<T *> * and QmlList<T *> *.
//
// QList<T *> * types can accept a list of objects, or a single binding
// QmlList<T *> * types can accept a list of objects
bool QmlCompiler::buildListProperty(QmlParser::Property *prop,
                                    QmlParser::Object *obj,
                                    const BindingContext &ctxt)
{
    Q_ASSERT(QmlMetaType::isList(prop->type) ||
             QmlEnginePrivate::get(engine)->isQmlList(prop->type));

    int t = prop->type;

    obj->addValueProperty(prop);

    if (QmlEnginePrivate::get(engine)->isQmlList(t)) {
        int listType = QmlEnginePrivate::get(engine)->qmlListType(t);
        bool listTypeIsInterface = QmlMetaType::isInterface(listType);

        for (int ii = 0; ii < prop->values.count(); ++ii) {
            Value *v = prop->values.at(ii);
            if (v->object) {
                v->type = Value::CreatedObject;
                COMPILE_CHECK(buildObject(v->object, ctxt));

                // We check object coercian here.  We check interface assignment
                // at runtime.
                if (!listTypeIsInterface) {
                    if (!canCoerce(listType, v->object)) {
                        COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Cannot assign object to list"));
                    }
                }

            } else {
                COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Cannot assign primitives to lists"));
            }
        }

    } else {
        int listType = QmlMetaType::listType(t);
        bool listTypeIsInterface = QmlMetaType::isInterface(listType);

        bool assignedBinding = false;
        for (int ii = 0; ii < prop->values.count(); ++ii) {
            Value *v = prop->values.at(ii);
            if (v->object) {
                v->type = Value::CreatedObject;
                COMPILE_CHECK(buildObject(v->object, ctxt));

                // We check object coercian here.  We check interface assignment
                // at runtime.
                if (!listTypeIsInterface) {
                    if (!canCoerce(listType, v->object)) {
                        COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Cannot assign object to list"));
                    }
                }

            } else if (v->value.isScript()) {
                if (assignedBinding)
                    COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Can only assign one binding to lists"));

                assignedBinding = true;
                COMPILE_CHECK(buildBinding(v, prop, ctxt));
                v->type = Value::PropertyBinding;
            } else {
                COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Cannot assign primitives to lists"));
            }
        }

    }

    return true;
}

// Compiles an assignment to a QmlScriptString property
bool QmlCompiler::buildScriptStringProperty(QmlParser::Property *prop,
                                            QmlParser::Object *obj,
                                            const BindingContext &ctxt)
{
    if (prop->values.count() > 1) 
        COMPILE_EXCEPTION(prop->values.at(1), QCoreApplication::translate("QmlCompiler", "Cannot assign multiple values to a script property"));

    if (prop->values.at(0)->object || !prop->values.at(0)->value.isScript())
        COMPILE_EXCEPTION(prop->values.at(0), QCoreApplication::translate("QmlCompiler", "Invalid property assignment: script expected"));

    obj->addScriptStringProperty(prop, ctxt.stack);

    return true;
}

// Compile regular property assignments of the form "property: <value>"
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
bool QmlCompiler::buildPropertyAssignment(QmlParser::Property *prop,
                                          QmlParser::Object *obj,
                                          const BindingContext &ctxt)
{
    obj->addValueProperty(prop);

    for (int ii = 0; ii < prop->values.count(); ++ii) {
        Value *v = prop->values.at(ii);
        if (v->object) {

            COMPILE_CHECK(buildPropertyObjectAssignment(prop, obj, v, ctxt));

        } else {

            COMPILE_CHECK(buildPropertyLiteralAssignment(prop, obj, v, ctxt));

        }
    }

    return true;
}

// Compile assigning a single object instance to a regular property
bool QmlCompiler::buildPropertyObjectAssignment(QmlParser::Property *prop,
                                                QmlParser::Object *obj,
                                                QmlParser::Value *v,
                                                const BindingContext &ctxt)
{
    Q_ASSERT(prop->index != -1);
    Q_ASSERT(v->object->type != -1);

    if (!obj->metaObject()->property(prop->index).isWritable())
        COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: \"%1\" is a read-only property").arg(QString::fromUtf8(prop->name)));

    if (QmlMetaType::isInterface(prop->type)) {

        // Assigning an object to an interface ptr property
        COMPILE_CHECK(buildObject(v->object, ctxt));

        v->type = Value::CreatedObject;

    } else if (prop->type == -1) {

        // Assigning an object to a QVariant
        COMPILE_CHECK(buildObject(v->object, ctxt));

        v->type = Value::CreatedObject;
    } else {
        // Normally buildObject() will set this up, but we need the static
        // meta object earlier to test for assignability.  It doesn't matter
        // that there may still be outstanding synthesized meta object changes
        // on this type, as they are not relevant for assignability testing
        v->object->metatype = output->types.at(v->object->type).metaObject();
        Q_ASSERT(v->object->metaObject());

        // Will be true if the assigned type inherits QmlPropertyValueSource
        bool isPropertyValue = false;
        // Will be true if the assigned type inherits QmlPropertyValueInterceptor
        bool isPropertyInterceptor = false;
        if (QmlType *valueType = toQmlType(v->object)) {
            isPropertyValue = valueType->propertyValueSourceCast() != -1;
            isPropertyInterceptor = valueType->propertyValueInterceptorCast() != -1;
        }

        // We want to raw metaObject here as the raw metaobject is the
        // actual property type before we applied any extensions that might
        // effect the properties on the type, but don't effect assignability
        const QMetaObject *propertyMetaObject =
            QmlEnginePrivate::get(engine)->rawMetaObjectForType(prop->type);

        // Will be true if the assgned type inherits propertyMetaObject
        bool isAssignable = false;
        // Determine isAssignable value
        if (propertyMetaObject) {
            const QMetaObject *c = v->object->metatype;
            while(c) {
                isAssignable |= (QmlMetaPropertyPrivate::equal(c, propertyMetaObject));
                c = c->superClass();
            }
        }

        if (isAssignable) {
            // Simple assignment
            COMPILE_CHECK(buildObject(v->object, ctxt));

            v->type = Value::CreatedObject;
        } else if (propertyMetaObject == &QmlComponent::staticMetaObject) {
            // Automatic "Component" insertion
            QmlParser::Object *root = v->object;
            QmlParser::Object *component = new QmlParser::Object;
            component->type = componentTypeRef();
            component->typeName = "Qt/Component";
            component->metatype = &QmlComponent::staticMetaObject;
            component->location = root->location;
            QmlParser::Value *componentValue = new QmlParser::Value;
            componentValue->object = root;
            component->getDefaultProperty()->addValue(componentValue);
            v->object = component;
            COMPILE_CHECK(buildPropertyObjectAssignment(prop, obj, v, ctxt));
        } else if (isPropertyValue || isPropertyInterceptor) {
            // Assign as a property value source
            COMPILE_CHECK(buildObject(v->object, ctxt));

            if (isPropertyInterceptor && prop->parent->synthdata.isEmpty())
                   buildDynamicMeta(prop->parent, ForceCreation);
            v->type = isPropertyValue ? Value::ValueSource : Value::ValueInterceptor;
        } else {
            COMPILE_EXCEPTION(v->object, QCoreApplication::translate("QmlCompiler","Cannot assign object to property"));
        }
    }

    return true;
}

// Compile assigning a literal or binding to a regular property
bool QmlCompiler::buildPropertyLiteralAssignment(QmlParser::Property *prop,
                                                 QmlParser::Object *obj,
                                                 QmlParser::Value *v,
                                                 const BindingContext &ctxt)
{
    Q_ASSERT(prop->index != -1);

    if (v->value.isScript()) {

        //optimization for <Type>.<EnumValue> enum assignments
        bool isEnumAssignment = false;
        COMPILE_CHECK(testQualifiedEnumAssignment(obj->metaObject()->property(prop->index), obj, v, &isEnumAssignment));
        if (isEnumAssignment) {
            v->type = Value::Literal;
            return true;
        }

        COMPILE_CHECK(buildBinding(v, prop, ctxt));

        v->type = Value::PropertyBinding;

    } else {

        COMPILE_CHECK(testLiteralAssignment(obj->metaObject()->property(prop->index), v));

        v->type = Value::Literal;
    }

    return true;
}

bool QmlCompiler::testQualifiedEnumAssignment(const QMetaProperty &prop,
                                              QmlParser::Object *obj,
                                              QmlParser::Value *v,
                                              bool *isAssignment)
{
    *isAssignment = false;
    if (!prop.isEnumType())
        return true;

    if (!prop.isWritable())
        COMPILE_EXCEPTION(v, QCoreApplication::translate("QmlCompiler","Invalid property assignment: \"%1\" is a read-only property").arg(QString::fromUtf8(prop.name())));

    QString string = v->value.asString();
    if (!string.at(0).isUpper())
        return true;

    QStringList parts = string.split(QLatin1Char('.'));
    if (parts.count() != 2)
        return true;

    QString typeName = parts.at(0);
    QmlType *type = 0;
    QmlEnginePrivate::get(engine)->resolveType(unit->imports, typeName.toUtf8(),
                                               &type, 0, 0, 0, 0);

    if (!type || obj->typeName != type->qmlTypeName())
        return true;

    QString enumValue = parts.at(1);
    int value;
    if (prop.isFlagType()) {
        value = prop.enumerator().keysToValue(enumValue.toUtf8().constData());
    } else
        value = prop.enumerator().keyToValue(enumValue.toUtf8().constData());
    if (value == -1)
        return true;

    v->type = Value::Literal;
    v->value = QmlParser::Variant(enumValue);
    *isAssignment = true;

    return true;
}

// Ensures that the dynamic meta specification on obj is valid
bool QmlCompiler::checkDynamicMeta(QmlParser::Object *obj)
{
    QSet<QByteArray> propNames;
    QSet<QByteArray> methodNames;
    bool seenDefaultProperty = false;

    // Check properties
    for (int ii = 0; ii < obj->dynamicProperties.count(); ++ii) {
        const QmlParser::Object::DynamicProperty &prop =
            obj->dynamicProperties.at(ii);

        if (prop.isDefaultProperty) {
            if (seenDefaultProperty)
                COMPILE_EXCEPTION(&prop, QCoreApplication::translate("QmlCompiler","Duplicate default property"));
            seenDefaultProperty = true;
        }

        if (propNames.contains(prop.name))
            COMPILE_EXCEPTION(&prop, QCoreApplication::translate("QmlCompiler","Duplicate property name"));

        propNames.insert(prop.name);
    }

    for (int ii = 0; ii < obj->dynamicSignals.count(); ++ii) {
        QByteArray name = obj->dynamicSignals.at(ii).name;
        if (methodNames.contains(name))
            COMPILE_EXCEPTION(obj, QCoreApplication::translate("QmlCompiler","Duplicate signal name"));
        methodNames.insert(name);
    }
    for (int ii = 0; ii < obj->dynamicSlots.count(); ++ii) {
        QByteArray name = obj->dynamicSlots.at(ii).name;
        if (methodNames.contains(name))
            COMPILE_EXCEPTION(obj, QCoreApplication::translate("QmlCompiler","Duplicate method name"));
        methodNames.insert(name);
    }

    return true;
}

bool QmlCompiler::mergeDynamicMetaProperties(QmlParser::Object *obj)
{
    for (int ii = 0; ii < obj->dynamicProperties.count(); ++ii) {
        const Object::DynamicProperty &p = obj->dynamicProperties.at(ii);

        if (!p.defaultValue || p.type == Object::DynamicProperty::Alias)
            continue;

        Property *property = 0;
        if (p.isDefaultProperty)
            property = obj->getDefaultProperty();
        else
            property = obj->getProperty(p.name);

        if (property->value)
            COMPILE_EXCEPTION(property, QCoreApplication::translate("QmlCompiler","Invalid property nesting"));

        for (int ii = 0; ii < p.defaultValue->values.count(); ++ii) {
            Value *v = p.defaultValue->values.at(ii);
            v->addref();
            property->values.append(v);
        }
    }
    return true;
}

Q_GLOBAL_STATIC(QAtomicInt, classIndexCounter)

bool QmlCompiler::buildDynamicMeta(QmlParser::Object *obj, DynamicMetaMode mode)
{
    Q_ASSERT(obj);
    Q_ASSERT(obj->metatype);

    if (mode != ForceCreation &&
        obj->dynamicProperties.isEmpty() &&
        obj->dynamicSignals.isEmpty() &&
        obj->dynamicSlots.isEmpty())
        return true;

    COMPILE_CHECK(checkDynamicMeta(obj));

    QByteArray dynamicData(sizeof(QmlVMEMetaData), (char)0);

    QByteArray newClassName = obj->metatype->className();
    newClassName.append("_QML_");
    int idx = classIndexCounter()->fetchAndAddRelaxed(1);
    newClassName.append(QByteArray::number(idx));
    if (compileState.root == obj) {
        QString path = output->url.path();
        int lastSlash = path.lastIndexOf(QLatin1Char('/'));
        if (lastSlash > -1) {
            QString nameBase = path.mid(lastSlash + 1, path.length()-lastSlash-5);
            if (!nameBase.isEmpty() && nameBase.at(0).isUpper())
                newClassName = nameBase.toUtf8() + "_QMLTYPE_" + QByteArray::number(idx);
        }
    }

    QMetaObjectBuilder builder;
    builder.setClassName(newClassName);
    builder.setFlags(QMetaObjectBuilder::DynamicMetaObject);

    bool hasAlias = false;
    for (int ii = 0; ii < obj->dynamicProperties.count(); ++ii) {
        const Object::DynamicProperty &p = obj->dynamicProperties.at(ii);

        int propIdx =
            obj->metaObject()->indexOfProperty(p.name.constData());
        if (-1 != propIdx) {
            QMetaProperty prop = obj->metaObject()->property(propIdx);
            if (prop.isFinal())
                COMPILE_EXCEPTION(&p, QCoreApplication::translate("QmlCompiler","Cannot override FINAL property"));
        }

        if (p.isDefaultProperty &&
            (p.type != Object::DynamicProperty::Alias ||
             mode == ResolveAliases))
            builder.addClassInfo("DefaultProperty", p.name);

        QByteArray type;
        int propertyType = 0;
        bool readonly = false;
        switch(p.type) {
        case Object::DynamicProperty::Alias:
            hasAlias = true;
            continue;
            break;
        case Object::DynamicProperty::CustomList:
        case Object::DynamicProperty::Custom:
            {
                QByteArray customTypeName;
                QmlType *qmltype = 0;
                QUrl url;
                QmlEnginePrivate *priv = QmlEnginePrivate::get(engine);
                if (!priv->resolveType(unit->imports, p.customType, &qmltype, 
                                       &url, 0, 0, 0)) 
                    COMPILE_EXCEPTION(&p, QCoreApplication::translate("QmlCompiler","Invalid property type"));

                if (!qmltype) {
                    QmlCompositeTypeData *tdata = priv->typeManager.get(url);
                    Q_ASSERT(tdata);
                    Q_ASSERT(tdata->status == QmlCompositeTypeData::Complete);

                    QmlCompiledData *data = tdata->toCompiledComponent(engine);
                    customTypeName = data->root->className();
                } else {
                    customTypeName = qmltype->typeName();
                }

                if (p.type == Object::DynamicProperty::Custom) {
                    type = customTypeName + '*';
                    propertyType = QMetaType::QObjectStar;
                } else {
                    readonly = true;
                    type = "QmlList<";
                    type.append(customTypeName);
                    type.append("*>*");
                    propertyType = qMetaTypeId<QmlList<QObject*>* >();
                }
            }
            break;
        case Object::DynamicProperty::Variant:
            propertyType = -1;
            type = "QVariant";
            break;
        case Object::DynamicProperty::Int:
            propertyType = QVariant::Int;
            type = "int";
            break;
        case Object::DynamicProperty::Bool:
            propertyType = QVariant::Bool;
            type = "bool";
            break;
        case Object::DynamicProperty::Real:
            propertyType = QVariant::Double;
            type = "double";
            break;
        case Object::DynamicProperty::String:
            propertyType = QVariant::String;
            type = "QString";
            break;
        case Object::DynamicProperty::Url:
            propertyType = QVariant::Url;
            type = "QUrl";
            break;
        case Object::DynamicProperty::Color:
            propertyType = QVariant::Color;
            type = "QColor";
            break;
        case Object::DynamicProperty::Date:
            propertyType = QVariant::Date;
            type = "QDate";
            break;
        }

        ((QmlVMEMetaData *)dynamicData.data())->propertyCount++;
        QmlVMEMetaData::PropertyData propertyData = { propertyType };
        dynamicData.append((char *)&propertyData, sizeof(propertyData));

        builder.addSignal(p.name + "Changed()");
        QMetaPropertyBuilder propBuilder = 
            builder.addProperty(p.name, type, builder.methodCount() - 1);
        propBuilder.setScriptable(true);
        propBuilder.setWritable(!readonly);
    }

    if (mode == ResolveAliases) {
        for (int ii = 0; ii < obj->dynamicProperties.count(); ++ii) {
            const Object::DynamicProperty &p = obj->dynamicProperties.at(ii);

            if (p.type == Object::DynamicProperty::Alias) {
                ((QmlVMEMetaData *)dynamicData.data())->aliasCount++;
                compileAlias(builder, dynamicData, obj, p);
            }
        }
    }

    for (int ii = 0; ii < obj->dynamicSignals.count(); ++ii) {
        const Object::DynamicSignal &s = obj->dynamicSignals.at(ii);
        QByteArray sig(s.name + '(');
        for (int jj = 0; jj < s.parameterTypes.count(); ++jj) {
            if (jj) sig.append(',');
            sig.append(s.parameterTypes.at(jj));
        }
        sig.append(')');
        QMetaMethodBuilder b = builder.addSignal(sig);
        b.setParameterNames(s.parameterNames);
        ((QmlVMEMetaData *)dynamicData.data())->signalCount++;
    }

    QStringList funcScripts;

    for (int ii = 0; ii < obj->dynamicSlots.count(); ++ii) {
        Object::DynamicSlot &s = obj->dynamicSlots[ii];
        QByteArray sig(s.name + '(');
        QString funcScript(QLatin1String("(function("));

        for (int jj = 0; jj < s.parameterNames.count(); ++jj) {
            if (jj) { 
                sig.append(',');
                funcScript.append(QLatin1Char(','));
            }
            funcScript.append(QLatin1String(s.parameterNames.at(jj)));
            sig.append("QVariant");
        }
        sig.append(')');
        funcScript.append(QLatin1Char(')'));
        funcScript.append(s.body);
        funcScript.append(QLatin1Char(')'));
        funcScripts << funcScript;

        QMetaMethodBuilder b = builder.addSlot(sig);
        b.setReturnType("QVariant");
        b.setParameterNames(s.parameterNames);

        ((QmlVMEMetaData *)dynamicData.data())->methodCount++;
        QmlVMEMetaData::MethodData methodData =
             { s.parameterNames.count(), 0, funcScript.length(), 0 };

        dynamicData.append((char *)&methodData, sizeof(methodData));
    }

    for (int ii = 0; ii < obj->dynamicSlots.count(); ++ii) {
        const QString &funcScript = funcScripts.at(ii);
        QmlVMEMetaData::MethodData *data =
            ((QmlVMEMetaData *)dynamicData.data())->methodData() + ii;

        data->bodyOffset = dynamicData.size();

        dynamicData.append((const char *)funcScript.constData(),
                           (funcScript.length() * sizeof(QChar)));
    }

    obj->metadata = builder.toRelocatableData();
    builder.fromRelocatableData(&obj->extObject, obj->metatype, obj->metadata);

    if (mode == IgnoreAliases && hasAlias)
        compileState.aliasingObjects << obj;

    obj->synthdata = dynamicData;

    return true;
}

#include <qmljsparser_p.h>

static QStringList astNodeToStringList(QmlJS::AST::Node *node)
{
    if (node->kind == QmlJS::AST::Node::Kind_IdentifierExpression) {
        QString name =
            static_cast<QmlJS::AST::IdentifierExpression *>(node)->name->asString();
        return QStringList() << name;
    } else if (node->kind == QmlJS::AST::Node::Kind_FieldMemberExpression) {
        QmlJS::AST::FieldMemberExpression *expr = static_cast<QmlJS::AST::FieldMemberExpression *>(node);

        QStringList rv = astNodeToStringList(expr->base);
        if (rv.isEmpty())
            return rv;
        rv.append(expr->name->asString());
        return rv;
    }
    return QStringList();
}

bool QmlCompiler::compileAlias(QMetaObjectBuilder &builder,
                               QByteArray &data,
                               Object *obj,
                               const Object::DynamicProperty &prop)
{
    if (!prop.defaultValue)
        COMPILE_EXCEPTION(obj, QCoreApplication::translate("QmlCompiler","No property alias location"));

    if (prop.defaultValue->values.count() != 1 ||
        prop.defaultValue->values.at(0)->object ||
        !prop.defaultValue->values.at(0)->value.isScript())
        COMPILE_EXCEPTION(prop.defaultValue, QCoreApplication::translate("QmlCompiler","Invalid alias location"));

    QmlJS::AST::Node *node = prop.defaultValue->values.at(0)->value.asAST();
    if (!node)
        COMPILE_EXCEPTION(obj, QCoreApplication::translate("QmlCompiler","No property alias location")); // ### Can this happen?

    QStringList alias = astNodeToStringList(node);

    if (alias.count() != 1 && alias.count() != 2)
        COMPILE_EXCEPTION(prop.defaultValue, QCoreApplication::translate("QmlCompiler","Invalid alias reference. An alias reference must be specified as <id> or <id>.<property>"));

    if (!compileState.ids.contains(alias.at(0)))
        COMPILE_EXCEPTION(prop.defaultValue, QCoreApplication::translate("QmlCompiler","Invalid alias reference. Unable to find id \"%1\"").arg(alias.at(0)));

    Object *idObject = compileState.ids[alias.at(0)];

    QByteArray typeName;

    int propIdx = -1;
    int flags = 0;
    bool writable = false;
    if (alias.count() == 2) {
        propIdx = idObject->metaObject()->indexOfProperty(alias.at(1).toUtf8().constData());

        if (-1 == propIdx)
            COMPILE_EXCEPTION(prop.defaultValue, QCoreApplication::translate("QmlCompiler","Invalid alias location"));

        QMetaProperty aliasProperty = idObject->metaObject()->property(propIdx);
        writable = aliasProperty.isWritable();

        if (aliasProperty.isEnumType()) 
            typeName = "int";  // Avoid introducing a dependency on the aliased metaobject
        else
            typeName = aliasProperty.typeName();
    } else {
        typeName = idObject->metaObject()->className();

        //use the base type since it has been registered with metatype system
        int index = typeName.indexOf("_QML_");
        if (index != -1) {
            typeName = typeName.left(index);
        } else {
            index = typeName.indexOf("_QMLTYPE_");
            const QMetaObject *mo = idObject->metaObject();
            while (index != -1 && mo) {
                typeName = mo->superClass()->className();
                index = typeName.indexOf("_QMLTYPE_");
                mo = mo->superClass();
            }
        }

        typeName += '*';
    }

    if (typeName.endsWith('*'))
        flags |= QML_ALIAS_FLAG_PTR;

    data.append((const char *)&idObject->idIndex, sizeof(idObject->idIndex));
    data.append((const char *)&propIdx, sizeof(propIdx));
    data.append((const char *)&flags, sizeof(flags));

    builder.addSignal(prop.name + "Changed()");
    QMetaPropertyBuilder propBuilder = 
        builder.addProperty(prop.name, typeName.constData(), builder.methodCount() - 1);
    propBuilder.setScriptable(true);
    propBuilder.setWritable(writable);
    return true;
}

bool QmlCompiler::buildBinding(QmlParser::Value *value,
                               QmlParser::Property *prop,
                               const BindingContext &ctxt)
{
    Q_ASSERT(prop->index != -1);
    Q_ASSERT(prop->parent);
    Q_ASSERT(prop->parent->metaObject());

    QMetaProperty mp = prop->parent->metaObject()->property(prop->index);
    if (!mp.isWritable() && !QmlMetaType::isList(prop->type))
        COMPILE_EXCEPTION(prop, QCoreApplication::translate("QmlCompiler","Invalid property assignment: \"%1\" is a read-only property").arg(QString::fromUtf8(prop->name)));

    BindingReference reference;
    reference.expression = value->value;
    reference.property = prop;
    reference.value = value;
    reference.bindingContext = ctxt;
    addBindingReference(reference);

    return true;
}

void QmlCompiler::genBindingAssignment(QmlParser::Value *binding,
                                       QmlParser::Property *prop,
                                       QmlParser::Object *obj,
                                       QmlParser::Property *valueTypeProperty)
{
    Q_UNUSED(obj);
    Q_ASSERT(compileState.bindings.contains(binding));

    const BindingReference &ref = compileState.bindings.value(binding);
    if (ref.dataType == BindingReference::Experimental) {
        QmlInstruction store;
        store.type = QmlInstruction::StoreCompiledBinding;
        store.assignBinding.value = ref.compiledIndex;
        store.assignBinding.context = ref.bindingContext.stack;
        store.assignBinding.owner = ref.bindingContext.owner;
        if (valueTypeProperty) 
            store.assignBinding.property = (valueTypeProperty->index & 0xFFFF) |
                                           ((valueTypeProperty->type & 0xFF)) << 16 |
                                           ((prop->index & 0xFF) << 24);
        else 
            store.assignBinding.property = prop->index;
        store.line = binding->location.start.line;
        output->bytecode << store;
        return;
    }

    QmlInstruction store;
    store.type = QmlInstruction::StoreBinding;
    store.assignBinding.value = output->indexForByteArray(ref.compiledData);
    store.assignBinding.context = ref.bindingContext.stack;
    store.assignBinding.owner = ref.bindingContext.owner;
    store.line = binding->location.start.line;

    Q_ASSERT(ref.bindingContext.owner == 0 ||
             (ref.bindingContext.owner != 0 && valueTypeProperty));
    if (ref.bindingContext.owner) {
        store.assignBinding.property = genValueTypeData(prop, valueTypeProperty);
    } else {
        store.assignBinding.property = genPropertyData(prop);
    }

    output->bytecode << store;
}

int QmlCompiler::genContextCache()
{
    if (compileState.ids.count() == 0)
        return -1;

    QmlIntegerCache *cache = new QmlIntegerCache(engine);

    for (QHash<QString, QmlParser::Object *>::ConstIterator iter = compileState.ids.begin();
         iter != compileState.ids.end();
         ++iter) 
        cache->add(iter.key(), (*iter)->idIndex);

    output->contextCaches.append(cache);
    return output->contextCaches.count() - 1;
}

int QmlCompiler::genValueTypeData(QmlParser::Property *valueTypeProp, 
                                  QmlParser::Property *prop)
{
    return output->indexForByteArray(QmlMetaPropertyPrivate::saveValueType(prop->parent->metaObject(), prop->index, valueTypeProp->index, valueTypeProp->type));
}

int QmlCompiler::genPropertyData(QmlParser::Property *prop)
{
    return output->indexForByteArray(QmlMetaPropertyPrivate::saveProperty(prop->parent->metaObject(), prop->index));
}

bool QmlCompiler::completeComponentBuild()
{
    componentStat.ids = compileState.ids.count();

    for (int ii = 0; ii < compileState.aliasingObjects.count(); ++ii) {
        Object *aliasObject = compileState.aliasingObjects.at(ii);
        COMPILE_CHECK(buildDynamicMeta(aliasObject, ResolveAliases));
    }

    QmlBindingCompiler::Expression expr;
    expr.component = compileState.root;
    expr.ids = compileState.ids;

    QmlBindingCompiler bindingCompiler;

    for (QHash<QmlParser::Value*,BindingReference>::Iterator iter = compileState.bindings.begin(); iter != compileState.bindings.end(); ++iter) {
        BindingReference &binding = *iter;

        expr.context = binding.bindingContext.object;
        expr.property = binding.property;
        expr.expression = binding.expression;
        expr.imports = unit->imports;

        if (qmlExperimental()) {
            int index = bindingCompiler.compile(expr, QmlEnginePrivate::get(engine));
            if (index != -1) {
                qWarning() << "Accepted for optimization:" << qPrintable(expr.expression.asScript());
                binding.dataType = BindingReference::Experimental;
                binding.compiledIndex = index;
                componentStat.optimizedBindings++;
                continue;
            } else {
                qWarning() << "Rejected for optimization:" << qPrintable(expr.expression.asScript());
            }
        }

        binding.dataType = BindingReference::QtScript;

        // Pre-rewrite the expression
        QString expression = binding.expression.asScript();

        // ### Optimize
        QmlRewrite::SharedBindingTester sharableTest;
        bool isSharable = sharableTest.isSharable(expression);
        
        QmlRewrite::RewriteBinding rewriteBinding;
        expression = rewriteBinding(expression);

        quint32 length = expression.length();
        quint32 pc; 
        
        if (isSharable) {
            pc = output->cachedClosures.count();
            pc |= 0x80000000;
            output->cachedClosures.append(0);
        } else {
            pc = output->cachedPrograms.length();
            output->cachedPrograms.append(0);
        }

        binding.compiledData =
            QByteArray((const char *)&pc, sizeof(quint32)) +
            QByteArray((const char *)&length, sizeof(quint32)) +
            QByteArray((const char *)expression.constData(), 
                       expression.length() * sizeof(QChar));

        componentStat.scriptBindings++;
    }

    if (bindingCompiler.isValid()) {
        compileState.compiledBindingData = bindingCompiler.program();
        QmlBindingCompiler::dump(compileState.compiledBindingData);
    }

    saveComponentState();

    return true;
}

void QmlCompiler::dumpStats()
{
    qWarning().nospace() << "QML Document: " << output->url.toString();
    for (int ii = 0; ii < savedComponentStats.count(); ++ii) {
        const ComponentStat &stat = savedComponentStats.at(ii);
        qWarning().nospace() << "    Component Line " << stat.lineNumber;
        qWarning().nospace() << "        Total Objects:      " << stat.objects;
        qWarning().nospace() << "        IDs Used:           " << stat.ids;
        qWarning().nospace() << "        Optimized Bindings: " << stat.optimizedBindings;
        qWarning().nospace() << "        QScript Bindings:   " << stat.scriptBindings;
    }
}

/*!
    Returns true if from can be assigned to a (QObject) property of type
    to.
*/
bool QmlCompiler::canCoerce(int to, QmlParser::Object *from)
{
    const QMetaObject *toMo = 
        QmlEnginePrivate::get(engine)->rawMetaObjectForType(to);
    const QMetaObject *fromMo = from->metaObject();

    while (fromMo) {
        if (QmlMetaPropertyPrivate::equal(fromMo, toMo))
            return true;
        fromMo = fromMo->superClass();
    }
    return false;
}

/*!
    Returns true if from can be assigned to a (QObject) property of type
    to.
*/
bool QmlCompiler::canCoerce(int to, int from)
{
    const QMetaObject *toMo = 
        QmlEnginePrivate::get(engine)->rawMetaObjectForType(to);
    const QMetaObject *fromMo = 
        QmlEnginePrivate::get(engine)->rawMetaObjectForType(from);

    while (fromMo) {
        if (QmlMetaPropertyPrivate::equal(fromMo, toMo))
            return true;
        fromMo = fromMo->superClass();
    }
    return false;
}

QmlType *QmlCompiler::toQmlType(QmlParser::Object *from)
{
    // ### Optimize
    const QMetaObject *mo = from->metatype;
    QmlType *type = 0;
    while (!type && mo) {
        type = QmlMetaType::qmlType(mo);
        mo = mo->superClass();
    }
   return type;
}

QStringList QmlCompiler::deferredProperties(QmlParser::Object *obj)
{
    const QMetaObject *mo = obj->metatype;

    int idx = mo->indexOfClassInfo("DeferredPropertyNames");
    if (idx == -1)
        return QStringList();

    QMetaClassInfo classInfo = mo->classInfo(idx);
    QStringList rv = QString::fromUtf8(classInfo.value()).split(QLatin1Char(','));
    return rv;
}

QT_END_NAMESPACE
