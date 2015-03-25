/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#include <QtCore/qdebug.h>

#define QT_DEBUG_SHADER_CACHE
#define QT_MEEGO_EXPERIMENTAL_SHADERCACHE
#define QT_OPENGL_ES_2
#define QT_BOOTSTRAPPED

typedef int GLsizei;
typedef unsigned int GLenum;

#include "../../gl2paintengineex/qglshadercache_meego_p.h"

#include <stdlib.h>
#include <stdio.h>

int main()
{
    ShaderCacheSharedMemory shm;

    if (!shm.isAttached()) {
        fprintf(stderr, "Unable to attach to shared memory\n");
        return EXIT_FAILURE;
    }

    ShaderCacheLocker locker(&shm);
    if (!locker.isLocked()) {
        fprintf(stderr, "Unable to lock shared memory\n");
        return EXIT_FAILURE;
    }

    void *data = shm.data();
    Q_ASSERT(data);

    CachedShaders *cache = reinterpret_cast<CachedShaders *>(data);

    for (int i = 0; i < cache->shaderCount; ++i) {
        printf("Shader %d: %d bytes\n", i, cache->headers[i].size);
    }

    printf("\nSummary:\n\n"
           "    Amount of cached shaders: %d\n"
           "                  Bytes used: %d\n"
           "             Bytes available: %d\n",
           cache->shaderCount, cache->dataSize, cache->availableSize());

    return EXIT_SUCCESS;
}

