/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_EMBEDDED_LICENSE$
**
****************************************************************************/

#ifndef QCOEFEPINPUTCONTEXT_P_H
#define QCOEFEPINPUTCONTEXT_P_H

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

#ifndef QT_NO_IM

#include "qinputcontext.h"
#include <qhash.h>
#include <private/qcore_symbian_p.h>
#include <private/qt_s60_p.h>

#include <fepbase.h>
#include <aknedsts.h>

QT_BEGIN_NAMESPACE

class Q_GUI_EXPORT QCoeFepInputContext : public QInputContext,
                                         public MCoeFepAwareTextEditor,
                                         public MCoeFepAwareTextEditor_Extension1,
                                         public MObjectProvider
{
    Q_OBJECT

public:
    QCoeFepInputContext(QObject *parent = 0);
    ~QCoeFepInputContext();

    QString identifierName() { return QLatin1String("coefep"); }
    QString language();

    void reset();
    void update();

    bool filterEvent(const QEvent *event);
    void mouseHandler( int x, QMouseEvent *event);
    bool isComposing() const { return m_isEditing; }

    void setFocusWidget(QWidget * w);
    void widgetDestroyed(QWidget *w);

    TCoeInputCapabilities inputCapabilities();

private:
    void commitCurrentString();
    void updateHints();
    void applyHints(Qt::InputMethodHints hints);
    void applyFormat(QList<QInputMethodEvent::Attribute> *attributes);

    // From MCoeFepAwareTextEditor
public:
    void StartFepInlineEditL(const TDesC& aInitialInlineText, TInt aPositionOfInsertionPointInInlineText,
            TBool aCursorVisibility, const MFormCustomDraw* aCustomDraw,
            MFepInlineTextFormatRetriever& aInlineTextFormatRetriever,
            MFepPointerEventHandlerDuringInlineEdit& aPointerEventHandlerDuringInlineEdit);
    void UpdateFepInlineTextL(const TDesC& aNewInlineText, TInt aPositionOfInsertionPointInInlineText);
    void SetInlineEditingCursorVisibilityL(TBool aCursorVisibility);
    void CancelFepInlineEdit();
    TInt DocumentLengthForFep() const;
    TInt DocumentMaximumLengthForFep() const;
    void SetCursorSelectionForFepL(const TCursorSelection& aCursorSelection);
    void GetCursorSelectionForFep(TCursorSelection& aCursorSelection) const;
    void GetEditorContentForFep(TDes& aEditorContent, TInt aDocumentPosition, TInt aLengthToRetrieve) const;
    void GetFormatForFep(TCharFormat& aFormat, TInt aDocumentPosition) const;
    void GetScreenCoordinatesForFepL(TPoint& aLeftSideOfBaseLine, TInt& aHeight, TInt& aAscent,
            TInt aDocumentPosition) const;
private:
    void DoCommitFepInlineEditL();
    MCoeFepAwareTextEditor_Extension1* Extension1(TBool& aSetToTrue);

    // From MCoeFepAwareTextEditor_Extension1
public:
    void SetStateTransferingOwnershipL(MCoeFepAwareTextEditor_Extension1::CState* aState, TUid aTypeSafetyUid);
    MCoeFepAwareTextEditor_Extension1::CState* State(TUid aTypeSafetyUid);

    // From MObjectProvider
public:
    TTypeUid::Ptr MopSupplyObject(TTypeUid id);

private:
    QSymbianControl *m_parent;
    CAknEdwinState *m_fepState;
    QString m_preeditString;
    Qt::InputMethodHints m_lastImHints;
    TUint m_textCapabilities;
    bool m_isEditing;
    bool m_inDestruction;
    int m_cursorVisibility;
    int m_inlinePosition;
    MFepInlineTextFormatRetriever *m_formatRetriever;
    MFepPointerEventHandlerDuringInlineEdit *m_pointerHandler;
};

QT_END_NAMESPACE

#endif // QT_NO_IM

#endif // QCOEFEPINPUTCONTEXT_P_H
