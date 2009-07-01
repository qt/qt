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

#include "qfxintegermodel.h"

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(QFxIntegerModel, IntegerModel)

class QFxIntegerModelPrivate
{
public:
    QFxIntegerModelPrivate() : minimum(0), maximum(0) {}
    int minimum;
    int maximum;
};

QFxIntegerModel::QFxIntegerModel(QObject *parent)
    : QListModelInterface(parent)
{
    d = new QFxIntegerModelPrivate;
}

QFxIntegerModel::~QFxIntegerModel()
{
    delete d;
}

int QFxIntegerModel::minimum() const
{
    return d->minimum;
}

void QFxIntegerModel::setMinimum(int minimum)
{
    d->minimum = minimum;
}

int QFxIntegerModel::maximum() const
{
    return d->maximum;
}

void QFxIntegerModel::setMaximum(int maximum)
{
    d->maximum = maximum;
}

int QFxIntegerModel::count() const
{
    return qMax(0, d->maximum - d->minimum + 1);
}

QHash<int,QVariant> QFxIntegerModel::data(int index, const QList<int> &roles) const
{
    QHash<int,QVariant> returnHash;

    for (int i = 0; i < roles.size(); ++i) {
        int role = roles.at(i);
        QVariant info;
        switch(role) {
        case Qt::DisplayRole:
            info = QVariant(QString::number(d->minimum+index));
            break;
        default:
            break;
        }
        returnHash.insert(role, info);
    }
    return returnHash;
}

QString QFxIntegerModel::toString(int role) const
{
    switch(role) {
    case Qt::DisplayRole:
        return QLatin1String("display");
    default:
        return QLatin1String("");
    }
}

QList<int> QFxIntegerModel::roles() const
{
    return QList<int>() << Qt::DisplayRole;
}

QT_END_NAMESPACE
