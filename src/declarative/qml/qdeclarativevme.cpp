/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "private/qdeclarativevme_p.h"

#include "private/qdeclarativecompiler_p.h"
#include "private/qdeclarativeboundsignal_p.h"
#include "private/qdeclarativestringconverters_p.h"
#include "private/qmetaobjectbuilder_p.h"
#include "private/qdeclarativedata_p.h"
#include "qdeclarative.h"
#include "private/qdeclarativecustomparser_p.h"
#include "qdeclarativeengine.h"
#include "qdeclarativecontext.h"
#include "qdeclarativecomponent.h"
#include "private/qdeclarativebinding_p.h"
#include "private/qdeclarativeengine_p.h"
#include "private/qdeclarativecomponent_p.h"
#include "private/qdeclarativevmemetaobject_p.h"
#include "private/qdeclarativebinding_p_p.h"
#include "private/qdeclarativecontext_p.h"
#include "private/qdeclarativecompiledbindings_p.h"
#include "private/qdeclarativeglobal_p.h"
#include "qdeclarativescriptstring.h"

#include <QStack>
#include <QWidget>
#include <QColor>
#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <QtCore/qdebug.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdatetime.h>

QT_BEGIN_NAMESPACE

QDeclarativeVME::QDeclarativeVME()
{
}

#define VME_EXCEPTION(desc) \
    { \
        QDeclarativeError error; \
        error.setDescription(desc.trimmed()); \
        error.setLine(instr.line); \
        error.setUrl(comp->url); \
        vmeErrors << error; \
        break; \
    }

struct ListInstance
{
    ListInstance() 
        : type(0) {}
    ListInstance(int t) 
        : type(t) {}

    int type;
    QDeclarativeListProperty<void> qListProperty;
};

QObject *QDeclarativeVME::run(QDeclarativeContextData *ctxt, QDeclarativeCompiledData *comp, 
                              int start, int count, const QBitField &bindingSkipList)
{
    QDeclarativeVMEStack<QObject *> stack;

    if (start == -1) start = 0;
    if (count == -1) count = comp->bytecode.count();

    return run(stack, ctxt, comp, start, count, bindingSkipList);
}

void QDeclarativeVME::runDeferred(QObject *object)
{
    QDeclarativeData *data = QDeclarativeData::get(object);

    if (!data || !data->context || !data->deferredComponent)
        return;

    QDeclarativeContextData *ctxt = data->context;
    QDeclarativeCompiledData *comp = data->deferredComponent;
    int start = data->deferredIdx + 1;
    int count = data->deferredComponent->bytecode.at(data->deferredIdx).defer.deferCount;
    QDeclarativeVMEStack<QObject *> stack;
    stack.push(object);

    run(stack, ctxt, comp, start, count, QBitField());
}

QObject *QDeclarativeVME::run(QDeclarativeVMEStack<QObject *> &stack, 
                              QDeclarativeContextData *ctxt, 
                              QDeclarativeCompiledData *comp, 
                              int start, int count, 
                              const QBitField &bindingSkipList)
{
    Q_ASSERT(comp);
    Q_ASSERT(ctxt);
    const QList<QDeclarativeCompiledData::TypeReference> &types = comp->types;
    const QList<QString> &primitives = comp->primitives;
    const QList<QByteArray> &datas = comp->datas;
    const QList<QDeclarativeCompiledData::CustomTypeData> &customTypeData = comp->customTypeData;
    const QList<int> &intData = comp->intData;
    const QList<float> &floatData = comp->floatData;
    const QList<QDeclarativePropertyCache *> &propertyCaches = comp->propertyCaches;
    const QList<QDeclarativeParser::Object::ScriptBlock> &scripts = comp->scripts;
    const QList<QUrl> &urls = comp->urls;

    QDeclarativeEnginePrivate::SimpleList<QDeclarativeAbstractBinding> bindValues;
    QDeclarativeEnginePrivate::SimpleList<QDeclarativeParserStatus> parserStatus;

    QDeclarativeVMEStack<ListInstance> qliststack;

    vmeErrors.clear();
    QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(ctxt->engine);

    int status = -1;    //for dbus
    QDeclarativePropertyPrivate::WriteFlags flags = QDeclarativePropertyPrivate::BypassInterceptor;

    for (int ii = start; !isError() && ii < (start + count); ++ii) {
        const QDeclarativeInstruction &instr = comp->bytecode.at(ii);

        switch(instr.type) {
        case QDeclarativeInstruction::Init:
            {
                if (instr.init.bindingsSize) 
                    bindValues = QDeclarativeEnginePrivate::SimpleList<QDeclarativeAbstractBinding>(instr.init.bindingsSize);
                if (instr.init.parserStatusSize)
                    parserStatus = QDeclarativeEnginePrivate::SimpleList<QDeclarativeParserStatus>(instr.init.parserStatusSize);
                if (instr.init.contextCache != -1) 
                    ctxt->setIdPropertyData(comp->contextCaches.at(instr.init.contextCache));
                if (instr.init.compiledBinding != -1) 
                    ctxt->optimizedBindings = new QDeclarativeCompiledBindings(datas.at(instr.init.compiledBinding).constData(), ctxt);
            }
            break;

        case QDeclarativeInstruction::CreateObject:
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

                    VME_EXCEPTION(QCoreApplication::translate("QDeclarativeVME","Unable to create object of type %1").arg(QString::fromLatin1(types.at(instr.create.type).className)));
                }

                QDeclarativeData *ddata = QDeclarativeData::get(o);
                Q_ASSERT(ddata);

                if (stack.isEmpty()) {
                    if (ddata->context) {
                        Q_ASSERT(ddata->context != ctxt);
                        Q_ASSERT(ddata->outerContext);
                        Q_ASSERT(ddata->outerContext != ctxt);
                        QDeclarativeContextData *c = ddata->context;
                        while (c->linkedContext) c = c->linkedContext;
                        c->linkedContext = ctxt;
                    } else {
                        ctxt->addObject(o);
                    }

                    ddata->ownContext = true;
                } else if (!ddata->context) {
                    ctxt->addObject(o);
                }

                ddata->setImplicitDestructible();
                ddata->outerContext = ctxt;
                ddata->lineNumber = instr.line;
                ddata->columnNumber = instr.create.column;

                if (instr.create.data != -1) {
                    QDeclarativeCustomParser *customParser =
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
			    QDeclarative_setParent_noEvent(o, parent);
                    } 
                }
                stack.push(o);
            }
            break;

        case QDeclarativeInstruction::CreateSimpleObject:
            {
                QObject *o = (QObject *)operator new(instr.createSimple.typeSize + 
                                                     sizeof(QDeclarativeData));   
                ::memset(o, 0, instr.createSimple.typeSize + sizeof(QDeclarativeData));
                instr.createSimple.create(o);

                QDeclarativeData *ddata = 
                    (QDeclarativeData *)(((const char *)o) + instr.createSimple.typeSize);
                ddata->lineNumber = instr.line;
                ddata->columnNumber = instr.createSimple.column;

                QObjectPrivate::get(o)->declarativeData = ddata;                                                      
                ddata->context = ddata->outerContext = ctxt;
                ddata->nextContextObject = ctxt->contextObjects; 
                if (ddata->nextContextObject) 
                    ddata->nextContextObject->prevContextObject = &ddata->nextContextObject; 
                ddata->prevContextObject = &ctxt->contextObjects; 
                ctxt->contextObjects = ddata; 

                QObject *parent = stack.top();                                                                    
                QDeclarative_setParent_noEvent(o, parent);                                                        

                stack.push(o);
            }
            break;

        case QDeclarativeInstruction::SetId:
            {
                QObject *target = stack.top();
                ctxt->setIdProperty(instr.setId.index, target);
            }
            break;


        case QDeclarativeInstruction::SetDefault:
            {
                ctxt->contextObject = stack.top();
            }
            break;

        case QDeclarativeInstruction::CreateComponent:
            {
                QDeclarativeComponent *qcomp = 
                    new QDeclarativeComponent(ctxt->engine, comp, ii + 1, instr.createComponent.count,
                                              stack.isEmpty() ? 0 : stack.top());

                QDeclarativeData *ddata = QDeclarativeData::get(qcomp, true);
                Q_ASSERT(ddata);

                ctxt->addObject(qcomp);

                if (stack.isEmpty()) 
                    ddata->ownContext = true;

                ddata->setImplicitDestructible();
                ddata->outerContext = ctxt;
                ddata->lineNumber = instr.line;
                ddata->columnNumber = instr.create.column;

                QDeclarativeComponentPrivate::get(qcomp)->creationContext = ctxt;

                stack.push(qcomp);
                ii += instr.createComponent.count;
            }
            break;

        case QDeclarativeInstruction::StoreMetaObject:
            {
                QObject *target = stack.top();

                QMetaObject mo;
                const QByteArray &metadata = datas.at(instr.storeMeta.data);
                QMetaObjectBuilder::fromRelocatableData(&mo, 0, metadata);

                const QDeclarativeVMEMetaData *data = 
                    (const QDeclarativeVMEMetaData *)datas.at(instr.storeMeta.aliasData).constData();

                (void)new QDeclarativeVMEMetaObject(target, &mo, data, comp);

                QDeclarativeData *ddata = QDeclarativeData::get(target, true);
                if (ddata->propertyCache) ddata->propertyCache->release();
                ddata->propertyCache = propertyCaches.at(instr.storeMeta.propertyCache);
                ddata->propertyCache->addref();
            }
            break;

        case QDeclarativeInstruction::StoreVariant:
            {
                QObject *target = stack.top();
                // XXX - can be more efficient
                QVariant v = QDeclarativeStringConverters::variantFromString(primitives.at(instr.storeString.value));
                void *a[] = { &v, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeString.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StoreVariantInteger:
            {
                QObject *target = stack.top();
                QVariant v(instr.storeInteger.value);
                void *a[] = { &v, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeString.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StoreVariantDouble:
            {
                QObject *target = stack.top();
                QVariant v(instr.storeDouble.value);
                void *a[] = { &v, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeString.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StoreVariantBool:
            {
                QObject *target = stack.top();
                QVariant v(instr.storeBool.value);
                void *a[] = { &v, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeString.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StoreString:
            {
                QObject *target = stack.top();
                void *a[] = { (void *)&primitives.at(instr.storeString.value), 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeString.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StoreUrl:
            {
                QObject *target = stack.top();
                void *a[] = { (void *)&urls.at(instr.storeUrl.value), 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeUrl.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StoreFloat:
            {
                QObject *target = stack.top();
                float f = instr.storeFloat.value;
                void *a[] = { &f, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty,
                                      instr.storeFloat.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StoreDouble:
            {
                QObject *target = stack.top();
                double d = instr.storeDouble.value;
                void *a[] = { &d, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty,
                                      instr.storeDouble.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StoreBool:
            {
                QObject *target = stack.top();
                void *a[] = { (void *)&instr.storeBool.value, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeBool.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StoreInteger:
            {
                QObject *target = stack.top();
                void *a[] = { (void *)&instr.storeInteger.value, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeInteger.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StoreColor:
            {
                QObject *target = stack.top();
                QColor c = QColor::fromRgba(instr.storeColor.value);
                void *a[] = { &c, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeColor.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StoreDate:
            {
                QObject *target = stack.top();
                QDate d = QDate::fromJulianDay(instr.storeDate.value);
                void *a[] = { &d, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeDate.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StoreTime:
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

        case QDeclarativeInstruction::StoreDateTime:
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

        case QDeclarativeInstruction::StorePoint:
            {
                QObject *target = stack.top();
                QPoint p = QPointF(floatData.at(instr.storeRealPair.valueIndex),
                                   floatData.at(instr.storeRealPair.valueIndex+1)).toPoint();
                void *a[] = { &p, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeRealPair.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StorePointF:
            {
                QObject *target = stack.top();
                QPointF p(floatData.at(instr.storeRealPair.valueIndex),
                          floatData.at(instr.storeRealPair.valueIndex+1));
                void *a[] = { &p, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeRealPair.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StoreSize:
            {
                QObject *target = stack.top();
                QSize p = QSizeF(floatData.at(instr.storeRealPair.valueIndex),
                                 floatData.at(instr.storeRealPair.valueIndex+1)).toSize();
                void *a[] = { &p, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeRealPair.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StoreSizeF:
            {
                QObject *target = stack.top();
                QSizeF s(floatData.at(instr.storeRealPair.valueIndex),
                         floatData.at(instr.storeRealPair.valueIndex+1));
                void *a[] = { &s, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeRealPair.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StoreRect:
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

        case QDeclarativeInstruction::StoreRectF:
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

        case QDeclarativeInstruction::StoreVector3D:
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

        case QDeclarativeInstruction::StoreObject:
            {
                QObject *assignObj = stack.pop();
                QObject *target = stack.top();

                void *a[] = { (void *)&assignObj, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeObject.propertyIndex, a);
            }
            break;


        case QDeclarativeInstruction::AssignCustomType:
            {
                QObject *target = stack.top();
                QDeclarativeCompiledData::CustomTypeData data = customTypeData.at(instr.assignCustomType.valueIndex);
                const QString &primitive = primitives.at(data.index);
                QDeclarativeMetaType::StringConverter converter = 
                    QDeclarativeMetaType::customStringConverter(data.type);
                QVariant v = (*converter)(primitive);

                QMetaProperty prop = 
                        target->metaObject()->property(instr.assignCustomType.propertyIndex);
                if (v.isNull() || ((int)prop.type() != data.type && prop.userType() != data.type)) 
                    VME_EXCEPTION(QCoreApplication::translate("QDeclarativeVME","Cannot assign value %1 to property %2").arg(primitive).arg(QString::fromUtf8(prop.name())));

                void *a[] = { (void *)v.data(), 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.assignCustomType.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::AssignSignalObject:
            {
                // XXX optimize

                QObject *assign = stack.pop();
                QObject *target = stack.top();
                int sigIdx = instr.assignSignalObject.signal;
                const QByteArray &pr = datas.at(sigIdx);

                QDeclarativeProperty prop(target, QString::fromUtf8(pr));
                if (prop.type() & QDeclarativeProperty::SignalProperty) {

                    QMetaMethod method = QDeclarativeMetaType::defaultMethod(assign);
                    if (method.signature() == 0)
                        VME_EXCEPTION(QCoreApplication::translate("QDeclarativeVME","Cannot assign object type %1 with no default method").arg(QString::fromLatin1(assign->metaObject()->className())));

                    if (!QMetaObject::checkConnectArgs(prop.method().signature(), method.signature()))
                        VME_EXCEPTION(QCoreApplication::translate("QDeclarativeVME","Cannot connect mismatched signal/slot %1 %vs. %2").arg(QString::fromLatin1(method.signature())).arg(QString::fromLatin1(prop.method().signature())));

                    QMetaObject::connect(target, prop.index(), assign, method.methodIndex());

                } else {
                    VME_EXCEPTION(QCoreApplication::translate("QDeclarativeVME","Cannot assign an object to signal property %1").arg(QString::fromUtf8(pr)));
                }


            }
            break;

        case QDeclarativeInstruction::StoreSignal:
            {
                QObject *target = stack.top();
                QObject *context = stack.at(stack.count() - 1 - instr.assignBinding.context);
                
                QMetaMethod signal = target->metaObject()->method(instr.storeSignal.signalIndex);

                QDeclarativeBoundSignal *bs = new QDeclarativeBoundSignal(target, signal, target);
                QDeclarativeExpression *expr = 
                    new QDeclarativeExpression(ctxt, context, primitives.at(instr.storeSignal.value));
                expr->setSourceLocation(comp->name, instr.line);
                bs->setExpression(expr);
            }
            break;

        case QDeclarativeInstruction::StoreImportedScript:
            {
                ctxt->addImportedScript(scripts.at(instr.storeScript.value));
            }
            break;

        case QDeclarativeInstruction::StoreScriptString:
            {
                QObject *target = stack.top();
                QObject *scope = stack.at(stack.count() - 1 - instr.storeScriptString.scope);
                QDeclarativeScriptString ss;
                ss.setContext(ctxt->asQDeclarativeContext());
                ss.setScopeObject(scope);
                ss.setScript(primitives.at(instr.storeScriptString.value));

                void *a[] = { &ss, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeScriptString.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::BeginObject:
            {
                QObject *target = stack.top();
                QDeclarativeParserStatus *status = reinterpret_cast<QDeclarativeParserStatus *>(reinterpret_cast<char *>(target) + instr.begin.castValue);
                parserStatus.append(status);
                status->d = &parserStatus.values[parserStatus.count - 1];

                status->classBegin();
            }
            break;

        case QDeclarativeInstruction::StoreBinding:
            {
                QObject *target = 
                    stack.at(stack.count() - 1 - instr.assignBinding.owner);
                QObject *context = 
                    stack.at(stack.count() - 1 - instr.assignBinding.context);

                QDeclarativeProperty mp = 
                    QDeclarativePropertyPrivate::restore(datas.at(instr.assignBinding.property), target, ctxt);

                int coreIndex = mp.index();

                if (stack.count() == 1 && bindingSkipList.testBit(coreIndex))  
                    break;

                QDeclarativeBinding *bind = new QDeclarativeBinding((void *)datas.at(instr.assignBinding.value).constData(), comp, context, ctxt, comp->name, instr.line, 0);
                bindValues.append(bind);
                bind->m_mePtr = &bindValues.values[bindValues.count - 1];
                bind->setTarget(mp);
                bind->addToObject(target);
            }
            break;

        case QDeclarativeInstruction::StoreCompiledBinding:
            {
                QObject *target = 
                    stack.at(stack.count() - 1 - instr.assignBinding.owner);
                QObject *scope = 
                    stack.at(stack.count() - 1 - instr.assignBinding.context);

                int property = instr.assignBinding.property;
                if (stack.count() == 1 && bindingSkipList.testBit(property & 0xFFFF))  
                    break;

                QDeclarativeAbstractBinding *binding = 
                    ctxt->optimizedBindings->configBinding(instr.assignBinding.value, target, scope, property);
                bindValues.append(binding);
                binding->m_mePtr = &bindValues.values[bindValues.count - 1];
                binding->addToObject(target);
            }
            break;

        case QDeclarativeInstruction::StoreValueSource:
            {
                QObject *obj = stack.pop();
                QDeclarativePropertyValueSource *vs = reinterpret_cast<QDeclarativePropertyValueSource *>(reinterpret_cast<char *>(obj) + instr.assignValueSource.castValue);
                QObject *target = stack.at(stack.count() - 1 - instr.assignValueSource.owner);

                QDeclarativeProperty prop = 
                    QDeclarativePropertyPrivate::restore(datas.at(instr.assignValueSource.property), target, ctxt);
                obj->setParent(target);
                vs->setTarget(prop);
            }
            break;

        case QDeclarativeInstruction::StoreValueInterceptor:
            {
                QObject *obj = stack.pop();
                QDeclarativePropertyValueInterceptor *vi = reinterpret_cast<QDeclarativePropertyValueInterceptor *>(reinterpret_cast<char *>(obj) + instr.assignValueInterceptor.castValue);
                QObject *target = stack.at(stack.count() - 1 - instr.assignValueInterceptor.owner);
                QDeclarativeProperty prop = 
                    QDeclarativePropertyPrivate::restore(datas.at(instr.assignValueInterceptor.property), target, ctxt);
                obj->setParent(target);
                vi->setTarget(prop);
                QDeclarativeVMEMetaObject *mo = static_cast<QDeclarativeVMEMetaObject *>((QMetaObject*)target->metaObject());
                mo->registerInterceptor(prop.index(), QDeclarativePropertyPrivate::valueTypeCoreIndex(prop), vi);
            }
            break;

        case QDeclarativeInstruction::StoreObjectQList:
            {
                QObject *assign = stack.pop();

                const ListInstance &list = qliststack.top();
                list.qListProperty.append((QDeclarativeListProperty<void>*)&list.qListProperty, assign);
            }
            break;

        case QDeclarativeInstruction::AssignObjectList:
            {
                // This is only used for assigning interfaces
                QObject *assign = stack.pop();
                const ListInstance &list = qliststack.top();

                int type = list.type;

                void *ptr = 0;

                const char *iid = QDeclarativeMetaType::interfaceIId(type);
                if (iid) 
                    ptr = assign->qt_metacast(iid);
                if (!ptr) 
                    VME_EXCEPTION(QCoreApplication::translate("QDeclarativeVME","Cannot assign object to list"));


                list.qListProperty.append((QDeclarativeListProperty<void>*)&list.qListProperty, ptr);
            }
            break;

        case QDeclarativeInstruction::StoreVariantObject:
            {
                QObject *assign = stack.pop();
                QObject *target = stack.top();

                QVariant v = QVariant::fromValue(assign);
                void *a[] = { &v, 0, &status, &flags };
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeObject.propertyIndex, a);
            }
            break;

        case QDeclarativeInstruction::StoreInterface:
            {
                QObject *assign = stack.pop();
                QObject *target = stack.top();

                int coreIdx = instr.storeObject.propertyIndex;
                QMetaProperty prop = target->metaObject()->property(coreIdx);
                int t = prop.userType();
                const char *iid = QDeclarativeMetaType::interfaceIId(t);
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
                    VME_EXCEPTION(QCoreApplication::translate("QDeclarativeVME","Cannot assign object to interface property"));
            }
            break;
            
        case QDeclarativeInstruction::FetchAttached:
            {
                QObject *target = stack.top();

                QObject *qmlObject = qmlAttachedPropertiesObjectById(instr.fetchAttached.id, target);

                if (!qmlObject)
                    VME_EXCEPTION(QCoreApplication::translate("QDeclarativeVME","Unable to create attached object"));

                stack.push(qmlObject);
            }
            break;

        case QDeclarativeInstruction::FetchQList:
            {
                QObject *target = stack.top();

                qliststack.push(ListInstance(instr.fetchQmlList.type));

                void *a[1];
                a[0] = (void *)&(qliststack.top().qListProperty);
                QMetaObject::metacall(target, QMetaObject::ReadProperty, 
                                      instr.fetchQmlList.property, a);
            }
            break;

        case QDeclarativeInstruction::FetchObject:
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
                    VME_EXCEPTION(QCoreApplication::translate("QDeclarativeVME","Cannot set properties on %1 as it is null").arg(QString::fromUtf8(target->metaObject()->property(instr.fetch.property).name())));

                stack.push(obj);
            }
            break;

        case QDeclarativeInstruction::PopQList:
            {
                qliststack.pop();
            }
            break;

        case QDeclarativeInstruction::Defer:
            {
                if (instr.defer.deferCount) {
                    QObject *target = stack.top();
                    QDeclarativeData *data = 
                        QDeclarativeData::get(target, true);
                    comp->addref();
                    data->deferredComponent = comp;
                    data->deferredIdx = ii;
                    ii += instr.defer.deferCount;
                }
            }
            break;

        case QDeclarativeInstruction::PopFetchedObject:
            {
                stack.pop();
            }
            break;

        case QDeclarativeInstruction::FetchValueType:
            {
                QObject *target = stack.top();
                QDeclarativeValueType *valueHandler = 
                    ep->valueTypes[instr.fetchValue.type];
                valueHandler->read(target, instr.fetchValue.property);
                stack.push(valueHandler);
            }
            break;

        case QDeclarativeInstruction::PopValueType:
            {
                QDeclarativeValueType *valueHandler = 
                    static_cast<QDeclarativeValueType *>(stack.pop());
                QObject *target = stack.top();
                valueHandler->write(target, instr.fetchValue.property, 
                                    QDeclarativePropertyPrivate::BypassInterceptor);
            }
            break;

        default:
            qFatal("QDeclarativeCompiledData: Internal error - unknown instruction %d", instr.type);
            break;
        }
    }

    if (isError()) {
        if (!stack.isEmpty()) {
            delete stack.at(0); // ### What about failures in deferred creation?
        } else {
            ctxt->destroy();
        }

        QDeclarativeEnginePrivate::clear(bindValues);
        QDeclarativeEnginePrivate::clear(parserStatus);
        ep->finalizedParserStatus.clear();
        return 0;
    }

    if (bindValues.count)
        ep->bindValues << bindValues;
    if (parserStatus.count)
        ep->parserStatus << parserStatus;

    Q_ASSERT(stack.count() == 1);
    return stack.top();
}

bool QDeclarativeVME::isError() const
{
    return !vmeErrors.isEmpty();
}

QList<QDeclarativeError> QDeclarativeVME::errors() const
{
    return vmeErrors;
}

QObject *
QDeclarativeCompiledData::TypeReference::createInstance(QDeclarativeContextData *ctxt, 
                                                        const QBitField &bindings) const
{
    if (type) {
        QObject *rv = 0;
        void *memory = 0;

        type->create(&rv, &memory, sizeof(QDeclarativeData));
        QDeclarativeData *ddata = new (memory) QDeclarativeData;
        ddata->ownMemory = false;
        QObjectPrivate::get(rv)->declarativeData = ddata;

        return rv;
    } else {
        Q_ASSERT(component);
        return QDeclarativeComponentPrivate::get(component)->create(ctxt, bindings);
    } 
}


QT_END_NAMESPACE
