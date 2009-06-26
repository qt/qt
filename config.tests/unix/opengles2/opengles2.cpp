#include <GLES2/gl2.h>

int main(int, char **)
{
    glUniform1f(1, GLfloat(1.0));
    glClear(GL_COLOR_BUFFER_BIT);

    return 0;
}
