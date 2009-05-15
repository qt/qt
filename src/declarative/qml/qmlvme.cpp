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

#include "qmlvme_p.h"
#include <qfxperf.h>
#include <private/qmlboundsignal_p.h>
#include <private/qmlstringconverters_p.h>
#include "private/qmetaobjectbuilder_p.h"
#include <qml.h>
#include <private/qmlcustomparser_p.h>
#include <qperformancelog.h>
#include <QStack>
#include <QWidget>
#include <private/qmlcompiledcomponent_p.h>
#include <QColor>
#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <qmlengine.h>
#include <qmlcontext.h>
#include <qmlcomponent.h>
#include <qmlbindablevalue.h>
#include <private/qmlengine_p.h>
#include <private/qmlcomponent_p.h>
#include "private/qmlvmemetaobject_p.h"
#include <QtCore/qdebug.h>
#include <QtCore/qvarlengtharray.h>
#include <private/qmlbindablevalue_p.h>

QT_BEGIN_NAMESPACE
Q_DECLARE_PERFORMANCE_LOG(QFxCompiler) {
    Q_DECLARE_PERFORMANCE_METRIC(InstrCreateObject);
    Q_DECLARE_PERFORMANCE_METRIC(InstrCreateCustomObject);
    Q_DECLARE_PERFORMANCE_METRIC(InstrSetId);
    Q_DECLARE_PERFORMANCE_METRIC(InstrSetDefault);
    Q_DECLARE_PERFORMANCE_METRIC(InstrCreateComponent);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreMetaObject);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreReal);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreInteger);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreBool);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreString);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreColor);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreDate);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreDateTime);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreTime);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStorePoint);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreSize);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreVariant);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreObject);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreSignal);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreObjectQmlList);
    Q_DECLARE_PERFORMANCE_METRIC(InstrAssignConstant);
    Q_DECLARE_PERFORMANCE_METRIC(InstrAssignSignalObject);
    Q_DECLARE_PERFORMANCE_METRIC(InstrAssignBinding);
    Q_DECLARE_PERFORMANCE_METRIC(InstrAssignCompiledBinding);
    Q_DECLARE_PERFORMANCE_METRIC(InstrAssignValueSource);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreBinding);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreCompiledBinding);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreValueSource);
    Q_DECLARE_PERFORMANCE_METRIC(InstrTryBeginObject);
    Q_DECLARE_PERFORMANCE_METRIC(InstrBeginObject);
    Q_DECLARE_PERFORMANCE_METRIC(InstrTryCompleteObject);
    Q_DECLARE_PERFORMANCE_METRIC(InstrCompleteObject);
    Q_DECLARE_PERFORMANCE_METRIC(InstrAssignObject);
    Q_DECLARE_PERFORMANCE_METRIC(InstrAssignObjectList);
    Q_DECLARE_PERFORMANCE_METRIC(InstrFetchAttached);
    Q_DECLARE_PERFORMANCE_METRIC(InstrFetchQmlList);
    Q_DECLARE_PERFORMANCE_METRIC(InstrFetchQList);
    Q_DECLARE_PERFORMANCE_METRIC(InstrFetchObject);
    Q_DECLARE_PERFORMANCE_METRIC(InstrResolveFetchObject);
    Q_DECLARE_PERFORMANCE_METRIC(InstrPopFetchedObject);
    Q_DECLARE_PERFORMANCE_METRIC(InstrPopQList);
    Q_DECLARE_PERFORMANCE_METRIC(InstrPushProperty);
    Q_DECLARE_PERFORMANCE_METRIC(InstrAssignStackObject);
    Q_DECLARE_PERFORMANCE_METRIC(InstrStoreStackObject);
    Q_DECLARE_PERFORMANCE_METRIC(InstrNoOp);
    Q_DECLARE_PERFORMANCE_METRIC(Dummy);
}

Q_DEFINE_PERFORMANCE_LOG(QFxCompiler, "QFxCompiler") {
    Q_DEFINE_PERFORMANCE_METRIC(InstrCreateObject, "CreateObject");
    Q_DEFINE_PERFORMANCE_METRIC(InstrCreateCustomObject, "CreateCustomObject");
    Q_DEFINE_PERFORMANCE_METRIC(InstrSetId, "SetId");
    Q_DEFINE_PERFORMANCE_METRIC(InstrSetDefault, "SetDefault");
    Q_DEFINE_PERFORMANCE_METRIC(InstrCreateComponent, "CreateComponent");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreMetaObject, "StoreMetaObject");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreReal, "StoreReal");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreInteger, "StoreInteger");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreBool, "StoreBool");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreString, "StoreString");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreColor, "StoreColor");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreDate, "StoreDate");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreDateTime, "StoreDateTime");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreTime, "StoreTime");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStorePoint, "StorePoint(F)");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreSize, "StoreSize(F)");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreVariant, "StoreVariant");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreObject, "StoreObject");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreSignal, "StoreSignal");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreObjectQmlList, "StoreObjectQmlList");
    Q_DEFINE_PERFORMANCE_METRIC(InstrAssignConstant, "AssignConstant");
    Q_DEFINE_PERFORMANCE_METRIC(InstrAssignSignalObject, "AssignSignalObject");
    Q_DEFINE_PERFORMANCE_METRIC(InstrAssignBinding, "AssignBinding");
    Q_DEFINE_PERFORMANCE_METRIC(InstrAssignCompiledBinding, "AssignCompiledBinding");
    Q_DEFINE_PERFORMANCE_METRIC(InstrAssignValueSource, "AssignValueSource");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreBinding, "StoreBinding");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreCompiledBinding, "StoreCompiledBinding");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreValueSource, "StoreValueSource");
    Q_DEFINE_PERFORMANCE_METRIC(InstrTryBeginObject, "TryBeginObject");
    Q_DEFINE_PERFORMANCE_METRIC(InstrBeginObject, "BeginObject");
    Q_DEFINE_PERFORMANCE_METRIC(InstrTryCompleteObject, "TryCompleteObject");
    Q_DEFINE_PERFORMANCE_METRIC(InstrCompleteObject, "CompleteObject");
    Q_DEFINE_PERFORMANCE_METRIC(InstrAssignObject, "AssignObject");
    Q_DEFINE_PERFORMANCE_METRIC(InstrAssignObjectList, "AssignObjectList");
    Q_DEFINE_PERFORMANCE_METRIC(InstrFetchAttached, "FetchAttached");
    Q_DEFINE_PERFORMANCE_METRIC(InstrFetchQmlList, "FetchQmlList");
    Q_DEFINE_PERFORMANCE_METRIC(InstrFetchQList, "FetchQList");
    Q_DEFINE_PERFORMANCE_METRIC(InstrFetchObject, "FetchObject");
    Q_DEFINE_PERFORMANCE_METRIC(InstrResolveFetchObject, "ResolveFetchObject");
    Q_DEFINE_PERFORMANCE_METRIC(InstrPopFetchedObject, "PopFetchedObject");
    Q_DEFINE_PERFORMANCE_METRIC(InstrPopQList, "PopQList");
    Q_DEFINE_PERFORMANCE_METRIC(InstrPushProperty, "PushProperty");
    Q_DEFINE_PERFORMANCE_METRIC(InstrAssignStackObject, "AssignStackObject");
    Q_DEFINE_PERFORMANCE_METRIC(InstrStoreStackObject, "StoreStackObject");
    Q_DEFINE_PERFORMANCE_METRIC(InstrNoOp, "NoOp");
    Q_DEFINE_PERFORMANCE_METRIC(Dummy, "Dummy");
}

static inline int qIndexOfProperty(QObject *o, const char *name)
{
    int idx = o->metaObject()->indexOfProperty(name);
    return idx;
}

QmlVME::QmlVME()
{
}

#define VME_EXCEPTION(desc) \
    { \
        QString str; \
        QDebug d(&str); \
        d << desc; \
        str = str.trimmed(); \
        QmlError error; \
        error.setDescription(str); \
        error.setLine(instr.line); \
        error.setUrl(comp->url); \
        vmeErrors << error; \
        break; \
    }

struct ListInstance
{
    ListInstance() {}
    ListInstance(const QVariant &l, int t)
        : list(l), type(t), qmlListInterface(0) {}
    ListInstance(QmlPrivate::ListInterface *q, int t)
        : type(t), qmlListInterface(q) {}

    QVariant list;
    int type;
    QmlPrivate::ListInterface *qmlListInterface;
};

QObject *QmlVME::run(QmlContext *ctxt, QmlCompiledComponent *comp, int start, int count)
{
    // XXX - All instances of QmlContext::activeContext() here should be 
    // replaced with the use of ctxt.  However, this cannot be done until 
    // behaviours stop modifying the active context and expecting the 
    // instantiation to notice.  Instead, QmlParserStatus::beginClass() should 
    // be able to return a QmlContext that is used for expressions and 
    // sub-instances on that type.
    Q_ASSERT(comp);
    Q_ASSERT(ctxt);
    const QList<QmlCompiledComponent::TypeReference> &types = comp->types;
    const QList<QString> &primitives = comp->primitives;
    const QList<QByteArray> &datas = comp->datas;
    const QList<QMetaObject *> &synthesizedMetaObjects = comp->synthesizedMetaObjects;;
    const QList<QmlCompiledData::CustomTypeData> &customTypeData = comp->customTypeData;

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::CompileRun> cr;
#endif
    QmlEnginePrivate::SimpleList<QmlBindableValue> bindValues;
    QmlEnginePrivate::SimpleList<QmlParserStatus> parserStatus;

    QStack<QObject *> stack;
    QStack<ListInstance> qliststack;

    QStack<QmlMetaProperty> pushedProperties;
    QObject **savedObjects = 0;

    vmeErrors.clear();

    if (start == -1) start = 0;
    if (count == -1) count = comp->bytecode.count();

    for (int ii = start; !isError() && ii < (start + count); ++ii) {
        QmlInstruction &instr = comp->bytecode[ii];

        if (instr.type >= QmlInstruction::StoreInstructionsStart &&
           instr.type <= QmlInstruction::StoreInstructionsEnd) {

            runStoreInstruction(stack, instr, comp);

        } else {

        switch(instr.type) {
        case QmlInstruction::Init:
            {
                if (instr.init.dataSize) {
                    savedObjects = new QObject*[instr.init.dataSize];
                    ::memset(savedObjects, 0,
                            sizeof(QObject *)*instr.init.dataSize);
                }

                if (instr.init.bindingsSize) 
                    bindValues = QmlEnginePrivate::SimpleList<QmlBindableValue>(instr.init.bindingsSize);
                if (instr.init.parserStatusSize)
                    parserStatus = QmlEnginePrivate::SimpleList<QmlParserStatus>(instr.init.parserStatusSize);
            }
            break;

        case QmlInstruction::CreateObject:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrCreateObject> cc;
#endif
                QObject *o = types.at(instr.create.type).createInstance(QmlContext::activeContext());
                if (!o) {
                    if(types.at(instr.create.type).component)
                        vmeErrors << types.at(instr.create.type).component->errors();

                    VME_EXCEPTION("Unable to create object of type" << types.at(instr.create.type).className);
                }

                if (instr.create.data != -1) {
                    QmlCustomParser *customParser =
                        types.at(instr.create.type).type->customParser();
                    customParser->setCustomData(o, datas.at(instr.create.data));
                }
                if (!stack.isEmpty()) {
                    QObject *parent = stack.top();
                    o->setParent(parent);
                }
                stack.push(o);
            }
            break;

        case QmlInstruction::SetId:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrSetId> cc;
#endif
                QObject *target = stack.top();
                QmlContext *ctxt = 
                    QmlContext::activeContext();
                ctxt->setContextProperty(primitives.at(instr.setId.value), target);

                if (instr.setId.save != -1)
                    savedObjects[instr.setId.save] = target;
            }
            break;


        case QmlInstruction::SetDefault:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrSetDefault> cc;
#endif
                QObject *target = stack.top();
                QmlContext::activeContext()->addDefaultObject(target);
            }
            break;

        case QmlInstruction::CreateComponent:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrCreateComponent> cc;
#endif
                QObject *qcomp = new QmlComponent(ctxt->engine(), comp, ii + 1, instr.createComponent.count, stack.isEmpty() ? 0 : stack.top());
                stack.push(qcomp);
                ii += instr.createComponent.count;
            }
            break;

        case QmlInstruction::StoreMetaObject:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStoreMetaObject> cc;
#endif
                QObject *target = stack.top();
                new QmlVMEMetaObject(target, synthesizedMetaObjects.at(instr.storeMeta.data), &comp->primitives, instr.storeMeta.slotData, comp);
            }
            break;

        case QmlInstruction::AssignCustomType:
            {
                QObject *target = stack.top();
                void *a[1];
                QmlCompiledComponent::CustomTypeData data = customTypeData.at(instr.assignCustomType.valueIndex);
                const QString &primitive = primitives.at(data.index);
                QmlMetaType::StringConverter converter = 
                    QmlMetaType::customStringConverter(data.type);
                QVariant v = (*converter)(primitive);

                QMetaProperty prop = 
                        target->metaObject()->property(instr.assignCustomType.propertyIndex);
                if (v.isNull() || ((int)prop.type() != data.type && prop.userType() != data.type)) 
                    VME_EXCEPTION("Cannot assign value" << primitive << "to property" << prop.name());

                a[0] = (void *)v.data();
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.assignCustomType.propertyIndex, a);
            }
            break;

        case QmlInstruction::AssignSignalObject:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrAssignSignalObject> cc;
#endif
                // XXX optimize

                QObject *assign = stack.pop();
                QObject *target = stack.top();
                int sigIdx = instr.assignSignalObject.signal;
                const QByteArray &pr = datas.at(sigIdx);

                QmlMetaProperty prop(target, QLatin1String(pr));
                if (prop.type() & QmlMetaProperty::SignalProperty) {

                    QMetaMethod method = QmlMetaType::defaultMethod(assign);
                    if (method.signature() == 0)
                        VME_EXCEPTION("Cannot assign object type" << assign->metaObject()->className() << "with no default method");

                    if (!QMetaObject::checkConnectArgs(prop.method().signature(), method.signature()))
                        VME_EXCEPTION("Cannot connect mismatched signal/slot" << method.signature() << prop.method().signature());

                    QMetaObject::connect(target, prop.coreIndex(), assign, method.methodIndex());

                } else if (prop.type() & QmlMetaProperty::Property) {
                    instr.type = QmlInstruction::AssignObject;
                    instr.assignObject.castValue = 0;
                    instr.assignObject.property = sigIdx;
                    --ii;
                } else {
                    VME_EXCEPTION("Cannot assign an object to signal property" << pr);
                }


            }
            break;

        case QmlInstruction::StoreSignal:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStoreSignal> cc;
#endif
                QObject *target = stack.top();
                // XXX scope
                QMetaMethod signal = 
                    target->metaObject()->method(instr.storeSignal.signalIndex);

                if (signal.parameterTypes().isEmpty()) {
                    (void *)new QmlBoundSignal(QmlContext::activeContext(), primitives.at(instr.storeSignal.value), target, instr.storeSignal.signalIndex, target);
                } else {
                    (void *)new QmlBoundSignalProxy(new QmlContext(QmlContext::activeContext(), target), primitives.at(instr.storeSignal.value), target, instr.storeSignal.signalIndex, target);
                }
            }
            break;

        case QmlInstruction::TryBeginObject:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrTryBeginObject> cc;
#endif
                QObject *target = stack.top();
                QmlParserStatus *status = 
                    qobject_cast<QmlParserStatus *>(target);

                if (status) {
                    instr.type = QmlInstruction::BeginObject;
                    instr.begin.castValue = int(reinterpret_cast<char *>(status) - reinterpret_cast<char *>(target));
                    --ii;
                } else {
                    instr.type = QmlInstruction::NoOp;
                }
            }
            break;

        case QmlInstruction::BeginObject:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrBeginObject> cc;
#endif
                QObject *target = stack.top();
                QmlParserStatus *status = reinterpret_cast<QmlParserStatus *>(reinterpret_cast<char *>(target) + instr.begin.castValue);
                parserStatus.append(status);
                status->d = &parserStatus.values[parserStatus.count - 1];

                status->classBegin();
            }
            break;

        case QmlInstruction::TryCompleteObject:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrTryCompleteObject> cc;
#endif
                QObject *target = stack.top();
                QmlParserStatus *status = 
                    qobject_cast<QmlParserStatus *>(target);

                if (status) {
                    instr.type = QmlInstruction::CompleteObject;
                    instr.complete.castValue = int(reinterpret_cast<char *>(status) - reinterpret_cast<char *>(target));
                    --ii;
                } else {
                    instr.type = QmlInstruction::NoOp;
                }
            }
            break;

        case QmlInstruction::CompleteObject:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrCompleteObject> cc;
#endif
                QObject *target = stack.top();
                QmlParserStatus *status = reinterpret_cast<QmlParserStatus *>(reinterpret_cast<char *>(target) + instr.complete.castValue);
                status->classComplete();
            }
            break;

        case QmlInstruction::AssignCompiledBinding:
        case QmlInstruction::AssignBinding:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrAssignBinding> cc;
#endif
                QObject *target = stack.top();
                const QByteArray &pr = datas.at(instr.fetch.property);
                int idx = qIndexOfProperty(target, pr);

                // XXX - need to check if the type is QmlBindableValue*
                if (idx == -1) {
                    VME_EXCEPTION("Unknown property" << pr);
                } else {
                    if (QmlInstruction::AssignCompiledBinding == instr.type)
                        instr.type = QmlInstruction::StoreCompiledBinding;
                    else
                        instr.type = QmlInstruction::StoreBinding;
                    instr.assignBinding.property = idx;
                    instr.assignBinding.category = QmlMetaProperty::Unknown;
                }
                ii--;
            }
            break;

        case QmlInstruction::AssignValueSource:
            {
                QObject *target = stack.at(stack.count() - 2);
                int propIdx = instr.assignValueSource.property;
                QByteArray pr;
                if (propIdx == -1) {
                    pr = QmlMetaType::defaultProperty(target).name();
                    if (pr.isEmpty())
                        VME_EXCEPTION("Unable to resolve default property");
                } else {
                    pr = datas.at(propIdx);
                }

                int coreIdx = qIndexOfProperty(target, pr);
                if (coreIdx != -1) {
                    instr.type = QmlInstruction::StoreValueSource;
                    instr.assignValueSource.property = coreIdx;
                    ii--;
                } else {
                    VME_EXCEPTION("Unknown property" << pr);
                }
            }
            break;

        case QmlInstruction::PushProperty:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrPushProperty> cc;
#endif
                QObject *target = stack.top();
                QmlMetaProperty mp(target, instr.pushProperty.property,
                                   QmlMetaProperty::Object);
                pushedProperties.push(mp);
            }
            break;

        case QmlInstruction::StoreCompiledBinding:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStoreCompiledBinding> cc;
#endif
                QObject *target = stack.top();
                QObject *context = 
                    stack.at(stack.count() - 1 - instr.assignBinding.context);

                QmlMetaProperty mp(target, instr.assignBinding.property,
                                   (QmlMetaProperty::PropertyCategory)instr.assignBinding.category);
                if (!mp.isWritable())
                    VME_EXCEPTION("Cannot assign a binding to read-only property" << mp.name());

                QmlBindableValue *bind = new QmlBindableValue((void *)datas.at(instr.assignBinding.value).constData(), comp, context, 0);
                bindValues.append(bind);
                QmlBindableValuePrivate *p = 
                    static_cast<QmlBindableValuePrivate *>(QObjectPrivate::get(bind));
                p->mePtr = &bindValues.values[bindValues.count - 1];
                QFx_setParent_noEvent(bind, target);

                bind->setTarget(mp);
            }
            break;

        case QmlInstruction::StoreBinding:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStoreBinding> cc;
#endif
                QObject *target = stack.top();
                QObject *context = 
                    stack.at(stack.count() - 1 - instr.assignBinding.context);

                QmlMetaProperty mp(target, instr.assignBinding.property, 
                                   (QmlMetaProperty::PropertyCategory)instr.assignBinding.category);
                if (!mp.isWritable())
                    VME_EXCEPTION("Cannot assign a binding to read-only property" << mp.name());

                QmlBindableValue *bind = new QmlBindableValue(primitives.at(instr.assignBinding.value), context, false);
                bindValues.append(bind);
                QmlBindableValuePrivate *p = 
                    static_cast<QmlBindableValuePrivate *>(QObjectPrivate::get(bind));
                p->mePtr = &bindValues.values[bindValues.count - 1];
                QFx_setParent_noEvent(bind, target);

                bind->setTarget(mp);
            }
            break;

        case QmlInstruction::StoreValueSource:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStoreValueSource> cc;
#endif
                QObject *assign = stack.pop();
                QmlPropertyValueSource *vs = 
                    static_cast<QmlPropertyValueSource *>(assign);
                QObject *target = stack.top();
                vs->setParent(target);
                vs->setTarget(QmlMetaProperty(target, instr.assignValueSource.property));
            }
            break;

        case QmlInstruction::AssignObjectList:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrAssignObjectList> cc;
#endif
                QObject *assign = stack.pop();
                const ListInstance &list = qliststack.top();
                if (list.qmlListInterface) {
                    int type = list.type;

                    void *d = 0;
                    void *ptr = 0;
                    bool found = false;

                    if (QmlMetaType::isInterface(type)) {
                        const char *iid = QmlMetaType::interfaceIId(type);
                        if (iid) 
                            ptr = assign->qt_metacast(iid);
                        if (ptr) {
                            d = &ptr;
                            found = true;
                        }
                    } else {
                        const QMetaObject *mo = 
                            QmlMetaType::rawMetaObjectForType(type);

                        const QMetaObject *assignMo = assign->metaObject();
                        while(!found && assignMo) {
                            if (assignMo == mo)
                                found = true;
                            else
                                assignMo = assignMo->superClass();
                        }

                        // NOTE: This assumes a cast to QObject does not alter 
                        // the object pointer
                        d = (void *)&assign;
                    }


                    if (!found) 
                        VME_EXCEPTION("Cannot assign object to list");

                    list.qmlListInterface->append(d);

                } else {
                    int type = list.type;

                    if (QmlMetaType::isInterface(type)) {
                        void *ptr = 0;
                        const char *iid = QmlMetaType::interfaceIId(type);
                        if (iid) 
                            ptr = assign->qt_metacast(iid);
                        QVariant v(list.type, &ptr);
                        QmlMetaType::append(list.list, v);
                    } else {
                        QVariant v = QmlMetaType::fromObject(assign, list.type);
                        QmlMetaType::append(list.list, v);
                    }
                }
            }
            break;

        case QmlInstruction::AssignObject:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrAssignObject> cc;
#endif
                QObject *assign = stack.pop();
                QObject *target = stack.top();

                QByteArray property;
                if (instr.assignObject.property == -1) {
                    // XXX - optimize!
                    property = 
                        QmlMetaType::defaultProperty(target).name();
                } else {
                    property = datas.at(instr.assignObject.property);
                }

                int coreIdx = qIndexOfProperty(target, property);

                if (coreIdx != -1) {
                    QMetaProperty prop = 
                        target->metaObject()->property(coreIdx);
                    int t = prop.userType();
                    // XXX - optimize!
                    if (QmlMetaType::isList(t)) {
                        QVariant list = prop.read(target);
                        int listtype = QmlMetaType::listType(t);
                        QVariant v = QmlMetaType::fromObject(assign, listtype);
                        QmlMetaType::append(list, v);
                    } else if (QmlMetaType::isQmlList(t)) {

                        // XXX - optimize!
                        QVariant list = prop.read(target);
                        QmlPrivate::ListInterface *li = 
                            *(QmlPrivate::ListInterface **)list.constData();

                        int type = li->type();

                        const QMetaObject *mo = 
                            QmlMetaType::rawMetaObjectForType(type);

                        const QMetaObject *assignMo = assign->metaObject();
                        bool found = false;
                        while(!found && assignMo) {
                            if (assignMo == mo)
                                found = true;
                            else
                                assignMo = assignMo->superClass();
                        }

                        if (!found) 
                            VME_EXCEPTION("Cannot assign object to list");

                        // NOTE: This assumes a cast to QObject does not alter 
                        // the object pointer
                        void *d = (void *)&assign;
                        li->append(d);

                    } else if (QmlMetaType::isInterface(t)) {
                        const char *iid = QmlMetaType::interfaceIId(t);
                        bool ok = false;
                        if (iid) {
                            void *ptr = assign->qt_metacast(iid);
                            if (ptr) {
                                void *a[1];
                                a[0] = &ptr;
                                QMetaObject::metacall(target, QMetaObject::WriteProperty,
                                                      coreIdx, a);
                                ok = true;
                            }
                        } 

                        if (!ok) 
                            VME_EXCEPTION("Cannot assign object to interface property" << property);

                    } else if (prop.userType() == -1 /* means qvariant */) {
                        prop.write(target, qVariantFromValue(assign));
                    } else {
                        const QMetaObject *propmo = 
                            QmlMetaType::rawMetaObjectForType(t);

                        bool isPropertyValue = false;
                        bool isAssignable = false;
                        const QMetaObject *c = assign->metaObject();
                        while(c) {
                            isPropertyValue = isPropertyValue || (c == &QmlPropertyValueSource::staticMetaObject);
                            isAssignable = isAssignable || (c == propmo);
                            c = c->superClass();
                        }

                        if (isAssignable) {
                            // XXX - optimize!
                            QVariant v = QmlMetaType::fromObject(assign, t);
                            prop.write(target, v);
                        } else if (isPropertyValue) {
                            QmlPropertyValueSource *vs = 
                                static_cast<QmlPropertyValueSource *>(assign);
                            vs->setParent(target);
                            vs->setTarget(QmlMetaProperty(target, coreIdx));
                        } else {
                            VME_EXCEPTION("Cannot assign to" << property);
                        }
                    }


                } else {
                    if (instr.assignObject.property == -1) {
                        VME_EXCEPTION("Cannot assign to default property");
                    } else {
                        VME_EXCEPTION("Cannot assign to non-existant property" << property);
                    }
                }

            }
            break;

        case QmlInstruction::FetchAttached:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrFetchAttached> cc;
#endif
                QObject *target = stack.top();

                QObject *qmlObject = qmlAttachedPropertiesObjectById(instr.fetchAttached.id, target);

                if (!qmlObject)
                    VME_EXCEPTION("Unable to create attached object");

                stack.push(qmlObject);
            }
            break;

        case QmlInstruction::FetchQmlList:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrFetchQmlList> cc;
#endif
                QObject *target = stack.top();

                void *a[1];
                // We know that QmlList<*> can be converted to 
                // QmlPrivate::ListInterface
                QmlPrivate::ListInterface *list = 0;
                a[0] = &list;
                QMetaObject::metacall(target, QMetaObject::ReadProperty, 
                                      instr.fetchQmlList.property, a);
                if (!list) 
                    VME_EXCEPTION("Cannot assign to null list");

                qliststack.push(ListInstance(list, instr.fetchQmlList.type));
            }
            break;

        case QmlInstruction::FetchQList:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrFetchQList> cc;
#endif
                QObject *target = stack.top();
                QMetaProperty prop = 
                    target->metaObject()->property(instr.fetch.property);
                QVariant v = prop.read(target);
                qliststack.push(ListInstance(v, QmlMetaType::listType(prop.userType())));
            }
            break;

        case QmlInstruction::ResolveFetchObject:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrResolveFetchObject> cc;
#endif
                QObject *target = stack.top();
                const QByteArray &pr = datas.at(instr.fetch.property);
                int idx = qIndexOfProperty(target, pr);
                if (idx == -1)
                    VME_EXCEPTION("Cannot resolve property" << pr);
                QMetaProperty prop = target->metaObject()->property(idx);
                instr.type = QmlInstruction::FetchObject;
                instr.fetch.property = idx;
                if (QmlMetaType::isObject(prop.userType())) {
                    instr.fetch.isObject = true;
                } else if (prop.userType() == -1) {
                    instr.fetch.isObject = false;
                } else {
                    VME_EXCEPTION("Cannot set properties on" << prop.name() << "as it is of unknown type");
                }
                ii--;
            }
            break;

        case QmlInstruction::FetchObject:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrFetchObject> cc;
#endif
                QObject *target = stack.top();

                QObject *obj = 0;
                if (instr.fetch.isObject) {
                    // NOTE: This assumes a cast to QObject does not alter the 
                    // object pointer
                    void *a[1];
                    a[0] = &obj;
                    QMetaObject::metacall(target, QMetaObject::ReadProperty, 
                                          instr.fetch.property, a);
                } else {
                    void *a[1];
                    QVariant var;
                    a[0] = &var;
                    QMetaObject::metacall(target, QMetaObject::ReadProperty, 
                                          instr.fetch.property, a);
                    obj = QmlMetaType::toQObject(var);

                }

                if (!obj)
                    VME_EXCEPTION("Cannot set properties on" << target->metaObject()->property(instr.fetch.property).name() << "as it is null");

                stack.push(obj);
            }
            break;

        case QmlInstruction::PopQList:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrPopQList> cc;
#endif
                qliststack.pop();
            }
            break;

        case QmlInstruction::PopFetchedObject:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrPopFetchedObject> cc;
#endif
                stack.pop();
            }
            break;

        case QmlInstruction::AssignStackObject:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrAssignStackObject> cc;
#endif

                QObject *obj = savedObjects[instr.assignStackObject.object];
                const QmlMetaProperty &prop = 
                    pushedProperties.at(instr.assignStackObject.property);


                const QMetaObject *mo = 
                    QmlMetaType::rawMetaObjectForType(prop.propertyType());
                const QMetaObject *assignMo = obj->metaObject();

                bool found = false;
                while(!found && assignMo) {
                    if (assignMo == mo)
                        found = true;
                    else
                        assignMo = assignMo->superClass();
                }

                if (!found) 
                    VME_EXCEPTION("Unable to assign object");

                instr.type = QmlInstruction::StoreStackObject;
                --ii;
            }
            break;

        case QmlInstruction::StoreStackObject:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStoreStackObject> cc;
#endif

                const QmlMetaProperty &prop = 
                    pushedProperties.at(instr.assignStackObject.property);
                QObject *obj = savedObjects[instr.assignStackObject.object];

                // NOTE: This assumes a cast to QObject does not alter the 
                // object pointer
                void *a[1];
                a[0] = (void *)&obj;
                QMetaObject::metacall(prop.object(), QMetaObject::WriteProperty,
                                      prop.coreIndex(), a);
            }
            break;

        case QmlInstruction::NoOp:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrNoOp> cc;
#endif
            }
            break;
            
        default:
            qFatal("QmlCompiledComponent: Internal error - unknown instruction %d", instr.type);
            break;
        }
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

    QmlEnginePrivate *ep = ctxt->engine()->d_func();
    if (bindValues.count)
        ep->bindValues << bindValues;
    if (parserStatus.count)
        ep->parserStatus << parserStatus;

    comp->dumpPost();

    if (savedObjects)
        delete [] savedObjects;

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

void QmlVME::runStoreInstruction(QStack<QObject *> &stack,
                                 QmlInstruction &instr, 
                                 QmlCompiledData *comp)
{
    const QList<QString> &primitives = comp->primitives;
    const QList<int> &intData = comp->intData;
    const QList<float> &floatData = comp->floatData;

    switch(instr.type) {
        case QmlInstruction::StoreVariant:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStoreVariant> cc;
#endif
                QObject *target = stack.top();
                void *a[1];
                // XXX - can be more efficient
                QVariant v = QmlStringConverters::variantFromString(primitives.at(instr.storeString.value));
                a[0] = (void *)&v;
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeString.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreString:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStoreString> cc;
#endif
                QObject *target = stack.top();
                void *a[1];
                a[0] = (void *)&primitives.at(instr.storeString.value);
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeString.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreReal:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStoreReal> cc;
#endif
                QObject *target = stack.top();
                qreal r = instr.storeReal.value;
                void *a[1];
                a[0] = &r;
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeReal.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreBool:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStoreBool> cc;
#endif
                QObject *target = stack.top();
                void *a[1];
                a[0] = (void *)&instr.storeBool.value;
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeBool.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreInteger:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStoreInteger> cc;
#endif
                QObject *target = stack.top();
                void *a[1];
                a[0] = (void *)&instr.storeInteger.value;
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeReal.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreColor:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStoreColor> cc;
#endif
                QObject *target = stack.top();
                void *a[1];
                QColor c = QColor::fromRgba(instr.storeColor.value);
                a[0] = (void *)&c;
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeColor.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreDate:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStoreDate> cc;
#endif
                QObject *target = stack.top();
                void *a[1];
                QDate d = QDate::fromJulianDay(instr.storeDate.value);
                a[0] = (void *)&d;
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeDate.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreTime:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                //QFxCompilerTimer<QFxCompiler::InstrStoreTime> cc;
#endif
                QObject *target = stack.top();
                void *a[1];
                QTime t;
                t.setHMS(intData.at(instr.storeTime.valueIndex),
                         intData.at(instr.storeTime.valueIndex+1),
                         intData.at(instr.storeTime.valueIndex+2),
                         intData.at(instr.storeTime.valueIndex+3));
                a[0] = (void *)&t;
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeTime.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreDateTime:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                //QFxCompilerTimer<QFxCompiler::InstrStoreDateTime> cc;
#endif
                QObject *target = stack.top();
                void *a[1];
                QTime t;
                t.setHMS(intData.at(instr.storeDateTime.valueIndex+1),
                         intData.at(instr.storeDateTime.valueIndex+2),
                         intData.at(instr.storeDateTime.valueIndex+3),
                         intData.at(instr.storeDateTime.valueIndex+4));
                QDateTime dt(QDate::fromJulianDay(intData.at(instr.storeDateTime.valueIndex)), t);
                a[0] = (void *)&dt;
                QMetaObject::metacall(target, QMetaObject::WriteProperty,
                                      instr.storeDateTime.propertyIndex, a);
            }
            break;

        case QmlInstruction::StorePoint:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStorePoint> cc;
#endif
                QObject *target = stack.top();
                void *a[1];
                QPoint p = QPointF(floatData.at(instr.storeRealPair.valueIndex),
                                   floatData.at(instr.storeRealPair.valueIndex+1)).toPoint();
                a[0] = (void *)&p;
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeRealPair.propertyIndex, a);
            }
            break;

        case QmlInstruction::StorePointF:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStorePoint> cc;
#endif
                QObject *target = stack.top();
                void *a[1];
                QPointF p(floatData.at(instr.storeRealPair.valueIndex),
                          floatData.at(instr.storeRealPair.valueIndex+1));
                a[0] = (void *)&p;
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeRealPair.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreSize:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStoreSize> cc;
#endif
                QObject *target = stack.top();
                void *a[1];
                QSize p = QSizeF(floatData.at(instr.storeRealPair.valueIndex),
                                 floatData.at(instr.storeRealPair.valueIndex+1)).toSize();
                a[0] = (void *)&p;
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeRealPair.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreSizeF:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStoreSize> cc;
#endif
                QObject *target = stack.top();
                void *a[1];
                QSizeF s(floatData.at(instr.storeRealPair.valueIndex),
                         floatData.at(instr.storeRealPair.valueIndex+1));
                a[0] = (void *)&s;
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeRealPair.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreRect:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                //QFxCompilerTimer<QFxCompiler::InstrStoreRect> cc;
#endif
                QObject *target = stack.top();
                void *a[1];
                QRect r = QRectF(floatData.at(instr.storeRect.valueIndex),
                                 floatData.at(instr.storeRect.valueIndex+1),
                                 floatData.at(instr.storeRect.valueIndex+2),
                                 floatData.at(instr.storeRect.valueIndex+3)).toRect();
                a[0] = (void *)&r;
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeRect.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreRectF:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                //QFxCompilerTimer<QFxCompiler::InstrStoreRect> cc;
#endif
                QObject *target = stack.top();
                void *a[1];
                QRectF r(floatData.at(instr.storeRect.valueIndex),
                         floatData.at(instr.storeRect.valueIndex+1),
                         floatData.at(instr.storeRect.valueIndex+2),
                         floatData.at(instr.storeRect.valueIndex+3));
                a[0] = (void *)&r;
                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeRect.propertyIndex, a);
            }
            break;

        case QmlInstruction::StoreObject:
            {
#ifdef Q_ENABLE_PERFORMANCE_LOG
                QFxCompilerTimer<QFxCompiler::InstrStoreObject> cc;
#endif
                QObject *assignObj = stack.pop();
                QObject *target = stack.top();

                void *a[1];
                void *obj = (void *)(((char *)assignObj) + instr.storeObject.cast);
                a[0] = (void *)&obj;

                QMetaObject::metacall(target, QMetaObject::WriteProperty, 
                                      instr.storeObject.propertyIndex, a);
            }
            break;
        default:
            qFatal("QmlCompiledComponent: Internal error - unknown instruction %d", instr.type);
            break;
    }

}

QT_END_NAMESPACE
