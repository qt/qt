/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include <qfxtester.h>
#include <QDebug>
#include <QApplication>
#include <qmlview.h>
#include <QFile>
#include <QmlComponent>
#include <QDir>
#include <QCryptographicHash>
#include <private/qabstractanimation_p.h>
#include <private/qmlgraphicsitem_p.h>

QT_BEGIN_NAMESPACE


QmlGraphicsTester::QmlGraphicsTester(const QString &script, QmlViewer::ScriptOptions opts, 
                     QmlView *parent)
: QAbstractAnimation(parent), m_script(script), m_view(parent), filterEvents(true), options(opts), 
  testscript(0), hasCompleted(false), hasFailed(false)
{
    parent->viewport()->installEventFilter(this);
    parent->installEventFilter(this);
    QUnifiedTimer::instance()->setConsistentTiming(true);
    if (options & QmlViewer::Play)
        this->run();
    start();
}

QmlGraphicsTester::~QmlGraphicsTester()
{
    if (!hasFailed && 
        options & QmlViewer::Record && 
        options & QmlViewer::SaveOnExit)
        save();
}

int QmlGraphicsTester::duration() const
{
    return -1;
}

void QmlGraphicsTester::addMouseEvent(Destination dest, QMouseEvent *me)
{
    MouseEvent e(me);
    e.destination = dest;
    m_mouseEvents << e;
}

void QmlGraphicsTester::addKeyEvent(Destination dest, QKeyEvent *ke)
{
    KeyEvent e(ke);
    e.destination = dest;
    m_keyEvents << e;
}

bool QmlGraphicsTester::eventFilter(QObject *o, QEvent *e)
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

void QmlGraphicsTester::executefailure()
{
    hasFailed = true;

    if (options & QmlViewer::ExitOnFailure)
        exit(-1);
}

void QmlGraphicsTester::imagefailure()
{
    hasFailed = true;

    if (options & QmlViewer::ExitOnFailure)
        exit(-1);
}

void QmlGraphicsTester::complete()
{
    if ((options & QmlViewer::TestErrorProperty) && !hasFailed) {
        QString e = m_view->rootObject()->property("error").toString();
        if (!e.isEmpty()) {
            qWarning() << "Test failed:" << e;
            hasFailed = true;
        }
    }
    if (options & QmlViewer::ExitOnComplete) 
        QApplication::exit(hasFailed?-1:0);

    if (hasCompleted)
        return;
    hasCompleted = true;

    if (options & QmlViewer::Play)
        qWarning("Script playback complete");
}

void QmlGraphicsTester::run()
{
    QmlComponent c(m_view->engine(), m_script + QLatin1String(".qml"));

    testscript = qobject_cast<QmlGraphicsVisualTest *>(c.create());
    if (testscript) testscript->setParent(this);
    else { executefailure(); exit(-1); }
    testscriptidx = 0;
}

void QmlGraphicsTester::save()
{
    QString filename = m_script + QLatin1String(".qml");
    QFileInfo filenameInfo(filename);
    QDir saveDir = filenameInfo.absoluteDir();
    saveDir.mkpath(".");

    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QTextStream ts(&file);

    ts << "import Qt.VisualTest 4.6\n\n";
    ts << "VisualTest {\n";

    int imgCount = 0;
    QList<KeyEvent> keyevents = m_savedKeyEvents;
    QList<MouseEvent> mouseevents = m_savedMouseEvents;
    for (int ii = 0; ii < m_savedFrameEvents.count(); ++ii) {
        const FrameEvent &fe = m_savedFrameEvents.at(ii);
        ts << "    Frame {\n";
        ts << "        msec: " << fe.msec << "\n";
        if (!fe.hash.isEmpty()) {
            ts << "        hash: \"" << fe.hash.toHex() << "\"\n";
        } else if (!fe.image.isNull()) {
            QString filename = filenameInfo.baseName() + "." + QString::number(imgCount) + ".png";
            fe.image.save(m_script + "." + QString::number(imgCount) + ".png");
            imgCount++;
            ts << "        image: \"" << filename << "\"\n";
        }
        ts << "    }\n";

        while (!mouseevents.isEmpty() && 
               mouseevents.first().msec == fe.msec) {
            MouseEvent me = mouseevents.takeFirst();

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

        while (!keyevents.isEmpty() &&
               keyevents.first().msec == fe.msec) {
            KeyEvent ke = keyevents.takeFirst();

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

    ts << "}\n";
    file.close();
}

void QmlGraphicsTester::updateCurrentTime(int msec)
{
    QmlGraphicsItemPrivate::setConsistentTime(msec);

    QImage img(m_view->width(), m_view->height(), QImage::Format_RGB32);

    if (options & QmlViewer::TestImages) {
        img.fill(qRgb(255,255,255));
        QPainter p(&img);
        m_view->render(&p);
    }

    FrameEvent fe;
    fe.msec = msec;
    if (msec == 0 || !(options & QmlViewer::TestImages)) {
        // Skip first frame, skip if not doing images
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

        if (QmlGraphicsVisualTestFrame *frame = qobject_cast<QmlGraphicsVisualTestFrame *>(event)) {
            if (frame->msec() < msec) {
                if (options & QmlViewer::TestImages && !(options & QmlViewer::Record)) {
                    qWarning() << "QmlGraphicsTester: Extra frame.  Seen:" 
                               << msec << "Expected:" << frame->msec();
                    imagefailure();
                }
            } else if (frame->msec() == msec) {
                if (!frame->hash().isEmpty() && frame->hash().toUtf8() != fe.hash.toHex()) {
                    if (options & QmlViewer::TestImages && !(options & QmlViewer::Record)) {
                        qWarning() << "QmlGraphicsTester: Mismatched frame hash.  Seen:" 
                                   << fe.hash.toHex() << "Expected:" 
                                   << frame->hash().toUtf8();
                        imagefailure();
                    }
                }
            } else if (frame->msec() > msec) {
                break;
            }

            if (options & QmlViewer::TestImages && !(options & QmlViewer::Record) && !frame->image().isEmpty()) {
                QImage goodImage(frame->image().toLocalFile());
                if (goodImage != img) {
                    QString reject(frame->image().toLocalFile() + ".reject.png");
                    qWarning() << "QmlGraphicsTester: Image mismatch.  Reject saved to:" 
                               << reject;
                    img.save(reject);
                    imagefailure();
                }
            }
        } else if (QmlGraphicsVisualTestMouse *mouse = qobject_cast<QmlGraphicsVisualTestMouse *>(event)) {
            QPoint pos(mouse->x(), mouse->y());
            QPoint globalPos = m_view->mapToGlobal(QPoint(0, 0)) + pos;
            QMouseEvent event((QEvent::Type)mouse->type(), pos, globalPos, (Qt::MouseButton)mouse->button(), (Qt::MouseButtons)mouse->buttons(), (Qt::KeyboardModifiers)mouse->modifiers());

            MouseEvent me(&event);
            me.msec = msec;
            if (!mouse->sendToViewport()) {
                QCoreApplication::sendEvent(m_view, &event);
                me.destination = View;
            } else {
                QCoreApplication::sendEvent(m_view->viewport(), &event);
                me.destination = ViewPort;
            }
            m_savedMouseEvents.append(me);
        } else if (QmlGraphicsVisualTestKey *key = qobject_cast<QmlGraphicsVisualTestKey *>(event)) {

            QKeyEvent event((QEvent::Type)key->type(), key->key(), (Qt::KeyboardModifiers)key->modifiers(), QString::fromUtf8(QByteArray::fromHex(key->text().toUtf8())), key->autorep(), key->count());

            KeyEvent ke(&event);
            ke.msec = msec;
            if (!key->sendToViewport()) {
                QCoreApplication::sendEvent(m_view, &event);
                ke.destination = View;
            } else {
                QCoreApplication::sendEvent(m_view->viewport(), &event);
                ke.destination = ViewPort;
            }
            m_savedKeyEvents.append(ke);
        } 
        testscriptidx++;
    }

    filterEvents = true;

    if (testscript && testscript->count() <= testscriptidx)
        complete();
}

void QmlGraphicsTester::registerTypes()
{
    QML_REGISTER_TYPE(Qt.VisualTest, 4,6, VisualTest, QmlGraphicsVisualTest);
    QML_REGISTER_TYPE(Qt.VisualTest, 4,6, Frame, QmlGraphicsVisualTestFrame);
    QML_REGISTER_TYPE(Qt.VisualTest, 4,6, Mouse, QmlGraphicsVisualTestMouse);
    QML_REGISTER_TYPE(Qt.VisualTest, 4,6, Key, QmlGraphicsVisualTestKey);
}

QT_END_NAMESPACE
