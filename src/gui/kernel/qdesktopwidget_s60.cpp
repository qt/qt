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

#include "qdesktopwidget.h"
#include "qapplication_p.h"
#include "qwidget_p.h"
#include "qt_s60_p.h"
#include <w32std.h>

#include "hal.h"
#include "hal_data.h"

QT_BEGIN_NAMESPACE

class QDesktopWidgetPrivate : public QWidgetPrivate
{

public:
    QDesktopWidgetPrivate();
    ~QDesktopWidgetPrivate();

    static void init(QDesktopWidget *that);
    static void cleanup();

    static int screenCount;
    static int primaryScreen;
};

int QDesktopWidgetPrivate::screenCount = 1;
int QDesktopWidgetPrivate::primaryScreen = 0;

QDesktopWidgetPrivate::QDesktopWidgetPrivate()
{
}

QDesktopWidgetPrivate::~QDesktopWidgetPrivate()
{
}

void QDesktopWidgetPrivate::init(QDesktopWidget *that)
{
    TInt screenCount=0;
    TInt result=0;

    result = HAL::Get(0, HALData::EDisplayNumberOfScreens, screenCount);
    QDesktopWidgetPrivate::screenCount = screenCount;
    if( result != KErrNone)
        {
        // ### What to do if no screens found?
        }
}

void QDesktopWidgetPrivate::cleanup()
{
}


QDesktopWidget::QDesktopWidget()
    : QWidget(*new QDesktopWidgetPrivate, 0, Qt::Desktop)
{
    setObjectName(QLatin1String("desktop"));
    QDesktopWidgetPrivate::init(this);
}

QDesktopWidget::~QDesktopWidget()
{
    QDesktopWidgetPrivate::cleanup();
}

bool QDesktopWidget::isVirtualDesktop() const
{
    return true;
}

int QDesktopWidget::primaryScreen() const
{
    return QDesktopWidgetPrivate::primaryScreen;
}

int QDesktopWidget::numScreens() const
{
    Q_D(const QDesktopWidget);
    return QDesktopWidgetPrivate::screenCount;
}

QWidget *QDesktopWidget::screen(int /* screen */)
{
    return this;
}

const QRect QDesktopWidget::availableGeometry(int /* screen */) const
{
    TRect clientRect = static_cast<CEikAppUi*>(S60->appUi())->ClientRect();
    return qt_TRect2QRect(clientRect);
}

const QRect QDesktopWidget::screenGeometry(int /* screen */) const
{
    Q_D(const QDesktopWidget);
    return QRect(0, 0, S60->screenWidthInPixels, S60->screenHeightInPixels);
    }

int QDesktopWidget::screenNumber(const QWidget *widget) const
{
    return QDesktopWidgetPrivate::primaryScreen;
}

int QDesktopWidget::screenNumber(const QPoint &point) const
{
    return QDesktopWidgetPrivate::primaryScreen;
}

void QDesktopWidget::resizeEvent(QResizeEvent *)
{
}

QT_END_NAMESPACE
