/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qvarlengtharray.h>
#include <qwidget.h>
#include <private/qmacinputcontext_p.h>
#include "qtextformat.h"
#include <qdebug.h>
#include <private/qapplication_p.h>

QT_BEGIN_NAMESPACE

extern bool qt_sendSpontaneousEvent(QObject*, QEvent*);

#if (MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_5)
#  define typeRefCon typeSInt32
#  define typeByteCount typeSInt32
#endif

static QTextFormat qt_mac_compose_format()
{
    QTextCharFormat ret;
    ret.setFontUnderline(true);
    return ret;
}

QMacInputContext::QMacInputContext(QObject *parent)
    : QInputContext(parent), composing(false), recursionGuard(false), textDocument(0)
{
//    createTextDocument();
}

QMacInputContext::~QMacInputContext()
{
#ifdef Q_WS_MAC32
    if(textDocument)
        DeleteTSMDocument(textDocument);
#endif
}

void
QMacInputContext::createTextDocument()
{
#ifdef Q_WS_MAC32
    if(!textDocument) {
        InterfaceTypeList itl = { kUnicodeDocument };
        NewTSMDocument(1, itl, &textDocument, SRefCon(this));
    }
#endif
}


QString QMacInputContext::language()
{
    return QString();
}


void QMacInputContext::mouseHandler(int pos, QMouseEvent *e)
{
#ifdef Q_WS_MAC32
    if(e->type() != QEvent::MouseButtonPress)
        return;

    if (!composing)
        return;
    if (pos < 0 || pos > currentText.length())
        reset();
    // ##### handle mouse position
#endif
}

#if !defined QT_MAC_USE_COCOA

void QMacInputContext::reset()
{
    if (recursionGuard)
        return;
    if (!currentText.isEmpty()){
        QInputMethodEvent e;
        e.setCommitString(currentText);
        qt_sendSpontaneousEvent(focusWidget(), &e);
        currentText = QString();
    }
    recursionGuard = true;
    createTextDocument();
    composing = false;
    ActivateTSMDocument(textDocument);
    FixTSMDocument(textDocument);
    recursionGuard = false;
}

bool QMacInputContext::isComposing() const
{
    return composing;
}
#endif 

void QMacInputContext::setFocusWidget(QWidget *w)
{
    createTextDocument();
#ifdef Q_WS_MAC32
    if(w)
        ActivateTSMDocument(textDocument);
    else
        DeactivateTSMDocument(textDocument);
#endif
    QInputContext::setFocusWidget(w);
}


static EventTypeSpec input_events[] = {
    { kEventClassTextInput, kEventTextInputUnicodeForKeyEvent },
    { kEventClassTextInput, kEventTextInputOffsetToPos },
    { kEventClassTextInput, kEventTextInputUpdateActiveInputArea }
};
static EventHandlerUPP input_proc_handlerUPP = 0;
static EventHandlerRef input_proc_handler = 0;

void
QMacInputContext::initialize()
{
#ifdef Q_WS_MAC32
    if(!input_proc_handler) {
        input_proc_handlerUPP = NewEventHandlerUPP(QMacInputContext::globalEventProcessor);
        InstallEventHandler(GetApplicationEventTarget(), input_proc_handlerUPP,
                            GetEventTypeCount(input_events), input_events,
                            0, &input_proc_handler);
    }
#endif
}

void
QMacInputContext::cleanup()
{
#ifdef Q_WS_MAC32
    if(input_proc_handler) {
        RemoveEventHandler(input_proc_handler);
        input_proc_handler = 0;
    }
    if(input_proc_handlerUPP) {
        DisposeEventHandlerUPP(input_proc_handlerUPP);
        input_proc_handlerUPP = 0;
    }
#endif
}

OSStatus
QMacInputContext::globalEventProcessor(EventHandlerCallRef, EventRef event, void *)
{
#ifdef Q_WS_MAC32
    QScopedLoopLevelCounter loopLevelCounter(QApplicationPrivate::instance()->threadData);

    SRefCon refcon = 0;
    GetEventParameter(event, kEventParamTextInputSendRefCon, typeRefCon, 0,
                      sizeof(refcon), 0, &refcon);
    QMacInputContext *context = reinterpret_cast<QMacInputContext*>(refcon);

    bool handled_event=true;
    UInt32 ekind = GetEventKind(event), eclass = GetEventClass(event);
    switch(eclass) {
    case kEventClassTextInput: {
        handled_event = false;
        QWidget *widget = QApplicationPrivate::focus_widget;
        if(!widget || (context && widget->inputContext() != context)) {
            handled_event = false;
        } else if(ekind == kEventTextInputOffsetToPos) {
            if(!widget->testAttribute(Qt::WA_InputMethodEnabled)) {
                handled_event = false;
                break;
            }

            QRect mr(widget->inputMethodQuery(Qt::ImMicroFocus).toRect());
            QPoint mp(widget->mapToGlobal(QPoint(mr.topLeft())));
            Point pt;
            pt.h = mp.x();
            pt.v = mp.y() + mr.height();
            SetEventParameter(event, kEventParamTextInputReplyPoint, typeQDPoint,
                              sizeof(pt), &pt);
            handled_event = true;
        } else if(ekind == kEventTextInputUpdateActiveInputArea) {
            if(!widget->testAttribute(Qt::WA_InputMethodEnabled)) {
                handled_event = false;
                break;
            }

            if (context->recursionGuard)
                break;

            ByteCount unilen = 0;
            GetEventParameter(event, kEventParamTextInputSendText, typeUnicodeText,
                              0, 0, &unilen, 0);
            UniChar *unicode = (UniChar*)NewPtr(unilen);
            GetEventParameter(event, kEventParamTextInputSendText, typeUnicodeText,
                              0, unilen, 0, unicode);
            QString text((QChar*)unicode, unilen / sizeof(UniChar));
            DisposePtr((char*)unicode);

            ByteCount fixed_length = 0;
            GetEventParameter(event, kEventParamTextInputSendFixLen, typeByteCount, 0,
                              sizeof(fixed_length), 0, &fixed_length);
            if(fixed_length == ULONG_MAX || fixed_length == unilen) {
                QInputMethodEvent e;
                e.setCommitString(text);
                context->currentText = QString();
                qt_sendSpontaneousEvent(context->focusWidget(), &e);
                handled_event = true;
                context->reset();
            } else {
                ByteCount rngSize = 0;
                OSStatus err = GetEventParameter(event, kEventParamTextInputSendHiliteRng, typeTextRangeArray, 0,
                                                 0, &rngSize, 0);
                QVarLengthArray<TextRangeArray> highlight(rngSize);
                if (noErr == err) {
                    err = GetEventParameter(event, kEventParamTextInputSendHiliteRng, typeTextRangeArray, 0,
                                            rngSize, &rngSize, highlight.data());
                }
                context->composing = true;
                if(fixed_length > 0) {
                    const int qFixedLength = fixed_length / sizeof(UniChar);
                    QList<QInputMethodEvent::Attribute> attrs;
                    attrs << QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat,
                                                          qFixedLength, text.length()-qFixedLength,
                                                          qt_mac_compose_format());
                    QInputMethodEvent e(text, attrs);
                    context->currentText = text;
                    e.setCommitString(text.left(qFixedLength), 0, qFixedLength);
                    qt_sendSpontaneousEvent(widget, &e);
                    handled_event = true;
                } else {
                    /* Apple's enums that they have removed from Tiger :(
                    enum {
                        kCaretPosition = 1,
                        kRawText = 2,
                        kSelectedRawText = 3,
                        kConvertedText = 4,
                        kSelectedConvertedText = 5,
                        kBlockFillText = 6,
                        kOutlineText = 7,
                        kSelectedText = 8
                    };
                    */
#ifndef kConvertedText
#define kConvertedText 4
#endif
#ifndef kCaretPosition
#define kCaretPosition 1
#endif
                    QList<QInputMethodEvent::Attribute> attrs;
                    if (!highlight.isEmpty()) {
                        TextRangeArray *data = highlight.data();
                        for (int i = 0; i < data->fNumOfRanges; ++i) {
                            int start = data->fRange[i].fStart / sizeof(UniChar);
                            int len = (data->fRange[i].fEnd - data->fRange[i].fStart) / sizeof(UniChar);
                            if (data->fRange[i].fHiliteStyle == kCaretPosition) {
                                attrs << QInputMethodEvent::Attribute(QInputMethodEvent::Cursor, start, 0, QVariant());
                                continue;
                            }
                            QTextCharFormat format;
                            format.setFontUnderline(true);
                            if (data->fRange[i].fHiliteStyle == kConvertedText)
                                format.setUnderlineColor(Qt::gray);
                            else
                                format.setUnderlineColor(Qt::black);
                            attrs << QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, start, len, format);
                        }
                    } else {
                        attrs << QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat,
                                0, text.length(), qt_mac_compose_format());
                    }
                    context->currentText = text;
                    QInputMethodEvent e(text, attrs);
                    qt_sendSpontaneousEvent(widget, &e);
                    handled_event = true;
                }
            }
#if 0
            if(!context->composing)
                handled_event = false;
#endif

            extern bool qt_mac_eat_unicode_key; //qapplication_mac.cpp
            qt_mac_eat_unicode_key = handled_event;
        } else if(ekind == kEventTextInputUnicodeForKeyEvent) {
            EventRef key_ev = 0;
            GetEventParameter(event, kEventParamTextInputSendKeyboardEvent, typeEventRef, 0,
                              sizeof(key_ev), 0, &key_ev);
            QString text;
            ByteCount unilen = 0;
            if(GetEventParameter(key_ev, kEventParamKeyUnicodes, typeUnicodeText, 0, 0, &unilen, 0) == noErr) {
                UniChar *unicode = (UniChar*)NewPtr(unilen);
                GetEventParameter(key_ev, kEventParamKeyUnicodes, typeUnicodeText, 0, unilen, 0, unicode);
                text = QString((QChar*)unicode, unilen / sizeof(UniChar));
                DisposePtr((char*)unicode);
            }
            unsigned char chr = 0;
            GetEventParameter(key_ev, kEventParamKeyMacCharCodes, typeChar, 0, sizeof(chr), 0, &chr);
            if(!chr || chr >= 128 || (text.length() > 0 && (text.length() > 1 || text.at(0) != QLatin1Char(chr))))
                handled_event = !widget->testAttribute(Qt::WA_InputMethodEnabled);
        }
        break; }
    default:
        break;
    }
    if(!handled_event) //let the event go through
        return eventNotHandledErr;
#endif
    return noErr; //we eat the event
}

QT_END_NAMESPACE
