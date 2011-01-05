#ifndef QTESTLITEEGLINTEGRATION_H
#define QTESTLITEEGLINTEGRATION_H

#include "qtestlitestaticinfo.h"
#include "../eglconvenience/qeglconvenience.h"

class QTestLiteEglIntegration
{
public:
    static VisualID getCompatibleVisualId(Display *display, EGLConfig config);
};

#endif // QTESTLITEEGLINTEGRATION_H
