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

#include "mouselook_cam.h"

MouselookCam::MouselookCam() : x_rotation_(0), y_rotation_(0) {
}

void MouselookCam::rotateX(GLfloat delta) {
  x_rotation_ -= delta;
}

void MouselookCam::rotateY(GLfloat delta) {
  y_rotation_ -= delta;
}

void MouselookCam::translate(const QVector3D vector) {
  position_ -= cameraMatrix().transformed(vector);
}

void MouselookCam::translateWorld(const QVector3D vector) {
  position_ -= vector;
}

Matrix MouselookCam::cameraMatrix() {
  Matrix ret = Matrix::identityMatrix();
  ret = ret.postMultipliedBy(Matrix::translationMatrix(position_));
  ret = ret.postMultipliedBy(Matrix::rotationMatrix(QVector3D(0, 1, 0), x_rotation_));
  ret = ret.postMultipliedBy(Matrix::rotationMatrix(QVector3D(1, 0, 0), y_rotation_));
  return ret;
}

void MouselookCam::applyRotation() {
  glLoadMatrixf(cameraMatrix().rotationComponent().data());
}

void MouselookCam::apply() {
  glLoadMatrixf(cameraMatrix().data());
}

QString MouselookCam::debugMatrix() {
  Matrix m = cameraMatrix();
  static const int field_width = 6;
  static const int precision = 3;
  return QString("%1 %5 %9 %13\n"
                 "%2 %6 %10 %14\n"
                 "%3 %7 %11 %15\n"
                 "%4 %8 %12 %16")
      .arg(m.data()[0], field_width, 'f', precision).arg(m.data()[1], field_width, 'f', precision)
      .arg(m.data()[2], field_width, 'f', precision).arg(m.data()[3], field_width, 'f', precision)
      .arg(m.data()[4], field_width, 'f', precision).arg(m.data()[5], field_width, 'f', precision)
      .arg(m.data()[6], field_width, 'f', precision).arg(m.data()[7], field_width, 'f', precision)
      .arg(m.data()[8], field_width, 'f', precision).arg(m.data()[9], field_width, 'f', precision)
      .arg(m.data()[10], field_width, 'f', precision).arg(m.data()[11], field_width, 'f', precision)
      .arg(m.data()[12], field_width, 'f', precision).arg(m.data()[13], field_width, 'f', precision)
      .arg(m.data()[14], field_width, 'f', precision).arg(m.data()[15], field_width, 'f', precision);
}
