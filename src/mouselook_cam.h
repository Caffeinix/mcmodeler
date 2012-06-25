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

#ifndef MOUSELOOK_CAM_H
#define MOUSELOOK_CAM_H

#include <QVector3D>

#include "camera.h"
#include "matrix.h"

/**
  * Concrete camera implementation implementing mouselook.  This means the camera will rotate and translate, but will
  * avoid introducing roll even if the camera is made to look straight up or down.  And it does it without quaternions,
  * because I have no idea how quaternions work.
  */
class MouselookCam : public CameraInterface {
 public:
  MouselookCam();
  virtual void rotateX(GLfloat delta);
  virtual void rotateY(GLfloat delta);
  virtual void translate(const QVector3D vector);
  virtual void translateWorld(const QVector3D vector);

  virtual void applyRotation();
  virtual void apply();

  QString debugMatrix();
 private:
  Matrix cameraMatrix();
  QVector3D position_;
  float x_rotation_;
  float y_rotation_;
};

#endif // MOUSELOOK_CAM_H
