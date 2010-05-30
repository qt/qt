/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef PatternistSDK_ExitCode_H
#define PatternistSDK_ExitCode_H

#include "Global.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternistSDK
{
    /**
     * @short Houses program exit codes for PatternistSDK utilities.
     *
     * @ingroup PatternistSDK
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class Q_PATTERNISTSDK_EXPORT ExitCode
    {
    public:
        /**
         * Exit codes for programs part of PatternistSDK. The values for the enums are specified
         * to make it easily understandable what number a symbol corresponds to.
         */
        enum Code
        {
            Success             = 0,
            InvalidArgCount     = 1,
            InvalidArgs         = 2,

            /**
             * Used in @c patternistrunsuite
             */
            InvalidCatalogFile  = 3,
            FileNotExists       = 4,
            OpenError           = 5,
            WriteError          = 6,

            /**
             * Used in the test program used for comparing files on the byte level.
             */
            NotEqual            = 7,
            UnevenTestCount     = 8,
            InternalError       = 9,
            Regression          = 10
        };

    private:
        /**
         * This constructor is private and has no implementation, because this
         * class is not supposed to be instantiated.
         */
        inline ExitCode();

        Q_DISABLE_COPY(ExitCode)
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
// vim: et:ts=4:sw=4:sts=4
