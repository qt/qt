/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
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

#include "qapplication.h"

#ifndef QT_NO_DRAGANDDROP

#include "qwidget.h"
#include "qdatetime.h"
#include "qbitmap.h"
#include "qcursor.h"
#include "qevent.h"
#include "qpainter.h"
#include "qdnd_p.h"

#include <COECNTRL.H>
// pointer cursor
#include <w32std.h>
#include <gdi.h>
QT_BEGIN_NAMESPACE
//### artistic impression of Symbians default DnD cursor ?

static QPixmap *defaultPm = 0;
static const int default_pm_hotx = -50;
static const int default_pm_hoty = -50;
static const char *const default_pm[] = {
"13 9 3 1",
".      c None",
"       c #000000",
"X      c #FFFFFF",
"X X X X X X X",
" X X X X X X ",
"X ......... X",
" X.........X ",
"X ......... X",
" X.........X ",
"X ......... X",
" X X X X X X ",
"X X X X X X X",
};
//### actions need to be redefined for S60
// Shift/Ctrl handling, and final drop status
static Qt::DropAction global_accepted_action = Qt::MoveAction;
static Qt::DropActions possible_actions = Qt::IgnoreAction;


// static variables in place of a proper cross-process solution
static QDrag *drag_object;
static bool qt_symbian_dnd_dragging = false;


static Qt::KeyboardModifiers oldstate;

class QShapedPixmapWidget
{
public:
    QShapedPixmapWidget(RWsSession aWsSession,RWindowTreeNode* aNode)
    {
     sprite = RWsSprite(aWsSession);     
     cursorSprite.iBitmap = 0; 
     cursorSprite.iMaskBitmap = 0;
     cursorSprite.iInvertMask = EFalse;
     cursorSprite.iOffset = TPoint(0,0);
     cursorSprite.iInterval = TTimeIntervalMicroSeconds32(0);
     cursorSprite.iDrawMode = CGraphicsContext::EDrawModePEN;
     sprite.Construct(*aNode,TPoint(0,0), ESpriteNoShadows | ESpriteNoChildClip);
     sprite.AppendMember(cursorSprite);
     sprite.Activate();
    }
    ~QShapedPixmapWidget()
    {
        sprite.Close();
        cursorSprite.iBitmap = 0;
        delete cursorBitmap;
        cursorBitmap = 0; //redundant...
    }
    void disableCursor()
    {
        cursorSprite.iBitmap = 0;
        sprite.UpdateMember(0,cursorSprite);
    }
    void enableCursor()
    {
        cursorSprite.iBitmap = cursorBitmap;
        sprite.UpdateMember(0,cursorSprite);
    }
    void setPixmap(QPixmap pm)
    {
        //### heaplock centralized.
        QImage temp = pm.toImage();
        QSize size = pm.size();
        temp.bits();        
        CFbsBitmap *curbm = q_check_ptr(new CFbsBitmap());		// CBase derived object needs check on new
        curbm->Create(TSize(size.width(),size.height()),EColor16MA);
        curbm->LockHeap(ETrue);
        memcpy((uchar*)curbm->DataAddress(),temp.bits(),temp.numBytes());
        curbm->UnlockHeap(ETrue);
        delete cursorSprite.iBitmap;
        cursorSprite.iBitmap = curbm;
        cursorBitmap = curbm;
        sprite.UpdateMember(0,cursorSprite);
    }
    CFbsBitmap *cursorBitmap;
    RWsPointerCursor pointerCursor;
    RWsSprite sprite;
    TSpriteMember cursorSprite;

};


static QShapedPixmapWidget *qt_symbian_dnd_deco = 0;

void QDragManager::updatePixmap()
{
    if (qt_symbian_dnd_deco) {
        QPixmap pm;
        QPoint pm_hot(default_pm_hotx,default_pm_hoty);
        if (drag_object) {
            pm = drag_object->pixmap();
            if (!pm.isNull())
                pm_hot = drag_object->hotSpot();
        }
        if (pm.isNull()) {
            if (!defaultPm)
                defaultPm = new QPixmap(default_pm);
            pm = *defaultPm;
        }
        qt_symbian_dnd_deco->setPixmap(pm);
    }
}

void QDragManager::timerEvent(QTimerEvent *) { }

void QDragManager::move(const QPoint&) {
}

void QDragManager::updateCursor()
{
}


bool QDragManager::eventFilter(QObject *o, QEvent *e)
{
 if (beingCancelled) {
        return false;
    }
    if (!o->isWidgetType())
        return false;

    switch(e->type()) {
        case QEvent::MouseButtonPress:
        {
        }   
        case QEvent::MouseMove:
        {
            if (!object) { //#### this should not happen
                qWarning("QDragManager::eventFilter: No object");
                return true;
            }
            QDragManager *manager = QDragManager::self();
            QMimeData *dropData = manager->object ? manager->dragPrivate()->data : manager->dropData;
            if (manager->object)
                possible_actions =  manager->dragPrivate()->possible_actions;
            else
                possible_actions = Qt::IgnoreAction;

            QMouseEvent *me = (QMouseEvent *)e;

            if (me->buttons()) {
                Qt::DropAction prevAction = global_accepted_action;
                QWidget *cw = QApplication::widgetAt(me->globalPos());
                // map the Coords relative to the window.
                if (!cw)
                    return true;
                TPoint windowPos = cw->effectiveWinId()->PositionRelativeToScreen();
                qt_symbian_dnd_deco->sprite.SetPosition(TPoint(me->globalX()- windowPos.iX,me->globalY()- windowPos.iY));                                

                while (cw && !cw->acceptDrops() && !cw->isWindow())
                    cw = cw->parentWidget();

                if (object->target() != cw) {
                    if (object->target()) {
                        QDragLeaveEvent dle;
                        QApplication::sendEvent(object->target(), &dle);
                        willDrop = false;
                        global_accepted_action = Qt::IgnoreAction;
                        updateCursor();
                        restoreCursor = true;
                        object->d_func()->target = 0;
                    }
                    if (cw && cw->acceptDrops()) {
                        object->d_func()->target = cw;
                        QDragEnterEvent dee(cw->mapFromGlobal(me->globalPos()), possible_actions, dropData,
                                            me->buttons(), me->modifiers());
                        QApplication::sendEvent(object->target(), &dee);
                        willDrop = dee.isAccepted() && dee.dropAction() != Qt::IgnoreAction;
                        global_accepted_action = willDrop ? dee.dropAction() : Qt::IgnoreAction;
                        updateCursor();
                        restoreCursor = true;
                    }
                } else if (cw) {
                    QDragMoveEvent dme(cw->mapFromGlobal(me->globalPos()), possible_actions, dropData,
                                       me->buttons(), me->modifiers());
                    if (global_accepted_action != Qt::IgnoreAction) {
                        dme.setDropAction(global_accepted_action);
                        dme.accept();
                    }
                    QApplication::sendEvent(cw, &dme);
                    willDrop = dme.isAccepted();
                    global_accepted_action = willDrop ? dme.dropAction() : Qt::IgnoreAction;
                    updatePixmap();
                    updateCursor();
                }
                if (global_accepted_action != prevAction)
                    emitActionChanged(global_accepted_action);
            }
            return true; // Eat all mouse events
        }

        case QEvent::MouseButtonRelease:
        {
            qApp->removeEventFilter(this);
            if (restoreCursor) {
                qt_symbian_dnd_deco->disableCursor();
                willDrop = false;
                restoreCursor = false;
            }
            if (object && object->target()) {

                QMouseEvent *me = (QMouseEvent *)e;

                QDragManager *manager = QDragManager::self();
                QMimeData *dropData = manager->object ? manager->dragPrivate()->data : manager->dropData;

                QDropEvent de(object->target()->mapFromGlobal(me->globalPos()), possible_actions, dropData,
                              me->buttons(), me->modifiers());
                QApplication::sendEvent(object->target(), &de);
                if (de.isAccepted())
                    global_accepted_action = de.dropAction();
                else
                    global_accepted_action = Qt::IgnoreAction;

                if (object)
                    object->deleteLater();
                drag_object = object = 0;
            }
            eventLoop->exit();
            return true; // Eat all mouse events
        }

        default:
             break;
    }
    return false;
}

Qt::DropAction QDragManager::drag(QDrag *o)
{
    Q_ASSERT(!qt_symbian_dnd_dragging);
    if (object == o || !o || !o->source())
         return Qt::IgnoreAction;

    if (object) {
        cancel();
        qApp->removeEventFilter(this);
        beingCancelled = false;
    }

    object = drag_object = o;
    RWsSession winSession = o->source()->effectiveWinId()->ControlEnv()->WsSession();
    Q_ASSERT(!qt_symbian_dnd_deco);
    qt_symbian_dnd_deco = new QShapedPixmapWidget(winSession, o->source()->effectiveWinId()->DrawableWindow());

    oldstate = Qt::NoModifier; // #### Should use state that caused the drag
    willDrop = false;
    updatePixmap();
    updateCursor();
    restoreCursor = true;

    object->d_func()->target = 0;
    TPoint windowPos = source()->effectiveWinId()->PositionRelativeToScreen();
    qt_symbian_dnd_deco->sprite.SetPosition(TPoint(QCursor::pos().x()- windowPos.iX ,QCursor::pos().y() - windowPos.iY));
    
    QPoint hotspot = drag_object->hotSpot();
    qt_symbian_dnd_deco->cursorSprite.iOffset = TPoint(- hotspot.x(),- hotspot.y());
    qt_symbian_dnd_deco->sprite.UpdateMember(0,qt_symbian_dnd_deco->cursorSprite);

    qApp->installEventFilter(this);

    global_accepted_action = Qt::MoveAction;
    qt_symbian_dnd_dragging = true;

    eventLoop = new QEventLoop;
    // block
    (void) eventLoop->exec(QEventLoop::AllEvents);
    delete eventLoop;
    eventLoop = 0;

    delete qt_symbian_dnd_deco;
    qt_symbian_dnd_deco = 0;
    qt_symbian_dnd_dragging = false;


    return global_accepted_action;
}


void QDragManager::cancel(bool deleteSource)
{
    beingCancelled = true;

    if (object->target()) {
        QDragLeaveEvent dle;
        QApplication::sendEvent(object->target(), &dle);
    }

    if (drag_object) {
        if (deleteSource)
            object->deleteLater();
        drag_object = object = 0;
    }

    delete qt_symbian_dnd_deco;
    qt_symbian_dnd_deco = 0;

    global_accepted_action = Qt::IgnoreAction;
}


void QDragManager::drop()
{
}

QVariant QDropData::retrieveData_sys(const QString &mimetype, QVariant::Type type) const
{
    if (!drag_object)
        return QVariant();
    QByteArray data =  drag_object->mimeData()->data(mimetype);
    if (type == QVariant::String)
        return QString::fromUtf8(data);
    return data;
}

bool QDropData::hasFormat_sys(const QString &format) const
{
    return formats().contains(format);
}

QStringList QDropData::formats_sys() const
{
    if (drag_object)
        return drag_object->mimeData()->formats();
    return QStringList();
}

QT_END_NAMESPACE
#endif // QT_NO_DRAGANDDROP
