/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QScrollArea>

class QScroller;
class QGridLayout;
class QSpinBox;
class QComboBox;
class QCheckBox;
class QPlainTextEdit;

class MetricItemUpdater;
class SnapOverlay;

class SettingsWidget : public QScrollArea
{
    Q_OBJECT

public:
    SettingsWidget(bool smallscreen = false);

    void setScroller(QWidget *widget);

protected:
    bool eventFilter(QObject *, QEvent *);

private slots:
    void scrollTo();
    void snapModeChanged(int);
    void snapPositionsChanged();

private:
    enum SnapMode {
        NoSnap,
        SnapToInterval,
        SnapToList
    };

    void addToGrid(QGridLayout *grid, QWidget *label, int widgetCount, ...);
    QList<qreal> toPositionList(QPlainTextEdit *list, int vmin, int vmax);
    void updateScrollRanges();

    QWidget *m_widget;
    QSpinBox *m_scrollx, *m_scrolly, *m_scrolltime;
    QList<MetricItemUpdater *> m_metrics;

    SnapMode m_snapxmode;
    QComboBox *m_snapx;
    QWidget *m_snapxinterval;
    QPlainTextEdit *m_snapxlist;
    QSpinBox *m_snapxfirst;
    QSpinBox *m_snapxstep;

    SnapMode m_snapymode;
    QComboBox *m_snapy;
    QWidget *m_snapyinterval;
    QPlainTextEdit *m_snapylist;
    QSpinBox *m_snapyfirst;
    QSpinBox *m_snapystep;
    SnapOverlay *m_snapoverlay;

    bool m_smallscreen;
};

#endif
