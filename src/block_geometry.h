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

#ifndef BLOCK_GEOMETRY_H
#define BLOCK_GEOMETRY_H

#include <QObject>

class BlockGeometry : public QObject {
  Q_OBJECT
  Q_ENUMS(Geometry)
 public:
  enum Geometry {
    kGeometryCube,
    kGeometrySlab,
    kGeometryBed,
    kGeometryCactus,
    kGeometryCake,
    kGeometryChest,
    kGeometryDoor,
    kGeometryFence,
    kGeometryField,
    kGeometryFlow,
    kGeometryLadder,
    kGeometryLeaves,
    kGeometryPane,
    kGeometryPressurePlate,
    kGeometrySnow,
    kGeometryStairs,
    kGeometryTrack,
    kGeometryTorch
  };

 private:
  explicit BlockGeometry(QObject* parent = NULL) : QObject(parent) {}
};

#endif // BLOCK_GEOMETRY_H
