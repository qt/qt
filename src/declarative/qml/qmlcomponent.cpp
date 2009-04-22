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

#include "qmlcomponent.h"
#include "qmlcomponent_p.h"
#include "private/qmlcontext_p.h"
#include "private/qmlengine_p.h"
#include "qmlvme_p.h"
#include "qml.h"
#include <QStack>
#include <qfxperf.h>
#include <QStringList>
#include <qmlengine.h>
#include <QFileInfo>
#include <qmlbindablevalue.h>
#include "private/qmlxmlparser_p.h"
#include "qmlcompiledcomponent_p.h"
#include <QtCore/qdebug.h>
#include <QApplication>


QT_BEGIN_NAMESPACE
class QByteArray;

/*!
    \class QmlComponent
    \brief The QmlComponent class encapsulates a QML component description.
    \mainclass
*/
QML_DEFINE_TYPE(QmlComponent,Component);

void QmlComponentPrivate::typeDataReady()
{
    Q_Q(QmlComponent);

    Q_ASSERT(typeData);

    fromTypeData(typeData);
    typeData = 0;

    emit q->readyChanged();
}

void QmlComponentPrivate::fromTypeData(QmlCompositeTypeManager::TypeData *data)
{
    name = data->url;
    QmlCompiledComponent *c = data->toCompiledComponent(engine);

    if(!c) {
        Q_ASSERT(data->status == QmlCompositeTypeManager::TypeData::Error);

        qWarning().nospace() << "QmlComponent: "
                             << data->errorDescription.toLatin1().constData();
    } else {

        cc = c;

    }

    data->release();
}

/*!
    Construct a null QmlComponent.
*/
QmlComponent::QmlComponent(QObject *parent)
    : QObject(*(new QmlComponentPrivate), parent)
{
    Q_D(QmlComponent);
    d->name = QLatin1String("<unspecified file>");
}

/*!
    Destruct the QmlComponent.
*/
QmlComponent::~QmlComponent()
{
    Q_D(QmlComponent);
    if (d->typeData) {
        d->typeData->remWaiter(d);
        d->typeData->release();
    }
    if (d->cc)
        d->cc->release();
}

/*!
    \property QmlComponent::name
    \brief the component's name.

    The component's name is used in error and warning messages.  If available,
    the XML source file name is used as the component's name, otherwise it is
    set to "<unspecified file>".
*/
QString QmlComponent::name() const
{
    Q_D(const QmlComponent);
    return d->name;
}

void QmlComponent::setName(const QString &name)
{
    Q_D(QmlComponent);
    d->name = name;
}

/*!
    \internal
*/
QmlComponent::QmlComponent(QmlEngine *engine, QmlCompiledComponent *cc, int start, int count, QObject *parent)
    : QObject(*(new QmlComponentPrivate), parent)
{
    Q_D(QmlComponent);
    d->engine = engine;
    d->name = QLatin1String("<unspecified file>");
    d->cc = cc;
    cc->addref();
    d->start = start;
    d->count = count;
}

QmlComponent::QmlComponent(QmlEngine *engine, const QUrl &url, QObject *parent)
: QObject(*(new QmlComponentPrivate), parent)
{
    Q_D(QmlComponent);
    d->engine = engine;
    d->name = url.toString();

    QmlCompositeTypeManager::TypeData *data = 
        engine->d_func()->typeManager.get(url);

    if(data->status == QmlCompositeTypeManager::TypeData::Waiting) {

        d->typeData = data;
        d->typeData->addWaiter(d);

    } else {

        d->fromTypeData(data);

    }
}

/*!
    Create a QmlComponent with no data. Set setData().
*/
QmlComponent::QmlComponent(QmlEngine *engine, QObject *parent)
    : QObject(*(new QmlComponentPrivate), parent)
{
    Q_D(QmlComponent);
    d->engine = engine;
}

/*!
    Create a QmlComponent from the given XML \a data.  If provided, \a filename
    is used to set the component name, and to provide a base path for items
    resolved by this component.
*/
QmlComponent::QmlComponent(QmlEngine *engine, const QByteArray &data, const QUrl &url, QObject *parent)
    : QObject(*(new QmlComponentPrivate), parent)
{
    Q_D(QmlComponent);
    d->engine = engine;
    setData(data,url);
}

/*!
    Sets the QmlComponent to use the given XML \a data.  If provided, \a filename
    is used to set the component name, and to provide a base path for items
    resolved by this component.

    Currently only supported to call once per component.
*/
void QmlComponent::setData(const QByteArray &data, const QUrl &url)
{
    Q_D(QmlComponent);

    if(d->cc) d->cc->release();
    if(d->typeData) d->typeData->release();
    d->cc = 0;
    d->typeData = 0;

    d->name = url.toString();

    QmlCompositeTypeManager::TypeData *typeData = 
        d->engine->d_func()->typeManager.getImmediate(data, url);
    
    if(typeData->status == QmlCompositeTypeManager::TypeData::Waiting) {

        d->typeData = typeData;
        d->typeData->addWaiter(d);

    } else {

        d->fromTypeData(typeData);

    }

}

bool QmlComponent::isReady() const
{
    Q_D(const QmlComponent);

    return d->engine && !d->typeData;
}

/*!
    \internal
*/
QmlComponent::QmlComponent(QmlComponentPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
    Q_D(QmlComponent);
    d->name = QLatin1String("<unspecified file>");
    d->cc = new QmlCompiledComponent;
}

/*!
    Create an object instance from this component.  Returns 0 if creation
    failed.  \a context specifies the context within which to create the object
    instance.  

    If \a context is 0 (the default), it will create the instance in the
    engine' s \l {QmlEngine::rootContext()}{root context}.
*/
QObject *QmlComponent::create(QmlContext *context)
{
    Q_D(QmlComponent);

    if(!context)
        context = d->engine->rootContext();

    if(context->engine() != d->engine) {
        qWarning("QmlComponent::create(): Must create component in context from the same QmlEngine");
        return 0;
    }

    QObject *rv = beginCreate(context);
    completeCreate();
    return rv;
}

/*!
    This method provides more advanced control over component instance creation.
    In general, programmers should use QmlComponent::create() to create a 
    component.

    Create an object instance from this component.  Returns 0 if creation
    failed.  \a context specifies the context within which to create the object
    instance.  

    When QmlComponent constructs an instance, it occurs in three steps:
    \list 1
    \i The object hierarchy is created, and constant values are assigned.
    \i Property bindings are evaluated for the the first time.
    \i If applicable, QmlParserStatus::componentComplete() is called on objects.
    \endlist 
    QmlComponent::beginCreate() differs from QmlComponent::create() in that it
    only performs step 1.  QmlComponent::completeCreate() must be called to 
    complete steps 2 and 3.

    This breaking point is sometimes useful when using attached properties to
    communicate information to an instantiated component, as it allows their
    initial values to be configured before property bindings take effect.
*/
QObject *QmlComponent::beginCreate(QmlContext *context)
{
    Q_D(QmlComponent);

    if(!context) {
        qWarning("QmlComponent::beginCreate(): Cannot create a component in a null context");
        return 0;
    }

    if(context->engine() != d->engine) {
        qWarning("QmlComponent::beginCreate(): Must create component in context from the same QmlEngine");
        return 0;
    }

    if (d->completePending) {
        qWarning("QmlComponent: Cannot create new component instance before completing the previous");
        return 0;
    }
    if (!d->cc) {
        qWarning("QmlComponent: Cannot load component data");
        return 0;
    }

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::CreateComponent> perf;
#endif
    if(!d->engine->d_func()->rootComponent)
        d->engine->d_func()->rootComponent = this;

    QmlContext *ctxt = 
        new QmlContext(context, 0);
    static_cast<QmlContextPrivate*>(ctxt->d_ptr)->component = d->cc;
    static_cast<QmlContextPrivate*>(ctxt->d_ptr)->component->addref();
    ctxt->activate();

    QmlVME vme;
    QObject *rv = vme.run(ctxt, d->cc, d->start, d->count);
    if(vme.isError()) {
        qWarning().nospace()
#ifdef QML_VERBOSEERRORS_ENABLED
                             << "QmlComponent: "
#endif
                             << vme.errorDescription().toLatin1().constData() << " @"
                             << d->name.toLatin1().constData() << ":" << vme.errorLine();
    }


    ctxt->deactivate();

    if(rv) {
        QFx_setParent_noEvent(ctxt, rv);
        QmlEnginePrivate *ep = d->engine->d_func();
        if(ep->rootComponent == this) {
            ep->rootComponent = 0;

            d->bindValues = ep->currentBindValues;
            d->parserStatus = ep->currentParserStatus;
            ep->currentBindValues.clear();
            ep->currentParserStatus.clear();
            d->completePending = true;
        }
    } else {
        delete ctxt;
    }

    return rv;
}

/*!
    This method provides more advanced control over component instance creation.
    In general, programmers should use QmlComponent::create() to create a 
    component.

    Complete a component creation begin with QmlComponent::beginCreate().
*/
void QmlComponent::completeCreate()
{
    Q_D(QmlComponent);
    if (d->completePending) {
        {
#ifdef Q_ENABLE_PERFORMANCE_LOG
            QFxPerfTimer<QFxPerf::BindInit> bi;
#endif
            for(int ii = 0; ii < d->bindValues.count(); ++ii)
                d->bindValues.at(ii)->init();
        }
        QSet<QmlParserStatus *> done;
        for(int ii = 0; ii < d->parserStatus.count(); ++ii) {
            QmlParserStatus *ps = d->parserStatus.at(ii);
            if(!done.contains(ps)) {
                done.insert(ps);
                ps->componentComplete();
            }
        }

        d->bindValues.clear();
        d->parserStatus.clear();
        d->completePending = false;
    }
}
QT_END_NAMESPACE
