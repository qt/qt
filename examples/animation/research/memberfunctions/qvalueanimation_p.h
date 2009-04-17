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

#ifndef QVALUEANIMATION_P_H
#define QVALUEANIMATION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QIODevice. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qvariantanimation_p.h>   //###

QT_BEGIN_NAMESPACE

class QValueAnimationPrivate : public QVariantAnimationPrivate
{
   Q_DECLARE_PUBLIC(QValueAnimation)
public:
    QValueAnimationPrivate() : QVariantAnimationPrivate(), animProp(0)
    {
    }

    void initDefaultStartValue();

    AbstractProperty *animProp;

    //###TODO
};

QT_END_NAMESPACE

#endif //QVALUEANIMATION_P_H
