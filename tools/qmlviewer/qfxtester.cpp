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

#include <qfxtester.h>
#include <QDebug>
#include <QApplication>
#include <qmlview.h>
#include <QFile>
#include <QmlComponent>
#include <QCryptographicHash>
#include <private/qabstractanimation_p.h>
#include <private/qfxitem_p.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt.VisualTest, 4, 6, (QT_VERSION&0x00ff00)>>8, VisualTest, QFxVisualTest);
QML_DEFINE_TYPE(Qt.VisualTest, 4, 6, (QT_VERSION&0x00ff00)>>8, Frame, QFxVisualTestFrame);
QML_DEFINE_TYPE(Qt.VisualTest, 4, 6, (QT_VERSION&0x00ff00)>>8, Mouse, QFxVisualTestMouse);
QML_DEFINE_TYPE(Qt.VisualTest, 4, 6, (QT_VERSION&0x00ff00)>>8, Key, QFxVisualTestKey);

QFxTester::QFxTester(const QString &script, QmlViewer::ScriptOptions opts, 
                     QmlView *parent)
: QAbstractAnimation(parent), m_view(parent), filterEvents(true), options(opts), 
  testscript(0), hasFailed(false)
{
    parent->viewport()->installEventFilter(this);
    parent->installEventFilter(this);
    QUnifiedTimer::instance()->setConsistentTiming(true);
    if (options & QmlViewer::Play)
        this->run(script);
    start();
}

int QFxTester::duration() const
{
    return -1;
}

void QFxTester::addMouseEvent(Destination dest, QMouseEvent *me)
{
    MouseEvent e(me);
    e.destination = dest;
    m_mouseEvents << e;
}

void QFxTester::addKeyEvent(Destination dest, QKeyEvent *ke)
{
    KeyEvent e(ke);
    e.destination = dest;
    m_keyEvents << e;
}

bool QFxTester::eventFilter(QObject *o, QEvent *e)
{
    if (!filterEvents)
        return false;

    Destination destination;
    if (o == m_view) {
        destination = View;
    } else if (o == m_view->viewport()) {
        destination = ViewPort;
    } else {
        return false;
    }

    switch (e->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
            addKeyEvent(destination, (QKeyEvent *)e);
            return true;
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::MouseButtonDblClick:
            addMouseEvent(destination, (QMouseEvent *)e);
            return true;
        default:
            break;
    }
    return false;
}

void QFxTester::executefailure()
{
    hasFailed = true;

    if (options & QmlViewer::ExitOnFailure)
        exit(-1);
}

void QFxTester::imagefailure()
{
    hasFailed = true;

    if (options & QmlViewer::ExitOnFailure)
        exit(-1);
}

void QFxTester::complete()
{
    if (options & QmlViewer::ExitOnComplete)
        QApplication::exit(hasFailed?-1:0);
}

void QFxTester::run(const QString &name)
{
    QmlComponent c(m_view->engine(), name + QLatin1String(".qml"));

    testscript = qobject_cast<QFxVisualTest *>(c.create());
    if (testscript) testscript->setParent(this);
    testscriptidx = 0;
}

void QFxTester::save(const QString &name)
{
    QFile file(name + QLatin1String(".qml"));
    file.open(QIODevice::WriteOnly);
    QTextStream ts(&file);

    ts << "import Qt.VisualTest 4.6\n\n";
    ts << "VisualTest {\n";

    int imgCount = 0;
    for (int ii = 0; ii < m_savedFrameEvents.count(); ++ii) {
        const FrameEvent &fe = m_savedFrameEvents.at(ii);
        ts << "    Frame {\n";
        ts << "        msec: " << fe.msec << "\n";
        if (!fe.hash.isEmpty()) {
            ts << "        hash: \"" << fe.hash.toHex() << "\"\n";
        } else if (!fe.image.isNull()) {
            QString filename = name + "." + QString::number(imgCount++) + ".png";
            fe.image.save(filename);
            ts << "        image: \"" << filename << "\"\n";
        }
        ts << "    }\n";

        while (!m_savedMouseEvents.isEmpty() && 
               m_savedMouseEvents.first().msec == fe.msec) {
            MouseEvent me = m_savedMouseEvents.takeFirst();

            ts << "    Mouse {\n";
            ts << "        type: " << me.type << "\n";
            ts << "        button: " << me.button << "\n";
            ts << "        buttons: " << me.buttons << "\n";
            ts << "        x: " << me.pos.x() << "; y: " << me.pos.y() << "\n";
            ts << "        modifiers: " << me.modifiers << "\n";
            if (me.destination == ViewPort)
                ts << "        sendToViewport: true\n";
            ts << "    }\n";
        }

        while (!m_savedKeyEvents.isEmpty() &&
               m_savedKeyEvents.first().msec == fe.msec) {
            KeyEvent ke = m_savedKeyEvents.takeFirst();

            ts << "    Key {\n";
            ts << "        type: " << ke.type << "\n";
            ts << "        key: " << ke.key << "\n";
            ts << "        modifiers: " << ke.modifiers << "\n";
            ts << "        text: \"" << ke.text.toUtf8().toHex() << "\"\n";
            ts << "        autorep: " << (ke.autorep?"true":"false") << "\n";
            ts << "        count: " << ke.count << "\n";
            if (ke.destination == ViewPort)
                ts << "        sendToViewport: true\n";
            ts << "    }\n";
        }
    }
    m_savedFrameEvents.clear();

    ts << "}\n";
    file.close();
}

void QFxTester::updateCurrentTime(int msec)
{
    QFxItemPrivate::setConsistentTime(msec);

    QImage img(m_view->width(), m_view->height(), QImage::Format_RGB32);

    QPainter p(&img);
    m_view->render(&p);

    FrameEvent fe;
    fe.msec = msec;
    if (msec == 0) {
        // Skip first frame
    } else if (0 == (m_savedFrameEvents.count() % 60)) {
        fe.image = img;
    } else {
        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData((const char *)img.bits(), img.bytesPerLine() * img.height());
        fe.hash = hash.result();
    }
    m_savedFrameEvents.append(fe);

    // Deliver mouse events
    filterEvents = false;

    if (!testscript) {
        for (int ii = 0; ii < m_mouseEvents.count(); ++ii) {
            MouseEvent &me = m_mouseEvents[ii];
            me.msec = msec;
            QMouseEvent event(me.type, me.pos, me.button, me.buttons, me.modifiers);

            if (me.destination == View) {
                QCoreApplication::sendEvent(m_view, &event);
            } else {
                QCoreApplication::sendEvent(m_view->viewport(), &event);
            }
        }

        for (int ii = 0; ii < m_keyEvents.count(); ++ii) {
            KeyEvent &ke = m_keyEvents[ii];
            ke.msec = msec;
            QKeyEvent event(ke.type, ke.key, ke.modifiers, ke.text, ke.autorep, ke.count);

            if (ke.destination == View) {
                QCoreApplication::sendEvent(m_view, &event);
            } else {
                QCoreApplication::sendEvent(m_view->viewport(), &event);
            }
        }
        m_savedMouseEvents.append(m_mouseEvents);
        m_savedKeyEvents.append(m_keyEvents);
    }

    m_mouseEvents.clear();
    m_keyEvents.clear();

    // Advance test script
    static int imgCount = 0;
    while (testscript && testscript->count() > testscriptidx) {

        QObject *event = testscript->event(testscriptidx);

        if (QFxVisualTestFrame *frame = qobject_cast<QFxVisualTestFrame *>(event)) {
            if (frame->msec() < msec) {
                if (options & QmlViewer::TestImages) {
                    qWarning() << "QFxTester: Extra frame.  Seen:" 
                               << msec << "Expected:" << frame->msec();
                    imagefailure();
                }
            } else if (frame->msec() == msec) {
                if (frame->hash().toUtf8() != fe.hash.toHex()) {
                    if (options & QmlViewer::TestImages) {
                        qWarning() << "QFxTester: Mismatched frame hash.  Seen:" 
                                   << fe.hash.toHex() << "Expected:" 
                                   << frame->hash().toUtf8();
                        imagefailure();
                    }
                }
            } else if (frame->msec() > msec) {
                break;
            }

            if (options & QmlViewer::TestImages && !frame->image().isEmpty()) {
                QImage goodImage(frame->image());
                if (goodImage != img) {
                    QString reject(frame->image() + ".reject.png");
                    qWarning() << "QFxTester: Image mismatch.  Reject saved to:" 
                               << reject;
                    img.save(reject);
                    imagefailure();
                }
            }
        } else if (QFxVisualTestMouse *mouse = qobject_cast<QFxVisualTestMouse *>(event)) {
            QPoint pos(mouse->x(), mouse->y());
            QPoint globalPos = m_view->mapToGlobal(QPoint(0, 0)) + pos;
            QMouseEvent event((QEvent::Type)mouse->type(), pos, globalPos, (Qt::MouseButton)mouse->button(), (Qt::MouseButtons)mouse->buttons(), (Qt::KeyboardModifiers)mouse->modifiers());

            if (!mouse->sendToViewport()) {
                QCoreApplication::sendEvent(m_view, &event);
            } else {
                QCoreApplication::sendEvent(m_view->viewport(), &event);
            }
        } else if (QFxVisualTestKey *key = qobject_cast<QFxVisualTestKey *>(event)) {

            QKeyEvent event((QEvent::Type)key->type(), key->key(), (Qt::KeyboardModifiers)key->modifiers(), QString::fromUtf8(QByteArray::fromHex(key->text().toUtf8())), key->autorep(), key->count());

            if (!key->sendToViewport()) {
                QCoreApplication::sendEvent(m_view, &event);
            } else {
                QCoreApplication::sendEvent(m_view->viewport(), &event);
            }
        } 
        testscriptidx++;
    }

    filterEvents = true;

    if (testscript && testscript->count() <= testscriptidx)
        complete();
}

QT_END_NAMESPACE
