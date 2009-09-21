/****************************************************************************
**
** Copyright (C) 1992-$THISYEAR$ $TROLLTECH$. All rights reserved.
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QFXTESTER_H
#define QFXTESTER_H

#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <qmlviewer.h>

QT_BEGIN_NAMESPACE

class QFxVisualTest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject *>* events READ events CONSTANT)
    Q_CLASSINFO("DefaultProperty", "events")
public:
    QFxVisualTest() {}

    QList<QObject *> *events() { return &m_events; }

    int count() const { return m_events.count(); }
    QObject *event(int idx) { return m_events.at(idx); }

private:
    QList<QObject *> m_events;
};
QML_DECLARE_TYPE(QFxVisualTest)

class QFxVisualTestFrame : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int msec READ msec WRITE setMsec)
    Q_PROPERTY(QString hash READ hash WRITE setHash)
    Q_PROPERTY(QString image READ image WRITE setImage)
public:
    QFxVisualTestFrame() : m_msec(-1) {}

    int msec() const { return m_msec; }
    void setMsec(int m) { m_msec = m; }

    QString hash() const { return m_hash; }
    void setHash(const QString &hash) { m_hash = hash; }

    QString image() const { return m_image; }
    void setImage(const QString &image) { m_image = image; }

private:
    int m_msec;
    QString m_hash;
    QString m_image;
};
QML_DECLARE_TYPE(QFxVisualTestFrame)

class QFxVisualTestMouse : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int type READ type WRITE setType)
    Q_PROPERTY(int button READ button WRITE setButton)
    Q_PROPERTY(int buttons READ buttons WRITE setButtons)
    Q_PROPERTY(int x READ x WRITE setX)
    Q_PROPERTY(int y READ y WRITE setY)
    Q_PROPERTY(int modifiers READ modifiers WRITE setModifiers)
    Q_PROPERTY(bool sendToViewport READ sendToViewport WRITE setSendToViewport)
public:
    QFxVisualTestMouse() : m_type(0), m_button(0), m_buttons(0), m_x(0), m_y(0), m_modifiers(0), m_viewport(false) {}

    int type() const { return m_type; }
    void setType(int t) { m_type = t; }
    
    int button() const { return m_button; }
    void setButton(int b) { m_button = b; }

    int buttons() const { return m_buttons; }
    void setButtons(int b) { m_buttons = b; }

    int x() const { return m_x; }
    void setX(int x) { m_x = x; }

    int y() const { return m_y; }
    void setY(int y) { m_y = y; }

    int modifiers() const { return m_modifiers; }
    void setModifiers(int modifiers) { m_modifiers = modifiers; }

    bool sendToViewport() const { return m_viewport; }
    void setSendToViewport(bool v) { m_viewport = v; }
private:
    int m_type;
    int m_button;
    int m_buttons;
    int m_x;
    int m_y;
    int m_modifiers;
    bool m_viewport;
};
QML_DECLARE_TYPE(QFxVisualTestMouse)

class QFxVisualTestKey : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int type READ type WRITE setType)
    Q_PROPERTY(int key READ key WRITE setKey)
    Q_PROPERTY(int modifiers READ modifiers WRITE setModifiers)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(bool autorep READ autorep WRITE setAutorep)
    Q_PROPERTY(int count READ count WRITE setCount)
    Q_PROPERTY(bool sendToViewport READ sendToViewport WRITE setSendToViewport)
public:
    QFxVisualTestKey() : m_type(0), m_key(0), m_modifiers(0), m_autorep(false), m_count(0), m_viewport(false) {}

    int type() const { return m_type; }
    void setType(int t) { m_type = t; }

    int key() const { return m_key; }
    void setKey(int k) { m_key = k; }

    int modifiers() const { return m_modifiers; }
    void setModifiers(int m) { m_modifiers = m; }

    QString text() const { return m_text; }
    void setText(const QString &t) { m_text = t; }

    bool autorep() const { return m_autorep; }
    void setAutorep(bool a) { m_autorep = a; }

    int count() const { return m_count; }
    void setCount(int c) { m_count = c; }

    bool sendToViewport() const { return m_viewport; }
    void setSendToViewport(bool v) { m_viewport = v; }
private:
    int m_type;
    int m_key;
    int m_modifiers;
    QString m_text;
    bool m_autorep;
    int m_count;
    bool m_viewport;
};
QML_DECLARE_TYPE(QFxVisualTestKey)

class QFxTester : public QAbstractAnimation
{
public:
    QFxTester(const QString &script, QmlViewer::ScriptOptions options, QFxView *parent);

    virtual int duration() const;

    void run(const QString &);
    void save(const QString &);

    void executefailure();
protected:
    virtual void updateCurrentTime(int msecs);
    virtual bool eventFilter(QObject *, QEvent *);

private:
    void imagefailure();
    void complete();

    enum Destination { View, ViewPort };
    void addKeyEvent(Destination, QKeyEvent *);
    void addMouseEvent(Destination, QMouseEvent *);
    QFxView *m_view;

    struct MouseEvent {
        MouseEvent(QMouseEvent *e)
            : type(e->type()), button(e->button()), buttons(e->buttons()), 
              pos(e->pos()), modifiers(e->modifiers()), destination(View) {}

        QEvent::Type type;
        Qt::MouseButton button;
        Qt::MouseButtons buttons;
        QPoint pos;
        Qt::KeyboardModifiers modifiers;
        Destination destination;

        int msec;
    };
    struct KeyEvent {
        KeyEvent(QKeyEvent *e)
            : type(e->type()), key(e->key()), modifiers(e->modifiers()), text(e->text()),
              autorep(e->isAutoRepeat()), count(e->count()), destination(View) {}
        QEvent::Type type;
        int key;
        Qt::KeyboardModifiers modifiers;
        QString text;
        bool autorep;
        ushort count;
        Destination destination;

        int msec;
    };
    struct FrameEvent {
        QImage image;
        QByteArray hash;
        int msec;
    };
    QList<MouseEvent> m_mouseEvents;
    QList<KeyEvent> m_keyEvents;

    QList<MouseEvent> m_savedMouseEvents;
    QList<KeyEvent> m_savedKeyEvents;
    QList<FrameEvent> m_savedFrameEvents;
    bool filterEvents;

    QmlViewer::ScriptOptions options;
    int testscriptidx;
    QFxVisualTest *testscript;

    bool hasFailed;
};


QT_END_NAMESPACE

#endif // QFXTESTER_H
