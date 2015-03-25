/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QFUTUREWATCHER_H
#define QFUTUREWATCHER_H

#include <QtCore/qfuture.h>

#include <QtCore/qobject.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QEvent;

class QFutureWatcherBasePrivate;

#ifdef QT_NO_QFUTURE
class QFutureInterfaceBase;
#endif

class Q_CORE_EXPORT QFutureWatcherBase : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QFutureWatcherBase)

public:
    QFutureWatcherBase(QObject *parent = 0);

    int progressValue() const;
    int progressMinimum() const;
    int progressMaximum() const;
    QString progressText() const;

    bool isStarted() const;
    bool isFinished() const;
    bool isRunning() const;
    bool isCanceled() const;
    bool isPaused() const;

    void waitForFinished();

    void setPendingResultsLimit(int limit);

    bool event(QEvent *event);

Q_SIGNALS:
    void started();
    void finished();
    void canceled();
    void paused();
    void resumed();
    void resultReadyAt(int resultIndex);
    void resultsReadyAt(int beginIndex, int endIndex);
    void progressRangeChanged(int minimum, int maximum);
    void progressValueChanged(int progressValue);
    void progressTextChanged(const QString &progressText);

public Q_SLOTS:
    void cancel();
    void setPaused(bool paused);
    void pause();
    void resume();
    void togglePaused();

protected:
    void connectNotify (const char * signal);
    void disconnectNotify (const char * signal);

    // called from setFuture() implemented in template sub-classes
    void connectOutputInterface();
    void disconnectOutputInterface(bool pendingAssignment = false);

private:
    // implemented in the template sub-classes
    virtual const QFutureInterfaceBase &futureInterface() const = 0;
    virtual QFutureInterfaceBase &futureInterface() = 0;
};

#ifndef QT_NO_QFUTURE

template <typename T>
class QFutureWatcher : public QFutureWatcherBase
{
public:
    QFutureWatcher(QObject *_parent = 0)
        : QFutureWatcherBase(_parent)
    { }
    ~QFutureWatcher()
    { disconnectOutputInterface(); }

    void setFuture(const QFuture<T> &future);
    QFuture<T> future() const
    { return m_future; }

    T result() const { return m_future.result(); }
    T resultAt(int index) const { return m_future.resultAt(index); }

#ifdef qdoc
    int progressValue() const;
    int progressMinimum() const;
    int progressMaximum() const;
    QString progressText() const;

    bool isStarted() const;
    bool isFinished() const;
    bool isRunning() const;
    bool isCanceled() const;
    bool isPaused() const;

    void waitForFinished();

    void setPendingResultsLimit(int limit);

Q_SIGNALS:
    void started();
    void finished();
    void canceled();
    void paused();
    void resumed();
    void resultReadyAt(int resultIndex);
    void resultsReadyAt(int beginIndex, int endIndex);
    void progressRangeChanged(int minimum, int maximum);
    void progressValueChanged(int progressValue);
    void progressTextChanged(const QString &progressText);

public Q_SLOTS:
    void cancel();
    void setPaused(bool paused);
    void pause();
    void resume();
    void togglePaused();
#endif

private:
    QFuture<T> m_future;
    const QFutureInterfaceBase &futureInterface() const { return m_future.d; }
    QFutureInterfaceBase &futureInterface() { return m_future.d; }
};

template <typename T>
Q_INLINE_TEMPLATE void QFutureWatcher<T>::setFuture(const QFuture<T> &_future)
{
    if (_future == m_future)
        return;

    disconnectOutputInterface(true);
    m_future = _future;
    connectOutputInterface();
}

template <>
class QFutureWatcher<void> : public QFutureWatcherBase
{
public:
    QFutureWatcher(QObject *_parent = 0)
        : QFutureWatcherBase(_parent)
    { }
    ~QFutureWatcher()
    { disconnectOutputInterface(); }

    void setFuture(const QFuture<void> &future);
    QFuture<void> future() const
    { return m_future; }

private:
    QFuture<void> m_future;
    const QFutureInterfaceBase &futureInterface() const { return m_future.d; }
    QFutureInterfaceBase &futureInterface() { return m_future.d; }
};

Q_INLINE_TEMPLATE void QFutureWatcher<void>::setFuture(const QFuture<void> &_future)
{
    if (_future == m_future)
        return;

    disconnectOutputInterface(true);
    m_future = _future;
    connectOutputInterface();
}

#endif // QT_NO_QFUTURE

QT_END_NAMESPACE
QT_END_HEADER

#endif // QFUTUREWATCHER_H
