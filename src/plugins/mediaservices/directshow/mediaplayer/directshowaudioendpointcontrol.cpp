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

#include "directshowaudioendpointcontrol.h"

#include "directshowglobal.h"
#include "directshowplayerservice.h"


QT_BEGIN_NAMESPACE

DirectShowAudioEndpointControl::DirectShowAudioEndpointControl(
        DirectShowPlayerService *service, QObject *parent)
    : QMediaControl(parent)
    , m_service(service)
    , m_bindContext(0)
    , m_deviceEnumerator(0)
{
    if (CreateBindCtx(0, &m_bindContext) == S_OK) {
        m_deviceEnumerator = com_new<ICreateDevEnum>(CLSID_SystemDeviceEnum);

        updateEndpoints();

        setActiveEndpoint(m_defaultEndpoint);
    }
}

DirectShowAudioEndpointControl::~DirectShowAudioEndpointControl()
{
    foreach (IMoniker *moniker, m_devices)
        moniker->Release();

    if (m_bindContext)
        m_bindContext->Release();

    if (m_deviceEnumerator)
        m_deviceEnumerator->Release();
}

QList<QString> DirectShowAudioEndpointControl::availableEndpoints() const
{
    return m_devices.keys();
}

QString DirectShowAudioEndpointControl::endpointDescription(const QString &name) const
{
    return name.section(QLatin1Char('\\'), -1);
}

QString DirectShowAudioEndpointControl::defaultEndpoint() const
{
    return m_defaultEndpoint;
}

QString DirectShowAudioEndpointControl::activeEndpoint() const
{
    return m_activeEndpoint;
}

void DirectShowAudioEndpointControl::setActiveEndpoint(const QString &name)
{
    if (m_activeEndpoint == name)
        return;

    if (IMoniker *moniker = m_devices.value(name, 0)) {
        IBaseFilter *filter = 0;

        if (moniker->BindToObject(
                m_bindContext,
                0,
                __uuidof(IBaseFilter),
                reinterpret_cast<void **>(&filter)) == S_OK) {
            m_service->setAudioOutput(filter);

            filter->Release();
        }
    }
}

void DirectShowAudioEndpointControl::updateEndpoints()
{
    IMalloc *oleMalloc = 0;
    if (m_deviceEnumerator && CoGetMalloc(1, &oleMalloc) == S_OK) {
        IEnumMoniker *monikers = 0;

        if (m_deviceEnumerator->CreateClassEnumerator(
                CLSID_AudioRendererCategory, &monikers, 0) == S_OK) {
            for (IMoniker *moniker = 0; monikers->Next(1, &moniker, 0) == S_OK; moniker->Release()) {
                OLECHAR *string = 0;
                if (moniker->GetDisplayName(m_bindContext, 0, &string) == S_OK) {
                    QString deviceId = QString::fromWCharArray(string);
                    oleMalloc->Free(string);

                    moniker->AddRef();
                    m_devices.insert(deviceId, moniker);

                    if (m_defaultEndpoint.isEmpty()
                            || deviceId.endsWith(QLatin1String("Default DirectSound Device"))) {
                        m_defaultEndpoint = deviceId;
                    }
                }
            }
            monikers->Release();
        }
        oleMalloc->Release();
    }
}

QT_END_NAMESPACE
