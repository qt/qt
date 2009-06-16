/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTVARIANT_P_H
#define QSCRIPTVARIANT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qvariant.h>

#ifndef QT_NO_SCRIPT

#include "JSObject.h"

QT_BEGIN_NAMESPACE

namespace QScript
{

class QVariantWrapperObject : public JSC::JSObject
{
public:
     // work around CELL_SIZE limitation
    struct Data
    {
        QVariant value;
    };

    explicit QVariantWrapperObject(WTF::PassRefPtr<JSC::Structure> sid);
    ~QVariantWrapperObject();
    
    virtual const JSC::ClassInfo* classInfo() const { return &info; }
    static const JSC::ClassInfo info;

    inline const QVariant &value() const { return data->value; }
    inline void setValue(const QVariant &value) { data->value = value; }

private:
    Data *data;
};

class QVariantPrototype : public QVariantWrapperObject
{
public:
    QVariantPrototype(JSC::ExecState*, WTF::PassRefPtr<JSC::Structure>,
                      JSC::Structure* prototypeFunctionStructure);
};

} // namespace QScript

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
