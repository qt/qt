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

#ifndef QT_NO_IM

#include "qcoefepinputcontext_p.h"
#include <qapplication.h>
#include <qtextformat.h>

#include <fepitfr.h>

#include <limits.h>
// You only find these enumerations on SDK 5 onwards, so we need to provide our own
// to remain compatible with older releases. They won't be called by pre-5.0 SDKs.

// MAknEdStateObserver::EAknCursorPositionChanged
#define QT_EAknCursorPositionChanged MAknEdStateObserver::EAknEdwinStateEvent(6)
// MAknEdStateObserver::EAknActivatePenInputRequest
#define QT_EAknActivatePenInputRequest MAknEdStateObserver::EAknEdwinStateEvent(7)

QT_BEGIN_NAMESPACE

QCoeFepInputContext::QCoeFepInputContext(QObject *parent)
    : QInputContext(parent),
      m_fepState(new (ELeave) CAknEdwinState),
      m_lastImHints(Qt::ImhNone),
      m_textCapabilities(TCoeInputCapabilities::EAllText),
      m_isEditing(false),
      m_inDestruction(false),
      m_cursorVisibility(1),
      m_inlinePosition(0),
      m_formatRetriever(0),
      m_pointerHandler(0)
{
    m_fepState->SetObjectProvider(this);
    m_fepState->SetFlags(EAknEditorFlagDefault);
    m_fepState->SetDefaultInputMode( EAknEditorTextInputMode );
    m_fepState->SetPermittedInputModes( EAknEditorAllInputModes );
    m_fepState->SetDefaultCase( EAknEditorLowerCase );
    m_fepState->SetPermittedCases( EAknEditorLowerCase|EAknEditorUpperCase );
    m_fepState->SetSpecialCharacterTableResourceId( 0 );
    m_fepState->SetNumericKeymap( EAknEditorStandardNumberModeKeymap );
}

QCoeFepInputContext::~QCoeFepInputContext()
{
    m_inDestruction = true;

    // This is to make sure that the FEP manager "forgets" about us,
    // otherwise we may get callbacks even after we're destroyed.
    // The call below is essentially equivalent to InputCapabilitiesChanged(),
    // but is synchronous, rather than asynchronous.
    CCoeEnv::Static()->SyncNotifyFocusObserversOfChangeInFocus();

    if (m_fepState)
        delete m_fepState;
}

void QCoeFepInputContext::reset()
{
    CCoeEnv::Static()->Fep()->CancelTransaction();
}

void QCoeFepInputContext::update()
{
    updateHints(false);

    // For pre-5.0 SDKs, we don't do text updates on S60 side.
    if (QSysInfo::s60Version() != QSysInfo::SV_S60_5_0) {
        return;
    }

    // Don't be fooled (as I was) by the name of this enumeration.
    // What it really does is tell the virtual keyboard UI that the text has been
    // updated and it should be reflected in the internal display of the VK.
    m_fepState->ReportAknEdStateEventL(QT_EAknCursorPositionChanged);
}

void QCoeFepInputContext::setFocusWidget(QWidget *w)
{
    commitCurrentString(false);

    QInputContext::setFocusWidget(w);

    updateHints(true);
}

void QCoeFepInputContext::widgetDestroyed(QWidget *w)
{
    // Make sure that the input capabilities of whatever new widget got focused are queried.
    CCoeControl *ctrl = w->effectiveWinId();
    if (ctrl->IsFocused()) {
        ctrl->SetFocus(false);
        ctrl->SetFocus(true);
    }
}

/*!
    Definition of struct for mapping Symbian to ISO locale
    ### REMOVE
    See below.
*/
struct symbianToISO {
    int symbian_language;
    char iso_name[8];
};

/*!
    Mapping from Symbian to ISO locale
    ### REMOVE
    This was taken from the preliminary QLocale port to S60, and should be
    removed once that is finished.
*/
static const symbianToISO symbian_to_iso_list[] = {
    { ELangEnglish,             "en_GB" },
    { ELangFrench,              "fr_FR" },
    { ELangGerman,              "de_DE" },
    { ELangSpanish,             "es_ES" },
    { ELangItalian,             "it_IT" },
    { ELangSwedish,             "sv_SE" },
    { ELangDanish,              "da_DK" },
    { ELangNorwegian,           "no_NO" },
    { ELangFinnish,             "fi_FI" },
    { ELangAmerican,            "en_US" },
    { ELangPortuguese,          "pt_PT" },
    { ELangTurkish,             "tr_TR" },
    { ELangIcelandic,           "is_IS" },
    { ELangRussian,             "ru_RU" },
    { ELangHungarian,           "hu_HU" },
    { ELangDutch,               "nl_NL" },
    { ELangBelgianFlemish,      "nl_BE" },
    { ELangCzech,               "cs_CZ" },
    { ELangSlovak,              "sk_SK" },
    { ELangPolish,              "pl_PL" },
    { ELangSlovenian,           "sl_SI" },
    { ELangTaiwanChinese,       "zh_TW" },
    { ELangHongKongChinese,     "zh_HK" },
    { ELangPrcChinese,          "zh_CN" },
    { ELangJapanese,            "ja_JP" },
    { ELangThai,                "th_TH" },
    { ELangArabic,              "ar_AE" },
    { ELangTagalog,             "tl_PH" },
    { ELangBulgarian,           "bg_BG" },
    { ELangCatalan,             "ca_ES" },
    { ELangCroatian,            "hr_HR" },
    { ELangEstonian,            "et_EE" },
    { ELangFarsi,               "fa_IR" },
    { ELangCanadianFrench,      "fr_CA" },
    { ELangGreek,               "el_GR" },
    { ELangHebrew,              "he_IL" },
    { ELangHindi,               "hi_IN" },
    { ELangIndonesian,          "id_ID" },
    { ELangLatvian,             "lv_LV" },
    { ELangLithuanian,          "lt_LT" },
    { ELangMalay,               "ms_MY" },
    { ELangBrazilianPortuguese, "pt_BR" },
    { ELangRomanian,            "ro_RO" },
    { ELangSerbian,             "sr_YU" },
    { ELangLatinAmericanSpanish, "es" },
    { ELangUkrainian,           "uk_UA" },
    { ELangUrdu,                "ur_PK" }, // India/Pakistan
    { ELangVietnamese,          "vi_VN" },
#ifdef __E32LANG_H__
// 5.0
    { ELangBasque,              "eu_ES" },
    { ELangGalician,            "gl_ES" },
#endif
    //{ ELangEnglish_Apac,        "en" },
    //{ ELangEnglish_Taiwan,      "en_TW" },
    //{ ELangEnglish_HongKong,    "en_HK" },
    //{ ELangEnglish_Prc,         "en_CN" },
    //{ ELangEnglish_Japan,       "en_JP"},
    //{ ELangEnglish_Thailand,    "en_TH" },
    //{ ELangMalay_Apac,          "ms" }
};

/*!
    Number of Symbian to ISO locale mappings
    ### Remove.
    See comment for array above.
*/
static const int symbian_to_iso_count
    = sizeof(symbian_to_iso_list)/sizeof(symbianToISO);

QString QCoeFepInputContext::language()
{
    TLanguage lang = m_fepState->LocalLanguage();
    if (lang < symbian_to_iso_count) {
        return QLatin1String(symbian_to_iso_list[lang].iso_name);
    } else {
        return QLatin1String("C");
    }
}

bool QCoeFepInputContext::filterEvent(const QEvent *event)
{
    // The CloseSoftwareInputPanel event is not handled here, because the VK will automatically
    // close when it discovers that the underlying widget does not have input capabilities.

    if (!focusWidget())
        return false;

    if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        const QKeyEvent *keyEvent = static_cast<const QKeyEvent *>(event);
        Q_ASSERT(m_lastImHints == focusWidget()->inputMethodHints());
        if (keyEvent->key() == Qt::Key_F20 && m_lastImHints & Qt::ImhHiddenText) {
            // Special case in Symbian. On editors with secret text, F20 is for some reason
            // considered to be a backspace.
            QKeyEvent modifiedEvent(keyEvent->type(), Qt::Key_Backspace, keyEvent->modifiers(),
                    keyEvent->text(), keyEvent->isAutoRepeat(), keyEvent->count());
            QApplication::sendEvent(focusWidget(), &modifiedEvent);
            return true;
        }
    }

    // For pre-5.0 SDKs, we don't launch the keyboard.
    if (QSysInfo::s60Version() != QSysInfo::SV_S60_5_0) {
        return false;
    }

    if (event->type() == QEvent::RequestSoftwareInputPanel) {
        // Notify S60 that we want the virtual keyboard to show up.
        QSymbianControl *sControl;
        sControl = focusWidget()->effectiveWinId()->MopGetObject(sControl);
        Q_ASSERT(sControl);

        // The FEP UI temporarily steals focus when it shows up the first time, causing
        // all sorts of weird effects on the focused widgets. Since it will immediately give
        // back focus to us, we temporarily disable focus handling until the job's done.
        if (sControl) {
            sControl->setIgnoreFocusChanged(true);
        }

        m_fepState->ReportAknEdStateEventL(MAknEdStateObserver::QT_EAknActivatePenInputRequest);

        if (sControl) {
            sControl->setIgnoreFocusChanged(false);
        }
        return true;
    }

    return false;
}

void QCoeFepInputContext::mouseHandler( int x, QMouseEvent *event)
{
    Q_ASSERT(m_isEditing);
    Q_ASSERT(focusWidget());

    if (event->type() == QEvent::MouseButtonPress && event->button() == Qt::LeftButton) {
        commitCurrentString(false);
        int pos = focusWidget()->inputMethodQuery(Qt::ImCursorPosition).toInt();

        QList<QInputMethodEvent::Attribute> attributes;
        attributes << QInputMethodEvent::Attribute(QInputMethodEvent::Selection, pos + x, 0, QVariant());
        QInputMethodEvent event("", attributes);
        sendEvent(event);
    }
}

TCoeInputCapabilities QCoeFepInputContext::inputCapabilities()
{
    if (m_inDestruction) {
        return TCoeInputCapabilities(TCoeInputCapabilities::ENone, 0, 0);
    }

    return TCoeInputCapabilities(m_textCapabilities, this, 0);
}

static QTextCharFormat qt_TCharFormat2QTextCharFormat(const TCharFormat &cFormat)
{
    QTextCharFormat qFormat;

    QBrush foreground(QColor(cFormat.iFontPresentation.iTextColor.Internal()));
    qFormat.setForeground(foreground);

    qFormat.setFontStrikeOut(cFormat.iFontPresentation.iStrikethrough == EStrikethroughOn);
    qFormat.setFontUnderline(cFormat.iFontPresentation.iUnderline == EUnderlineOn);

    return qFormat;
}

void QCoeFepInputContext::updateHints(bool mustUpdateInputCapabilities)
{
    QWidget *w = focusWidget();
    if (w) {
        Qt::InputMethodHints hints = w->inputMethodHints();
        if (hints != m_lastImHints) {
            m_lastImHints = hints;
            applyHints(hints);
        } else if (!mustUpdateInputCapabilities) {
            // Optimization. Return immediately if there was no change.
            return;
        }
    }
    CCoeEnv::Static()->InputCapabilitiesChanged();
}

void QCoeFepInputContext::applyHints(Qt::InputMethodHints hints)
{
    using namespace Qt;

    bool numbersOnly = hints & ImhDigitsOnly || hints & ImhFormattedNumbersOnly
            || hints & ImhDialableCharactersOnly;
    bool noOnlys = !(numbersOnly || hints & ImhUppercaseOnly
            || hints & ImhLowercaseOnly);
    TInt flags;
    Qt::InputMethodHints oldHints = hints;

    // Some sanity checking. Make sure that only one preference is set.
    InputMethodHints prefs = ImhPreferNumbers | ImhPreferUppercase | ImhPreferLowercase;
    prefs &= hints;
    if (prefs != ImhPreferNumbers && prefs != ImhPreferUppercase && prefs != ImhPreferLowercase) {
        hints &= ~prefs;
    }
    if (!noOnlys) {
        // Make sure that the preference is within the permitted set.
        if (hints & ImhPreferNumbers && !(hints & ImhDigitsOnly || hints & ImhFormattedNumbersOnly
                || hints & ImhDialableCharactersOnly)) {
            hints &= ~ImhPreferNumbers;
        } else if (hints & ImhPreferUppercase && !(hints & ImhUppercaseOnly)) {
            hints &= ~ImhPreferUppercase;
        } else if (hints & ImhPreferLowercase && !(hints & ImhLowercaseOnly)) {
            hints &= ~ImhPreferLowercase;
        }
        // If there is no preference, set it to something within the permitted set.
        if (!(hints & ImhPreferNumbers || hints & ImhPreferUppercase || hints & ImhPreferLowercase)) {
            if (hints & ImhLowercaseOnly) {
                hints |= ImhPreferLowercase;
            } else if (hints & ImhUppercaseOnly) {
                hints |= ImhPreferUppercase;
            } else if (hints & ImhDigitsOnly || hints & ImhFormattedNumbersOnly
                    || hints & ImhDialableCharactersOnly) {
                hints |= ImhPreferNumbers;
            }
        }
    }

    if (hints & ImhPreferNumbers) {
        m_fepState->SetDefaultInputMode(EAknEditorNumericInputMode);
        m_fepState->SetCurrentInputMode(EAknEditorNumericInputMode);
    } else {
        m_fepState->SetDefaultInputMode(EAknEditorTextInputMode);
        m_fepState->SetCurrentInputMode(EAknEditorTextInputMode);
    }
    flags = 0;
    if (numbersOnly) {
        flags |= EAknEditorNumericInputMode;
    }
    if (hints & ImhUppercaseOnly || hints & ImhLowercaseOnly) {
        flags |= EAknEditorTextInputMode;
    }
    if (flags == 0) {
        flags = EAknEditorAllInputModes;
    }
    m_fepState->SetPermittedInputModes(flags);
    m_fepState->ReportAknEdStateEventL(MAknEdStateObserver::EAknEdwinStateInputModeUpdate);

    if (hints & ImhPreferLowercase) {
        m_fepState->SetDefaultCase(EAknEditorLowerCase);
        m_fepState->SetCurrentCase(EAknEditorLowerCase);
    } else if (hints & ImhPreferUppercase) {
        m_fepState->SetDefaultCase(EAknEditorUpperCase);
        m_fepState->SetCurrentCase(EAknEditorUpperCase);
    } else if (hints & ImhNoAutoUppercase) {
        m_fepState->SetDefaultCase(EAknEditorLowerCase);
        m_fepState->SetCurrentCase(EAknEditorLowerCase);
    } else {
        m_fepState->SetDefaultCase(EAknEditorTextCase);
        m_fepState->SetCurrentCase(EAknEditorTextCase);
    }
    flags = 0;
    if (hints & ImhUppercaseOnly) {
        flags |= EAknEditorUpperCase;
    }
    if (hints & ImhLowercaseOnly) {
        flags |= EAknEditorLowerCase;
    }
    if (flags == 0) {
        flags = EAknEditorAllCaseModes;
        if (hints & ImhNoAutoUppercase) {
            flags &= ~EAknEditorTextCase;
        }
    }
    m_fepState->SetPermittedCases(flags);
    m_fepState->ReportAknEdStateEventL(MAknEdStateObserver::EAknEdwinStateCaseModeUpdate);

    flags = 0;
    if (hints & ImhUppercaseOnly && !(hints & ImhLowercaseOnly)
            || hints & ImhLowercaseOnly && !(hints & ImhUppercaseOnly)) {
        flags |= EAknEditorFlagFixedCase;
    }
    // Using T9 and hidden text together may actually crash the FEP, so check for hidden text too.
    if (hints & ImhNoPredictiveText || hints & ImhHiddenText) {
        flags |= EAknEditorFlagNoT9;
    }
    m_fepState->SetFlags(flags);
    m_fepState->ReportAknEdStateEventL(MAknEdStateObserver::EAknEdwinStateFlagsUpdate);

    if (hints & ImhFormattedNumbersOnly) {
        flags = EAknEditorCalculatorNumberModeKeymap;
    } else if (hints & ImhDigitsOnly) {
        flags = EAknEditorPlainNumberModeKeymap;
    } else {
        // ImhDialableCharactersOnly is the fallback as well, so we don't need to check for
        // that flag.
        flags = EAknEditorStandardNumberModeKeymap;
    }
    m_fepState->SetNumericKeymap(static_cast<TAknEditorNumericKeymap>(flags));

    if (hints & ImhHiddenText) {
        m_textCapabilities = TCoeInputCapabilities::EAllText | TCoeInputCapabilities::ESecretText;
    } else {
        m_textCapabilities = TCoeInputCapabilities::EAllText;
    }
}

void QCoeFepInputContext::applyFormat(QList<QInputMethodEvent::Attribute> *attributes)
{
    TCharFormat cFormat;
    TInt numChars = 0;
    TInt charPos = 0;
    int oldSize = attributes->size();
    while (m_formatRetriever) {
        m_formatRetriever->GetFormatOfFepInlineText(cFormat, numChars, charPos);
        if (numChars <= 0) {
            // This shouldn't happen according to S60 docs, but apparently does sometimes.
            break;
        }
        attributes->append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat,
                                                        charPos,
                                                        numChars,
                                                        QVariant(qt_TCharFormat2QTextCharFormat(cFormat))));
        charPos += numChars;
        if (charPos >= m_preeditString.size()) {
            break;
        }
    }

    if (attributes->size() == oldSize) {
        // S60 didn't provide any format, so let's give our own instead.
        attributes->append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat,
                                                        0,
                                                        m_preeditString.size(),
                                                        standardFormat(PreeditFormat)));
    }
}

void QCoeFepInputContext::StartFepInlineEditL(const TDesC& aInitialInlineText,
        TInt aPositionOfInsertionPointInInlineText, TBool aCursorVisibility, const MFormCustomDraw* /*aCustomDraw*/,
        MFepInlineTextFormatRetriever& aInlineTextFormatRetriever,
        MFepPointerEventHandlerDuringInlineEdit& aPointerEventHandlerDuringInlineEdit)
{
    QWidget *w = focusWidget();
    if (!w)
        return;

    m_isEditing = true;

    QList<QInputMethodEvent::Attribute> attributes;

    m_cursorVisibility = aCursorVisibility ? 1 : 0;
    m_inlinePosition = aPositionOfInsertionPointInInlineText;
    m_preeditString = qt_TDesC2QStringL(aInitialInlineText);

    m_formatRetriever = &aInlineTextFormatRetriever;
    m_pointerHandler = &aPointerEventHandlerDuringInlineEdit;

    applyFormat(&attributes);

    attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::Cursor,
                                                   m_inlinePosition,
                                                   m_cursorVisibility,
                                                   QVariant()));
    QInputMethodEvent event(m_preeditString, attributes);
    sendEvent(event);
}

void QCoeFepInputContext::UpdateFepInlineTextL(const TDesC& aNewInlineText,
        TInt aPositionOfInsertionPointInInlineText)
{
    QWidget *w = focusWidget();
    if (!w)
        return;

    m_inlinePosition = aPositionOfInsertionPointInInlineText;

    QList<QInputMethodEvent::Attribute> attributes;
    applyFormat(&attributes);
    attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::Cursor,
                                                   m_inlinePosition,
                                                   m_cursorVisibility,
                                                   QVariant()));
    m_preeditString = qt_TDesC2QStringL(aNewInlineText);
    QInputMethodEvent event(m_preeditString, attributes);
    sendEvent(event);
}

void QCoeFepInputContext::SetInlineEditingCursorVisibilityL(TBool aCursorVisibility)
{
    QWidget *w = focusWidget();
    if (!w)
        return;

    m_cursorVisibility = aCursorVisibility ? 1 : 0;

    QList<QInputMethodEvent::Attribute> attributes;
    attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::Cursor,
                                                   m_inlinePosition,
                                                   m_cursorVisibility,
                                                   QVariant()));
    QInputMethodEvent event(m_preeditString, attributes);
    sendEvent(event);
}

void QCoeFepInputContext::CancelFepInlineEdit()
{
    QList<QInputMethodEvent::Attribute> attributes;
    QInputMethodEvent event("", attributes);
    event.setCommitString("", 0, 0);
    m_preeditString.clear();
    sendEvent(event);

    m_isEditing = false;
}

TInt QCoeFepInputContext::DocumentLengthForFep() const
{
    QWidget *w = focusWidget();
    if (!w)
        return 0;

    QVariant variant = w->inputMethodQuery(Qt::ImSurroundingText);
    return variant.value<QString>().size() + m_preeditString.size();
}

TInt QCoeFepInputContext::DocumentMaximumLengthForFep() const
{
    QWidget *w = focusWidget();
    if (!w)
        return 0;

    QVariant variant = w->inputMethodQuery(Qt::ImMaximumTextLength);
    int size;
    if (variant.isValid()) {
        size = variant.toInt();
    } else {
        size = INT_MAX; // Sensible default for S60.
    }
    return size;
}

void QCoeFepInputContext::SetCursorSelectionForFepL(const TCursorSelection& aCursorSelection)
{
    QWidget *w = focusWidget();
    if (!w)
        return;

    int pos = aCursorSelection.iAnchorPos;
    int length = aCursorSelection.iCursorPos - pos;

    QList<QInputMethodEvent::Attribute> attributes;
    attributes << QInputMethodEvent::Attribute(QInputMethodEvent::Selection, pos, length, QVariant());
    QInputMethodEvent event(m_preeditString, attributes);
    sendEvent(event);
}

void QCoeFepInputContext::GetCursorSelectionForFep(TCursorSelection& aCursorSelection) const
{
    QWidget *w = focusWidget();
    if (!w)
        return;

    int cursor = w->inputMethodQuery(Qt::ImCursorPosition).toInt() + m_preeditString.size();
    int anchor = w->inputMethodQuery(Qt::ImAnchorPosition).toInt() + m_preeditString.size();
    aCursorSelection.iAnchorPos = anchor;
    aCursorSelection.iCursorPos = cursor;
}

void QCoeFepInputContext::GetEditorContentForFep(TDes& aEditorContent, TInt aDocumentPosition,
        TInt aLengthToRetrieve) const
{
    QWidget *w = focusWidget();
    if (!w)
        return;

    QString text = w->inputMethodQuery(Qt::ImSurroundingText).value<QString>();
    // FEP expects the preedit string to be part of the editor content, so let's mix it in.
    int cursor = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
    text.insert(cursor, m_preeditString);
    aEditorContent.Copy(qt_QString2TPtrC(text.mid(aDocumentPosition, aLengthToRetrieve)));
}

void QCoeFepInputContext::GetFormatForFep(TCharFormat& aFormat, TInt /* aDocumentPosition */) const
{
    QWidget *w = focusWidget();
    if (!w)
        return;

    QFont font = w->inputMethodQuery(Qt::ImFont).value<QFont>();
    QFontMetrics metrics(font);
    //QString name = font.rawName();
    QString name = font.defaultFamily(); // TODO! FIXME! Should be the above.
    QHBufC hBufC(name);
    aFormat = TCharFormat(hBufC->Des(), metrics.height());
}

void QCoeFepInputContext::GetScreenCoordinatesForFepL(TPoint& aLeftSideOfBaseLine, TInt& aHeight,
        TInt& aAscent, TInt /* aDocumentPosition */) const
{
    QWidget *w = focusWidget();
    if (!w)
        return;

    QRect rect = w->inputMethodQuery(Qt::ImMicroFocus).value<QRect>();
    aLeftSideOfBaseLine.iX = rect.left();
    aLeftSideOfBaseLine.iY = rect.bottom();

    QFont font = w->inputMethodQuery(Qt::ImFont).value<QFont>();
    QFontMetrics metrics(font);
    aHeight = metrics.height();
    aAscent = metrics.ascent();
}

void QCoeFepInputContext::DoCommitFepInlineEditL()
{
    commitCurrentString(true);
}

void QCoeFepInputContext::commitCurrentString(bool triggeredBySymbian)
{
    if (m_preeditString.size() == 0) {
        return;
    }

    QList<QInputMethodEvent::Attribute> attributes;
    QInputMethodEvent event("", attributes);
    event.setCommitString(m_preeditString, 0, 0);//m_preeditString.size());
    m_preeditString.clear();
    sendEvent(event);

    m_isEditing = false;

    if (!triggeredBySymbian) {
        CCoeEnv::Static()->Fep()->CancelTransaction();
    }
}

MCoeFepAwareTextEditor_Extension1* QCoeFepInputContext::Extension1(TBool& aSetToTrue)
{
    aSetToTrue = ETrue;
    return this;
}

void QCoeFepInputContext::SetStateTransferingOwnershipL(MCoeFepAwareTextEditor_Extension1::CState* aState,
        TUid /*aTypeSafetyUid*/)
{
    // Note: The S60 docs are wrong! See the State() function.
    if (m_fepState)
        delete m_fepState;
    m_fepState = static_cast<CAknEdwinState *>(aState);
}

MCoeFepAwareTextEditor_Extension1::CState* QCoeFepInputContext::State(TUid /*aTypeSafetyUid*/)
{
    // Note: The S60 docs are horribly wrong when describing the
    // SetStateTransferingOwnershipL function and this function. They say that the former
    // sets a CState object identified by the TUid, and the latter retrieves it.
    // In reality, the CState is expected to always be a CAknEdwinState (even if it was not
    // previously set), and the TUid is ignored. All in all, there is a single CAknEdwinState
    // per QCoeFepInputContext, which should be deleted if the SetStateTransferingOwnershipL
    // function is used to set a new one.
    return m_fepState;
}

TTypeUid::Ptr QCoeFepInputContext::MopSupplyObject(TTypeUid /*id*/)
{
    return TTypeUid::Null();
}

QT_END_NAMESPACE

#endif // QT_NO_IM
