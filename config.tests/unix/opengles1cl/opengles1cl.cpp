#include <GLES/gl.h>

int main(int, char **)
{
    GLfixed a = 0;
    glColor4x(a, a, a, a);
    glClear(GL_COLOR_BUFFER_BIT);

    return 0;
}
