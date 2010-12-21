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

