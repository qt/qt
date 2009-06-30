#include <GLES/gl.h>

int main(int, char **)
{
    GLfloat a = 1.0f;
    glColor4f(a, a, a, a);
    glClear(GL_COLOR_BUFFER_BIT);

    return 0;
}
