/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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


#include <QtNetwork/qsslsocket.h>
#include <QtCore/qmutex.h>
#include <QtCore/qdatetime.h>

#include "private/qsslcontext_p.h"
#include "private/qsslsocket_p.h"
#include "private/qsslsocket_openssl_p.h"
#include "private/qsslsocket_openssl_symbols_p.h"

QT_BEGIN_NAMESPACE

// defined in qsslsocket_openssl.cpp:
extern int q_X509Callback(int ok, X509_STORE_CTX *ctx);
extern QString getErrorsFromOpenSsl();

QSslContext::QSslContext()
    : ctx(0),
    pkey(0),
    session(0),
    m_sessionTicketLifeTimeHint(-1)
{
}

QSslContext::~QSslContext()
{
    if (ctx)
        // This will decrement the reference count by 1 and free the context eventually when possible
        q_SSL_CTX_free(ctx);

    if (pkey)
        q_EVP_PKEY_free(pkey);

    if (session)
        q_SSL_SESSION_free(session);
}

QSslContext* QSslContext::fromConfiguration(QSslSocket::SslMode mode, const QSslConfiguration &configuration, bool allowRootCertOnDemandLoading)
{
    QSslContext *sslContext = new QSslContext();
    sslContext->sslConfiguration = configuration;
    sslContext->errorCode = QSslError::NoError;

    bool client = (mode == QSslSocket::SslClientMode);

    bool reinitialized = false;
init_context:
    switch (sslContext->sslConfiguration.protocol()) {
    case QSsl::SslV2:
#ifndef OPENSSL_NO_SSL2
        sslContext->ctx = q_SSL_CTX_new(client ? q_SSLv2_client_method() : q_SSLv2_server_method());
#else
        sslContext->ctx = 0; // SSL 2 not supported by the system, but chosen deliberately -> error
#endif
        break;
    case QSsl::SslV3:
        sslContext->ctx = q_SSL_CTX_new(client ? q_SSLv3_client_method() : q_SSLv3_server_method());
        break;
    case QSsl::SecureProtocols: // SslV2 will be disabled below
    case QSsl::TlsV1SslV3: // SslV2 will be disabled below
    case QSsl::AnyProtocol:
    default:
        sslContext->ctx = q_SSL_CTX_new(client ? q_SSLv23_client_method() : q_SSLv23_server_method());
        break;
    case QSsl::TlsV1:
        sslContext->ctx = q_SSL_CTX_new(client ? q_TLSv1_client_method() : q_TLSv1_server_method());
        break;
    }
    if (!sslContext->ctx) {
        // After stopping Flash 10 the SSL library looses its ciphers. Try re-adding them
        // by re-initializing the library.
        if (!reinitialized) {
            reinitialized = true;
            if (q_SSL_library_init() == 1)
                goto init_context;
        }

        sslContext->errorStr = QSslSocket::tr("Error creating SSL context (%1)").arg(QSslSocketBackendPrivate::getErrorsFromOpenSsl());
        sslContext->errorCode = QSslError::UnspecifiedError;
        return sslContext;
    }

    // Enable bug workarounds.
    long options;
    QSsl::SslProtocol protocol = sslContext->sslConfiguration.protocol();
    QSsl::SslOptions configurationOptions = sslContext->sslConfiguration.d->sslOptions;
    if (protocol == QSsl::TlsV1SslV3 || protocol == QSsl::SecureProtocols)
        options = SSL_OP_ALL|SSL_OP_NO_SSLv2;
    else
        options = SSL_OP_ALL;

    // This option is disabled by default, so we need to be able to clear it
    if (configurationOptions & QSsl::SslOptionDisableEmptyFragments)
        options |= SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS;
    else
        options &= ~SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS;

#ifdef SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION
    // This option is disabled by default, so we need to be able to clear it
    if (configurationOptions & QSsl::SslOptionDisableLegacyRenegotiation)
        options &= ~SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION;
    else
        options |= SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION;
#endif

#ifdef SSL_OP_NO_TICKET
    if (configurationOptions & QSsl::SslOptionDisableSessionTickets)
        options |= SSL_OP_NO_TICKET;
#endif
#ifdef SSL_OP_NO_COMPRESSION
    if (configurationOptions & QSsl::SslOptionDisableCompression)
        options |= SSL_OP_NO_COMPRESSION;
#endif

    q_SSL_CTX_set_options(sslContext->ctx, options);

    // Initialize ciphers
    QByteArray cipherString;
    int first = true;
    QList<QSslCipher> ciphers = sslContext->sslConfiguration.ciphers();
    if (ciphers.isEmpty())
        ciphers = QSslSocketPrivate::defaultCiphers();
    foreach (const QSslCipher &cipher, ciphers) {
        if (first)
            first = false;
        else
            cipherString.append(':');
        cipherString.append(cipher.name().toLatin1());
    }

    if (!q_SSL_CTX_set_cipher_list(sslContext->ctx, cipherString.data())) {
        sslContext->errorStr = QSslSocket::tr("Invalid or empty cipher list (%1)").arg(QSslSocketBackendPrivate::getErrorsFromOpenSsl());
        sslContext->errorCode = QSslError::UnspecifiedError;
        return sslContext;
    }

    // Add all our CAs to this store.
    QList<QSslCertificate> expiredCerts;
    foreach (const QSslCertificate &caCertificate, sslContext->sslConfiguration.caCertificates()) {
        // add expired certs later, so that the
        // valid ones are used before the expired ones
        if (! caCertificate.isValid()) {
            expiredCerts.append(caCertificate);
        } else {
            q_X509_STORE_add_cert(sslContext->ctx->cert_store, (X509 *)caCertificate.handle());
        }
    }

    bool addExpiredCerts = true;
#if defined(Q_OS_MAC) && (MAC_OS_X_VERSION_MAX_ALLOWED == MAC_OS_X_VERSION_10_5)
    //On Leopard SSL does not work if we add the expired certificates.
    if (QSysInfo::MacintoshVersion == QSysInfo::MV_10_5)
        addExpiredCerts = false;
#endif
    // now add the expired certs
    if (addExpiredCerts) {
        foreach (const QSslCertificate &caCertificate, expiredCerts) {
            q_X509_STORE_add_cert(sslContext->ctx->cert_store, reinterpret_cast<X509 *>(caCertificate.handle()));
        }
    }

    if (QSslSocketPrivate::s_loadRootCertsOnDemand && allowRootCertOnDemandLoading) {
        // tell OpenSSL the directories where to look up the root certs on demand
        QList<QByteArray> unixDirs = QSslSocketPrivate::unixRootCertDirectories();
        for (int a = 0; a < unixDirs.count(); ++a)
            q_SSL_CTX_load_verify_locations(sslContext->ctx, 0, unixDirs.at(a).constData());
    }

    // Register a custom callback to get all verification errors.
    X509_STORE_set_verify_cb_func(sslContext->ctx->cert_store, q_X509Callback);

    if (!sslContext->sslConfiguration.localCertificate().isNull()) {
        // Require a private key as well.
        if (sslContext->sslConfiguration.privateKey().isNull()) {
            sslContext->errorStr = QSslSocket::tr("Cannot provide a certificate with no key, %1").arg(QSslSocketBackendPrivate::getErrorsFromOpenSsl());
            sslContext->errorCode = QSslError::UnspecifiedError;
            return sslContext;
        }

        // Load certificate
        if (!q_SSL_CTX_use_certificate(sslContext->ctx, (X509 *)sslContext->sslConfiguration.localCertificate().handle())) {
            sslContext->errorStr = QSslSocket::tr("Error loading local certificate, %1").arg(QSslSocketBackendPrivate::getErrorsFromOpenSsl());
            sslContext->errorCode = QSslError::UnspecifiedError;
            return sslContext;
        }

        // Load private key
        sslContext->pkey = q_EVP_PKEY_new();
        // before we were using EVP_PKEY_assign_R* functions and did not use EVP_PKEY_free.
        // this lead to a memory leak. Now we use the *_set1_* functions which do not
        // take ownership of the RSA/DSA key instance because the QSslKey already has ownership.
        if (configuration.d->privateKey.algorithm() == QSsl::Rsa)
            q_EVP_PKEY_set1_RSA(sslContext->pkey, reinterpret_cast<RSA *>(configuration.d->privateKey.handle()));
        else
            q_EVP_PKEY_set1_DSA(sslContext->pkey, reinterpret_cast<DSA *>(configuration.d->privateKey.handle()));

        if (!q_SSL_CTX_use_PrivateKey(sslContext->ctx, sslContext->pkey)) {
            sslContext->errorStr = QSslSocket::tr("Error loading private key, %1").arg(QSslSocketBackendPrivate::getErrorsFromOpenSsl());
            sslContext->errorCode = QSslError::UnspecifiedError;
            return sslContext;
        }

        // Check if the certificate matches the private key.
        if (!q_SSL_CTX_check_private_key(sslContext->ctx)) {
            sslContext->errorStr = QSslSocket::tr("Private key does not certify public key, %1").arg(QSslSocketBackendPrivate::getErrorsFromOpenSsl());
            sslContext->errorCode = QSslError::UnspecifiedError;
            return sslContext;
        }
    }

    // Initialize peer verification.
    if (sslContext->sslConfiguration.peerVerifyMode() == QSslSocket::VerifyNone) {
        q_SSL_CTX_set_verify(sslContext->ctx, SSL_VERIFY_NONE, 0);
    } else {
        q_SSL_CTX_set_verify(sslContext->ctx, SSL_VERIFY_PEER, q_X509Callback);
    }

    // Set verification depth.
    if (sslContext->sslConfiguration.peerVerifyDepth() != 0)
        q_SSL_CTX_set_verify_depth(sslContext->ctx, sslContext->sslConfiguration.peerVerifyDepth());

    // set persisted session if the user set it
    if (!configuration.d->sslSession.isEmpty())
        sslContext->setSessionASN1(configuration.d->sslSession);

    return sslContext;
}

// Needs to be deleted by caller
SSL* QSslContext::createSsl()
{
    SSL* ssl = q_SSL_new(ctx);
    q_SSL_clear(ssl);

    if (!session && !sessionASN1().isEmpty()
            && !sslConfiguration.testSslOption(QSsl::SslOptionDisableSessionTickets)) {
        const unsigned char *data = reinterpret_cast<const unsigned char *>(m_sessionASN1.constData());
        session = q_d2i_SSL_SESSION(0, &data, m_sessionASN1.size()); // refcount is 1 already, set by function above
    }

    if (session) {
        // Try to resume the last session we cached
        if (!q_SSL_set_session(ssl, session)) {
            qWarning("could not set SSL session");
            q_SSL_SESSION_free(session);
            session = 0;
        }
    }
    return ssl;
}

// We cache exactly one session here
bool QSslContext::cacheSession(SSL* ssl)
{
    // dont cache the same session again
    if (session && session == q_SSL_get_session(ssl))
        return true;

    // decrease refcount of currently stored session
    // (this might happen if there are several concurrent handshakes in flight)
    if (session)
        q_SSL_SESSION_free(session);

    // cache the session the caller gave us and increase reference count
    session = q_SSL_get1_session(ssl);

    if (session && !sslConfiguration.testSslOption(QSsl::SslOptionDisableSessionTickets)
            && sslConfiguration.d->cacheSslSession == true) {
        int sessionSize = q_i2d_SSL_SESSION(session, 0);
        if (sessionSize > 0) {
            m_sessionASN1.resize(sessionSize);
            unsigned char *data = reinterpret_cast<unsigned char *>(m_sessionASN1.data());
            if (!q_i2d_SSL_SESSION(session, &data))
                qWarning("could not store persistent version of SSL session");
            m_sessionTicketLifeTimeHint = session->tlsext_tick_lifetime_hint;
        }
    }

    return (session != 0);
}

QByteArray QSslContext::sessionASN1() const
{
    return m_sessionASN1;
}

void QSslContext::setSessionASN1(const QByteArray &session)
{
    m_sessionASN1 = session;
}

int QSslContext::sessionTicketLifeTimeHint() const
{
    return m_sessionTicketLifeTimeHint;
}

QSslError::SslError QSslContext::error() const
{
    return errorCode;
}

QString QSslContext::errorString() const
{
    return errorStr;
}

QT_END_NAMESPACE
