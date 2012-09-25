/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
/* TODO
 *   Support inputMethodHints to restrict input (needs additional features in IMF).
*/

// #define QBBINPUTCONTEXT_DEBUG
// #define QBBINPUTCONTEXT_IMF_EVENT_DEBUG

#define STRX(x) #x
#define STR(x) STRX(x)
#define TAG __FILE__ "(" STR(__LINE__) ")" << __func__ << ":"

#include <qbbeventthread.h>
#include <qbbinputcontext.h>
#include <qbbabstractvirtualkeyboard.h>

#include <QAction>
#include <QCoreApplication>
#include <QDebug>
#include <QInputMethodEvent>
#include <QMutex>
#include <QTextCharFormat>
#include <QVariant>
#include <QVariantHash>
#include <QWaitCondition>

#include <dlfcn.h>
#include "imf/imf_client.h"
#include "imf/input_control.h"
#include <process.h>
#include <sys/keycodes.h>

// Someone tell me why input_control methods are in this namespace, but the rest is not.
using namespace InputMethodSystem;

#define qs(x) QString::fromLatin1(x)
#define iarg(name) event->mArgs[qs(#name)] = QVariant::fromValue(name)
#define parg(name) event->mArgs[qs(#name)] = QVariant::fromValue((void*)name)
namespace
{

spannable_string_t* toSpannableString(QString const& text);
static const input_session_t *sInputSession = 0;
bool isSessionOkay(input_session_t *ic)
{
    return ic !=0 && sInputSession != 0 && ic->component_id == sInputSession->component_id;
}

enum ImfEventType
{
    ImfBeginBatchEdit,
    ImfClearMetaKeyStates,
    ImfCommitText,
    ImfDeleteSurroundingText,
    ImfEndBatchEdit,
    ImfFinishComposingText,
    ImfGetCursorCapsMode,
    ImfGetCursorPosition,
    ImfGetExtractedText,
    ImfGetSelectedText,
    ImfGetTextAfterCursor,
    ImfGetTextBeforeCursor,
    ImfPerformEditorAction,
    ImfReportFullscreenMode,
    ImfSendEvent,
    ImfSendAsyncEvent,
    ImfSetComposingRegion,
    ImfSetComposingText,
    ImfSetSelection
};

// We use this class as a round about way to support a posting synchronous event into
// Qt's main thread from the IMF thread.
class ImfEventResult
{
public:
    ImfEventResult()
    {
        mMutex.lock();
    }

    ~ImfEventResult()
    {
        mMutex.unlock();
    }

    void wait()
    {
        mWait.wait(&mMutex);
    }

    void signal()
    {
        mWait.wakeAll();
    }

    void setResult(QVariant const& result)
    {
        mMutex.lock();
        mRetVal = result;
        signal();
        mMutex.unlock();
    }

    QVariant& getResult()
    {
        return mRetVal;
    }

private:
    QVariant mRetVal;
    QMutex mMutex;
    QWaitCondition mWait;
};

class ImfEvent : public QEvent
{
    public:
        ImfEvent(input_session_t* session, ImfEventType type, ImfEventResult* result) :
            QEvent((QEvent::Type)sUserEventType),
            mSession(session),
            mImfType(type),
            mResult(result)
        {
        }
        ~ImfEvent() { }

    input_session_t* mSession;
    ImfEventType mImfType;
    QVariantHash mArgs;
    ImfEventResult *mResult;

    static int sUserEventType;
};
int ImfEvent::sUserEventType = QEvent::registerEventType();

static int32_t imfBeginBatchEdit(input_session_t* ic)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfBeginBatchEdit, &result);
    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    result.wait();
    int32_t ret = result.getResult().toInt();

    return ret;
}

static int32_t imfClearMetaKeyStates(input_session_t* ic, int32_t states)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfClearMetaKeyStates, &result);
    iarg(states);

    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    result.wait();
    int32_t ret = result.getResult().toInt();

    return ret;
}

static int32_t imfCommitText(input_session_t* ic, spannable_string_t* text, int32_t new_cursor_position)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfCommitText, &result);
    parg(text);
    iarg(new_cursor_position);

    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    result.wait();
    int32_t ret = result.getResult().toInt();

    return ret;
}

static int32_t imfDeleteSurroundingText(input_session_t* ic, int32_t left_length, int32_t right_length)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfDeleteSurroundingText, &result);
    iarg(left_length);
    iarg(right_length);

    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    result.wait();
    int32_t ret = result.getResult().toInt();

    return ret;
}

static int32_t imfEndBatchEdit(input_session_t* ic)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfEndBatchEdit, &result);

    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    result.wait();
    int32_t ret = result.getResult().toInt();

    return ret;
}

static int32_t imfFinishComposingText(input_session_t* ic)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfFinishComposingText, &result);

    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    result.wait();
    int32_t ret = result.getResult().toInt();

    return ret;
}

static int32_t imfGetCursorCapsMode(input_session_t* ic, int32_t req_modes)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfGetCursorCapsMode, &result);
    iarg(req_modes);

    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    int32_t ret = result.getResult().value<int>();
    return ret;
}

static int32_t imfGetCursorPosition(input_session_t* ic)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfGetCursorPosition, &result);

    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    result.wait();
    int32_t ret = result.getResult().toInt();

    return ret;
}

static extracted_text_t* imfGetExtractedText(input_session_t* ic, extracted_text_request_t* request, int32_t flags)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic)) {
        extracted_text_t *et = (extracted_text_t *)calloc(sizeof(extracted_text_t),1);
        et->text = (spannable_string_t *)calloc(sizeof(spannable_string_t),1);
        return et;
    }

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfGetExtractedText, &result);
    parg(request);
    iarg(flags);

    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    result.wait();
    void* ret = result.getResult().value<void*>();
    return (extracted_text_t*)ret;
}

static spannable_string_t* imfGetSelectedText(input_session_t* ic, int32_t flags)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return toSpannableString("");

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfGetSelectedText, &result);
    iarg(flags);

    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    result.wait();
    void* ret = result.getResult().value<void*>();
    return (spannable_string_t*)ret;
}

static spannable_string_t* imfGetTextAfterCursor(input_session_t* ic, int32_t n, int32_t flags)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return toSpannableString("");

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfGetTextAfterCursor, &result);
    iarg(n);
    iarg(flags);

    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    result.wait();
    void* ret = result.getResult().value<void*>();
    return (spannable_string_t*)ret;
}

static spannable_string_t* imfGetTextBeforeCursor(input_session_t* ic, int32_t n, int32_t flags)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return toSpannableString("");

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfGetTextBeforeCursor, &result);
    iarg(n);
    iarg(flags);

    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    result.wait();
    void* ret = result.getResult().value<void*>();
    return (spannable_string_t*)ret;
}

static int32_t imfPerformEditorAction(input_session_t* ic, int32_t editor_action)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfPerformEditorAction, &result);
    iarg(editor_action);

    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    result.wait();
    int32_t ret = result.getResult().value<int>();
    return ret;
}

static int32_t imfReportFullscreenMode(input_session_t* ic, int32_t enabled)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfReportFullscreenMode, &result);
    iarg(enabled);

    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    result.wait();
    int32_t ret = result.getResult().value<int>();
    return ret;
}

static int32_t imfSendEvent(input_session_t* ic, event_t * event)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    ImfEvent* imfEvent = new ImfEvent(ic, ImfSendEvent, 0);
    imfEvent->mArgs[qs("event")] = QVariant::fromValue((void*)event);

    QCoreApplication::postEvent(QCoreApplication::instance(), imfEvent);

    return 0;
}

static int32_t imfSendAsyncEvent(input_session_t* ic, event_t * event)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    ImfEvent* imfEvent = new ImfEvent(ic, ImfSendAsyncEvent, 0);
    imfEvent->mArgs[qs("event")] = QVariant::fromValue((void*)event);

    QCoreApplication::postEvent(QCoreApplication::instance(), imfEvent);

    return 0;
}

static int32_t imfSetComposingRegion(input_session_t* ic, int32_t start, int32_t end)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfSetComposingRegion, &result);
    iarg(start);
    iarg(end);

    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    result.wait();
    int32_t ret = result.getResult().value<int>();
    return ret;
}

static int32_t imfSetComposingText(input_session_t* ic, spannable_string_t* text, int32_t new_cursor_position)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfSetComposingText, &result);
    parg(text);
    iarg(new_cursor_position);

    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    result.wait();
    int32_t ret = result.getResult().value<int>();
    return ret;
}

static int32_t imfSetSelection(input_session_t* ic, int32_t start, int32_t end)
{
#if defined(QBBINPUTCONTEXT_IMF_EVENT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    ImfEventResult result;
    ImfEvent* event = new ImfEvent(ic, ImfSetSelection, &result);
    iarg(start);
    iarg(end);

    QCoreApplication::postEvent(QCoreApplication::instance(), event);

    result.wait();
    int32_t ret = result.getResult().value<int>();
    return ret;
}

static connection_interface_t ic_funcs = {
    imfBeginBatchEdit,
    imfClearMetaKeyStates,
    imfCommitText,
    imfDeleteSurroundingText,
    imfEndBatchEdit,
    imfFinishComposingText,
    imfGetCursorCapsMode,
    imfGetCursorPosition,
    imfGetExtractedText,
    imfGetSelectedText,
    imfGetTextAfterCursor,
    imfGetTextBeforeCursor,
    imfPerformEditorAction,
    imfReportFullscreenMode,
    NULL, //ic_send_key_event
    imfSendEvent,
    imfSendAsyncEvent,
    imfSetComposingRegion,
    imfSetComposingText,
    imfSetSelection,
    NULL, //ic_set_candidates,
};

static void
initEvent(event_t *pEvent, const input_session_t *pSession, EventType eventType, int eventId)
{
    static int s_transactionId;

    // Make sure structure is squeaky clean since it's not clear just what is significant.
    memset(pEvent, 0, sizeof(event_t));
    pEvent->event_type = eventType;
    pEvent->event_id = eventId;
    pEvent->pid = getpid();
    pEvent->component_id = pSession->component_id;
    pEvent->transaction_id = ++s_transactionId;
}

spannable_string_t* toSpannableString(QString const& text)
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG << text;
#endif

    spannable_string_t *pString = (spannable_string_t *)malloc(sizeof(spannable_string_t));
    pString->str = (wchar_t *)malloc(sizeof(wchar_t) * text.length()+1);
    pString->length = text.length();
    pString->spans = NULL;
    pString->spans_count = 0;

    QChar const* pData = text.constData();
    wchar_t* pDst = pString->str;

    while (!pData->isNull())
    {
        *pDst = pData->unicode();
        pDst++;
        pData++;
    }
    *pDst = 0;

    return pString;
}

} // namespace

static const input_session_t *(*p_ictrl_open_session)(connection_interface_t *) = 0;
static void (*p_ictrl_close_session)(input_session_t *) = 0;
static int32_t (*p_ictrl_dispatch_event)(event_t*) = 0;
static int32_t (*p_imf_client_init)() = 0;
static void (*p_imf_client_disconnect)() = 0;
static int32_t (*p_vkb_init_selection_service)() = 0;
static int32_t (*p_ictrl_get_num_active_sessions)() = 0;
static bool s_imfInitFailed = false;

static bool imfAvailable()
{
    static bool s_imfDisabled = getenv("DISABLE_IMF") != NULL;
    static bool s_imfReady = false;

    if ( s_imfInitFailed || s_imfDisabled) {
        return false;
    }
    else if ( s_imfReady ) {
        return true;
    }

    if ( p_imf_client_init == NULL ) {
        void *handle = dlopen("libinput_client.so.1", 0);
        if ( handle ) {
            p_imf_client_init = (int32_t (*)()) dlsym(handle, "imf_client_init");
            p_imf_client_disconnect = (void (*)()) dlsym(handle, "imf_client_disconnect");
            p_ictrl_open_session = (const input_session_t* (*)(connection_interface_t*))dlsym(handle, "ictrl_open_session");
            p_ictrl_close_session = (void (*)(input_session_t*))dlsym(handle, "ictrl_close_session");
            p_ictrl_dispatch_event = (int32_t (*)(event_t*))dlsym(handle, "ictrl_dispatch_event");
            p_vkb_init_selection_service = (int32_t (*)())dlsym(handle, "vkb_init_selection_service");
            p_ictrl_get_num_active_sessions = (int32_t (*)())dlsym(handle, "ictrl_get_num_active_sessions");
        }
        else
        {
            qCritical() << TAG << "libinput_client.so.1 is not present - IMF services are disabled.";
            s_imfDisabled = true;
            return false;
        }
        if ( p_imf_client_init && p_ictrl_open_session && p_ictrl_dispatch_event ) {
            s_imfReady = true;
        }
        else {
            p_ictrl_open_session = NULL;
            p_ictrl_dispatch_event = NULL;
            s_imfDisabled = true;
            qCritical() << TAG << "libinput_client.so.1 did not contain the correct symbols, library mismatch? IMF services are disabled.";
            return false;
        }
    }

    return s_imfReady;
}

QT_BEGIN_NAMESPACE

QBBInputContext::QBBInputContext(QBBAbstractVirtualKeyboard &keyboard, QObject* parent):
         QInputContext(parent),
         mLastCaretPos(0),
         mIsComposing(false),
         mVirtualKeyboard(keyboard)
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!imfAvailable())
        return;

   if ( p_imf_client_init() != 0 ) {
        s_imfInitFailed = true;
        qCritical("imf_client_init failed - IMF services will be unavailable");
    }

    QCoreApplication::instance()->installEventFilter(this);

    // p_vkb_init_selection_service();
}

QBBInputContext::~QBBInputContext()
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!imfAvailable())
        return;

    QCoreApplication::instance()->removeEventFilter(this);
    p_imf_client_disconnect();
}

#define getarg(type, name) type name = imfEvent->mArgs[qs(#name)].value<type>()
#define getparg(type, name) type name = (type)(imfEvent->mArgs[qs(#name)].value<void*>())

bool QBBInputContext::eventFilter(QObject *obj, QEvent *event)
{

    if (event->type() == ImfEvent::sUserEventType) {
        // Forward the event to our real handler.
        ImfEvent* imfEvent = static_cast<ImfEvent*>(event);
        switch (imfEvent->mImfType) {
        case ImfBeginBatchEdit:
        {
            int32_t ret = onBeginBatchEdit(imfEvent->mSession);
            imfEvent->mResult->setResult(QVariant::fromValue(ret));
            break;
        }

        case ImfClearMetaKeyStates:
        {
            getarg(int32_t, states);
            int32_t ret = onClearMetaKeyStates(imfEvent->mSession, states);
            imfEvent->mResult->setResult(QVariant::fromValue(ret));
            break;
        }

        case ImfCommitText:
        {
            getparg(spannable_string_t*, text);
            getarg(int32_t, new_cursor_position);
            int32_t ret = onCommitText(imfEvent->mSession, text, new_cursor_position);
            imfEvent->mResult->setResult(QVariant::fromValue(ret));
            break;
        }

        case ImfDeleteSurroundingText:
        {
            getarg(int32_t, left_length);
            getarg(int32_t, right_length);
            int32_t ret = onDeleteSurroundingText(imfEvent->mSession, left_length, right_length);
            imfEvent->mResult->setResult(QVariant::fromValue(ret));
            break;
        }

        case ImfEndBatchEdit:
        {
            int32_t ret = onEndBatchEdit(imfEvent->mSession);
            imfEvent->mResult->setResult(QVariant::fromValue(ret));
            break;
        }

        case ImfFinishComposingText:
        {
            int32_t ret = onFinishComposingText(imfEvent->mSession);
            imfEvent->mResult->setResult(QVariant::fromValue(ret));
            break;
        }

        case ImfGetCursorCapsMode:
        {
            getarg(int32_t, req_modes);
            int32_t ret = onGetCursorCapsMode(imfEvent->mSession, req_modes);
            imfEvent->mResult->setResult(QVariant::fromValue(ret));
            break;
        }

        case ImfGetCursorPosition:
        {
            int32_t ret = onGetCursorPosition(imfEvent->mSession);
            imfEvent->mResult->setResult(QVariant::fromValue(ret));
            break;
        }

        case ImfGetExtractedText:
        {
            getparg(extracted_text_request_t*, request);
            getarg(int32_t, flags);
            extracted_text_t* ret = onGetExtractedText(imfEvent->mSession, request, flags);
            imfEvent->mResult->setResult(QVariant::fromValue((void*)ret));
            break;
        }

        case ImfGetSelectedText:
        {
            getarg(int32_t, flags);
            spannable_string_t* ret = onGetSelectedText(imfEvent->mSession, flags);
            imfEvent->mResult->setResult(QVariant::fromValue((void*)ret));
            break;
        }

        case ImfGetTextAfterCursor:
        {
            getarg(int32_t, n);
            getarg(int32_t, flags);
            spannable_string_t* ret = onGetTextAfterCursor(imfEvent->mSession, n, flags);
            imfEvent->mResult->setResult(QVariant::fromValue((void*)ret));
            break;
        }

        case ImfGetTextBeforeCursor:
        {
            getarg(int32_t, n);
            getarg(int32_t, flags);
            spannable_string_t* ret = onGetTextBeforeCursor(imfEvent->mSession, n, flags);
            imfEvent->mResult->setResult(QVariant::fromValue((void*)ret));
            break;
        }

        case ImfPerformEditorAction:
        {
            getarg(int32_t, editor_action);
            int32_t ret = onPerformEditorAction(imfEvent->mSession, editor_action);
            imfEvent->mResult->setResult(QVariant::fromValue(ret));
            break;
        }

        case ImfReportFullscreenMode:
        {
            getarg(int32_t, enabled);
            int32_t ret = onReportFullscreenMode(imfEvent->mSession, enabled);
            imfEvent->mResult->setResult(QVariant::fromValue(ret));
            break;
        }

        case ImfSendEvent:
        {
            getparg(event_t*, event);
            onSendEvent(imfEvent->mSession, event);
            break;
        }

        case ImfSendAsyncEvent:
        {
            getparg(event_t*, event);
            onSendAsyncEvent(imfEvent->mSession, event);
            break;
        }

        case ImfSetComposingRegion:
        {
            getarg(int32_t, start);
            getarg(int32_t, end);
            int32_t ret = onSetComposingRegion(imfEvent->mSession, start, end);
            imfEvent->mResult->setResult(QVariant::fromValue(ret));
            break;
        }

        case ImfSetComposingText:
        {
            getparg(spannable_string_t*, text);
            getarg(int32_t, new_cursor_position);
            int32_t ret = onSetComposingText(imfEvent->mSession, text, new_cursor_position);
            imfEvent->mResult->setResult(QVariant::fromValue(ret));
            break;
        }

        case ImfSetSelection:
        {
            getarg(int32_t, start);
            getarg(int32_t, end);
            int32_t ret = onSetSelection(imfEvent->mSession, start, end);
            imfEvent->mResult->setResult(QVariant::fromValue(ret));
            break;
        }
        }; //switch

        return true;
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

QString QBBInputContext::identifierName()
{
    return tr("PlayBook IMF");
}

QString QBBInputContext::language()
{
    return mVirtualKeyboard.languageId();
}

bool QBBInputContext::filterEvent( const QEvent *event )
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG << event;
#endif

    switch (event->type()) {
    case QEvent::CloseSoftwareInputPanel: {
        return dispatchCloseSoftwareInputPanel();
    }
    case QEvent::RequestSoftwareInputPanel: {
        return dispatchRequestSoftwareInputPanel();
    }
    default:
        return false;
    }
}

QList<QAction *> QBBInputContext::actions()
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif
    return QInputContext::actions();
}

QFont QBBInputContext::font() const
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif
    return QInputContext::font();
}

bool QBBInputContext::isComposing() const
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif
    return mIsComposing;
}

void QBBInputContext::mouseHandler(int x, QMouseEvent * event)
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif
    return QInputContext::mouseHandler(x, event);
}

void QBBInputContext::reset()
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    endComposition();
}

void QBBInputContext::setFocusWidget(QWidget * widget)
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (hasSession()) {
        endComposition();
        dispatchFocusEvent(FOCUS_LOST);
        closeSession();
    }

    // Update the widget before moving on.
    QInputContext::setFocusWidget(widget);

    // If we have hidden text, or any flags that restrict input (exclusive flags), then we just disable
    // imf for this field.
    if (widget != 0 && !(widget->inputMethodHints() & Qt::ImhHiddenText) && !(widget->inputMethodHints()  >= Qt::ImhDigitsOnly && widget->inputMethodHints() <= Qt::ImhUrlCharactersOnly))
    {
#if defined(QBBINPUTCONTEXT_DEBUG)
        qDebug() << TAG << "Starting input session for " << widget;
#endif
        openSession();
        dispatchFocusEvent(FOCUS_GAINED, widget->inputMethodHints());
    }
}

void QBBInputContext::update()
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    reset();

    QInputContext::update();
}

void QBBInputContext::widgetDestroyed(QWidget * widget)
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif
    QInputContext::widgetDestroyed(widget);
}

void QBBInputContext::closeSession()
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;;
#endif

    if (!imfAvailable())
        return;

    if (sInputSession) {
        p_ictrl_close_session((input_session_t *)sInputSession);
        sInputSession = 0;
    }
}

void QBBInputContext::openSession()
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;;
#endif

    if (!imfAvailable())
        return;

    closeSession();
    sInputSession = p_ictrl_open_session(&ic_funcs);
}

bool QBBInputContext::hasSession()
{
    return sInputSession != 0;
}

bool QBBInputContext::hasSelectedText()
{
    if (focusWidget()) {
        return focusWidget()->inputMethodQuery(Qt::ImCursorPosition).toInt() != focusWidget()->inputMethodQuery(Qt::ImAnchorPosition).toInt();
    }
    else
    {
        return false;
    }
}

bool QBBInputContext::dispatchRequestSoftwareInputPanel()
{
    mVirtualKeyboard.showKeyboard();
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << "QBB: requesting virtual keyboard";
#endif

    if (!imfAvailable() || !focusWidget())
        return true;

    // This also means that the caret position has moved
    int caretPos = focusWidget()->inputMethodQuery(Qt::ImCursorPosition).toInt();
    caret_event_t caretEvent;
    memset(&caretEvent, 0, sizeof(caret_event_t));
    initEvent(&caretEvent.event, sInputSession, EVENT_CARET, CARET_POS_CHANGED);
    caretEvent.old_pos = mLastCaretPos;
    mLastCaretPos = caretEvent.new_pos = caretPos;
    p_ictrl_dispatch_event((event_t *)&caretEvent);
    return true;
}

bool QBBInputContext::dispatchCloseSoftwareInputPanel()
{
    mVirtualKeyboard.hideKeyboard();
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << "QBB: hiding virtual keyboard";
#endif

    // This also means we are stopping composition, but we should already have done that.
    return true;
}

/*
 * IMF Event Dispatchers.
 */
bool QBBInputContext::dispatchFocusEvent(FocusEventId id, int hints)
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!sInputSession) {
        qWarning() << TAG << "Attempt to dispatch a focus event with no input session.";
        return false;
    }

    if (!imfAvailable())
        return false;

    // Set the last caret position to 0 since we don't really have one and we don't
    // want to have the old one.
    mLastCaretPos = 0;

    focus_event_t focusEvent;
    memset(&focusEvent, 0, sizeof(focusEvent));
    initEvent(&focusEvent.event, sInputSession, EVENT_FOCUS, id);
    focusEvent.style = DEFAULT_STYLE;

    if (hints && Qt::ImhNoPredictiveText)
        focusEvent.style |= NO_PREDICTION | NO_AUTO_CORRECTION;
    if (hints && Qt::ImhNoAutoUppercase)
        focusEvent.style |= NO_AUTO_TEXT;

    p_ictrl_dispatch_event((event_t *)&focusEvent);

    return true;
}

bool QBBInputContext::handleKeyboardEvent(int flags, int sym, int mod, int scan, int cap)
{
    if (!imfAvailable())
        return false;

    int key = (flags & KEY_SYM_VALID) ? sym : cap;
    bool navKey = false;
    switch ( key ) {
    case KEYCODE_RETURN:
         /* In a single line edit we should end composition because enter might be used by something.
            endComposition();
            return false;*/
        break;

    case KEYCODE_BACKSPACE:
    case KEYCODE_DELETE:
        // If there is a selection range, then we want a delete key to operate on that (by
        // deleting the contents of the select range) rather than operating on the composition
        // range.
        if (hasSelectedText())
            return false;
        break;
    case  KEYCODE_LEFT:
        key = NAVIGATE_LEFT;
        navKey = true;
        break;
    case  KEYCODE_RIGHT:
        key = NAVIGATE_RIGHT;
        navKey = true;
        break;
    case  KEYCODE_UP:
        key = NAVIGATE_UP;
        navKey = true;
        break;
    case  KEYCODE_DOWN:
        key = NAVIGATE_DOWN;
        navKey = true;
        break;
    case  KEYCODE_CAPS_LOCK:
    case  KEYCODE_LEFT_SHIFT:
    case  KEYCODE_RIGHT_SHIFT:
    case  KEYCODE_LEFT_CTRL:
    case  KEYCODE_RIGHT_CTRL:
    case  KEYCODE_LEFT_ALT:
    case  KEYCODE_RIGHT_ALT:
    case  KEYCODE_MENU:
    case  KEYCODE_LEFT_HYPER:
    case  KEYCODE_RIGHT_HYPER:
    case  KEYCODE_INSERT:
    case  KEYCODE_HOME:
    case  KEYCODE_PG_UP:
    case  KEYCODE_END:
    case  KEYCODE_PG_DOWN:
        // Don't send these
        key = 0;
        break;
    }

    if ( mod & KEYMOD_CTRL ) {
        // If CTRL is pressed, just let AIR handle it.  But terminate any composition first
        //endComposition();
        return false;
    }

    // Pass the keys we don't know about on through
    if ( key == 0 )
        return false;

    // IMF doesn't need key releases so just swallow them.
    if (!(flags & KEY_DOWN))
        return true;

    if ( navKey ) {
        // Even if we're forwarding up events, we can't do this for
        // navigation keys.
        if ( flags & KEY_DOWN ) {
            navigation_event_t navEvent;
            initEvent(&navEvent.event, sInputSession, EVENT_NAVIGATION, key);
            navEvent.magnitude = 1;
#if defined(QBBINPUTCONTEXT_DEBUG)
            qDebug() << TAG << "dispatch navigation event " << key;
#endif
            p_ictrl_dispatch_event(&navEvent.event);
        }
    }
    else {
        key_event_t keyEvent;
        initEvent(&keyEvent.event, sInputSession, EVENT_KEY, flags & KEY_DOWN ? IMF_KEY_DOWN : IMF_KEY_UP);
        keyEvent.key_code = key;
        keyEvent.character = 0;
        keyEvent.meta_key_state = 0;

        p_ictrl_dispatch_event(&keyEvent.event);
#if defined(QBBINPUTCONTEXT_DEBUG)
        qDebug() << TAG << "dispatch key event " << key;
#endif
    }

    scan = 0;
    return true;
}

void QBBInputContext::endComposition()
{
    if (!imfAvailable())
        return;

    if (!isComposing())
        return;

    QList<QInputMethodEvent::Attribute> attributes;
    QInputMethodEvent event(QLatin1String(""), attributes);
    event.setCommitString(mComposingText);
    mComposingText = QString();
    mIsComposing = false;
    sendEvent(event);

    action_event_t actionEvent;
    memset(&actionEvent, 0, sizeof(actionEvent));
    initEvent(&actionEvent.event, sInputSession, EVENT_ACTION, ACTION_END_COMPOSITION);
    p_ictrl_dispatch_event(&actionEvent.event);
}

void QBBInputContext::setComposingText(QString const& composingText)
{
    mComposingText = composingText;
    mIsComposing = true;

    QList<QInputMethodEvent::Attribute> attributes;
    QTextCharFormat format;
    format.setFontUnderline(true);
    attributes.push_back(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, 0, composingText.length(), format));

    QInputMethodEvent event(composingText, attributes);

    sendEvent(event);
}

int32_t QBBInputContext::processEvent(event_t* event)
{
    int32_t result = -1;
    switch (event->event_type) {
    case EVENT_SPELL_CHECK: {
        #if defined(QBBINPUTCONTEXT_DEBUG)
        qDebug() << TAG << "EVENT_SPELL_CHECK";
        #endif
        result = 0;
        break;
    }

    case EVENT_NAVIGATION: {
        #if defined(QBBINPUTCONTEXT_DEBUG)
        qDebug() << TAG << "EVENT_NAVIGATION";
        #endif

        int key = event->event_id == NAVIGATE_UP ? KEYCODE_UP :
            event->event_id == NAVIGATE_DOWN ? KEYCODE_DOWN :
            event->event_id == NAVIGATE_LEFT ? KEYCODE_LEFT :
            event->event_id == NAVIGATE_RIGHT ? KEYCODE_RIGHT : 0;

        QBBEventThread::injectKeyboardEvent(KEY_DOWN | KEY_CAP_VALID, key, 0, 0, 0);
        QBBEventThread::injectKeyboardEvent(KEY_CAP_VALID, key, 0, 0, 0);
        result = 0;
        break;
    }

    case EVENT_KEY: {
        #if defined(QBBINPUTCONTEXT_DEBUG)
        qDebug() << TAG << "EVENT_KEY";
        #endif
        key_event_t* kevent = (key_event_t*) event;

        QBBEventThread::injectKeyboardEvent(KEY_DOWN | KEY_SYM_VALID | KEY_CAP_VALID, kevent->key_code, 0, 0, kevent->key_code);
        QBBEventThread::injectKeyboardEvent(KEY_SYM_VALID | KEY_CAP_VALID, kevent->key_code, 0, 0, kevent->key_code);

        result = 0;
        break;
    }

    case EVENT_ACTION:
            // Don't care, indicates that IMF is done.
        break;

    case EVENT_CARET:
    case EVENT_NOTHING:
    case EVENT_FOCUS:
    case EVENT_USER_ACTION:
    case EVENT_STROKE:
    case EVENT_INVOKE_LATER:
        qCritical() << TAG << "Unsupported event type: " << event->event_type;
        break;
    default:
        qCritical() << TAG << "Unknown event type: " << event->event_type;
    }
    return result;
}

/*
 * IMF Event Handlers
 */

int32_t QBBInputContext::onBeginBatchEdit(input_session_t* ic)
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    // We don't care.
    return 0;
}

int32_t QBBInputContext::onClearMetaKeyStates(input_session_t* ic, int32_t states)
{
    Q_UNUSED(states);
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    // Should never get called.
    qCritical() << TAG << "onClearMetaKeyStates is unsupported.";
    return 0;
}

int32_t QBBInputContext::onCommitText(input_session_t* ic, spannable_string_t* text, int32_t new_cursor_position)
{
    Q_UNUSED(new_cursor_position);  // TODO: How can we set the cursor position it's not part of the API.
    if (!isSessionOkay(ic))
        return 0;

    QString commitString = QString::fromWCharArray(text->str, text->length);

#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG << "Committing [" << commitString << "]";
#endif

    QList<QInputMethodEvent::Attribute> attributes;
    QInputMethodEvent event(QLatin1String(""), attributes);
    event.setCommitString(commitString, 0, 0);

    sendEvent(event);
    mComposingText = QString();

    return 0;
}

int32_t QBBInputContext::onDeleteSurroundingText(input_session_t* ic, int32_t left_length, int32_t right_length)
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG << "L:" << left_length << " R:" << right_length;
#endif

    if (!isSessionOkay(ic))
        return 0;

    if (hasSelectedText()) {
        QBBEventThread::injectKeyboardEvent(KEY_DOWN | KEY_CAP_VALID, KEYCODE_DELETE, 0, 0, 0);
        QBBEventThread::injectKeyboardEvent(KEY_CAP_VALID, KEYCODE_DELETE, 0, 0, 0);
        reset();
        return 0;
    }

    int replacementLength = left_length + right_length;
    int replacementStart = -left_length;

    QList<QInputMethodEvent::Attribute> attributes;
    QInputMethodEvent event(QLatin1String(""), attributes);
    event.setCommitString(QLatin1String(""), replacementStart, replacementLength);
    sendEvent(event);

    return 0;
}

int32_t QBBInputContext::onEndBatchEdit(input_session_t* ic)
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    return 0;
}

int32_t QBBInputContext::onFinishComposingText(input_session_t* ic)
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    // Only update the control, no need to send a message back to imf (don't call
    // end composition)
    QList<QInputMethodEvent::Attribute> attributes;
    QInputMethodEvent event(QLatin1String(""), attributes);
    event.setCommitString(mComposingText);
    mComposingText = QString();
    mIsComposing = false;
    sendEvent(event);

    return 0;
}

int32_t QBBInputContext::onGetCursorCapsMode(input_session_t* ic, int32_t req_modes)
{
    Q_UNUSED(req_modes);
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    // Should never get called.
    qCritical() << TAG << "onGetCursorCapsMode is unsupported.";

    return 0;
}

int32_t QBBInputContext::onGetCursorPosition(input_session_t* ic)
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    mLastCaretPos = focusWidget()->inputMethodQuery(Qt::ImCursorPosition).toInt();
    return mLastCaretPos;
}

extracted_text_t* QBBInputContext::onGetExtractedText(input_session_t* ic, extracted_text_request_t* request, int32_t flags)
{
    Q_UNUSED(flags);
    Q_UNUSED(request);
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic)) {
        extracted_text_t *et = (extracted_text_t *)calloc(sizeof(extracted_text_t),1);
        et->text = (spannable_string_t *)calloc(sizeof(spannable_string_t),1);
        return 0;
    }

    // Used to update dictionaries, but not supported right now.
    extracted_text_t *et = (extracted_text_t *)calloc(sizeof(extracted_text_t),1);
    et->text = (spannable_string_t *)calloc(sizeof(spannable_string_t),1);

    return et;
}

spannable_string_t* QBBInputContext::onGetSelectedText(input_session_t* ic, int32_t flags)
{
    Q_UNUSED(flags);
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return toSpannableString("");

    QString text = focusWidget()->inputMethodQuery(Qt::ImCurrentSelection).toString();
    return toSpannableString(text);
}

spannable_string_t* QBBInputContext::onGetTextAfterCursor(input_session_t* ic, int32_t n, int32_t flags)
{
    Q_UNUSED(flags);
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return toSpannableString("");

    QString text = focusWidget()->inputMethodQuery(Qt::ImSurroundingText).toString();
    mLastCaretPos = focusWidget()->inputMethodQuery(Qt::ImCursorPosition).toInt();

    return toSpannableString(text.mid(mLastCaretPos+1, n));
}

spannable_string_t* QBBInputContext::onGetTextBeforeCursor(input_session_t* ic, int32_t n, int32_t flags)
{
    Q_UNUSED(flags);
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return toSpannableString("");

    QString text = focusWidget()->inputMethodQuery(Qt::ImSurroundingText).toString();
    mLastCaretPos = focusWidget()->inputMethodQuery(Qt::ImCursorPosition).toInt();

    if (n < mLastCaretPos)
    {
        return toSpannableString(text.mid(mLastCaretPos - n, n));
    }
    else
        return toSpannableString(text.mid(0, mLastCaretPos));
}

int32_t QBBInputContext::onPerformEditorAction(input_session_t* ic, int32_t editor_action)
{
    Q_UNUSED(editor_action);
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    // Should never get called.
    qCritical() << TAG << "onPerformEditorAction is unsupported.";

    return 0;
}

int32_t QBBInputContext::onReportFullscreenMode(input_session_t* ic, int32_t enabled)
{
    Q_UNUSED(enabled);
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    // Should never get called.
    qCritical() << TAG << "onReportFullscreenMode is unsupported.";

    return 0;
}

int32_t QBBInputContext::onSendEvent(input_session_t* ic, event_t * event)
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    return processEvent(event);
}

int32_t QBBInputContext::onSendAsyncEvent(input_session_t* ic, event_t * event)
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    return processEvent(event);
}

int32_t QBBInputContext::onSetComposingRegion(input_session_t* ic, int32_t start, int32_t end)
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    if (!focusWidget())
    {
        qCritical() << "No focus widget!";
        return 0;
    }

    QList<QInputMethodEvent::Attribute> attributes;

    mLastCaretPos = focusWidget()->inputMethodQuery(Qt::ImCursorPosition).toInt();
    QString text = focusWidget()->inputMethodQuery(Qt::ImSurroundingText).toString();
    QString empty = QString::fromLatin1("");
    text = text.mid(start, end - start);

    // Delete the current text.
    {
        QInputMethodEvent event(empty, attributes);
        event.setCommitString(empty, start - mLastCaretPos, end - start);
        sendEvent(event);
    }

    // Move the specified text into a preedit string.
    {
        setComposingText(text);
    }

    return 0;
}

int32_t QBBInputContext::onSetComposingText(input_session_t* ic, spannable_string_t* text, int32_t new_cursor_position)
{
    Q_UNUSED(new_cursor_position);
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    if (!focusWidget())
    {
        qCritical() << "No focus widget!";
        return 0;
    }

    mIsComposing = true;

    QString preeditString = QString::fromWCharArray(text->str, text->length);
    setComposingText(preeditString);

    return 0;
}

int32_t QBBInputContext::onSetSelection(input_session_t* ic, int32_t start, int32_t end)
{
    Q_UNUSED(start);
    Q_UNUSED(end);
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << TAG;
#endif

    if (!isSessionOkay(ic))
        return 0;

    // Should never get called.
    qCritical() << TAG << "onSetSelection is unsupported.";

    return 0;
}

QT_END_NAMESPACE
