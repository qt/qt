/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "directshowpinenum.h"

#include <QtMediaServices>


QT_BEGIN_NAMESPACE

DirectShowPinEnum::DirectShowPinEnum(const QList<IPin *> &pins)
    : m_ref(1)
    , m_pins(pins)
    , m_index(0)
{
    foreach (IPin *pin, m_pins)
        pin->AddRef();
}

DirectShowPinEnum::~DirectShowPinEnum()
{
    foreach (IPin *pin, m_pins)
        pin->Release();
}

HRESULT DirectShowPinEnum::QueryInterface(REFIID riid, void **ppvObject)
{
    if (riid == IID_IUnknown
            || riid == IID_IEnumPins) {
        AddRef();

        *ppvObject = static_cast<IEnumPins *>(this);

        return S_OK;
    } else {
        *ppvObject = 0;

        return E_NOINTERFACE;
    }
}

ULONG DirectShowPinEnum::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

ULONG DirectShowPinEnum::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);

    if (ref == 0) {
        delete this;
    }

    return ref;
}

HRESULT DirectShowPinEnum::Next(ULONG cPins, IPin **ppPins, ULONG *pcFetched)
{
    if (ppPins && (pcFetched || cPins == 1)) {
        ULONG count = qBound<ULONG>(0, cPins, m_pins.count() - m_index);

        for (ULONG i = 0; i < count; ++i, ++m_index) {
            ppPins[i] = m_pins.at(m_index);
            ppPins[i]->AddRef();
        }

        if (pcFetched)
            *pcFetched = count;

        return count == cPins ? S_OK : S_FALSE;
    } else {
        return E_POINTER;
    }
}

HRESULT DirectShowPinEnum::Skip(ULONG cPins)
{
    m_index = qMin(int(m_index + cPins), m_pins.count());

    return m_index < m_pins.count() ? S_OK : S_FALSE;
}

HRESULT DirectShowPinEnum::Reset()
{
    m_index = 0;

    return S_OK;
}

HRESULT DirectShowPinEnum::Clone(IEnumPins **ppEnum)
{
    if (ppEnum) {
        *ppEnum = new DirectShowPinEnum(m_pins);

        return S_OK;
    } else {
        return E_POINTER;
    }
}

QT_END_NAMESPACE
