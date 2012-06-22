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

#ifndef CAMERA_H
#define CAMERA_H

#include <QtOpenGL>

#include <QVector3D>

class CameraInterface {
 public:
  virtual void rotateX(GLfloat delta) = 0;
  virtual void rotateY(GLfloat delta) = 0;
  virtual void translate(const QVector3D vector) = 0;
  virtual void translateWorld(const QVector3D vector) = 0;
  virtual void applyRotation() = 0;
  virtual void apply() = 0;
};

#endif // CAMERA_H
