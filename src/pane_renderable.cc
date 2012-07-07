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

#include "pane_renderable.h"
#include "block_orientation.h"

PaneRenderable::PaneRenderable(const QVector3D& size) : RectangularPrismRenderable(size) {
}

PaneRenderable::~PaneRenderable() {}

PaneRenderable::Geometry PaneRenderable::createGeometry() {
  Geometry geometry;

  // Full width pane:
  const QVector3D& sz = size();
  QVector<QVector3D> verts;
  verts.resize(4);
  verts[kBottomRightCorner].setX(sz.x());
  verts[kTopRightCorner].setX(sz.x());
  verts[kTopRightCorner].setY(sz.y());
  verts[kTopLeftCorner].setY(sz.y());

  // back face - "extrude" verts down
  QVector<QVector3D> back(verts);
  for (int i = 0; i < 4; ++i) {
    back[i].setZ(-sz.z());
  }
  geometry << verts << back;

  // South half pane:
  verts.clear();
  verts.resize(4);
  verts[kBottomRightCorner].setX(-sz.z());
  verts[kTopRightCorner].setX(-sz.z());
  verts[kTopRightCorner].setY(sz.y());
  verts[kTopLeftCorner].setY(sz.y());
  back = verts;
  for (int i = 0; i < 4; ++i) {
    back[i].setZ(-sz.x() / 2);
  }
  // Need to reverse back and verts because my 3D visualization skills suck, so I did it backwards and can't figure out
  // how else to fix it so the normals and texture coordinates map the right way.
  geometry << back << verts;

  // North half pane: just re-render the south half and we'll move it differently in moveToOrigin().
  geometry << back << verts;

  // West half pane:
  verts.clear();
  verts.resize(4);
  verts[kBottomRightCorner].setX(sz.x() / 2);
  verts[kTopRightCorner].setX(sz.x() / 2);
  verts[kTopRightCorner].setY(sz.y());
  verts[kTopLeftCorner].setY(sz.y());
  back = verts;
  for (int i = 0; i < 4; ++i) {
    back[i].setZ(-sz.z());
  }
  geometry << verts << back;

  // East half pane:
  verts.clear();
  verts.resize(4);
  verts[kBottomLeftCorner].setX(sz.x() / 2);
  verts[kBottomRightCorner].setX(sz.x());
  verts[kTopLeftCorner].setX(sz.x() / 2);
  verts[kTopRightCorner].setX(sz.x());
  verts[kTopRightCorner].setY(sz.y());
  verts[kTopLeftCorner].setY(sz.y());
  back = verts;
  for (int i = 0; i < 4; ++i) {
    back[i].setZ(-sz.z());
  }
  geometry << verts << back;

  return geometry;
}

PaneRenderable::Geometry PaneRenderable::moveToOrigin(const PaneRenderable::Geometry& geometry) {
  const QVector3D& sz = size();
  Geometry out_geometry = geometry;
  for (int i = 0; i < 4; ++i) {
    // Full width pane:
    out_geometry[0][i] -= QVector3D(sz.x() / 2.0f, 0.5f, -sz.z() / 2.0f);
    out_geometry[1][i] -= QVector3D(sz.x() / 2.0f, 0.5f, -sz.z() / 2.0f);

    // South half pane:
    out_geometry[2][i] -= QVector3D(-sz.z() / 2.0f, 0.5f, -sz.x() / 2.0f);
    out_geometry[3][i] -= QVector3D(-sz.z() / 2.0f, 0.5f, -sz.x() / 2.0f);

    // North half pane:
    out_geometry[4][i] -= QVector3D(-sz.z() / 2.0f, 0.5f, 0);
    out_geometry[5][i] -= QVector3D(-sz.z() / 2.0f, 0.5f, 0);

    // West half pane:
    out_geometry[6][i] -= QVector3D(sz.x() / 2.0f, 0.5f, -sz.z() / 2.0f);
    out_geometry[7][i] -= QVector3D(sz.x() / 2.0f, 0.5f, -sz.z() / 2.0f);

    // East half pane:
    out_geometry[8][i] -= QVector3D(sz.x() / 2.0f, 0.5f, -sz.z() / 2.0f);
    out_geometry[9][i] -= QVector3D(sz.x() / 2.0f, 0.5f, -sz.z() / 2.0f);
  }
  return out_geometry;
}

PaneRenderable::TextureCoords PaneRenderable::createTextureCoords(const Geometry& geometry) {
  TextureCoords coords;
  coords << RectangularPrismRenderable::createTextureCoords(geometry);
  coords << RectangularPrismRenderable::createTextureCoords(geometry.mid(2));
  coords << RectangularPrismRenderable::createTextureCoords(geometry.mid(4));
  coords << RectangularPrismRenderable::createTextureCoords(geometry.mid(6));
  coords << RectangularPrismRenderable::createTextureCoords(geometry.mid(8));
  return coords;
}

void PaneRenderable::addGeometry(const PaneRenderable::Geometry& geometry,
                                 const PaneRenderable::TextureCoords& texture_coords) {
  // add the front face
  addQuad(geometry[0][kBottomLeftCorner], geometry[0][kBottomRightCorner],
          geometry[0][kTopRightCorner], geometry[0][kTopLeftCorner], texture_coords.at(0));

  // add the back face
  addQuad(geometry[1][kBottomRightCorner], geometry[1][kBottomLeftCorner],
          geometry[1][kTopLeftCorner], geometry[1][kTopRightCorner], texture_coords.at(1));

  // add the sides
  addQuad(geometry[1][kBottomLeftCorner], geometry[1][kBottomRightCorner],
          geometry[0][kBottomRightCorner], geometry[0][kBottomLeftCorner], texture_coords.at(2));  // Bottom
  addQuad(geometry[1][kBottomRightCorner], geometry[1][kTopRightCorner],
          geometry[0][kTopRightCorner], geometry[0][kBottomRightCorner], texture_coords.at(3));    // Right
  addQuad(geometry[1][kTopRightCorner], geometry[1][kTopLeftCorner],
          geometry[0][kTopLeftCorner], geometry[0][kTopRightCorner], texture_coords.at(4));        // Top
  addQuad(geometry[1][kTopLeftCorner], geometry[1][kBottomLeftCorner],
          geometry[0][kBottomLeftCorner], geometry[0][kTopLeftCorner], texture_coords.at(5));      // Left


  // add the front face
  addQuad(geometry[2][kBottomLeftCorner], geometry[2][kBottomRightCorner],
          geometry[2][kTopRightCorner], geometry[2][kTopLeftCorner], texture_coords.at(0 + 6));

  // add the back face
  addQuad(geometry[3][kBottomRightCorner], geometry[3][kBottomLeftCorner],
          geometry[3][kTopLeftCorner], geometry[3][kTopRightCorner], texture_coords.at(0 + 7));

  // add the sides
  addQuad(geometry[3][kBottomLeftCorner], geometry[3][kBottomRightCorner],
          geometry[2][kBottomRightCorner], geometry[2][kBottomLeftCorner], texture_coords.at(0 + 8));  // Bottom
  addQuad(geometry[3][kBottomRightCorner], geometry[3][kTopRightCorner],
          geometry[2][kTopRightCorner], geometry[2][kBottomRightCorner], texture_coords.at(0 + 9));    // Right
  addQuad(geometry[3][kTopRightCorner], geometry[3][kTopLeftCorner],
          geometry[2][kTopLeftCorner], geometry[2][kTopRightCorner], texture_coords.at(0 + 10));        // Top
  addQuad(geometry[3][kTopLeftCorner], geometry[3][kBottomLeftCorner],
          geometry[2][kBottomLeftCorner], geometry[2][kTopLeftCorner], texture_coords.at(0 + 11));      // Left


  // add the front face
  addQuad(geometry[4][kBottomLeftCorner], geometry[4][kBottomRightCorner],
          geometry[4][kTopRightCorner], geometry[4][kTopLeftCorner], texture_coords.at(6 + 6));

  // add the back face
  addQuad(geometry[5][kBottomRightCorner], geometry[5][kBottomLeftCorner],
          geometry[5][kTopLeftCorner], geometry[5][kTopRightCorner], texture_coords.at(6 + 7));

  // add the sides
  addQuad(geometry[5][kBottomLeftCorner], geometry[5][kBottomRightCorner],
          geometry[4][kBottomRightCorner], geometry[4][kBottomLeftCorner], texture_coords.at(6 + 8));  // Bottom
  addQuad(geometry[5][kBottomRightCorner], geometry[5][kTopRightCorner],
          geometry[4][kTopRightCorner], geometry[4][kBottomRightCorner], texture_coords.at(6 + 9));    // Right
  addQuad(geometry[5][kTopRightCorner], geometry[5][kTopLeftCorner],
          geometry[4][kTopLeftCorner], geometry[4][kTopRightCorner], texture_coords.at(6 + 10));        // Top
  addQuad(geometry[5][kTopLeftCorner], geometry[5][kBottomLeftCorner],
          geometry[4][kBottomLeftCorner], geometry[4][kTopLeftCorner], texture_coords.at(6 + 11));      // Left


  // add the front face
  addQuad(geometry[6][kBottomLeftCorner], geometry[6][kBottomRightCorner],
          geometry[6][kTopRightCorner], geometry[6][kTopLeftCorner], texture_coords.at(12 + 6));

  // add the back face
  addQuad(geometry[7][kBottomRightCorner], geometry[7][kBottomLeftCorner],
          geometry[7][kTopLeftCorner], geometry[7][kTopRightCorner], texture_coords.at(12 + 7));

  // add the sides
  addQuad(geometry[7][kBottomLeftCorner], geometry[7][kBottomRightCorner],
          geometry[6][kBottomRightCorner], geometry[6][kBottomLeftCorner], texture_coords.at(12 + 8));  // Bottom
  addQuad(geometry[7][kBottomRightCorner], geometry[7][kTopRightCorner],
          geometry[6][kTopRightCorner], geometry[6][kBottomRightCorner], texture_coords.at(12 + 9));    // Right
  addQuad(geometry[7][kTopRightCorner], geometry[7][kTopLeftCorner],
          geometry[6][kTopLeftCorner], geometry[6][kTopRightCorner], texture_coords.at(12 + 10));        // Top
  addQuad(geometry[7][kTopLeftCorner], geometry[7][kBottomLeftCorner],
          geometry[6][kBottomLeftCorner], geometry[6][kTopLeftCorner], texture_coords.at(12 + 11));      // Left


  // add the front face
  addQuad(geometry[8][kBottomLeftCorner], geometry[8][kBottomRightCorner],
          geometry[8][kTopRightCorner], geometry[8][kTopLeftCorner], texture_coords.at(12 + 12));

  // add the back face
  addQuad(geometry[9][kBottomRightCorner], geometry[9][kBottomLeftCorner],
          geometry[9][kTopLeftCorner], geometry[9][kTopRightCorner], texture_coords.at(12 + 13));

  // add the sides
  addQuad(geometry[9][kBottomLeftCorner], geometry[9][kBottomRightCorner],
          geometry[8][kBottomRightCorner], geometry[8][kBottomLeftCorner], texture_coords.at(12 + 14));  // Bottom
  addQuad(geometry[9][kBottomRightCorner], geometry[9][kTopRightCorner],
          geometry[8][kTopRightCorner], geometry[8][kBottomRightCorner], texture_coords.at(12 + 15));    // Right
  addQuad(geometry[9][kTopRightCorner], geometry[9][kTopLeftCorner],
          geometry[8][kTopLeftCorner], geometry[8][kTopRightCorner], texture_coords.at(12 + 16));        // Top
  addQuad(geometry[9][kTopLeftCorner], geometry[9][kBottomLeftCorner],
          geometry[8][kBottomLeftCorner], geometry[8][kTopLeftCorner], texture_coords.at(12 + 17));      // Left
}

bool PaneRenderable::shouldRenderQuad(int index,
                                      const QVector3D& location,
                                      const BlockOrientation* orientation) const {
  if (orientation == BlockOrientation::get("Facing north/south") ||
      orientation == BlockOrientation::get("Facing east/west")) {
    return index >= kFullWidthFront && index <= kFullWidthLeft;
  } else if (orientation == BlockOrientation::get("North half")) {
    return index >= kNorthHalfFront && index <= kNorthHalfLeft;
  } else if (orientation == BlockOrientation::get("South half")) {
    return index >= kSouthHalfFront && index <= kSouthHalfLeft;
  } else if (orientation == BlockOrientation::get("East half")) {
    return index >= kEastHalfFront && index <= kEastHalfLeft;
  } else if (orientation == BlockOrientation::get("West half")) {
    return index >= kWestHalfFront && index <= kWestHalfLeft;
  } else if (orientation == BlockOrientation::get("Northeast corner")) {
    return (index >= kNorthHalfFront && index <= kNorthHalfLeft) ||
           (index >= kEastHalfFront && index <= kEastHalfLeft);
  } else if (orientation == BlockOrientation::get("Northwest corner")) {
    return (index >= kNorthHalfFront && index <= kNorthHalfLeft) ||
           (index >= kWestHalfFront && index <= kWestHalfLeft);
  } else if (orientation == BlockOrientation::get("Southeast corner")) {
    return (index >= kSouthHalfFront && index <= kSouthHalfLeft) ||
           (index >= kEastHalfFront && index <= kEastHalfLeft);
  } else if (orientation == BlockOrientation::get("Southwest corner")) {
    return (index >= kSouthHalfFront && index <= kSouthHalfLeft) ||
           (index >= kWestHalfFront && index <= kWestHalfLeft);
  } else if (orientation == BlockOrientation::get("Cross")) {
    return index > kFullWidthLeft;
  } else {
    return false;
  }
}

void PaneRenderable::applyOrientationTransform(const BlockOrientation* orientation) const {
  if (orientation == BlockOrientation::get("Facing east/west")) {
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
  }
}

Texture PaneRenderable::textureForQuad(int index) const {
  return texture(index % 6);
}
