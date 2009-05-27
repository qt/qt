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

    static QVector<QRect> *rects;
    static QVector<QRect> *workrects;

    static int refcount;
};

int QDesktopWidgetPrivate::screenCount = 1;
int QDesktopWidgetPrivate::primaryScreen = 0;
QVector<QRect> *QDesktopWidgetPrivate::rects = 0;
QVector<QRect> *QDesktopWidgetPrivate::workrects = 0;
int QDesktopWidgetPrivate::refcount = 0;

QDesktopWidgetPrivate::QDesktopWidgetPrivate()
{
    ++refcount;
}

QDesktopWidgetPrivate::~QDesktopWidgetPrivate()
{
    if (!--refcount)
        cleanup();
}

void QDesktopWidgetPrivate::init(QDesktopWidget *that)
{
    int screenCount=0;

    if (HAL::Get(0, HALData::EDisplayNumberOfScreens, screenCount) == KErrNone)
        QDesktopWidgetPrivate::screenCount = screenCount;
    else
        QDesktopWidgetPrivate::screenCount = 0;

    rects = new QVector<QRect>();
    workrects = new QVector<QRect>();

    rects->resize(QDesktopWidgetPrivate::screenCount);
    workrects->resize(QDesktopWidgetPrivate::screenCount);

    // ### TODO: Implement proper multi-display support
    rects->resize(1);
    rects->replace(0, that->rect());
    workrects->resize(1);
    workrects->replace(0, that->rect());
}

void QDesktopWidgetPrivate::cleanup()
{
    delete rects;
    rects = 0;
    delete workrects;
    workrects = 0;
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
    Q_D(QDesktopWidget);
    QVector<QRect> oldrects;
    oldrects = *d->rects;
    QVector<QRect> oldworkrects;
    oldworkrects = *d->workrects;
    int oldscreencount = d->screenCount;

    QDesktopWidgetPrivate::cleanup();
    QDesktopWidgetPrivate::init(this);

    for (int i = 0; i < qMin(oldscreencount, d->screenCount); ++i) {
        QRect oldrect = oldrects[i];
        QRect newrect = d->rects->at(i);
        if (oldrect != newrect)
            emit resized(i);
    }

    for (int j = 0; j < qMin(oldscreencount, d->screenCount); ++j) {
        QRect oldrect = oldworkrects[j];
        QRect newrect = d->workrects->at(j);
        if (oldrect != newrect)
            emit workAreaResized(j);
    }
}

QT_END_NAMESPACE
