/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QVALUEANIMATION_H
#define QVALUEANIMATION_H

#if defined(QT_EXPERIMENTAL_SOLUTION)
# include "qvariantanimation.h"
#else
# include <QtCore/qvariantanimation.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGraphicsItem;
class QValueAnimationPrivate;

QT_MODULE(Gui)

struct AbstractProperty {
    virtual void write(const QVariant &value) = 0;
    virtual QVariant read() const = 0;
};

# define CALL_MEMBER_FN(object,ptrToMember)  ((object)->*(ptrToMember))
template <typename Target, typename T>
class MemberFunctionProperty : public AbstractProperty {
public:
    typedef void (Target::*RefWrite)(const T &);
    typedef T (Target::*ValRead)(void) const;

    MemberFunctionProperty(Target *target, ValRead readFunc, RefWrite writeFunc) 
        : m_target(target), m_readFn(readFunc), m_writeFn(writeFunc) {}

    virtual void write(const QVariant &value) {
        CALL_MEMBER_FN(m_target, m_writeFn)(qVariantValue<T>(value));
    }

    virtual QVariant read() const {
        if (m_readFn)
            return qVariantFromValue<T>(CALL_MEMBER_FN(m_target, m_readFn)());
        return QVariant();
    }

private:
    Target *m_target;
    ValRead m_readFn;
    RefWrite m_writeFn;
};


class QValueAnimation : public QVariantAnimation
{
    Q_OBJECT

public:
    QValueAnimation(QObject *parent = 0);
    ~QValueAnimation();

    template <typename Target, typename T>
    void setMemberFunction(Target *target, 
                            T (Target::*readFunc)(void) const,      // ### useValRead typedef
                            void (Target::*writeFunc)(const T &)    // ### use RefWrite typedef
                            ) {
        // ### ownership of MemberFunctionProperty
        AbstractProperty *animProp = new MemberFunctionProperty<Target, T>(target, readFunc, writeFunc);
        setProperty(animProp);
    }

    void updateCurrentValue(const QVariant &value);
    void updateState(QAbstractAnimation::State oldState, QAbstractAnimation::State newState);
    void setProperty(AbstractProperty *animProp);

private:
    Q_DISABLE_COPY(QValueAnimation);
    Q_DECLARE_PRIVATE(QValueAnimation);
};

#endif // QVALUEANIMATION_H
