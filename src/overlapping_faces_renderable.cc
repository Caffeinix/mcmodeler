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

#include "overlapping_faces_renderable.h"

#include <QVector>

#include "enums.h"
#include "render_delegate.h"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

OverlappingFacesRenderable::OverlappingFacesRenderable(const QVector3D& size, const QVector3D& overlap)
    : RectangularPrismRenderable(size), overlap_(overlap) {
}

OverlappingFacesRenderable::~OverlappingFacesRenderable() {}

void OverlappingFacesRenderable::addGeometry(const Geometry& geometry, const TextureCoords& texture_coords) {
  QVector3D olap = overlap();
  // add the front face
  addQuad(geometry[0][kBottomLeftCorner] + QVector3D(0, 0, -olap.z()),
          geometry[0][kBottomRightCorner] + QVector3D(0, 0, -olap.z()),
          geometry[0][kTopRightCorner] + QVector3D(0, 0, -olap.z()),
          geometry[0][kTopLeftCorner] + QVector3D(0, 0, -olap.z()), texture_coords.at(0));

  // add the back face
  addQuad(geometry[1][kBottomRightCorner] + QVector3D(0, 0, olap.z()),
          geometry[1][kBottomLeftCorner] + QVector3D(0, 0, olap.z()),
          geometry[1][kTopLeftCorner] + QVector3D(0, 0, olap.z()),
          geometry[1][kTopRightCorner] + QVector3D(0, 0, olap.z()), texture_coords.at(1));

  // add the sides
  addQuad(geometry[1][kBottomLeftCorner] + QVector3D(0, olap.y(), 0),
          geometry[1][kBottomRightCorner] + QVector3D(0, olap.y(), 0),
          geometry[0][kBottomRightCorner] + QVector3D(0, olap.y(), 0),
          geometry[0][kBottomLeftCorner] + QVector3D(0, olap.y(), 0),
          texture_coords.at(2)); // Bottom
  addQuad(geometry[1][kBottomRightCorner] + QVector3D(-olap.x(), 0, 0),
          geometry[1][kTopRightCorner] + QVector3D(-olap.x(), 0, 0),
          geometry[0][kTopRightCorner] + QVector3D(-olap.x(), 0, 0),
          geometry[0][kBottomRightCorner] + QVector3D(-olap.x(), 0, 0),
          texture_coords.at(3));      // Right
  addQuad(geometry[1][kTopRightCorner] + QVector3D(0, -olap.y(), 0),
          geometry[1][kTopLeftCorner] + QVector3D(0, -olap.y(), 0),
          geometry[0][kTopLeftCorner] + QVector3D(0, -olap.y(), 0),
          geometry[0][kTopRightCorner] + QVector3D(0, -olap.y(), 0),
          texture_coords.at(4));                    // Top
  addQuad(geometry[1][kTopLeftCorner] + QVector3D(olap.x(), 0, 0),
          geometry[1][kBottomLeftCorner] + QVector3D(olap.x(), 0, 0),
          geometry[0][kBottomLeftCorner] + QVector3D(olap.x(), 0, 0),
          geometry[0][kTopLeftCorner] + QVector3D(olap.x(), 0, 0),
          texture_coords.at(5));           // Left
}
