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

#include <private/qcursor_p.h>
#include <qcursor.h>
#include <qt_s60_p.h>

#ifdef QT_NO_CURSOR
QT_BEGIN_NAMESPACE

QPoint QCursor::pos()
{
    return S60->lastCursorPos;
}

void QCursor::setPos(int x, int y)
{
    S60->lastCursorPos = QPoint(x, y);
}

QT_END_NAMESPACE
#endif // QT_NO_CURSOR
