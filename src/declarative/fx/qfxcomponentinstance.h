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

#ifndef QFXCOMPONENTINSTANCE_H
#define QFXCOMPONENTINSTANCE_H

#include <QtDeclarative/qfxitem.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QFxComponentInstancePrivate;
class Q_DECLARATIVE_EXPORT QFxComponentInstance : public QFxItem
{
    Q_OBJECT
    Q_PROPERTY(QmlComponent *component READ component WRITE setComponent)
    Q_PROPERTY(QFxItem *instance READ instance)
    Q_CLASSINFO("DefaultProperty", "component")
public:
    QFxComponentInstance(QFxItem *parent=0);

    QmlComponent *component() const;
    void setComponent(QmlComponent *);

    QFxItem *instance() const;

Q_SIGNALS:
    void instanceChanged();

private Q_SLOTS:
    void updateSize();

private:
    void create();

protected:
    QFxComponentInstance(QFxComponentInstancePrivate &dd, QFxItem *parent);

private:
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr, QFxComponentInstance)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxComponentInstance)

QT_END_HEADER

#endif // QFXCOMPONENTINSTANCE_H
