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

#include "qmlvme_p.h"

#include "qmlcompiler_p.h"
#include "qmlboundsignal_p.h"
#include "qmlstringconverters_p.h"
#include "qmetaobjectbuilder_p.h"
#include "qmldeclarativedata_p.h"
#include "qml.h"
#include "qmlcustomparser_p.h"
#include "qmlengine.h"
#include "qmlcontext.h"
#include "qmlcomponent.h"
#include "qmlbinding.h"
#include "qmlengine_p.h"
#include "qmlcomponent_p.h"
#include "qmlvmemetaobject_p.h"
#include "qmlbinding_p.h"
#include "qmlcontext_p.h"
#include "qmlbindingoptimizations_p.h"
#include "qmlglobal_p.h"
#include "qmlscriptstring.h"

#include <qfxperf_p_p.h>

#include <QStack>
#include <QWidget>
#include <QColor>
#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <QtCore/qdebug.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qcoreapplication.h>

QT_BEGIN_NAMESPACE

QmlVME::QmlVME()
{
}

#define VME_EXCEPTION(desc) \
    { \
        QmlError error; \
        error.setDescription(desc.trimmed()); \
        error.setLine(instr.line); \
        error.setUrl(comp->url); \
        vmeErrors << error; \
        break; \
    }

struct ListInstance
{
    ListInstance() {}
    ListInstance(QList<void *> *q, int t)
        : type(t), qListInterface(q), qmlListInterface(0) {}
    ListInstance(QmlPrivate::ListInterface *q, int t)
        : type(t), qListInterface(0), qmlListInterface(q) {}

    int type;
    QList<void *> *qListInterface;
    QmlPrivate::ListInterface *qmlListInterface;
};

QObject *QmlVME::run(QmlContext *ctxt, QmlCompiledData *comp, 
                     int start, int count, 
                     const QBitField &bindingSkipList)
{
    QmlVMEStack<QObject *> stack;

    if (start == -1) start = 0;
    if (count == -1) count = comp->bytecode.count();

    return run(stack, ctxt, comp, start, count, bindingSkipList);
}

void QmlVME::runDeferred(QObject *object)
{
    QmlDeclarativeData *data = QmlDeclarativeData::get(object);

    if (!data || !data->context || !data->deferredComponent)
        return;

    QmlContext *ctxt = data->context;
    QmlCompiledData *comp = data->deferredComponent;
    int start = data->deferredIdx + 1;
    int count = data->deferredComponent->bytecode.at(data->deferredIdx).defer.deferCount;
    QmlVMEStack<QObject *> stack;
    stack.push(object);

    run(stack, ctxt, comp, start, count, QBitField());
}

QObject *QmlVME::run(QmlVMEStack<QObject *> &stack, QmlContext *ctxt, 
                     QmlCompiledData *comp, 
                     int start, int count, 
                     const QBitField &bindingSkipList)
{
    Q_ASSERT(comp);
    Q_ASSERT(ctxt);
    const QList<QmlCompiledData::TypeReference> &types = comp->types;
    const QList<QString> &primitives = comp->primitives;
    const QList<QByteArray> &datas = comp->datas;
    const QList<QmlCompiledData::CustomTypeData> &customTypeData = comp->customTypeData;
    const QList<int> &intData = comp->intData;
    const QList<float> &floatData = comp->floatData;
    const QList<QmlPropertyCache *> &propertyCaches = comp->propertyCaches;
    const QList<QmlParser::Object::ScriptBlock> &scripts = comp->scripts;
    const QList<QUrl> &urls = comp->urls;

    QmlEnginePrivate::SimpleList<QmlAbstractBinding> bindValues;
    QmlEnginePrivate::SimpleList<QmlParserStatus> parserStatus;

    QmlVMEStack<ListInstance> qliststack;

    vmeErrors.clear();
    QmlEnginePrivate *ep = QmlEnginePrivate::get(ctxt->engine());
    QmlContextPrivate *cp = (QmlContextPrivate *)QObjectPrivate::get(ctxt);

    int status = -1;    //for dbus
    QmlMetaProperty::WriteFlags flags = QmlMetaProperty::BypassInterceptor;

    for (int ii = start; !isError() && ii < (start + count); ++ii) {
        const QmlInstruction &instr = comp->bytecode.at(ii);

        switch(instr.type) {
        case QmlInstruction::Init:
            {
                if (instr.init.bindingsSize) 
                    bindValues = QmlEnginePrivate::SimpleList<QmlAbstractBinding>(instr.init.bindingsSize);
                if (instr.init.parserStatusSize)
                    parserStatus = QmlEnginePrivate::SimpleList<QmlParserStatus>(instr.init.parserStatusSize);
                if (instr.init.contextCache != -1) 
                    cp->setIdPropertyData(comp->contextCaches.at(instr.init.contextCache));
                if (instr.init.compiledBinding != -1) 
                    cp->optimizedBindings = new QmlOptimizedBindings(datas.at(instr.init.compiledBinding).constData(), ctxt);
            }
            break;

        case QmlInstruction::CreateObject:
            {
                QBitField bindings;
                if (instr.create.bindingBits != -1) {
                    const QByteArray &bits = datas.at(instr.create.bindingBits);
                    bindings = QBitField((const quint32*)bits.constData(),
                                         bits.size() * 8);
                }
                if (stack.isEmpty())
                    bindings = bindings.united(bindingSkipList);

                QObject *o = 
                    types.at(instr.create.type).createInstance(ctxt, bindings);
                if (!o) {
                    if(types.at(instr.create.type).component)
                        vmeErrors << types.at(instr.create.type).component->errors();

                    VME_EXCEPTION(QCoreApplication::translate("QmlVME","Unable to create object of type %1").arg(QString::fromLatin1(types.at(instr.create.type).className)));
                }

                QmlDeclarativeData *ddata = QmlDeclarativeData::get(o);
                Q_ASSERT(ddata);
                ddata->outerContext = ctxt;
                ddata->lineNumber = instr.line;
                ddata->columnNumber = instr.create.column;

                if (instr.create.data != -1) {
                    QmlCustomParser *customParser =
                        types.at(instr.create.type).type->customParser();
                    customParser->setCustomData(o, datas.at(instr.create.data));
                }
                if (!stack.isEmpty()) {
                    QObject *parent = stack.top();
                    if (o->isWidgetType()) { 
                        QWidget *widget = static_cast<QWidget*>(o); 
                        if (parent->isWidgetType()) { 
                            QWidget *parentWidget = static_cast<QWidget*>(parent); 
                            widget->setParent(parentWidget); 
                        } else { 
                            // TODO: parent might be a layout 
                        } 
                    } else { 
			    QmlGraphics_setParent_noEvent(o, parent);
       //                 o->setParent(parent); 
                    } 
                }
                stack.push(o);
            }
            break;

        case QmlInstruction::SetId:
            {
                QObject *target = stack.top();
                cp->setIdProperty(instr.setId.index, target);
            }
            break;


        case QmlInstruction::SetDefault:
            {
                QObject *target = stack.top();
                ctxt->addDefaultObject(target);
            }
            break;

        case QmlInstruction::CreateComponent:
            {
                QObject *qcomp = new QmlComponent(ctxt->engine(), comp, ii + 1, instr.createComponent.count, stack.isEmpty() ? 0 : stack.top());

                QmlEngine::setContextForObject(qcomp, ctxt);
                QmlDeclarativeData *ddata = QmlDeclarativeData::get(qcomp);
                Q_ASSERT(ddata);
                ddata->outerContext = ctxt;
                ddata->lineNumber = instr.line;
                ddata->columnNumber = instr.create.column;

                stack.push(qcomp);
                ii += instr.createComponent.count;
            }
            break;

        case QmlInstruction::StoreMetaObject:
            {
                QObject *target = stack.top();

                QMetaObject mo;
                const QByteArray &metadata = datas.at(instr.storeMeta.data);
                QMetaObjectBuilder::fromRelocatableData(&mo, 0, metadata);

                const QmlVMEMetaData *data = 
                    (const QmlVMEMetaData *)datas.at(instr.storeMeta.aliasData).constData();

                (void)new QmlVMEMetaObject(target, &mo, data, comp);

                QmlDeclarativeData *ddata = QmlDeclarativeData::get(target, true);
                if (ddata->propertyCache) ddata->propertyCache->release();
                ddata->propertyCache = propertyCaches.at(instr.storeMeta.propertyCache);
                ddata->propertyCache->addref();
            }
            break;

        case QmlInstruction::StoreVariant:
            {
                QObject *target = stack.top();
                // XXX - can be more efficient
                QVariant v = QmlStringConverters::variantFromString(primitives.at(instr.storeString.value));
                void *a[] = { &v, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeString.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreString:
            {
                QObject *target = stack.top();
                void *a[] = { (void *)&primitives.at(instr.storeString.value), 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeString.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreUrl:
            {
                QObject *target = stack.top();
                void *a[] = { (void *)&urls.at(instr.storeUrl.value), 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeUrl.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreFloat:
            {
                QObject *target = stack.top();
                float f = instr.storeFloat.value;
                void *a[] = { &f, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty,
                                      instr.storeFloat.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreDouble:
            {
                QObject *target = stack.top();
                double d = instr.storeDouble.value;
                void *a[] = { &d, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty,
                                      instr.storeDouble.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreBool:
            {
                QObject *target = stack.top();
                void *a[] = { (void *)&instr.storeBool.value, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeBool.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreInteger:
            {
                QObject *target = stack.top();
                void *a[] = { (void *)&instr.storeInteger.value, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeInteger.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreColor:
            {
                QObject *target = stack.top();
                QColor c = QColor::fromRgba(instr.storeColor.value);
                void *a[] = { &c, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeColor.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreDate:
            {
                QObject *target = stack.top();
                QDate d = QDate::fromJulianDay(instr.storeDate.value);
                void *a[] = { &d, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeDate.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreTime:
            {
                QObject *target = stack.top();
                QTime t;
                t.setHMS(intData.at(instr.storeTime.valueIndex),
                         intData.at(instr.storeTime.valueIndex+1),
                         intData.at(instr.storeTime.valueIndex+2),
                         intData.at(instr.storeTime.valueIndex+3));
                void *a[] = { &t, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeTime.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreDateTime:
            {
                QObject *target = stack.top();
                QTime t;
                t.setHMS(intData.at(instr.storeDateTime.valueIndex+1),
                         intData.at(instr.storeDateTime.valueIndex+2),
                         intData.at(instr.storeDateTime.valueIndex+3),
                         intData.at(instr.storeDateTime.valueIndex+4));
                QDateTime dt(QDate::fromJulianDay(intData.at(instr.storeDateTime.valueIndex)), t);
                void *a[] = { &dt, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty,
                                      instr.storeDateTime.propertyIndex, a);
            }
            break;

        case QmlInstruction::StorePoint:
            {
                QObject *target = stack.top();
                QPoint p = QPointF(floatData.at(instr.storeRealPair.valueIndex),
                                   floatData.at(instr.storeRealPair.valueIndex+1)).toPoint();
                void *a[] = { &p, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeRealPair.propertyIndex, a);
            }
            break;

        case QmlInstruction::StorePointF:
            {
                QObject *target = stack.top();
                QPointF p(floatData.at(instr.storeRealPair.valueIndex),
                          floatData.at(instr.storeRealPair.valueIndex+1));
                void *a[] = { &p, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeRealPair.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreSize:
            {
                QObject *target = stack.top();
                QSize p = QSizeF(floatData.at(instr.storeRealPair.valueIndex),
                                 floatData.at(instr.storeRealPair.valueIndex+1)).toSize();
                void *a[] = { &p, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeRealPair.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreSizeF:
            {
                QObject *target = stack.top();
                QSizeF s(floatData.at(instr.storeRealPair.valueIndex),
                         floatData.at(instr.storeRealPair.valueIndex+1));
                void *a[] = { &s, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeRealPair.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreRect:
            {
                QObject *target = stack.top();
                QRect r = QRectF(floatData.at(instr.storeRect.valueIndex),
                                 floatData.at(instr.storeRect.valueIndex+1),
                                 floatData.at(instr.storeRect.valueIndex+2),
                                 floatData.at(instr.storeRect.valueIndex+3)).toRect();
                void *a[] = { &r, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeRect.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreRectF:
            {
                QObject *target = stack.top();
                QRectF r(floatData.at(instr.storeRect.valueIndex),
                         floatData.at(instr.storeRect.valueIndex+1),
                         floatData.at(instr.storeRect.valueIndex+2),
                         floatData.at(instr.storeRect.valueIndex+3));
                void *a[] = { &r, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeRect.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreVector3D:
            {
                QObject *target = stack.top();
                QVector3D p(floatData.at(instr.storeVector3D.valueIndex),
                            floatData.at(instr.storeVector3D.valueIndex+1),
                            floatData.at(instr.storeVector3D.valueIndex+2));
                void *a[] = { &p, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeVector3D.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreObject:
            {
                QObject *assignObj = stack.pop();
                QObject *target = stack.top();

                void *a[] = { (void *)&assignObj, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeObject.propertyIndex, a);
            }
            break;


        case QmlInstruction::AssignCustomType:
            {
                QObject *target = stack.top();
                QmlCompiledData::CustomTypeData data = customTypeData.at(instr.assignCustomType.valueIndex);
                const QString &primitive = primitives.at(data.index);
                QmlMetaType::StringConverter converter = 
                    QmlMetaType::customStringConverter(data.type);
                QVariant v = (*converter)(primitive);

                QMetaProperty prop = 
                        target->metaObject()->property(instr.assignCustomType.propertyIndex);
                if (v.isNull() || ((int)prop.type() != data.type && prop.userType() != data.type)) 
                    VME_EXCEPTION(QCoreApplication::translate("QmlVME","Cannot assign value %1 to property %2").arg(primitive).arg(QString::fromUtf8(prop.name())));

                void *a[] = { (void *)v.data(), 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.assignCustomType.propertyIndex, a);
            }
            break;

        case QmlInstruction::AssignSignalObject:
            {
                // XXX optimize

                QObject *assign = stack.pop();
                QObject *target = stack.top();
                int sigIdx = instr.assignSignalObject.signal;
                const QByteArray &pr = datas.at(sigIdx);

                QmlMetaProperty prop(target, QString::fromUtf8(pr));
                if (prop.type() & QmlMetaProperty::SignalProperty) {

                    QMetaMethod method = QmlMetaType::defaultMethod(assign);
                    if (method.signature() == 0)
                        VME_EXCEPTION(QCoreApplication::translate("QmlVME","Cannot assign object type %1 with no default method").arg(QString::fromLatin1(assign->metaObject()->className())));

                    if (!QMetaObject::checkConnectArgs(prop.method().signature(), method.signature()))
                        VME_EXCEPTION(QCoreApplication::translate("QmlVME","Cannot connect mismatched signal/slot %1 %vs. %2").arg(QString::fromLatin1(method.signature())).arg(QString::fromLatin1(prop.method().signature())));

                    QMetaObject::connect(target, prop.coreIndex(), assign, method.methodIndex());

                } else {
                    VME_EXCEPTION(QCoreApplication::translate("QmlVME","Cannot assign an object to signal property %1").arg(QString::fromUtf8(pr)));
                }


            }
            break;

        case QmlInstruction::StoreSignal:
            {
                QObject *target = stack.top();
                // XXX scope
                QMetaMethod signal = 
                    target->metaObject()->method(instr.storeSignal.signalIndex);

                QmlBoundSignal *bs = new QmlBoundSignal(target, signal, target);
                QmlExpression *expr = 
                    new QmlExpression(ctxt, primitives.at(instr.storeSignal.value), target);
                expr->setSourceLocation(comp->name, instr.line);
                bs->setExpression(expr);
            }
            break;

        case QmlInstruction::StoreScript:
            {
                QObject *target = stack.top();
                cp->addScript(scripts.at(instr.storeScript.value), target);
            }
            break;

        case QmlInstruction::StoreScriptString:
            {
                QObject *target = stack.top();
                QObject *scope = stack.at(stack.count() - 1 - instr.storeScriptString.scope);
                QmlScriptString ss;
                ss.setContext(ctxt);
                ss.setScopeObject(scope);
                ss.setScript(primitives.at(instr.storeScriptString.value));

                void *a[] = { &ss, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeScriptString.propertyIndex, a);
            }
            break;

        case QmlInstruction::BeginObject:
            {
                QObject *target = stack.top();
                QmlParserStatus *status = reinterpret_cast<QmlParserStatus *>(reinterpret_cast<char *>(target) + instr.begin.castValue);
                parserStatus.append(status);
                status->d = &parserStatus.values[parserStatus.count - 1];

                status->classBegin();
            }
            break;

        case QmlInstruction::StoreBinding:
            {
                QObject *target = 
                    stack.at(stack.count() - 1 - instr.assignBinding.owner);
                QObject *context = 
                    stack.at(stack.count() - 1 - instr.assignBinding.context);

                QmlMetaProperty mp = 
                    QmlMetaPropertyPrivate::restore(datas.at(instr.assignBinding.property), target, ctxt);

                int coreIndex = mp.coreIndex();

                if (stack.count() == 1 && bindingSkipList.testBit(coreIndex))  
                    break;

                QmlBinding *bind = new QmlBinding((void *)datas.at(instr.assignBinding.value).constData(), comp, context, ctxt, comp->name, instr.line, 0);
                bindValues.append(bind);
                bind->m_mePtr = &bindValues.values[bindValues.count - 1];
                bind->setTarget(mp);
                bind->addToObject(target);
            }
            break;

        case QmlInstruction::StoreCompiledBinding:
            {
                QObject *target = 
                    stack.at(stack.count() - 1 - instr.assignBinding.owner);
                QObject *scope = 
                    stack.at(stack.count() - 1 - instr.assignBinding.context);

                int property = instr.assignBinding.property;
                if (stack.count() == 1 && bindingSkipList.testBit(property & 0xFFFF))  
                    break;

                QmlAbstractBinding *binding = 
                    cp->optimizedBindings->configBinding(instr.assignBinding.value, target, scope, property);
                bindValues.append(binding);
                binding->m_mePtr = &bindValues.values[bindValues.count - 1];
                binding->addToObject(target);
            }
            break;

        case QmlInstruction::StoreValueSource:
            {
                QObject *obj = stack.pop();
                QmlPropertyValueSource *vs = reinterpret_cast<QmlPropertyValueSource *>(reinterpret_cast<char *>(obj) + instr.assignValueSource.castValue);
                QObject *target = stack.at(stack.count() - 1 - instr.assignValueSource.owner);

                QmlMetaProperty prop = 
                    QmlMetaPropertyPrivate::restore(datas.at(instr.assignValueSource.property), target, ctxt);
                obj->setParent(target);
                vs->setTarget(prop);
            }
            break;

        case QmlInstruction::StoreValueInterceptor:
            {
                QObject *obj = stack.pop();
                QmlPropertyValueInterceptor *vi = reinterpret_cast<QmlPropertyValueInterceptor *>(reinterpret_cast<char *>(obj) + instr.assignValueInterceptor.castValue);
                QObject *target = stack.at(stack.count() - 1 - instr.assignValueInterceptor.owner);
                QmlMetaProperty prop = 
                    QmlMetaPropertyPrivate::restore(datas.at(instr.assignValueInterceptor.property), target, ctxt);
                obj->setParent(target);
                vi->setTarget(prop);
                QmlVMEMetaObject *mo = static_cast<QmlVMEMetaObject *>((QMetaObject*)target->metaObject());
                mo->registerInterceptor(prop.coreIndex(), prop.valueTypeCoreIndex(), vi);
            }
            break;

        case QmlInstruction::StoreObjectQmlList:
            {
                QObject *assign = stack.pop();
                const ListInstance &list = qliststack.top();

                void *d = (void *)&assign;
                list.qmlListInterface->append(d);
            }
            break;

        case QmlInstruction::StoreObjectQList:
            {
                QObject *assign = stack.pop();

                const ListInstance &list = qliststack.top();
                list.qListInterface->append((void *)assign);
            }
            break;

        case QmlInstruction::AssignObjectList:
            {
                // This is only used for assigning interfaces
                QObject *assign = stack.pop();
                const ListInstance &list = qliststack.top();

                int type = list.type;

                void *ptr = 0;

                const char *iid = QmlMetaType::interfaceIId(type);
                if (iid) 
                    ptr = assign->qt_metacast(iid);
                if (!ptr) 
                    VME_EXCEPTION(QCoreApplication::translate("QmlVME","Cannot assign object to list"));


                if (list.qmlListInterface) {
                    void *d = (void *)&ptr;
                    list.qmlListInterface->append(d);
                } else {
                    list.qListInterface->append(ptr);
                }
            }
            break;

        case QmlInstruction::StoreVariantObject:
            {
                QObject *assign = stack.pop();
                QObject *target = stack.top();

                QVariant v = QVariant::fromValue(assign);
                void *a[] = { &v, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeObject.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreInterface:
            {
                QObject *assign = stack.pop();
                QObject *target = stack.top();

                int coreIdx = instr.storeObject.propertyIndex;
                QMetaProperty prop = target->metaObject()->property(coreIdx);
                int t = prop.userType();
                const char *iid = QmlMetaType::interfaceIId(t);
                bool ok = false;
                if (iid) {
                    void *ptr = assign->qt_metacast(iid);
                    if (ptr) {
                        void *a[] = { &ptr, 0, &status, &flags };
                        QMetaObject::metacall(target, 
                                              QMetaObject::WriteProperty,
                                              coreIdx, a);
                        ok = true;
                    }
                } 

                if (!ok) 
                    VME_EXCEPTION(QCoreApplication::translate("QmlVME","Cannot assign object to interface property"));
            }
            break;
            
        case QmlInstruction::FetchAttached:
            {
                QObject *target = stack.top();

                QObject *qmlObject = qmlAttachedPropertiesObjectById(instr.fetchAttached.id, target);

                if (!qmlObject)
                    VME_EXCEPTION(QCoreApplication::translate("QmlVME","Unable to create attached object"));

                stack.push(qmlObject);
            }
            break;

        case QmlInstruction::FetchQmlList:
            {
                QObject *target = stack.top();

                void *a[1];
                // We know that QmlList<*> can be converted to 
                // QmlPrivate::ListInterface
                QmlPrivate::ListInterface *list = 0;
                a[0] = &list;
                QMetaObject::metacall(target, QMetaObject::ReadProperty, 
                                      instr.fetchQmlList.property, a);
                if (!list) 
                    VME_EXCEPTION(QCoreApplication::translate("QmlVME","Cannot assign to null list"));

                qliststack.push(ListInstance(list, instr.fetchQmlList.type));
            }
            break;

        case QmlInstruction::FetchQList:
            {
                QObject *target = stack.top();

                void *a[1];
                // We know that QList<T *>* can be converted to 
                // QList<void *>*
                QList<void *> *list = 0;
                a[0] = &list;
                QMetaObject::metacall(target, QMetaObject::ReadProperty, 
                                      instr.fetchQmlList.property, a);
                if (!list) 
                    VME_EXCEPTION(QCoreApplication::translate("QmlVME","Cannot assign to null list"));

                qliststack.push(ListInstance(list, instr.fetchQmlList.type));
            }
            break;

        case QmlInstruction::FetchObject:
            {
                QObject *target = stack.top();

                QObject *obj = 0;
                // NOTE: This assumes a cast to QObject does not alter the 
                // object pointer
                void *a[1];
                a[0] = &obj;
                QMetaObject::metacall(target, QMetaObject::ReadProperty, 
                                      instr.fetch.property, a);

                if (!obj)
                    VME_EXCEPTION(QCoreApplication::translate("QmlVME","Cannot set properties on %1 as it is null").arg(QString::fromUtf8(target->metaObject()->property(instr.fetch.property).name())));

                stack.push(obj);
            }
            break;

        case QmlInstruction::PopQList:
            {
                qliststack.pop();
            }
            break;

        case QmlInstruction::Defer:
            {
                if (instr.defer.deferCount) {
                    QObject *target = stack.top();
                    QmlDeclarativeData *data = 
                        QmlDeclarativeData::get(target, true);
                    comp->addref();
                    data->deferredComponent = comp;
                    data->deferredIdx = ii;
                    ii += instr.defer.deferCount;
                }
            }
            break;

        case QmlInstruction::PopFetchedObject:
            {
                stack.pop();
            }
            break;

        case QmlInstruction::FetchValueType:
            {
                QObject *target = stack.top();
                QmlValueType *valueHandler = 
                    ep->valueTypes[instr.fetchValue.type];
                valueHandler->read(target, instr.fetchValue.property);
                stack.push(valueHandler);
            }
            break;

        case QmlInstruction::PopValueType:
            {
                QmlValueType *valueHandler = 
                    static_cast<QmlValueType *>(stack.pop());
                QObject *target = stack.top();
                valueHandler->write(target, instr.fetchValue.property, QmlMetaProperty::BypassInterceptor);
            }
            break;

        default:
            qFatal("QmlCompiledData: Internal error - unknown instruction %d", instr.type);
            break;
        }
    }

    if (isError()) {
        if (!stack.isEmpty()) {
            delete stack.at(0);
        }

        QmlEnginePrivate::clear(bindValues);
        QmlEnginePrivate::clear(parserStatus);
        return 0;
    }

    if (bindValues.count)
        ep->bindValues << bindValues;
    if (parserStatus.count)
        ep->parserStatus << parserStatus;

    if (stack.isEmpty())
        return 0;
    else
        return stack.top();
    return 0;
}

bool QmlVME::isError() const
{
    return !vmeErrors.isEmpty();
}

QList<QmlError> QmlVME::errors() const
{
    return vmeErrors;
}

QT_END_NAMESPACE
