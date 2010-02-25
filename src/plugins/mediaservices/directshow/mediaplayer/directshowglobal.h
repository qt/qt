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

#ifndef DIRECTSHOWGLOBAL_H
#define DIRECTSHOWGLOBAL_H

#include <QtCore/qglobal.h>

#include <dshow.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

template <typename T> T *com_cast(IUnknown *unknown, const IID &iid)
{
    T *iface = 0;
    return unknown && unknown->QueryInterface(iid, reinterpret_cast<void **>(&iface)) == S_OK
        ? iface
        : 0;
}

template <typename T> T *com_new(const IID &clsid, const IID &iid)
{
    T *object = 0;
    return CoCreateInstance(
            clsid,
            NULL,
            CLSCTX_INPROC_SERVER,
            iid,
            reinterpret_cast<void **>(&object)) == S_OK
        ? object
        : 0;
}

QT_END_NAMESPACE

QT_END_HEADER

#ifndef __IFilterGraph2_INTERFACE_DEFINED__
#define __IFilterGraph2_INTERFACE_DEFINED__
#define INTERFACE IFilterGraph2
DECLARE_INTERFACE_(IFilterGraph2 ,IGraphBuilder)
{
    STDMETHOD(AddSourceFilterForMoniker)(THIS_ IMoniker *, IBindCtx *, LPCWSTR,IBaseFilter **) PURE;
    STDMETHOD(ReconnectEx)(THIS_ IPin *, const AM_MEDIA_TYPE *) PURE;
    STDMETHOD(RenderEx)(IPin *, DWORD, DWORD *) PURE;
};
#undef INTERFACE  
#endif

#ifndef __IAMFilterMiscFlags_INTERFACE_DEFINED__
#define __IAMFilterMiscFlags_INTERFACE_DEFINED__
#define INTERFACE IAMFilterMiscFlags
DECLARE_INTERFACE_(IAMFilterMiscFlags ,IUnknown)
{
    STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
    STDMETHOD_(ULONG,GetMiscFlags)(THIS) PURE;
};
#undef INTERFACE  
#endif

#ifndef __IFileSourceFilter_INTERFACE_DEFINED__
#define __IFileSourceFilter_INTERFACE_DEFINED__
#define INTERFACE IFileSourceFilter
DECLARE_INTERFACE_(IFileSourceFilter ,IUnknown)
{
    STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
    STDMETHOD(Load)(THIS_ LPCOLESTR, const AM_MEDIA_TYPE *) PURE;
    STDMETHOD(GetCurFile)(THIS_ LPOLESTR *ppszFileName, AM_MEDIA_TYPE *) PURE;
};
#undef INTERFACE  
#endif

#ifndef __IAMOpenProgress_INTERFACE_DEFINED__
#define __IAMOpenProgress_INTERFACE_DEFINED__
#define INTERFACE IAMOpenProgress
DECLARE_INTERFACE_(IAMOpenProgress ,IUnknown)
{
    STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
    STDMETHOD(QueryProgress)(THIS_ LONGLONG *, LONGLONG *) PURE;
    STDMETHOD(AbortOperation)(THIS) PURE;
};
#undef INTERFACE  
#endif

#ifndef __IFilterChain_INTERFACE_DEFINED__
#define __IFilterChain_INTERFACE_DEFINED__
#define INTERFACE IFilterChain
DECLARE_INTERFACE_(IFilterChain ,IUnknown)
{
    STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
    STDMETHOD(StartChain)(IBaseFilter *, IBaseFilter *) PURE;
    STDMETHOD(PauseChain)(IBaseFilter *, IBaseFilter *) PURE;
    STDMETHOD(StopChain)(IBaseFilter *, IBaseFilter *) PURE;
    STDMETHOD(RemoveChain)(IBaseFilter *, IBaseFilter *) PURE;
};
#undef INTERFACE  
#endif

#endif
