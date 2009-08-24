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

#ifndef QMLTRANSITION_H
#define QMLTRANSITION_H

#include <QtCore/qobject.h>
#include <QtDeclarative/qfxglobal.h>
#include <QtDeclarative/qmlstate.h>
#include <QtDeclarative/qml.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlAbstractAnimation;
class QmlTransitionPrivate;
class QmlTransitionManager;
class Q_DECLARATIVE_EXPORT QmlTransition : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlTransition)

    Q_PROPERTY(QString from READ fromState WRITE setFromState)
    Q_PROPERTY(QString to READ toState WRITE setToState)
    Q_PROPERTY(bool reversible READ reversible WRITE setReversible)
    Q_PROPERTY(QmlList<QmlAbstractAnimation *>* animations READ animations)
    Q_CLASSINFO("DefaultProperty", "animations")

public:
    QmlTransition(QObject *parent=0);
    ~QmlTransition();

    QString fromState() const;
    void setFromState(const QString &);

    QString toState() const;
    void setToState(const QString &);

    bool reversible() const;
    void setReversible(bool);

    QmlList<QmlAbstractAnimation *>* animations();

    void prepare(QmlStateOperation::ActionList &actions,
                 QList<QmlMetaProperty> &after,
                 QmlTransitionManager *end);

    void setReversed(bool r);
    void stop();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlTransition)

QT_END_HEADER

#endif // QMLTRANSITION_H
