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

#ifndef QWINNATIVEPANGESTURERECOGNIZER_WIN_P_H
#define QWINNATIVEPANGESTURERECOGNIZER_WIN_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QGestureRecognizer>
#include <objbase.h>

class IInkRectangle;
class TabletHardwareCapabilities;
class TabletPropertyMetricUnit;
DECLARE_INTERFACE_(IInkTablet, IDispatch)
{
	STDMETHOD(get_Name)(THIS_ BSTR *Name) PURE;
	STDMETHOD(get_PlugAndPlayId)(THIS_ BSTR *Id) PURE;
	STDMETHOD(get_MaximumInputRectangle)(THIS_ IInkRectangle **Rectangle) PURE;
	STDMETHOD(get_HardwareCapabilities)(THIS_ TabletHardwareCapabilities *Capabilities) PURE;
	STDMETHOD(IsPacketPropertySupported)(THIS_ BSTR packetPropertyName) PURE;
	STDMETHOD(GetPropertyMetrics)(THIS_ BSTR propertyName, long *Minimum, long *Maximum, TabletPropertyMetricUnit *Units, float *Resolution) PURE;
};
enum TabletDeviceKind
{
	TDK_Mouse = 0,
	TDK_Pen   = 1,
	TDK_Touch = 2
};
DECLARE_INTERFACE_(IInkTablet2, IDispatch)
{
	STDMETHOD(get_DeviceKind)(THIS_ TabletDeviceKind *Kind) PURE;
};
DECLARE_INTERFACE_(IInkTablets, IDispatch)
{
	STDMETHOD(get_Count)(THIS_ long *Count) PURE;
	STDMETHOD(get__NewEnum)(THIS_ IUnknown **_NewEnum) PURE;
	STDMETHOD(get_DefaultTablet)(THIS_ IInkTablet **DefaultTablet) PURE;
	STDMETHOD(Item)(THIS_ long Index, IInkTablet **Tablet) PURE;
	STDMETHOD(IsPacketPropertySupported)(THIS_ BSTR packetPropertyName, VARIANT_BOOL *Supported) PURE;
};

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_NATIVE_GESTURES)

class QWinNativePanGestureRecognizer : public QGestureRecognizer
{
public:
    QWinNativePanGestureRecognizer();

    QGesture *create(QObject *target);
    QGestureRecognizer::Result recognize(QGesture *state, QObject *watched, QEvent *event);
    void reset(QGesture *state);
};

#endif // QT_NO_NATIVE_GESTURES

QT_END_NAMESPACE

#endif // QWINNATIVEPANGESTURERECOGNIZER_WIN_P_H
