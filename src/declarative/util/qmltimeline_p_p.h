/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMLTIMELINE_H
#define QMLTIMELINE_H

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

#include <QtCore/QObject>
#include <QtCore/QAbstractAnimation>

QT_BEGIN_NAMESPACE

class QEasingCurve;
class QmlTimeLineValue;
class QmlTimeLineEvent;
struct QmlTimeLinePrivate;
class QmlTimeLineObject;
class Q_DECLARATIVE_EXPORT QmlTimeLine : public QAbstractAnimation
{
Q_OBJECT
public:
    QmlTimeLine(QObject *parent = 0);
    ~QmlTimeLine();

    enum SyncMode { LocalSync, GlobalSync };
    SyncMode syncMode() const;
    void setSyncMode(SyncMode);

    void pause(QmlTimeLineObject &, int);
    void execute(const QmlTimeLineEvent &);
    void set(QmlTimeLineValue &, qreal);

    int accel(QmlTimeLineValue &, qreal velocity, qreal accel);
    int accel(QmlTimeLineValue &, qreal velocity, qreal accel, qreal maxDistance);
    int accelDistance(QmlTimeLineValue &, qreal velocity, qreal distance);

    void move(QmlTimeLineValue &, qreal destination, int time = 500);
    void move(QmlTimeLineValue &, qreal destination, const QEasingCurve &, int time = 500);
    void moveBy(QmlTimeLineValue &, qreal change, int time = 500);
    void moveBy(QmlTimeLineValue &, qreal change, const QEasingCurve &, int time = 500);

    void sync();
    void setSyncPoint(int);
    int syncPoint() const;

    void sync(QmlTimeLineValue &);
    void sync(QmlTimeLineValue &, QmlTimeLineValue &);

    void reset(QmlTimeLineValue &);

    void complete();
    void clear();
    bool isActive() const;

    int time() const;

    virtual int duration() const;
Q_SIGNALS:
    void updated();
    void completed();

protected:
    virtual void updateCurrentTime(int);

private:
    void remove(QmlTimeLineObject *);
    friend class QmlTimeLineObject;
    friend struct QmlTimeLinePrivate;
    QmlTimeLinePrivate *d;
};

class Q_DECLARATIVE_EXPORT QmlTimeLineObject
{
public:
    QmlTimeLineObject();
    virtual ~QmlTimeLineObject();

protected:
    friend class QmlTimeLine;
    friend struct QmlTimeLinePrivate;
    QmlTimeLine *_t;
};

class Q_DECLARATIVE_EXPORT QmlTimeLineValue : public QmlTimeLineObject
{
public:
    QmlTimeLineValue(qreal v = 0.) : _v(v) {}

    virtual qreal value() const { return _v; }
    virtual void setValue(qreal v) { _v = v; }

    QmlTimeLine *timeLine() const { return _t; }

    operator qreal() const { return _v; }
    QmlTimeLineValue &operator=(qreal v) { setValue(v); return *this; }
private:
    friend class QmlTimeLine;
    friend struct QmlTimeLinePrivate;
    qreal _v;
};

class Q_DECLARATIVE_EXPORT QmlTimeLineEvent
{
public:
    QmlTimeLineEvent();
    QmlTimeLineEvent(const QmlTimeLineEvent &o);

    template<class T, void (T::*method)()>
    QmlTimeLineEvent(QmlTimeLineObject *b, T *c)
    {
	d0 = &callFunc<T, method>;
	d1 = (void *)c;
	d2 = b;
    }

    template<class T, void (T::*method)()>
    static QmlTimeLineEvent timeLineEvent(QmlTimeLineObject *b, T *c)
    {
        QmlTimeLineEvent rv;
	rv.d0 = &callFunc<T, method>;
	rv.d1 = (void *)c;
	rv.d2 = b;
	return rv;
    }

    QmlTimeLineEvent &operator=(const QmlTimeLineEvent &o);
    void execute() const;
    QmlTimeLineObject *eventObject() const;

private:
    typedef void (*CallFunc)(void *c);

    template <class T, void (T::*method)()>
    static void callFunc(void *c)
    {
        T *cls = (T *)c;
	(cls->*method)();
    }
    CallFunc d0;
    void *d1;
    QmlTimeLineObject *d2;
};

template<class T>
class QmlTimeLineValueProxy : public QmlTimeLineValue
{
public:
    QmlTimeLineValueProxy(T *cls, void (T::*func)(qreal), qreal v = 0.)
    : QmlTimeLineValue(v), _class(cls), _setFunctionReal(func), _setFunctionInt(0)
    {
        Q_ASSERT(_class);
    }

    QmlTimeLineValueProxy(T *cls, void (T::*func)(int), qreal v = 0.)
    : QmlTimeLineValue(v), _class(cls), _setFunctionReal(0), _setFunctionInt(func)
    {
        Q_ASSERT(_class);
    }

    virtual void setValue(qreal v)
    {
        QmlTimeLineValue::setValue(v);
        if (_setFunctionReal) (_class->*_setFunctionReal)(v);
        else if (_setFunctionInt) (_class->*_setFunctionInt)((int)v);
    }

private:
    T *_class;
    void (T::*_setFunctionReal)(qreal);
    void (T::*_setFunctionInt)(int);
};

QT_END_NAMESPACE

#endif
