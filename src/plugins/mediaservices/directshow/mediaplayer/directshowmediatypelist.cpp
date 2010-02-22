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

#include "directshowmediatypelist.h"

#include "directshowmediatype.h"
#include "videosurfacefilter.h"


QT_BEGIN_NAMESPACE

class DirectShowMediaTypeEnum : public IEnumMediaTypes
{
public:
    DirectShowMediaTypeEnum(DirectShowMediaTypeList *list, int token, int index = 0);
    ~DirectShowMediaTypeEnum();

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IEnumMediaTypes
    HRESULT STDMETHODCALLTYPE Next(
            ULONG cMediaTypes, AM_MEDIA_TYPE **ppMediaTypes, ULONG *pcFetched);
    HRESULT STDMETHODCALLTYPE Skip(ULONG cMediaTypes);
    HRESULT STDMETHODCALLTYPE Reset();

    HRESULT STDMETHODCALLTYPE Clone(IEnumMediaTypes **ppEnum);

private:
    LONG m_ref;
    DirectShowMediaTypeList *m_list;
    int m_mediaTypeToken;
    int m_index;
};


DirectShowMediaTypeEnum::DirectShowMediaTypeEnum(
        DirectShowMediaTypeList *list, int token, int index)
    : m_ref(1)
    , m_list(list)
    , m_mediaTypeToken(token)
    , m_index(index)
{
    m_list->AddRef();
}

DirectShowMediaTypeEnum::~DirectShowMediaTypeEnum()
{
    m_list->Release();
}

HRESULT DirectShowMediaTypeEnum::QueryInterface(REFIID riid, void **ppvObject)
{
    if (!ppvObject) {
        return E_POINTER;
    } else if (riid == IID_IUnknown
            || riid == IID_IEnumMediaTypes) {
        *ppvObject = static_cast<IEnumMediaTypes *>(this);
    } else {
        *ppvObject = 0;

        return E_NOINTERFACE;
    }

    AddRef();

    return S_OK;
}

ULONG DirectShowMediaTypeEnum::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

ULONG DirectShowMediaTypeEnum::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);

    if (ref == 0) {
        delete this;
    }

    return ref;
}

HRESULT DirectShowMediaTypeEnum::Next(
        ULONG cMediaTypes, AM_MEDIA_TYPE **ppMediaTypes, ULONG *pcFetched)
{
    return m_list->nextMediaType(m_mediaTypeToken, &m_index, cMediaTypes, ppMediaTypes, pcFetched);
}

HRESULT DirectShowMediaTypeEnum::Skip(ULONG cMediaTypes)
{
    return m_list->skipMediaType(m_mediaTypeToken, &m_index, cMediaTypes);
}

HRESULT DirectShowMediaTypeEnum::Reset()
{
    m_mediaTypeToken = m_list->currentMediaTypeToken();
    m_index = 0;

    return S_OK;
}

HRESULT DirectShowMediaTypeEnum::Clone(IEnumMediaTypes **ppEnum)
{
    return m_list->cloneMediaType(m_mediaTypeToken, m_index, ppEnum);
}


DirectShowMediaTypeList::DirectShowMediaTypeList()
    : m_mediaTypeToken(0)
{
}

IEnumMediaTypes *DirectShowMediaTypeList::createMediaTypeEnum()
{
    return new DirectShowMediaTypeEnum(this, m_mediaTypeToken, 0);
}


void DirectShowMediaTypeList::setMediaTypes(const QVector<AM_MEDIA_TYPE> &types)
{
    ++m_mediaTypeToken;

    m_mediaTypes = types;
}


int DirectShowMediaTypeList::currentMediaTypeToken()
{
    return m_mediaTypeToken;
}

HRESULT DirectShowMediaTypeList::nextMediaType(
        int token, int *index, ULONG count, AM_MEDIA_TYPE **types, ULONG *fetchedCount)
{
    if (!types || (count != 1 && !fetchedCount)) {
        return E_POINTER;
    } else if (m_mediaTypeToken != token) {
        return VFW_E_ENUM_OUT_OF_SYNC;
    } else {
        int boundedCount = qBound<int>(0, count, m_mediaTypes.count() - *index);

        for (int i = 0; i < boundedCount; ++i, ++(*index)) {
            types[i] = reinterpret_cast<AM_MEDIA_TYPE *>(CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE)));

            if (types[i]) {
                DirectShowMediaType::copy(types[i], m_mediaTypes.at(*index));
            } else {
                for (--i; i >= 0; --i)
                    CoTaskMemFree(types[i]);

                if (fetchedCount)
                    *fetchedCount = 0;

                return E_OUTOFMEMORY;
            }
        }
        if (fetchedCount)
            *fetchedCount = boundedCount;

        return boundedCount == count ? S_OK : S_FALSE;
    }
}

HRESULT DirectShowMediaTypeList::skipMediaType(int token, int *index, ULONG count)
{
    if (m_mediaTypeToken != token) {
        return VFW_E_ENUM_OUT_OF_SYNC;
    } else {
        *index = qMin<int>(*index + count, m_mediaTypes.size());

        return *index < m_mediaTypes.size() ? S_OK : S_FALSE;
    }
}

HRESULT DirectShowMediaTypeList::cloneMediaType(int token, int index, IEnumMediaTypes **enumeration)
{
    if (m_mediaTypeToken != token) {
        return VFW_E_ENUM_OUT_OF_SYNC;
    } else {
        *enumeration = new DirectShowMediaTypeEnum(this, token, index);

        return S_OK;
    }
}

QT_END_NAMESPACE
