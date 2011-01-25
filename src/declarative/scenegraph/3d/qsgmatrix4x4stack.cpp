/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsgmatrix4x4stack.h"
#include "qsgmatrix4x4stack_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QSGMatrix4x4Stack
    \brief The QSGMatrix4x4Stack class manages stacks of transformation matrices in GL applications.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::enablers

    Transformation matrices are one of the basic building blocks of
    3D applications, allowing object models to be positioned, scaled,
    rotated, and projected onto the screen.

    GL systems support several standard kinds of matrices, particularly
    modelview and projection matrices.  These matrices are typically
    organized into stacks, which allow the current matrix state to be
    saved with push() and restored later with pop().

    QSGMatrix4x4Stack assists QGLPainter with the management of matrix
    stacks, providing operations to set and modify transformation
    matrices in each of the standard matrix stacks.

    In the following example, a standard orthographic projection matrix for a
    view is set via the QGLPainter::projectionMatrix() stack, and
    then a modelview matrix is set via the QGLPainter::modelViewMatrix()
    stack to scale and translate an object prior to drawing:

    \code
    QGLPainter painter(this);

    QMatrix4x4 projm;
    projm.ortho(window->rect());
    painter.projectionMatrix() = projm;

    painter.modelViewMatrix().setToIdentity();
    painter.modelViewMatrix().translate(-1.0f, 2.0f, 0.0f);
    painter.modelViewMatrix().scale(0.5f);
    \endcode

    Later, the application can save the current modelview matrix state
    and draw a different object with a different modelview matrix:

    \code
    painter.modelViewMatrix().push();
    painter.modelViewMatrix().setToIdentity();
    painter.modelViewMatrix().scale(2.0f);
    \endcode

    For efficiency, the matrix values are kept client-side until they
    are needed by a QGLPainter::draw() operation.  Until then, changes
    to the matrix will not be reflected in the GL server.  The application
    can force the GL server to update the server with a call to
    QGLPainter::update().

    QSGMatrix4x4Stack is supported on all GL platforms, including OpenGL/ES 2.0
    which doesn't support matrix stacks natively.  On that platform, the
    matrix stack is simulated in client memory.  When the application
    selects a shader program to draw under OpenGL/ES 2.0, it calls
    top() to obtain the actual value to be set on the shader program.

    \sa QGLPainter
*/

/*!
    Creates a matrix stack.
*/
QSGMatrix4x4Stack::QSGMatrix4x4Stack()
    : d_ptr(new QSGMatrix4x4StackPrivate)
{
}

/*!
    Destroy this matrix stack.
*/
QSGMatrix4x4Stack::~QSGMatrix4x4Stack()
{
}

/*!
    Pushes the current matrix onto the matrix stack.  The matrix can
    be restored with pop().  The new top of stack will have the
    same value as the previous top of stack.

    The depths of the traditional \c{GL_MODELVIEW} and \c{GL_PROJECTION}
    matrix stacks in the GL server are system-dependent and easy to
    overflow in nested rendering code using \c{glPushMatrix()}.
    By contrast, the push() function provides an arbitrary-sized stack
    in client memory.

    \sa pop(), top()
*/
void QSGMatrix4x4Stack::push()
{
    Q_D(QSGMatrix4x4Stack);
    d->stack.push(d->matrix);
}

/*!
    Pops the top-most matrix from this matrix stack and sets the
    current matrix to the next value down.  Does nothing if the
    matrix stack contains a single entry.

    \sa push()
*/
void QSGMatrix4x4Stack::pop()
{
    Q_D(QSGMatrix4x4Stack);
    if (!d->stack.isEmpty())
        d->matrix = d->stack.pop();
    d->isDirty = true;
}

/*!
    Set the matrix at the top of this matrix stack to the identity matrix.

    \sa operator=()
*/
void QSGMatrix4x4Stack::setToIdentity()
{
    Q_D(QSGMatrix4x4Stack);
    d->matrix.setToIdentity();
    d->isDirty = true;
}

/*!
    Returns a const reference to the current matrix at the top of this
    matrix stack.  This is typically used to fetch the matrix so it can
    be set on user-defined shader programs.

    \sa operator=()
*/
const QMatrix4x4 &QSGMatrix4x4Stack::top() const
{
    Q_D(const QSGMatrix4x4Stack);
    return d->matrix;
}

/*!
    \fn QSGMatrix4x4Stack::operator const QMatrix4x4 &() const

    Returns a const reference to the current matrix at the top of
    this matrix stack.

    \sa top()
*/

/*!
    Assigns \a matrix to the matrix at the top of this matrix stack.

    \sa top()
*/
QSGMatrix4x4Stack& QSGMatrix4x4Stack::operator=(const QMatrix4x4& matrix)
{
    Q_D(QSGMatrix4x4Stack);
    d->matrix = matrix;
    d->isDirty = true;
    return *this;
}

/*!
    Multiplies the matrix at the top of this matrix stack by \a matrix.

    \sa top()
*/
QSGMatrix4x4Stack& QSGMatrix4x4Stack::operator*=(const QMatrix4x4& matrix)
{
    Q_D(QSGMatrix4x4Stack);
    d->matrix *= matrix;
    d->isDirty = true;
    return *this;
}

/*!
    Multiplies the current matrix at the top of this matrix stack by another
    that translates coordinates by (\a x, \a y, \a z).  The following example
    translates the modelview matrix by (1, -3, 0):

    \code
    QGLPainter painter(this);
    painter.modelViewMatrix().translate(1.0f, -3.0f, 0.0f);
    \endcode

    \sa scale(), rotate()
*/
void QSGMatrix4x4Stack::translate(qreal x, qreal y, qreal z)
{
    Q_D(QSGMatrix4x4Stack);
    d->matrix.translate(x, y, z);
    d->isDirty = true;
}

/*!
    Multiplies the current matrix at the top of this matrix statck by another
    that translates coordinates by the components of \a vector.

    \sa scale(), rotate()
*/
void QSGMatrix4x4Stack::translate(const QVector3D& vector)
{
    Q_D(QSGMatrix4x4Stack);
    d->matrix.translate(vector);
    d->isDirty = true;
}

/*!
    Multiplies the current matrix at the top of this matrix stack by another
    that scales coordinates by the components \a x, \a y, and \a z.
    The following example scales the modelview matrix by (1, 2, 1):

    \code
    QGLPainter painter(this);
    painter.modelViewMatrix().scale(1.0f, 2.0f, 1.0f);
    \endcode

    \sa translate(), rotate()
*/
void QSGMatrix4x4Stack::scale(qreal x, qreal y, qreal z)
{
    Q_D(QSGMatrix4x4Stack);
    d->matrix.scale(x, y, z);
    d->isDirty = true;
}

/*!
    Multiplies the current matrix at the top of this matrix stack by another
    that scales coordinates by the given \a factor.  The following example
    scales the modelview matrix by a factor of 2:

    \code
    QGLPainter painter(this);
    painter.modelViewMatrix().scale(2.0f);
    \endcode

    \sa translate(), rotate()
*/
void QSGMatrix4x4Stack::scale(qreal factor)
{
    Q_D(QSGMatrix4x4Stack);
    d->matrix.scale(factor);
    d->isDirty = true;
}

/*!
    Multiplies the current matrix at the top of this matrix stack by another
    that scales coordinates by the components of \a vector.

    \sa translate(), rotate()
*/
void QSGMatrix4x4Stack::scale(const QVector3D& vector)
{
    Q_D(QSGMatrix4x4Stack);
    d->matrix.scale(vector);
    d->isDirty = true;
}

/*!
    Multiplies the current matrix at the top of this matrix stack by another
    that rotates coordinates through \a angle degrees about the vector
    (\a x, \a y, \a z).  The following example rotates the modelview
    matrix by 45 degress about the vector (1, -3, 0):

    \code
    QGLPainter painter(this);
    painter.modelViewMatrix().rotate(45.0f, 1.0f, -3.0f, 0.0f);
    \endcode

    \sa scale(), translate()
*/
void QSGMatrix4x4Stack::rotate(qreal angle, qreal x, qreal y, qreal z)
{
    Q_D(QSGMatrix4x4Stack);
    d->matrix.rotate(angle, x, y, z);
    d->isDirty = true;
}

/*!
    Multiplies the current matrix at the top of this matrix stack by another
    that rotates coordinates through \a angle degrees about \a vector.

    \sa scale(), translate()
*/
void QSGMatrix4x4Stack::rotate(qreal angle, const QVector3D& vector)
{
    Q_D(QSGMatrix4x4Stack);
    d->matrix.rotate(angle, vector);
    d->isDirty = true;
}

/*!
    Multiplies the current matrix at the top of this matrix stack by the
    \a quaternion.  Thus \c {painter->modelViewMatrix().rotate(quaternion)}
    is equivalent to the following code:
    \code
    QMatrix4x4 mat;
    mat.rotate(quaternion);
    painter->modelViewMatrix() *= mat;
    \endcode
    which rotates coordinates according to the given \a quaternion.

    \sa scale(), translate()
*/
void QSGMatrix4x4Stack::rotate(const QQuaternion &quaternion)
{
    Q_D(QSGMatrix4x4Stack);
    d->matrix.rotate(quaternion);
    d->isDirty = true;
}

/*!
    Returns true if the top of this matrix stack has been modified;
    false otherwise.

    \sa setDirty()
*/
bool QSGMatrix4x4Stack::isDirty() const
{
    Q_D(const QSGMatrix4x4Stack);
    return d->isDirty;
}

/*!
    Sets the \a dirty flag on this matrix stack, which indicates
    if it has been modified.

    A matrix stack may also be set to dirty by translate(),
    scale(), operator*(), etc.

    \sa isDirty()
*/
void QSGMatrix4x4Stack::setDirty(bool dirty)
{
    Q_D(QSGMatrix4x4Stack);
    d->isDirty = dirty;
}

QT_END_NAMESPACE
