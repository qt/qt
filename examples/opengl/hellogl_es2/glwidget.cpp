/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "glwidget.h"
#include <QPainter>
#include <math.h>

#include "bubble.h"


const int bubbleNum = 8;

inline void CrossProduct(qreal &xOut, qreal &yOut, qreal &zOut, qreal x1, qreal y1, qreal z1, qreal x2, qreal y2, qreal z2)
{
   xOut = y1 * z2 - z1 * y2;
   yOut = z1 * x2 - x1 * z2;
   zOut = x1 * y2 - y1 * x2;
}

inline void Normalize(qreal &x, qreal &y, qreal &z)
{
    qreal l = sqrt(x*x + y*y + z*z);
    x = x / l;
    y = y / l;
    z = z / l;
}

inline void IdentityMatrix(GLfloat *m)
{
    m[0 * 4 + 0] = 1.0f;
    m[1 * 4 + 0] = 0.0f;
    m[2 * 4 + 0] = 0.0f;
    m[3 * 4 + 0] = 0.0f;
    m[0 * 4 + 1] = 0.0f;
    m[1 * 4 + 1] = 1.0f;
    m[2 * 4 + 1] = 0.0f;
    m[3 * 4 + 1] = 0.0f;
    m[0 * 4 + 2] = 0.0f;
    m[1 * 4 + 2] = 0.0f;
    m[2 * 4 + 2] = 1.0f;
    m[3 * 4 + 2] = 0.0f;
    m[0 * 4 + 3] = 0.0f;
    m[1 * 4 + 3] = 0.0f;
    m[2 * 4 + 3] = 0.0f;
    m[3 * 4 + 3] = 1.0f;
}

// Adjust a 4x4 matrix to apply a scale.
inline void ScaleMatrix(GLfloat *m, GLfloat scalex, GLfloat scaley, GLfloat scalez)
{
    m[0 * 4 + 0] *= scalex;
    m[0 * 4 + 1] *= scalex;
    m[0 * 4 + 2] *= scalex;
    m[0 * 4 + 3] *= scalex;
    m[1 * 4 + 0] *= scaley;
    m[1 * 4 + 1] *= scaley;
    m[1 * 4 + 2] *= scaley;
    m[1 * 4 + 3] *= scaley;
    m[2 * 4 + 0] *= scalez;
    m[2 * 4 + 1] *= scalez;
    m[2 * 4 + 2] *= scalez;
    m[2 * 4 + 3] *= scalez;
}

// Adjust a 4x4 matrix to apply a translation.
inline void TranslateMatrix(GLfloat *m, GLfloat translatex, GLfloat translatey, GLfloat translatez)
{
    m[3 * 4 + 0] += m[0 * 4 + 0] * translatex + m[1 * 4 + 0] * translatey + m[2 * 4 + 0] * translatez;
    m[3 * 4 + 1] += m[0 * 4 + 1] * translatex + m[1 * 4 + 1] * translatey + m[2 * 4 + 1] * translatez;
    m[3 * 4 + 2] += m[0 * 4 + 2] * translatex + m[1 * 4 + 2] * translatey + m[2 * 4 + 2] * translatez;
    m[3 * 4 + 3] += m[0 * 4 + 3] * translatex + m[1 * 4 + 3] * translatey + m[2 * 4 + 3] * translatez;
}

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Adjust a 4x4 matrix to apply a rotation.
inline void RotateMatrix(GLfloat *m, GLfloat angle, GLfloat vx, GLfloat vy, GLfloat vz)
{
    GLfloat len = sqrt(vx * vx + vy * vy + vz * vz);
    if (len != 0) {
        vx /= len;
        vy /= len;
        vz /= len;
    }

    GLfloat c, s, ic;
    c = cos(angle * M_PI / 180.0);
    s = sin(angle * M_PI / 180.0);
    ic = 1.0f - c;

    GLfloat rot[16];
    rot[0 * 4 + 0] = vx * vx * ic + c;
    rot[1 * 4 + 0] = vx * vy * ic - vz * s;
    rot[2 * 4 + 0] = vx * vz * ic + vy * s;
    rot[3 * 4 + 0] = 0.0f;
    rot[0 * 4 + 1] = vy * vx * ic + vz * s;
    rot[1 * 4 + 1] = vy * vy * ic + c;
    rot[2 * 4 + 1] = vy * vz * ic - vx * s;
    rot[3 * 4 + 1] = 0.0f;
    rot[0 * 4 + 2] = vx * vz * ic - vy * s;
    rot[1 * 4 + 2] = vy * vz * ic + vx * s;
    rot[2 * 4 + 2] = vz * vz * ic + c;
    rot[3 * 4 + 2] = 0.0f;
    rot[0 * 4 + 3] = 0.0f;
    rot[1 * 4 + 3] = 0.0f;
    rot[2 * 4 + 3] = 0.0f;
    rot[3 * 4 + 3] = 1.0f;

    GLfloat temp[16];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            temp[j * 4 + i] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                temp[j * 4 + i] += m[k * 4 + i] * rot[j * 4 + k];
            }
        }
    }

    qMemCopy(m, temp, sizeof(temp));
}

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    qtLogo = true;
    createdVertices = 0;
    createdNormals = 0;
    m_vertexNumber = 0;
    frames = 0;
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoBufferSwap(false);
    m_showBubbles = true;
#ifndef Q_WS_QWS
    setMinimumSize(300, 250);
#endif
}

GLWidget::~GLWidget()
{
  if (createdVertices)
      delete[] createdVertices;
  if (createdNormals)
      delete[] createdNormals;
}

void GLWidget::setScaling(int scale) {

    if (scale > 50)
        m_fScale = 1 + qreal(scale -50) / 50 * 0.5;
    else if (scale < 50)
        m_fScale =  1- (qreal(50 - scale) / 50 * 1/2);
    else 
      m_fScale = 1;
}

void GLWidget::setLogo() {
    qtLogo = true;
}

void GLWidget::setTexture() {
    qtLogo = false;
}

void GLWidget::showBubbles(bool bubbles)
{
   m_showBubbles = bubbles;
}

void GLWidget::paintQtLogo()
{
    glDisable(GL_TEXTURE_2D);
    glVertexAttribPointer(vertexAttr1, 3, GL_FLOAT, GL_FALSE, 0, createdVertices);
    glEnableVertexAttribArray(vertexAttr1);
    glVertexAttribPointer(normalAttr1, 3, GL_FLOAT, GL_FALSE, 0, createdNormals);
    glEnableVertexAttribArray(normalAttr1);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexNumber / 3);
    glDisableVertexAttribArray(normalAttr1);
    glDisableVertexAttribArray(vertexAttr1);
}

void GLWidget::paintTexturedCube()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_uiTexture);
    GLfloat afVertices[] = {
        -0.5, 0.5, 0.5, 0.5,-0.5,0.5,-0.5,-0.5,0.5,
        0.5, -0.5, 0.5, -0.5,0.5,0.5,0.5,0.5,0.5,
        -0.5, -0.5, -0.5, 0.5,-0.5,-0.5,-0.5,0.5,-0.5,
        0.5, 0.5, -0.5, -0.5,0.5,-0.5,0.5,-0.5,-0.5,

        0.5, -0.5, -0.5, 0.5,-0.5,0.5,0.5,0.5,-0.5,
        0.5, 0.5, 0.5, 0.5,0.5,-0.5,0.5,-0.5,0.5,
        -0.5, 0.5, -0.5, -0.5,-0.5,0.5,-0.5,-0.5,-0.5,
        -0.5, -0.5, 0.5, -0.5,0.5,-0.5,-0.5,0.5,0.5,

        0.5, 0.5,  -0.5, -0.5, 0.5,  0.5,  -0.5,  0.5,  -0.5,
        -0.5,  0.5,  0.5,  0.5,  0.5,  -0.5, 0.5, 0.5,  0.5,
        -0.5,  -0.5, -0.5, -0.5, -0.5, 0.5,  0.5, -0.5, -0.5,
        0.5, -0.5, 0.5,  0.5,  -0.5, -0.5, -0.5,  -0.5, 0.5
    };
    glVertexAttribPointer(vertexAttr2, 3, GL_FLOAT, GL_FALSE, 0, afVertices);
    glEnableVertexAttribArray(vertexAttr2);

    GLfloat afTexCoord[] = {
        0.0f,0.0f, 1.0f,1.0f, 1.0f,0.0f,
        1.0f,1.0f, 0.0f,0.0f, 0.0f,1.0f,
        1.0f,1.0f, 1.0f,0.0f, 0.0f,1.0f,
        0.0f,0.0f, 0.0f,1.0f, 1.0f,0.0f,

        1.0f,1.0f, 1.0f,0.0f, 0.0f,1.0f,
        0.0f,0.0f, 0.0f,1.0f, 1.0f,0.0f,
        0.0f,0.0f, 1.0f,1.0f, 1.0f,0.0f,
        1.0f,1.0f, 0.0f,0.0f, 0.0f,1.0f,

        0.0f,1.0f, 1.0f,0.0f, 1.0f,1.0f,
        1.0f,0.0f, 0.0f,1.0f, 0.0f,0.0f,
        1.0f,0.0f, 1.0f,1.0f, 0.0f,0.0f,
        0.0f,1.0f, 0.0f,0.0f, 1.0f,1.0f
    };
    glVertexAttribPointer(texCoordAttr2, 2, GL_FLOAT, GL_FALSE, 0, afTexCoord);
    glEnableVertexAttribArray(texCoordAttr2);

    GLfloat afNormals[] = {

        0,0,-1, 0,0,-1, 0,0,-1,
        0,0,-1, 0,0,-1, 0,0,-1,
        0,0,1, 0,0,1, 0,0,1,
        0,0,1, 0,0,1, 0,0,1,

        -1,0,0, -1,0,0, -1,0,0,
        -1,0,0, -1,0,0, -1,0,0,
        1,0,0, 1,0,0, 1,0,0,
        1,0,0, 1,0,0, 1,0,0,

        0,-1,0, 0,-1,0, 0,-1,0,
        0,-1,0, 0,-1,0, 0,-1,0,
        0,1,0, 0,1,0, 0,1,0,
        0,1,0, 0,1,0, 0,1,0
    };
    glVertexAttribPointer(normalAttr2, 3, GL_FLOAT, GL_FALSE, 0, afNormals);
    glEnableVertexAttribArray(normalAttr2);

    glUniform1i(textureUniform2, 0);    // use texture unit 0

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDisableVertexAttribArray(vertexAttr2);
    glDisableVertexAttribArray(normalAttr2);
    glDisableVertexAttribArray(texCoordAttr2);
}

static void reportCompileErrors(GLuint shader, const char *src)
{
    GLint value = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &value);
    bool compiled = (value != 0);
    value = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &value);
    if (!compiled && value > 1) {
        char *log = new char [value];
        GLint len;
        glGetShaderInfoLog(shader, value, &len, log);
        qWarning("%s\n", log);
        qWarning("when compiling:\n%s\n", src);
        delete [] log;
    }
}

static void reportLinkErrors(GLuint program, const char *vsrc, const char *fsrc)
{
    GLint value = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &value);
    bool linked = (value != 0);
    value = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &value);
    if (!linked && value > 1) {
        char *log = new char [value];
        GLint len;
        glGetProgramInfoLog(program, value, &len, log);
        qWarning("%s\n", log);
        qWarning("when linking:\n%s\nwith:\n%s\n", vsrc, fsrc);
        delete [] log;
    }
}

void GLWidget::initializeGL ()
{
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &m_uiTexture);
    m_uiTexture = bindTexture(QImage(":/qt.png"));

    GLuint vshader1 = glCreateShader(GL_VERTEX_SHADER);
    const char *vsrc1[1] = {
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec3 normal;\n"
        "uniform mediump mat4 matrix;\n"
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
        "    float angle = max(dot(normal, toLight), 0.0);\n"
        "    vec3 col = vec3(0.40, 1.0, 0.0);\n"
        "    color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);\n"
        "    color = clamp(color, 0.0, 1.0);\n"
        "    gl_Position = matrix * vertex;\n"
        "}\n"
    };
    glShaderSource(vshader1, 1, vsrc1, 0);
    glCompileShader(vshader1);
    reportCompileErrors(vshader1, vsrc1[0]);

    GLuint fshader1 = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fsrc1[1] = {
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = color;\n"
        "}\n"
    };
    glShaderSource(fshader1, 1, fsrc1, 0);
    glCompileShader(fshader1);
    reportCompileErrors(fshader1, fsrc1[0]);

    program1 = glCreateProgram();
    glAttachShader(program1, vshader1);
    glAttachShader(program1, fshader1);
    glLinkProgram(program1);
    reportLinkErrors(program1, vsrc1[0], fsrc1[0]);

    vertexAttr1 = glGetAttribLocation(program1, "vertex");
    normalAttr1 = glGetAttribLocation(program1, "normal");
    matrixUniform1 = glGetUniformLocation(program1, "matrix");

    GLuint vshader2 = glCreateShader(GL_VERTEX_SHADER);
    const char *vsrc2[1] = {
        "attribute highp vec4 vertex;\n"
        "attribute highp vec4 texCoord;\n"
        "attribute mediump vec3 normal;\n"
        "uniform mediump mat4 matrix;\n"
        "varying highp vec4 texc;\n"
        "varying mediump float angle;\n"
        "void main(void)\n"
        "{\n"
        "    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
        "    angle = max(dot(normal, toLight), 0.0);\n"
        "    gl_Position = matrix * vertex;\n"
        "    texc = texCoord;\n"
        "}\n"
    };
    glShaderSource(vshader2, 1, vsrc2, 0);
    glCompileShader(vshader2);
    reportCompileErrors(vshader2, vsrc2[0]);

    GLuint fshader2 = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fsrc2[1] = {
        "varying highp vec4 texc;\n"
        "uniform sampler2D tex;\n"
        "varying mediump float angle;\n"
        "void main(void)\n"
        "{\n"
        "    highp vec3 color = texture2D(tex, texc.st).rgb;\n"
        "    color = color * 0.2 + color * 0.8 * angle;\n"
        "    gl_FragColor = vec4(clamp(color, 0.0, 1.0), 1.0);\n"
        "}\n"
    };
    glShaderSource(fshader2, 1, fsrc2, 0);
    glCompileShader(fshader2);
    reportCompileErrors(fshader2, fsrc2[0]);

    program2 = glCreateProgram();
    glAttachShader(program2, vshader2);
    glAttachShader(program2, fshader2);
    glLinkProgram(program2);
    reportLinkErrors(program2, vsrc2[0], fsrc2[0]);

    vertexAttr2 = glGetAttribLocation(program2, "vertex");
    normalAttr2 = glGetAttribLocation(program2, "normal");
    texCoordAttr2 = glGetAttribLocation(program2, "texCoord");
    matrixUniform2 = glGetUniformLocation(program2, "matrix");
    textureUniform2 = glGetUniformLocation(program2, "tex");

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    m_fAngle = 0;
    m_fScale = 1;
    createGeometry();
    createBubbles(bubbleNum - bubbles.count());
}

void GLWidget::paintGL()
{
    createBubbles(bubbleNum - bubbles.count());

    QPainter painter;
    painter.begin(this);

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    GLfloat modelview[16];
    IdentityMatrix(modelview);
    RotateMatrix(modelview, m_fAngle, 0.0, 1.0, 0.0);
    RotateMatrix(modelview, m_fAngle, 1.0, 0.0, 0.0);
    RotateMatrix(modelview, m_fAngle, 0.0, 0.0, 1.0);
    ScaleMatrix(modelview, m_fScale, m_fScale, m_fScale);
    TranslateMatrix(modelview, 0, -0.2, 0);

    if (qtLogo) {
        glUseProgram(program1);
        glUniformMatrix4fv(matrixUniform1, 1, GL_FALSE, modelview);
        paintQtLogo();
        glUseProgram(0);
    } else {
        glUseProgram(program2);
        glUniformMatrix4fv(matrixUniform2, 1, GL_FALSE, modelview);
        paintTexturedCube();
        glUseProgram(0);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    if (m_showBubbles)
        foreach (Bubble *bubble, bubbles) {
            bubble->drawBubble(&painter);
    }

    QString framesPerSecond;
    framesPerSecond.setNum(frames /(time.elapsed() / 1000.0), 'f', 2);

    painter.setPen(Qt::white);

    painter.drawText(20, 40, framesPerSecond + " fps");

    painter.end();
    
    swapBuffers();

    QMutableListIterator<Bubble*> iter(bubbles);

    while (iter.hasNext()) {
        Bubble *bubble = iter.next();
        bubble->move(rect());
    }
    if (!(frames % 100)) {
        time.start();
        frames = 0;
    }
    m_fAngle += 1.0f;
    frames ++;
}

void GLWidget::createBubbles(int number)
{
    for (int i = 0; i < number; ++i) {
        QPointF position(width()*(0.1 + (0.8*qrand()/(RAND_MAX+1.0))),
                        height()*(0.1 + (0.8*qrand()/(RAND_MAX+1.0))));
        qreal radius = qMin(width(), height())*(0.0175 + 0.0875*qrand()/(RAND_MAX+1.0));
        QPointF velocity(width()*0.0175*(-0.5 + qrand()/(RAND_MAX+1.0)),
                        height()*0.0175*(-0.5 + qrand()/(RAND_MAX+1.0)));

        bubbles.append(new Bubble(position, radius, velocity));
    }
}

void GLWidget::createGeometry()
{
    vertices.clear();
    normals.clear();

    qreal x1 = +0.06f;
    qreal y1 = -0.14f;
    qreal x2 = +0.14f;
    qreal y2 = -0.06f;
    qreal x3 = +0.08f;
    qreal y3 = +0.00f;
    qreal x4 = +0.30f;
    qreal y4 = +0.22f;

    quad(x1, y1, x2, y2, y2, x2, y1, x1);
    quad(x3, y3, x4, y4, y4, x4, y3, x3);

    extrude(x1, y1, x2, y2);
    extrude(x2, y2, y2, x2);
    extrude(y2, x2, y1, x1);
    extrude(y1, x1, x1, y1);
    extrude(x3, y3, x4, y4);
    extrude(x4, y4, y4, x4);
    extrude(y4, x4, y3, x3);

    const qreal Pi = 3.14159f;
    const int NumSectors = 100;

    for (int i = 0; i < NumSectors; ++i) {
        qreal angle1 = (i * 2 * Pi) / NumSectors;
        qreal x5 = 0.30 * sin(angle1);
        qreal y5 = 0.30 * cos(angle1);
        qreal x6 = 0.20 * sin(angle1);
        qreal y6 = 0.20 * cos(angle1);

        qreal angle2 = ((i + 1) * 2 * Pi) / NumSectors;
        qreal x7 = 0.20 * sin(angle2);
        qreal y7 = 0.20 * cos(angle2);
        qreal x8 = 0.30 * sin(angle2);
        qreal y8 = 0.30 * cos(angle2);

        quad(x5, y5, x6, y6, x7, y7, x8, y8);

        extrude(x6, y6, x7, y7);
        extrude(x8, y8, x5, y5);
    }

    m_vertexNumber = vertices.size();
    createdVertices = new GLfloat[m_vertexNumber];
    createdNormals = new GLfloat[m_vertexNumber];
    for (int i = 0;i < m_vertexNumber;i++) {
      createdVertices[i] = vertices.at(i) * 2;
      createdNormals[i] = normals.at(i);
    }
    vertices.clear();
    normals.clear();
}

void GLWidget::quad(qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3, qreal x4, qreal y4)
{
    qreal nx, ny, nz;

    vertices << x1 << y1 << -0.05f;
    vertices << x2 << y2 << -0.05f;
    vertices << x4 << y4 << -0.05f;

    vertices << x3 << y3 << -0.05f;
    vertices << x4 << y4 << -0.05f;
    vertices << x2 << y2 << -0.05f;

    CrossProduct(nx, ny, nz, x2 - x1, y2 - y1, 0, x4 - x1, y4 - y1, 0);
    Normalize(nx, ny, nz);

    normals << nx << ny << nz;
    normals << nx << ny << nz;
    normals << nx << ny << nz;

    normals << nx << ny << nz;
    normals << nx << ny << nz;
    normals << nx << ny << nz;

    vertices << x4 << y4 << 0.05f;
    vertices << x2 << y2 << 0.05f;
    vertices << x1 << y1 << 0.05f;

    vertices << x2 << y2 << 0.05f;
    vertices << x4 << y4 << 0.05f;
    vertices << x3 << y3 << 0.05f;

    CrossProduct(nx, ny, nz, x2 - x4, y2 - y4, 0, x1 - x4, y1 - y4, 0);
    Normalize(nx, ny, nz);

    normals << nx << ny << nz;
    normals << nx << ny << nz;
    normals << nx << ny << nz;

    normals << nx << ny << nz;
    normals << nx << ny << nz;
    normals << nx << ny << nz;
}

void GLWidget::extrude(qreal x1, qreal y1, qreal x2, qreal y2)
{
    qreal nx, ny, nz;

    vertices << x1 << y1 << +0.05f;
    vertices << x2 << y2 << +0.05f;
    vertices << x1 << y1 << -0.05f;

    vertices << x2 << y2 << -0.05f;
    vertices << x1 << y1 << -0.05f;
    vertices << x2 << y2 << +0.05f;

    CrossProduct(nx, ny, nz, x2 - x1, y2 - y1, 0.0f, 0.0f, 0.0f, -0.1f);
    Normalize(nx, ny, nz);

    normals << nx << ny << nz;
    normals << nx << ny << nz;
    normals << nx << ny << nz;

    normals << nx << ny << nz;
    normals << nx << ny << nz;
    normals << nx << ny << nz;
}
