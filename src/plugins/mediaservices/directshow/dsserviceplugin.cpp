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

#include <QtCore/qstring.h>
#include <QtCore/qdebug.h>
#include <QtCore/QFile>

#include "dsserviceplugin.h"

#ifdef QMEDIA_DIRECTSHOW_CAMERA
#include "dscameraservice.h"
#endif

#ifdef QMEDIA_DIRECTSHOW_PLAYER
#include "directshowplayerservice.h"
#endif

#include <qmediaserviceprovider.h>


#ifdef QMEDIA_DIRECTSHOW_CAMERA
#ifndef _STRSAFE_H_INCLUDED_
#include <tchar.h>
#endif
#include <dshow.h>
#include <objbase.h>
#include <initguid.h>
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "ole32.lib")
#include <windows.h>
#endif


QT_BEGIN_NAMESPACE

QStringList DSServicePlugin::keys() const
{
    return QStringList()
#ifdef QMEDIA_DIRECTSHOW_CAMERA
            << QLatin1String(Q_MEDIASERVICE_CAMERA)
#endif
#ifdef QMEDIA_DIRECTSHOW_PLAYER
            << QLatin1String(Q_MEDIASERVICE_MEDIAPLAYER)
#endif
            ;
}

QMediaService* DSServicePlugin::create(QString const& key)
{
#ifdef QMEDIA_DIRECTSHOW_CAMERA
    if (key == QLatin1String(Q_MEDIASERVICE_CAMERA))
        return new DSCameraService;
#endif
#ifdef QMEDIA_DIRECTSHOW_PLAYER
    if (key == QLatin1String(Q_MEDIASERVICE_MEDIAPLAYER))
        return new DirectShowPlayerService;
#endif

    qWarning() << "DirectShow service plugin: unsupported service -" << key;
    return 0;
}

void DSServicePlugin::release(QMediaService *service)
{
    delete service;
}

QList<QByteArray> DSServicePlugin::devices(const QByteArray &service) const
{
#ifdef QMEDIA_DIRECTSHOW_CAMERA
    if (service == Q_MEDIASERVICE_CAMERA) {
        if (m_cameraDevices.isEmpty())
            updateDevices();

        return m_cameraDevices;
    }
#endif

    return QList<QByteArray>();
}

QString DSServicePlugin::deviceDescription(const QByteArray &service, const QByteArray &device)
{
#ifdef QMEDIA_DIRECTSHOW_CAMERA
    if (service == Q_MEDIASERVICE_CAMERA) {
        if (m_cameraDevices.isEmpty())
            updateDevices();

        for (int i=0; i<m_cameraDevices.count(); i++)
            if (m_cameraDevices[i] == device)
                return m_cameraDescriptions[i];
    }
#endif
    return QString();
}

#ifdef QMEDIA_DIRECTSHOW_CAMERA
void DSServicePlugin::updateDevices() const
{
    m_cameraDevices.clear();
    m_cameraDescriptions.clear();

    CoInitialize(NULL);
    ICreateDevEnum* pDevEnum = NULL;
    IEnumMoniker* pEnum = NULL;
    // Create the System device enumerator
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
                 CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
		 reinterpret_cast<void**>(&pDevEnum));
    if(SUCCEEDED(hr)) {
        // Create the enumerator for the video capture category
	hr = pDevEnum->CreateClassEnumerator(
	     CLSID_VideoInputDeviceCategory, &pEnum, 0);
	pEnum->Reset();
	// go through and find all video capture devices
	IMoniker* pMoniker = NULL;
	while(pEnum->Next(1, &pMoniker, NULL) == S_OK) {
	    IPropertyBag *pPropBag;
	    hr = pMoniker->BindToStorage(0,0,IID_IPropertyBag,
	         (void**)(&pPropBag));
            if(FAILED(hr)) {
	        pMoniker->Release();
		continue; // skip this one
	    }
	    // Find the description
	    WCHAR str[120];
	    VARIANT varName;
	    varName.vt = VT_BSTR;
            hr = pPropBag->Read(L"FriendlyName", &varName, 0);
	    if(SUCCEEDED(hr)) {
	        StringCchCopyW(str,sizeof(str)/sizeof(str[0]),varName.bstrVal);
		QString temp(QString::fromUtf16((unsigned short*)str));
		m_cameraDevices.append(QString("ds:%1").arg(temp).toLocal8Bit().constData());
	        hr = pPropBag->Read(L"Description", &varName, 0);
	        StringCchCopyW(str,sizeof(str)/sizeof(str[0]),varName.bstrVal);
		QString temp2(QString::fromUtf16((unsigned short*)str));
		m_cameraDescriptions.append(temp2);
	    }
	    pPropBag->Release();
	    pMoniker->Release();
	}
    }
    CoUninitialize();
}
#endif

QT_END_NAMESPACE

Q_EXPORT_PLUGIN2(dsengine, DSServicePlugin);

