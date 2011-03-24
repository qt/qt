/*  This file is part of the KDE project.

Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 or 3 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "download.h"
#include "utils.h"
#include <QtCore/QDir>
#include <QtCore/private/qcore_symbian_p.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

static const TBool InheritDownloads = EFalse;

DownloadPrivate::DownloadPrivate(Download *parent)
    :   QObject(parent)
    ,   m_parent(parent)
    ,   m_download(0)
    ,   m_length(0)
{

}

DownloadPrivate::~DownloadPrivate()
{
    if (m_download)
        m_download->Delete();
    m_downloadManager.Disconnect();
    m_downloadManager.Close();
}

bool DownloadPrivate::start(int iap)
{
    TRACE_CONTEXT(DownloadPrivate::start, EVideoApi);
    Q_ASSERT(!m_download);
    // Connect to download manager
    RProcess process;
    const TUid uid3 = process.SecureId();
    TRAPD(err, m_downloadManager.ConnectL(uid3, *this, InheritDownloads));
    TRACE("connect err %d", err);
    if (KErrNone == err) {
        // Start download
        if (KUseDefaultIap != iap)
            m_downloadManager.SetIntAttribute(EDlMgrIap, iap);
        QHBufC url(m_parent->sourceUrl().toString());
        TPtr8 url8 = url->Des().Collapse();
        TRAP(err, m_download = &m_downloadManager.CreateDownloadL(url8));
        TRACE("start err %d", err);
        if (KErrNone == err)
            m_download->Start();
    }
    return (KErrNone == err);
}

void DownloadPrivate::resume()
{

}

void DownloadPrivate::HandleDMgrEventL(RHttpDownload &aDownload, THttpDownloadEvent aEvent)
{
    TRACE_CONTEXT(DownloadPrivate::HandleDMgrEventL, EVideoApi);
    Q_ASSERT(&aDownload == m_download);
    switch (aEvent.iDownloadState) {
    case EHttpDlPaused:
        if (EHttpContentTypeReceived == aEvent.iProgressState) {
            TRACE_0("paused, content type received");
            m_download->Start();
        }
        break;
    case EHttpDlInprogress:
        switch (aEvent.iProgressState) {
        case EHttpProgResponseHeaderReceived:
            {
            TFileName fileName;
            m_download->GetStringAttribute(EDlAttrDestFilename, fileName);
            TRACE("in progress, response header received, filename %S", &fileName);
            const QString fileNameQt = QDir::fromNativeSeparators(qt_TDesC2QString(fileName));
            m_parent->downloadStarted(fileNameQt);
            }
            break;
        case EHttpProgResponseBodyReceived:
            {
            TInt32 length = 0;
            m_download->GetIntAttribute(EDlAttrDownloadedSize, length);
            if (length != m_length) {
                TRACE("in progress, length %d", length);
                m_length = length;
                emit lengthChanged(m_length);
            }
            }
            break;
        }
        break;
    case EHttpDlCompleted:
        TRACE_0("complete");
        m_parent->complete();
        break;
    case EHttpDlFailed:
        TRACE_0("failed");
        m_parent->error();
        break;
    }
}

Download::Download(const QUrl &url, QObject *parent)
    :   QObject(parent)
    ,   m_private(new DownloadPrivate(this))
    ,   m_sourceUrl(url)
    ,   m_state(Idle)
{
    qRegisterMetaType<Download::State>();
    connect(m_private, SIGNAL(lengthChanged(qint64)), this, SIGNAL(lengthChanged(qint64)));
}

Download::~Download()
{

}

const QUrl &Download::sourceUrl() const
{
    return m_sourceUrl;
}

const QString &Download::targetFileName() const
{
    return m_targetFileName;
}

void Download::start(int iap)
{
    TRACE_CONTEXT(Download::start, EVideoApi);
    TRACE_ENTRY_0();
    Q_ASSERT(Idle == m_state);
    const bool ok = m_private->start(iap);
    setState(ok ? Initializing : Error);
    TRACE_EXIT_0();
}

void Download::resume()
{
    TRACE_CONTEXT(Download::resume, EVideoApi);
    TRACE_ENTRY_0();
    m_private->resume();
    TRACE_EXIT_0();
}

void Download::setState(State state)
{
    TRACE_CONTEXT(Download::setState, EVideoApi);
    TRACE("oldState %d newState %d", m_state, state);
    const State oldState = m_state;
    m_state = state;
    if (oldState != m_state)
        emit stateChanged(m_state);
}

void Download::error()
{
    TRACE_CONTEXT(Download::error, EVideoApi);
    TRACE_0("");
    setState(Error);
}

void Download::downloadStarted(const QString &targetFileName)
{
    TRACE_CONTEXT(Download::downloadStarted, EVideoApi);
    TRACE_0("downloadStarted");
    m_targetFileName = targetFileName;
    setState(Downloading);
}

void Download::complete()
{
    TRACE_CONTEXT(Download::complete, EVideoApi);
    TRACE_0("");
    setState(Complete);
}

QT_END_NAMESPACE

