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

#ifndef QS60STYLE_H
#define QS60STYLE_H

#include <QtGui/qcommonstyle.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#if !defined(QT_NO_STYLE_S60)

class QS60StylePrivate;

class Q_GUI_EXPORT QS60Style : public QCommonStyle
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QS60Style)

public:
    QS60Style();
    ~QS60Style();

    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = 0) const;
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const;
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const;
    void drawItemText(QPainter *painter, const QRect &rectangle, int alignment, const QPalette &palette, bool enabled, const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const;
    int pixelMetric(PixelMetric metric, const QStyleOption *option = 0, const QWidget *widget = 0) const;
    QSize sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &contentsSize, const QWidget *w = 0) const;
    int styleHint(StyleHint sh, const QStyleOption *opt = 0, const QWidget *w = 0,
                  QStyleHintReturn *shret = 0) const;
    QRect subControlRect(ComplexControl control, const QStyleOptionComplex *option, SubControl scontrol, const QWidget *widget = 0) const;
    QRect subElementRect(SubElement element, const QStyleOption *opt, const QWidget *widget = 0) const;
    void polish(QWidget *widget);
    void unpolish(QWidget *widget);

    void setStyleProperty(const char *name, const QVariant &value);
    QVariant styleProperty(const char *name) const;

#ifndef Q_WS_S60
    static QStringList partKeys();
    static QStringList colorListKeys();
    void setS60Theme(const QHash<QString, QPicture> &parts,
        const QHash<QPair<QString , int>, QColor> &colors);
#endif // !Q_WS_S60

#ifdef Q_WS_S60
public slots:
    void handleDynamicLayoutVariantSwitch();
    void handleSkinChange();
#endif // Q_WS_S60

protected slots:
    QIcon standardIconImplementation(
            StandardPixmap standardIcon, const QStyleOption * option = 0, const QWidget * widget = 0 ) const;

private:
    Q_DISABLE_COPY(QS60Style)
    friend class QStyleFactory;
};

#endif // QT_NO_STYLE_S60

QT_END_NAMESPACE

QT_END_HEADER

#endif // QS60STYLE_H
