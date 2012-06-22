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

#include <math.h>

#include "matrix.h"

Matrix Matrix::identityMatrix() {
  return Matrix();
}

Matrix Matrix::rotationMatrix(QVector3D axis, float angle) {
  const GLfloat x = axis.x();
  const GLfloat y = axis.y();
  const GLfloat z = axis.z();
  const GLfloat c = cosf(angle);
  const GLfloat s = sinf(angle);
  const GLfloat t = 1 - c;

  Matrix m;
  m.data_[0] = t * x * x + c;
  m.data_[4] = t * x * y - s * z;
  m.data_[8] = t * x * z + s * y;
  m.data_[12] = 0;

  m.data_[1] = t * x * y + s * z;
  m.data_[5] = t * y * y + c;
  m.data_[9] = t * y * z - s * x;
  m.data_[13] = 0;

  m.data_[2] = t * x * z - s * y;
  m.data_[6] = t * y * z + s * x;
  m.data_[10] = t * z * z + c;
  m.data_[14] = 0;

  m.data_[3] = 0;
  m.data_[7] = 0;
  m.data_[11] = 0;
  m.data_[15] = 1;
  return m;
}

Matrix Matrix::translationMatrix(QVector3D vector) {
  Matrix m;
  m.data_[12] = vector.x();
  m.data_[13] = vector.y();
  m.data_[14] = vector.z();
  return m;
}

Matrix::Matrix() {
  data_[0] = 1; data_[4] = 0; data_[8] = 0;  data_[12] = 0;
  data_[1] = 0; data_[5] = 1; data_[9] = 0;  data_[13] = 0;
  data_[2] = 0; data_[6] = 0; data_[10] = 1; data_[14] = 0;
  data_[3] = 0; data_[7] = 0; data_[11] = 0; data_[15] = 1;
}

Matrix Matrix::translationComponent() const {
  Matrix m;
  m.data_[0] = 1; m.data_[4] = 0; m.data_[8] = 0;  m.data_[12] = data_[12];
  m.data_[1] = 0; m.data_[5] = 1; m.data_[9] = 0;  m.data_[13] = data_[13];
  m.data_[2] = 0; m.data_[6] = 0; m.data_[10] = 1; m.data_[14] = data_[14];
  m.data_[3] = 0; m.data_[7] = 0; m.data_[11] = 0; m.data_[15] = 1;
  return m;
}

Matrix Matrix::rotationComponent() const {
  Matrix m;
  m.data_[0] = data_[0]; m.data_[4] = data_[4]; m.data_[8] = data_[8];   m.data_[12] = 0;
  m.data_[1] = data_[1]; m.data_[5] = data_[5]; m.data_[9] = data_[9];   m.data_[13] = 0;
  m.data_[2] = data_[2]; m.data_[6] = data_[6]; m.data_[10] = data_[10]; m.data_[14] = 0;
  m.data_[3] = 0; m.data_[7] = 0; m.data_[11] = 0; m.data_[15] = 1;
  return m;
}

Matrix Matrix::postMultipliedBy(const Matrix& m2) const {
  Matrix out;
  for (int row = 0; row < 4; ++row)
    for (int col = 0; col < 4; ++col) {
    GLfloat sum = 0;
    for (int i = 0; i < 4; ++i) {
      sum += data_[col * 4 + i] * m2.data_[i * 4 + row];
    }
    out.data_[4 * col + row] = sum;
  }
  return out;
}

Matrix Matrix::inverted(bool* invertable) const {
  int rows = 4;
  int cols = 4;
  Matrix rhs;
  Matrix lhs = *this;
  GLfloat temp;
  // Down
  for (int i = 0; i < rows; ++i) {
    // Pivoting
    int pivot = i;
    for (int j = i; j < rows; ++j) {
      if (qAbs(lhs.value(j, i)) > lhs.value(pivot, i))
        pivot = j;
    }
    // TODO: fuzzy compare.
    if (lhs.value(pivot, i) == 0) {
      if (invertable)
        *invertable = false;
      return rhs;
    }
    if (pivot != i) {
      for (int j = i; j < cols; ++j) {
        temp = lhs.value(pivot, j);
        lhs.setValue(pivot, j, lhs.value(i, j));
        lhs.setValue(i, j, temp);
      }
      for (int j = 0; j < cols; ++j) {
        temp = rhs.value(pivot, j);
        rhs.setValue(pivot, j, rhs.value(i, j));
        rhs.setValue(i, j, temp);
      }
    }

    // Normalize i-th row
    rhs.setRow(i, rhs.row(i) / lhs.value(i, i));
    for (int j = cols - 1; j > i; --j)
      lhs.setValue(i, j, lhs.value(i, j) / lhs.value(i, i));

    // Eliminate non-zeros in i-th column below the i-th row.
    for (int j = i + 1; j < rows; ++j) {
      rhs.setRow(j, rhs.row(j) - lhs.value(j, i) * rhs.row(i));
      for (int k = i + 1; k < cols; ++k)
        lhs.setValue(j, k, lhs.value(j, k) - lhs.value(j, i) * lhs.value(i, k));
    }
  }
  // Up
  for (int i = rows - 1; i > 0; --i) {
    for (int j = i - 1; j >= 0; --j)
      rhs.setRow(j, rhs.row(j) - lhs.value(j, i) * rhs.row(i));
  }
  if (invertable)
    *invertable = true;
  return rhs;
}
