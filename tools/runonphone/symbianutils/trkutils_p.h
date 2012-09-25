/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef DEBUGGER_TRK_PRIVATE_UTILS
#define DEBUGGER_TRK_PRIVATE_UTILS

#include "trkutils.h"
#include "symbianutils_global.h"

QT_BEGIN_NAMESPACE
class QDateTime;
QT_END_NAMESPACE

namespace trk {

void appendDateTime(QByteArray *ba, QDateTime dateTime, Endianness = TargetByteOrder);
// returns a QByteArray containing optionally
// the serial frame [0x01 0x90 <len>] and 0x7e encoded7d(ba) 0x7e
QByteArray frameMessage(byte command, byte token, const QByteArray &data, bool serialFrame);
bool extractResult(QByteArray *buffer, bool serialFrame, TrkResult *r, bool& linkEstablishmentMode, QByteArray *rawData = 0);

} // namespace trk

#endif // DEBUGGER_TRK_PRIVATE_UTILS
