/* Copyright 2012 Brian Ellis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <QtOpenGL>
#include <QVector3D>
#include <QVector4D>

/**
  * A generic 4x4 matrix class for doing 3D things.  Shockingly, Qt does not have one.
  */
class Matrix {
public:
  /**
    * Returns an identity matrix.
    */
  static Matrix identityMatrix();

  /**
    * Returns a matrix that will perform a rotation of \p angle radians around the unit vector \p axis.
    */
  static Matrix rotationMatrix(QVector3D axis, float angle);

  /**
    * Returns a matrix that will perform a translation along \p vector.
    */
  static Matrix translationMatrix(QVector3D vector);

  /**
    * Creates a matrix.  Its initial state is an identity matrix, making this identical to identityMatrix().
    */
  Matrix();

  /**
    * Returns a Matrix that performs the same translation as this Matrix, but without the rotation component.
    */
  Matrix translationComponent() const;

  /**
    * Returns a Matrix that performs the same rotation as this Matrix, but without the translation component.
    */
  Matrix rotationComponent() const;

  /**
    * Post-multiplies this matrix by \p m2.
    */
  Matrix postMultipliedBy(const Matrix& m2) const;

  /**
    * Attempts to return this matrix's inverted counterpart.
    * @param[out] invertable If not NULL, will be set to \c true if the matrix could be inverted, \c false otherwise.
    */
  Matrix inverted(bool* invertable = NULL) const;

  /**
    * Returns a copy of \p vec with its coordinates transformed by this Matrix.
    */
  QVector3D transformed(QVector3D vec) {
    QVector3D ret;
    ret.setX(vec.x() * value(0, 0) + vec.y() * value(1, 0) + vec.z() * value(2, 0) + value(3, 0));
    ret.setY(vec.x() * value(0, 1) + vec.y() * value(1, 1) + vec.z() * value(2, 1) + value(3, 1));
    ret.setZ(vec.x() * value(0, 2) + vec.y() * value(1, 2) + vec.z() * value(2, 2) + value(3, 2));
    return ret;
  }

  /**
    * Returns an array of GLfloats representing this Matrix.
    */
  const GLfloat* data() const {
    return data_;
  }

private:
  void setData(int i, GLfloat data) {
    data_[i] = data;
  }
  const GLfloat value(int row, int col) const {
    return data_[row + 4 * col];
  }
  void setValue(int row, int col, GLfloat data) {
    data_[row + 4 * col] = data;
  }
  QVector4D row(int row) const {
    QVector4D vec;
    vec.setX(data_[row]);
    vec.setY(data_[4 + row]);
    vec.setZ(data_[8 + row]);
    vec.setW(data_[12 + row]);
    return vec;
  }
  void setRow(int row, QVector4D vec) {
    data_[row] = vec.x();
    data_[4 + row] = vec.y();
    data_[8 + row] = vec.z();
    data_[12 + row] = vec.w();
  }

  GLfloat data_[16];
};

#endif // MATRIX_H
