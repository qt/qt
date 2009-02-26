/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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

#ifndef QGESTURERECOGNIZER_H
#define QGESTURERECOGNIZER_H

#include "qgesture.h"
#include "qevent.h"
#include "qlist.h"
#include "qset.h"

QT_BEGIN_NAMESPACE

class QGestureRecognizer : public QObject
{
    Q_OBJECT
public:
    enum Result
    {
        NotGesture,
        GestureStarted,
        GestureFinished,
        MaybeGesture
    };

    inline QGestureRecognizer() { }
    //### remove this ctor
    inline QGestureRecognizer(const char *type) : type_(QLatin1String(type)) { }
    inline QGestureRecognizer(const Qt::GestureType &type) : type_(type) { }
    inline Qt::GestureType gestureType() const { return type_; }

    virtual Result recognize(const QList<QEvent*> &inputEvents) = 0;
    virtual QGesture* makeEvent() const = 0;
    virtual void reset() = 0;

signals:
    void triggered(QGestureRecognizer::Result result);

protected:
    Qt::GestureType type_;
};

QT_END_NAMESPACE

#endif // QGESTURERECOGNIZER_P_H
