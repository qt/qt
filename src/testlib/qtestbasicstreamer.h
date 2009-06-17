/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtTest module of the Qt Toolkit.
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

#ifndef QTESTBASICSTREAMER_H
#define QTESTBASICSTREAMER_H

#include <QtCore/qglobal.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Test)

class QTestElement;
class QTestElementAttribute;
class QTestLogger;

class QTestBasicStreamer
{
    public:
        QTestBasicStreamer();
        virtual ~QTestBasicStreamer();

        virtual void output(QTestElement *element) const;

        void outputString(const char *msg) const;
        bool isTtyOutput();
        void startStreaming();
        void stopStreaming();

        void setLogger(const QTestLogger *tstLogger);
        const QTestLogger *logger() const;

    protected:
        virtual void formatStart(const QTestElement *element = 0, char *formatted = 0) const;
        virtual void formatEnd(const QTestElement *element = 0, char *formatted = 0) const;
        virtual void formatBeforeAttributes(const QTestElement *element = 0, char *formatted = 0) const;
        virtual void formatAfterAttributes(const QTestElement *element = 0, char *formatted = 0) const;
        virtual void formatAttributes(const QTestElement *element = 0, const QTestElementAttribute *attribute = 0, char *formatted = 0) const;
        virtual void outputElements(QTestElement *element, bool isChildElement = false) const;
        virtual void outputElementAttributes(const QTestElement *element, QTestElementAttribute *attribute) const;

    private:
        const QTestLogger *testLogger;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
