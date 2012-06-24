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

#ifndef ENUMS_H
#define ENUMS_H

enum Face {
  kFrontFace = 0,
  kBackFace = 1,
  kBottomFace = 2,
  kRightFace = 3,
  kTopFace = 4,
  kLeftFace = 5
};

enum Corner {
  kBottomLeftCorner = 0,
  kBottomRightCorner = 1,
  kTopRightCorner = 2,
  kTopLeftCorner = 3
};

enum BlockGeometry {
  kBlockGeometryBed,
  kBlockGeometryCactus,
  kBlockGeometryCake1,
  kBlockGeometryCake2,
  kBlockGeometryCake3,
  kBlockGeometryCake4,
  kBlockGeometryCake5,
  kBlockGeometryCake6,
  kBlockGeometryCube,
  kBlockGeometryDoor,
  kBlockGeometryFence,
  kBlockGeometryField,
  kBlockGeometrySlab,
  kBlockGeometryChest,
  kBlockGeometryPressurePlate,
  kBlockGeometryStairs,
  kBlockGeometryTorchOnFloor,
  kBlockGeometryTorchOnWall
};

#endif // ENUMS_H
