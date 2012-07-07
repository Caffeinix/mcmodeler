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

/**
  * Represents the faces of a rectangular prism, in the order OpenGL expects them to be supplied.
  */
enum Face {
  kFrontFace = 0,
  kBackFace = 1,
  kBottomFace = 2,
  kRightFace = 3,
  kTopFace = 4,
  kLeftFace = 5
};

/**
  * Represents the four corners of a quad in counter-clockwise order.
  */
enum Corner {
  kBottomLeftCorner = 0,
  kBottomRightCorner = 1,
  kTopRightCorner = 2,
  kTopLeftCorner = 3
};

/**
  * Represents all the known geometries that a block can have.
  */
enum BlockGeometry {
  kBlockGeometryBed,
  kBlockGeometryCactus,
  kBlockGeometryCake1,
  kBlockGeometryCake2,
  kBlockGeometryCake3,
  kBlockGeometryCake4,
  kBlockGeometryCake5,
  kBlockGeometryCake6,
  kBlockGeometryChest,
  kBlockGeometryCube,
  kBlockGeometryDoor,
  kBlockGeometryFence,
  kBlockGeometryField,
  kBlockGeometryLadder,
  kBlockGeometryPressurePlate,
  kBlockGeometrySlab,
  kBlockGeometrySnow,
  kBlockGeometryStairs,
  kBlockGeometryTorchOnFloor,
  kBlockGeometryTorchOnWall
};

#endif // ENUMS_H
