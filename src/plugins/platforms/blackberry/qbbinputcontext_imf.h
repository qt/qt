/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QBBINPUTCONTEXT_H
#define QBBINPUTCONTEXT_H

#include "imf/imf_client.h"
#include "imf/input_control.h"

#include <QtGui/QPlatformIntegration>
#include <qinputcontext.h>

QT_BEGIN_NAMESPACE

class QBBAbstractVirtualKeyboard;

class QBBInputContext : public QInputContext
{
public:
    explicit QBBInputContext(QBBAbstractVirtualKeyboard &keyboard, QObject* parent = 0);
    ~QBBInputContext();

    virtual QList<QAction *> actions();
    virtual bool filterEvent(const QEvent * event);
    virtual QFont font() const;
    virtual QString identifierName();
    virtual bool isComposing() const;
    virtual QString language();
    virtual void mouseHandler(int x, QMouseEvent * event);
    virtual void reset();
    virtual void setFocusWidget(QWidget * widget);
    virtual void update();
    virtual void widgetDestroyed(QWidget * widget);
    bool handleKeyboardEvent(int flags, int sym, int mod, int scan, int cap);

protected:
    // Filters only for IMF events.
    bool eventFilter(QObject *obj, QEvent *event);

private:
    // IMF Event dispatchers
    bool dispatchFocusEvent(FocusEventId id, int hints = Qt::ImhNone);
    bool dispatchRequestSoftwareInputPanel();
    bool dispatchCloseSoftwareInputPanel();
    int32_t processEvent(event_t* event);

    void closeSession();
    void openSession();
    bool hasSession();
    void endComposition();
    void setComposingText(QString const& composingText);
    bool hasSelectedText();

    // IMF Event handlers - these events will come in from QCoreApplication.
    int32_t onBeginBatchEdit(input_session_t* ic);
    int32_t onClearMetaKeyStates(input_session_t* ic, int32_t states);
    int32_t onCommitText(input_session_t* ic, spannable_string_t* text, int32_t new_cursor_position);
    int32_t onDeleteSurroundingText(input_session_t* ic, int32_t left_length, int32_t right_length);
    int32_t onEndBatchEdit(input_session_t* ic);
    int32_t onFinishComposingText(input_session_t* ic);
    int32_t onGetCursorCapsMode(input_session_t* ic, int32_t req_modes);
    int32_t onGetCursorPosition(input_session_t* ic);
    extracted_text_t* onGetExtractedText(input_session_t* ic, extracted_text_request_t* request, int32_t flags);
    spannable_string_t* onGetSelectedText(input_session_t* ic, int32_t flags);
    spannable_string_t* onGetTextAfterCursor(input_session_t* ic, int32_t n, int32_t flags);
    spannable_string_t* onGetTextBeforeCursor(input_session_t* ic, int32_t n, int32_t flags);
    int32_t onPerformEditorAction(input_session_t* ic, int32_t editor_action);
    int32_t onReportFullscreenMode(input_session_t* ic, int32_t enabled);
    int32_t onSendEvent(input_session_t* ic, event_t * event);
    int32_t onSendAsyncEvent(input_session_t* ic, event_t * event);
    int32_t onSetComposingRegion(input_session_t* ic, int32_t start, int32_t end);
    int32_t onSetComposingText(input_session_t* ic, spannable_string_t* text, int32_t new_cursor_position);
    int32_t onSetSelection(input_session_t* ic, int32_t start, int32_t end);
    int32_t onForceUpdate();

    int mLastCaretPos;
    bool mIsComposing;
    QString mComposingText;
    QBBAbstractVirtualKeyboard &mVirtualKeyboard;
};

QT_END_NAMESPACE

#endif // QBBINPUTCONTEXT_H
