/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef QTESTHELPER_H
#define QTESTHELPER_H

#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtTest/QSignalSpy>
#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE
namespace QTest
{

    /**
     * Starts an event loop that runs until the given signal is received. Optionally the event loop
     * can return earlier on a timeout.
     *
     * \return \p true if the requested signal was received
     *         \p false on timeout
     */
    bool waitForSignal(QObject *obj, const char *signal, int timeout = 0)
    {
        QEventLoop loop;
        QObject::connect(obj, signal, &loop, SLOT(quit()));
        QTimer timer;
        QSignalSpy timeoutSpy(&timer, SIGNAL(timeout()));
        if (timeout > 0) {
            QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
            timer.setSingleShot(true);
            timer.start(timeout);
        }
        loop.exec();
        return timeoutSpy.isEmpty();
    }


//    template<>
    char *toString(const Phonon::State &state)
    {
        switch (state) {
        case Phonon::LoadingState:
            return qstrdup("LoadingState");
        case Phonon::StoppedState:
            return qstrdup("StoppedState");
        case Phonon::PlayingState:
            return qstrdup("PlayingState");
        case Phonon::BufferingState:
            return qstrdup("BufferingState");
        case Phonon::PausedState:
            return qstrdup("PausedState");
        case Phonon::ErrorState:
            return qstrdup("ErrorState");
        }
        return 0;
    }

//    template<>
    char *toString(const QVariant::Type &type)
    {
        switch (type) {
        case QVariant::Invalid:
            return qstrdup("QVariant::Invalid");
        case QVariant::BitArray:
            return qstrdup("QVariant::BitArray");
        case QVariant::Bitmap:
            return qstrdup("QVariant::Bitmap");
        case QVariant::Bool:
            return qstrdup("QVariant::Bool");
        case QVariant::Brush:
            return qstrdup("QVariant::Brush");
        case QVariant::ByteArray:
            return qstrdup("QVariant::ByteArray");
        case QVariant::Char:
            return qstrdup("QVariant::Char");
        case QVariant::Color:
            return qstrdup("QVariant::Color");
        case QVariant::Cursor:
            return qstrdup("QVariant::Cursor");
        case QVariant::Date:
            return qstrdup("QVariant::Date");
        case QVariant::DateTime:
            return qstrdup("QVariant::DateTime");
        case QVariant::Double:
            return qstrdup("QVariant::Double");
        case QVariant::Font:
            return qstrdup("QVariant::Font");
        case QVariant::Icon:
            return qstrdup("QVariant::Icon");
        case QVariant::Image:
            return qstrdup("QVariant::Image");
        case QVariant::Int:
            return qstrdup("QVariant::Int");
        case QVariant::KeySequence:
            return qstrdup("QVariant::KeySequence");
        case QVariant::Line:
            return qstrdup("QVariant::Line");
        case QVariant::LineF:
            return qstrdup("QVariant::LineF");
        case QVariant::List:
            return qstrdup("QVariant::List");
        case QVariant::Locale:
            return qstrdup("QVariant::Locale");
        case QVariant::LongLong:
            return qstrdup("QVariant::LongLong");
        case QVariant::Map:
            return qstrdup("QVariant::Map");
        case QVariant::Matrix:
            return qstrdup("QVariant::Matrix");
        case QVariant::Transform:
            return qstrdup("QVariant::Transform");
        case QVariant::Palette:
            return qstrdup("QVariant::Palette");
        case QVariant::Pen:
            return qstrdup("QVariant::Pen");
        case QVariant::Pixmap:
            return qstrdup("QVariant::Pixmap");
        case QVariant::Point:
            return qstrdup("QVariant::Point");
        case QVariant::PointF:
            return qstrdup("QVariant::PointF");
        case QVariant::Polygon:
            return qstrdup("QVariant::Polygon");
        case QVariant::Rect:
            return qstrdup("QVariant::Rect");
        case QVariant::RectF:
            return qstrdup("QVariant::RectF");
        case QVariant::RegExp:
            return qstrdup("QVariant::RegExp");
        case QVariant::Region:
            return qstrdup("QVariant::Region");
        case QVariant::Size:
            return qstrdup("QVariant::Size");
        case QVariant::SizeF:
            return qstrdup("QVariant::SizeF");
        case QVariant::SizePolicy:
            return qstrdup("QVariant::SizePolicy");
        case QVariant::String:
            return qstrdup("QVariant::String");
        case QVariant::StringList:
            return qstrdup("QVariant::StringList");
        case QVariant::TextFormat:
            return qstrdup("QVariant::TextFormat");
        case QVariant::TextLength:
            return qstrdup("QVariant::TextLength");
        case QVariant::Time:
            return qstrdup("QVariant::Time");
        case QVariant::UInt:
            return qstrdup("QVariant::UInt");
        case QVariant::ULongLong:
            return qstrdup("QVariant::ULongLong");
        case QVariant::Url:
            return qstrdup("QVariant::Url");
        case QVariant::UserType:
            return qstrdup("QVariant::UserType");
        case QVariant::LastType:
            return qstrdup("QVariant::LastType");
        default:
            return 0;
        }
    }
} // namespace QTest
QT_END_NAMESPACE

#endif // QTESTHELPER_H
