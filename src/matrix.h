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

class Matrix {
public:
  static Matrix identityMatrix();
  static Matrix rotationMatrix(QVector3D axis, float angle);
  static Matrix translationMatrix(QVector3D vector);

  Matrix();

  Matrix translationComponent() const;
  Matrix rotationComponent() const;
  Matrix postMultipliedBy(const Matrix& m2) const;
  Matrix inverted(bool* invertable = NULL) const;

  QVector3D transformed(QVector3D vec) {
    QVector3D ret;
    ret.setX(vec.x() * value(0, 0) + vec.y() * value(1, 0) + vec.z() * value(2, 0) + value(3, 0));
    ret.setY(vec.x() * value(0, 1) + vec.y() * value(1, 1) + vec.z() * value(2, 1) + value(3, 1));
    ret.setZ(vec.x() * value(0, 2) + vec.y() * value(1, 2) + vec.z() * value(2, 2) + value(3, 2));
    return ret;
  }

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
