/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QDebug>
#include <QApplication>
#include <QTime>
#include <QDeclarativeContext>
#include <QGraphicsScene>
#include <QGraphicsRectItem>

class Timer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeComponent *component READ component WRITE setComponent)

public:
    Timer();

    QDeclarativeComponent *component() const;
    void setComponent(QDeclarativeComponent *);

    static Timer *timerInstance();

    void run(uint);

    bool willParent() const;
    void setWillParent(bool p);

private:
    void runTest(QDeclarativeContext *, uint);

    QDeclarativeComponent *m_component;
    static Timer *m_timer;

    bool m_willparent;
    QGraphicsScene m_scene;
    QGraphicsRectItem m_item;
};
QML_DECLARE_TYPE(Timer);

Timer *Timer::m_timer = 0;

Timer::Timer()
: m_component(0), m_willparent(false)
{
    if (m_timer)
        qWarning("Timer: Timer already registered");
    m_timer = this;

    m_scene.setItemIndexMethod(QGraphicsScene::NoIndex);
    m_scene.addItem(&m_item);
}

QDeclarativeComponent *Timer::component() const
{
    return m_component;
}

void Timer::setComponent(QDeclarativeComponent *c)
{
    m_component = c;
}

Timer *Timer::timerInstance()
{
    return m_timer;
}

void Timer::run(uint iterations)
{
    QDeclarativeContext context(qmlContext(this));

    QObject *o = m_component->create(&context);
    QGraphicsObject *go = qobject_cast<QGraphicsObject *>(o);
    if (m_willparent && go) 
        go->setParentItem(&m_item);
    delete o;
    
    runTest(&context, iterations);
}

bool Timer::willParent() const
{
    return m_willparent;
}

void Timer::setWillParent(bool p)
{
    m_willparent = p;
}

void Timer::runTest(QDeclarativeContext *context, uint iterations)
{
    QTime t;
    t.start();
    for (uint ii = 0; ii < iterations; ++ii) {
        QObject *o = m_component->create(context);
        QGraphicsObject *go = qobject_cast<QGraphicsObject *>(o);
        if (m_willparent && go) 
            go->setParentItem(&m_item);
        delete o;
    }

    int e = t.elapsed();

    qWarning() << "Total:" << e << "ms, Per iteration:" << qreal(e) / qreal(iterations) << "ms";

}

void usage(const char *name)
{
    qWarning("Usage: %s [-iterations <count>] [-parent] <qml file>", name);
    exit(-1);
}

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    qmlRegisterType<Timer>("QmlTime", 1, 0, "Timer");

    uint iterations = 1024;
    QString filename;
    bool willParent = false;

    for (int ii = 1; ii < argc; ++ii) {
        QByteArray arg(argv[ii]);

        if (arg == "-iterations") {
            if (ii + 1 < argc) {
                ++ii;
                QByteArray its(argv[ii]);
                bool ok = false;
                iterations = its.toUInt(&ok);
                if (!ok)
                    usage(argv[0]);
            } else {
                usage(argv[0]);
            }
        } else if (arg == "-parent") {
            willParent = true;
        } else {
            filename = QLatin1String(argv[ii]);
        }
    }

    if (filename.isEmpty())
#ifdef Q_OS_SYMBIAN
        filename = QLatin1String("./tests/item_creation/data.qml");
#else
        usage(argv[0]);
#endif

    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, filename);
    if (component.isError()) {
        qWarning() << component.errors();
        return -1;
    }

    QObject *obj = component.create();
    if (!obj) {
        qWarning() << component.errors();
        return -1;
    }

    Timer *timer = Timer::timerInstance();
    if (!timer) {
        qWarning() << "A Tester.Timer instance is required.";
        return -1;
    }

#ifdef Q_OS_SYMBIAN
    willParent = true;
#endif
    timer->setWillParent(willParent);

    if (!timer->component()) {
        qWarning() << "The timer has no component";
        return -1;
    }

#ifdef Q_OS_SYMBIAN
    iterations = 1024;
#endif

    timer->run(iterations);

    return 0;
}

#include "qmltime.moc"
