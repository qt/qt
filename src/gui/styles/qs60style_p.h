/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QS60STYLE_P_H
#define QS60STYLE_P_H

#include "qs60style.h"
#include "qcommonstyle_p.h"
#include <QtCore/qhash.h>

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

QT_BEGIN_NAMESPACE

const int MAX_NON_CUSTOM_PIXELMETRICS = 92;
const int CUSTOMVALUESCOUNT = 4;
enum {
    PM_Custom_FrameCornerWidth = MAX_NON_CUSTOM_PIXELMETRICS,
    PM_Custom_FrameCornerHeight,
    PM_Custom_BoldLineWidth,
    PM_Custom_ThinLineWidth
    };
const int MAX_PIXELMETRICS = MAX_NON_CUSTOM_PIXELMETRICS + CUSTOMVALUESCOUNT;

typedef struct {
    unsigned short height;
    unsigned short width;
    int major_version;
    int minor_version;
    bool mirroring; // TODO: (nice to have) Use Qt::LayoutDirection
    QString layoutName;
} layoutHeader;

#ifdef Q_OS_SYMBIAN
NONSHARABLE_CLASS (QS60StyleEnums)
#else
class QS60StyleEnums
#endif
: public QObject
{
#ifndef Q_WS_S60
    Q_OBJECT
    Q_ENUMS(FontCategories)
    Q_ENUMS(SkinParts)
    Q_ENUMS(ColorLists)
#endif // !Q_WS_S60

public:
    // S60 look-and-feel font categories
    enum FontCategories {
        FC_Undefined,
        FC_Primary,
        FC_Secondary,
        FC_Title,
        FC_PrimarySmall,
        FC_Digital
    };

    enum SkinParts {
        SP_QgnGrafBarFrameCenter,
        SP_QgnGrafBarFrameSideL,
        SP_QgnGrafBarFrameSideR,
        SP_QgnGrafBarProgress,
        SP_QgnGrafTabActiveL,
        SP_QgnGrafTabActiveM,
        SP_QgnGrafTabActiveR,
        SP_QgnGrafTabPassiveL,
        SP_QgnGrafTabPassiveM,
        SP_QgnGrafTabPassiveR,
        SP_QgnIndiCheckboxOff,
        SP_QgnIndiCheckboxOn,
        SP_QgnIndiMarkedAdd,
        SP_QgnIndiNaviArrowLeft,
        SP_QgnIndiNaviArrowRight,
        SP_QgnIndiRadiobuttOff,
        SP_QgnIndiRadiobuttOn,
        SP_QgnIndiSliderEdit,
        SP_QgnIndiSubMenu,
        SP_QgnNoteErased,
        SP_QgnNoteError,
        SP_QgnNoteInfo,
        SP_QgnNoteOk,
        SP_QgnNoteQuery,
        SP_QgnNoteWarning,
        SP_QgnPropFileSmall,
        SP_QgnPropFolderCurrent,
        SP_QgnPropFolderSmall,
        SP_QgnPropFolderSmallNew,
        SP_QgnPropPhoneMemcLarge,
        SP_QsnBgScreen,
        SP_QsnCpScrollBgBottom,
        SP_QsnCpScrollBgMiddle,
        SP_QsnCpScrollBgTop,
        SP_QsnCpScrollHandleBottom,
        SP_QsnCpScrollHandleMiddle,
        SP_QsnCpScrollHandleTop,
        SP_QsnFrButtonTbCornerTl,           // Button, normal state
        SP_QsnFrButtonTbCornerTr,
        SP_QsnFrButtonTbCornerBl,
        SP_QsnFrButtonTbCornerBr,
        SP_QsnFrButtonTbSideT,
        SP_QsnFrButtonTbSideB,
        SP_QsnFrButtonTbSideL,
        SP_QsnFrButtonTbSideR,
        SP_QsnFrButtonTbCenter,
        SP_QsnFrButtonTbCornerTlPressed,    // Button, pressed state
        SP_QsnFrButtonTbCornerTrPressed,
        SP_QsnFrButtonTbCornerBlPressed,
        SP_QsnFrButtonTbCornerBrPressed,
        SP_QsnFrButtonTbSideTPressed,
        SP_QsnFrButtonTbSideBPressed,
        SP_QsnFrButtonTbSideLPressed,
        SP_QsnFrButtonTbSideRPressed,
        SP_QsnFrButtonTbCenterPressed,
        SP_QsnFrCaleCornerTl,               // calendar grid item
        SP_QsnFrCaleCornerTr,
        SP_QsnFrCaleCornerBl,
        SP_QsnFrCaleCornerBr,
        SP_QsnFrCaleGSideT,
        SP_QsnFrCaleGSideB,
        SP_QsnFrCaleGSideL,
        SP_QsnFrCaleGSideR,
        SP_QsnFrCaleCenter,
        SP_QsnFrCaleHeadingCornerTl,        // calendar grid header
        SP_QsnFrCaleHeadingCornerTr,
        SP_QsnFrCaleHeadingCornerBl,
        SP_QsnFrCaleHeadingCornerBr,
        SP_QsnFrCaleHeadingSideT,
        SP_QsnFrCaleHeadingSideB,
        SP_QsnFrCaleHeadingSideL,
        SP_QsnFrCaleHeadingSideR,
        SP_QsnFrCaleHeadingCenter,
        SP_QsnFrInputCornerTl,              // Text input field
        SP_QsnFrInputCornerTr,
        SP_QsnFrInputCornerBl,
        SP_QsnFrInputCornerBr,
        SP_QsnFrInputSideT,
        SP_QsnFrInputSideB,
        SP_QsnFrInputSideL,
        SP_QsnFrInputSideR,
        SP_QsnFrInputCenter,
        SP_QsnFrListCornerTl,               // List background
        SP_QsnFrListCornerTr,
        SP_QsnFrListCornerBl,
        SP_QsnFrListCornerBr,
        SP_QsnFrListSideT,
        SP_QsnFrListSideB,
        SP_QsnFrListSideL,
        SP_QsnFrListSideR,
        SP_QsnFrListCenter,
        SP_QsnFrPopupCornerTl,              // Option menu background
        SP_QsnFrPopupCornerTr,
        SP_QsnFrPopupCornerBl,
        SP_QsnFrPopupCornerBr,
        SP_QsnFrPopupSideT,
        SP_QsnFrPopupSideB,
        SP_QsnFrPopupSideL,
        SP_QsnFrPopupSideR,
        SP_QsnFrPopupCenter,
        SP_QsnFrPopupPreviewCornerTl,       // tool tip background
        SP_QsnFrPopupPreviewCornerTr,
        SP_QsnFrPopupPreviewCornerBl,
        SP_QsnFrPopupPreviewCornerBr,
        SP_QsnFrPopupPreviewSideT,
        SP_QsnFrPopupPreviewSideB,
        SP_QsnFrPopupPreviewSideL,
        SP_QsnFrPopupPreviewSideR,
        SP_QsnFrPopupPreviewCenter,
        SP_QsnFrSetOptCornerTl,             // Settings list
        SP_QsnFrSetOptCornerTr,
        SP_QsnFrSetOptCornerBl,
        SP_QsnFrSetOptCornerBr,
        SP_QsnFrSetOptSideT,
        SP_QsnFrSetOptSideB,
        SP_QsnFrSetOptSideL,
        SP_QsnFrSetOptSideR,
        SP_QsnFrSetOptCenter
    };

    enum ColorLists {
        CL_QsnHighlightColors,
        CL_QsnIconColors,
        CL_QsnLineColors,
        CL_QsnOtherColors,
        CL_QsnParentColors,
        CL_QsnTextColors
    };
};

// Private class
#ifdef Q_OS_SYMBIAN
NONSHARABLE_CLASS (QS60StylePrivate)
#else
class QS60StylePrivate
#endif
: public QCommonStylePrivate
{
    Q_DECLARE_PUBLIC(QS60Style)

public:
    QS60StylePrivate();
    ~QS60StylePrivate();

    enum SkinElements {
        SE_ButtonNormal,
        SE_ButtonPressed,
        SE_FrameLineEdit,
        SE_ProgressBarGrooveHorizontal,
        SE_ProgressBarIndicatorHorizontal,
        SE_ProgressBarGrooveVertical,
        SE_ProgressBarIndicatorVertical,
        SE_ScrollBarGrooveHorizontal,
        SE_ScrollBarGrooveVertical,
        SE_ScrollBarHandleHorizontal,
        SE_ScrollBarHandleVertical,
        SE_SliderHandleHorizontal,
        SE_SliderHandleVertical,
        SE_TabBarTabEastActive,
        SE_TabBarTabEastInactive,
        SE_TabBarTabNorthActive,
        SE_TabBarTabNorthInactive,
        SE_TabBarTabSouthActive,
        SE_TabBarTabSouthInactive,
        SE_TabBarTabWestActive,
        SE_TabBarTabWestInactive,
        SE_ListHighlight,
        SE_OptionsMenu,
        SE_SettingsList,
        SE_TableItem,
        SE_TableHeaderItem,
        SE_ToolTip //own graphic available on 3.2+ releases
    };

    enum SkinFrameElements {
        SF_ButtonNormal,
        SF_ButtonPressed,
        SF_FrameLineEdit,
        SF_ListHighlight,
        SF_OptionsMenu,
        SF_SettingsList,
        SF_TableItem,
        SF_TableHeaderItem,
        SF_ToolTip
    };

    enum SkinElementFlag {
        SF_PointNorth =       0x0001, // North = the default
        SF_PointEast =        0x0002,
        SF_PointSouth =       0x0004,
        SF_PointWest =        0x0008,

        SF_StateEnabled =     0x0010, // Enabled = the default
        SF_StateDisabled =    0x0020,
        SF_ColorSkinned =     0x0040,
    };
    Q_DECLARE_FLAGS(SkinElementFlags, SkinElementFlag)

    // draws skin element
    static void drawSkinElement(SkinElements element, QPainter *painter,
        const QRect &rect, SkinElementFlags flags = KDefaultSkinElementFlags);
    // draws a specific skin part
    static void drawSkinPart(QS60StyleEnums::SkinParts part, QPainter *painter,
        const QRect &rect, SkinElementFlags flags = KDefaultSkinElementFlags);
    // sets style property
    void setStyleProperty(const char *name, const QVariant &value);
    // sets specific style property
    void setStyleProperty_specific(const char *name, const QVariant &value);
    // gets style property
    QVariant styleProperty(const char *name) const;
    // gets specific style property
    QVariant styleProperty_specific(const char *name) const;
    // gets pixel metrics value
    static short pixelMetric(int metric);
    // gets color. 'index' is NOT 0-based.
    // It corresponds to the enum key 1-based numbers of TAknsQsnXYZColorsIndex, not the values.
    static QColor s60Color(QS60StyleEnums::ColorLists list,
        int index, const QStyleOption *option);
    // gets state specific color
    static QColor stateColor(const QColor &color, const QStyleOption *option);
    static QColor lighterColor(const QColor &baseColor);
    static bool isSkinnableDialog(const QWidget *widget);
    // gets layout
    static const QHash<QStyle::PixelMetric, int> &s60StyleLayout();

    QFont s60Font(QS60StyleEnums::FontCategories fontCategory,
        int pointSize = -1) const;
    void clearCaches();
    static QPixmap backgroundTexture();

    static bool isTouchSupported();
    // calculates average color based on button skin graphics (minus borders).
    QColor colorFromFrameGraphics(QS60StylePrivate::SkinFrameElements frame) const;
    void setThemePalette(QApplication *application) const;

    static int focusRectPenWidth();

#if defined(QT_S60STYLE_LAYOUTDATA_SIMULATED)
    static const layoutHeader m_layoutHeaders[];
    static const short data[][MAX_PIXELMETRICS];

    void setCurrentLayout(int layoutIndex);
    void setActiveLayout();
    // Pointer
    static short const *m_pmPointer;
    // number of layouts supported by the style
    static const int m_numberOfLayouts;
#endif // defined(QT_S60STYLE_LAYOUTDATA_SIMULATED)

    mutable QHash<QPair<QS60StyleEnums::FontCategories , int>, QFont> m_mappedFontsCache;
    mutable QHash<QS60StylePrivate::SkinFrameElements, QColor> m_colorCache;

    // Has one entry per SkinFrameElements
    static const struct frameElementCenter {
        SkinElements element;
        QS60StyleEnums::SkinParts center;
    } m_frameElementsData[];

    static QPixmap frame(SkinFrameElements frame, const QSize &size,
        SkinElementFlags flags = KDefaultSkinElementFlags);

private:
    static void drawPart(QS60StyleEnums::SkinParts part, QPainter *painter,
        const QRect &rect, SkinElementFlags flags = KDefaultSkinElementFlags);
    static void drawRow(QS60StyleEnums::SkinParts start, QS60StyleEnums::SkinParts middle,
        QS60StyleEnums::SkinParts end, Qt::Orientation orientation, QPainter *painter,
        const QRect &rect, SkinElementFlags flags = KDefaultSkinElementFlags);
    static void drawFrame(SkinFrameElements frame, QPainter *painter,
        const QRect &rect, SkinElementFlags flags = KDefaultSkinElementFlags);

    static QSize partSize(QS60StyleEnums::SkinParts part,
        SkinElementFlags flags = KDefaultSkinElementFlags);
    static QPixmap part(QS60StyleEnums::SkinParts part, const QSize &size,
        SkinElementFlags flags = KDefaultSkinElementFlags);
    static QPixmap cachedPart(QS60StyleEnums::SkinParts part, const QSize &size,
        SkinElementFlags flags = KDefaultSkinElementFlags);
    static QPixmap cachedFrame(SkinFrameElements frame, const QSize &size,
        SkinElementFlags flags = KDefaultSkinElementFlags);

    static QFont s60Font_specific(QS60StyleEnums::FontCategories fontCategory, int pointSize);

    static void refreshUI();

    static QSize screenSize();

    static bool m_backgroundValid;
    const static SkinElementFlags KDefaultSkinElementFlags;
};

QT_END_NAMESPACE

#endif // QS60STYLE_P_H
