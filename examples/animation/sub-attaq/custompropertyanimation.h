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

#ifndef CUSTOMPROPERTYANIMATION_H
#define CUSTOMPROPERTYANIMATION_H

#include <QtCore/qvariantanimation.h>

QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE

struct AbstractProperty
{
    virtual QVariant read() const = 0;
    virtual void write(const QVariant &value) = 0;
};


class CustomPropertyAnimation : public QVariantAnimation
{
    Q_OBJECT

    template <typename Target, typename T, typename T2 = T>
    class MemberFunctionProperty : public AbstractProperty
    {
    public:
        typedef T (Target::*Getter)(void) const;
        typedef void (Target::*Setter)(T2);

        MemberFunctionProperty(Target* target, Getter getter, Setter setter)
            : m_target(target), m_getter(getter), m_setter(setter) {}

        virtual void write(const QVariant &value)
        {
            if (m_setter) (m_target->*m_setter)(qVariantValue<T>(value));
        }

        virtual QVariant read() const
        {
            if (m_getter) return qVariantFromValue<T>((m_target->*m_getter)());
            return QVariant();
        }

    private:
        Target *m_target;
        Getter m_getter;
        Setter m_setter;
    };

public:
    CustomPropertyAnimation(QObject *parent = 0);
    ~CustomPropertyAnimation();

    template<class Target, typename T>
    void setMemberFunctions(Target* target, T (Target::*getter)() const, void (Target::*setter)(const T& ))
    {
        setProperty(new MemberFunctionProperty<Target, T, const T&>(target, getter, setter));
    }

    template<class Target, typename T>
    void setMemberFunctions(Target* target, T (Target::*getter)() const, void (Target::*setter)(T))
    {
        setProperty(new MemberFunctionProperty<Target, T>(target, getter, setter));
    }

    void updateCurrentValue(const QVariant &value);
    void updateState(QAbstractAnimation::State oldState, QAbstractAnimation::State newState);
    void setProperty(AbstractProperty *animProp);

private:
    Q_DISABLE_COPY(CustomPropertyAnimation);
    AbstractProperty *animProp;
};

#endif // CUSTOMPROPERTYANIMATION_H
