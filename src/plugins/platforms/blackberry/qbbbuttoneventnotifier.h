/****************************************************************************
**
** Copyright (C) 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt-project.org/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QBBBUTTONSEVENTNOTIFIER_H
#define QBBBUTTONSEVENTNOTIFIER_H

#include <QObject>
#include <QHash>

QT_BEGIN_NAMESPACE

class QSocketNotifier;

class QBBButtonEventNotifier : public QObject
{
    Q_OBJECT
    Q_ENUMS(ButtonId)
public:
    enum ButtonId {
        bid_minus = 0,
        bid_playpause,
        bid_plus,
        bid_power,
        ButtonCount
    };

    enum ButtonState {
        ButtonUp,
        ButtonDown
    };

    explicit QBBButtonEventNotifier(QObject *parent = 0);
    ~QBBButtonEventNotifier();

public Q_SLOTS:
    void start();

private Q_SLOTS:
    void updateButtonStates();

private:
    void close();
    bool parsePPS(const QByteArray &ppsData, QHash<QByteArray, QByteArray> *messageFields) const;

    int mFd;
    QSocketNotifier *mReadNotifier;
    ButtonState mState[ButtonCount];
    QList<QByteArray> mButtonKeys;
};

QT_END_NAMESPACE

#endif // QBBBUTTONSEVENTNOTIFIER_H
