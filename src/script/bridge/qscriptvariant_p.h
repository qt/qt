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

#include "qscriptobject_p.h"

QT_BEGIN_NAMESPACE

namespace QScript
{

class QVariantDelegate : public QScriptObjectDelegate
{
public:
    QVariantDelegate(const QVariant &value);
    ~QVariantDelegate();

    QVariant &value();
    void setValue(const QVariant &value);

    Type type() const;

private:
    QVariant m_value;
};

class QVariantPrototype : public QScriptObject
{
public:
    QVariantPrototype(JSC::ExecState*, WTF::PassRefPtr<JSC::Structure>,
                      JSC::Structure* prototypeFunctionStructure);
};

} // namespace QScript

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
