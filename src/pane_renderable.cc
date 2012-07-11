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
  for (int t = 0, g = 0; t < texture_coords.size(); t += 6, g += 2) {
    // add the front face
    addQuad(geometry[g][kBottomLeftCorner], geometry[g][kBottomRightCorner],
            geometry[g][kTopRightCorner], geometry[g][kTopLeftCorner], texture_coords.at(t));

    // add the back face
    addQuad(geometry[g + 1][kBottomRightCorner], geometry[g + 1][kBottomLeftCorner],
            geometry[g + 1][kTopLeftCorner], geometry[g + 1][kTopRightCorner], texture_coords.at(t + 1));

    // add the sides
    addQuad(geometry[g + 1][kBottomLeftCorner], geometry[g + 1][kBottomRightCorner],
            geometry[g][kBottomRightCorner], geometry[g][kBottomLeftCorner], texture_coords.at(t + 2));  // Bottom
    addQuad(geometry[g + 1][kBottomRightCorner], geometry[g + 1][kTopRightCorner],
            geometry[g][kTopRightCorner], geometry[g][kBottomRightCorner], texture_coords.at(t + 3));    // Right
    addQuad(geometry[g + 1][kTopRightCorner], geometry[g + 1][kTopLeftCorner],
            geometry[g][kTopLeftCorner], geometry[g][kTopRightCorner], texture_coords.at(t + 4));        // Top
    addQuad(geometry[g + 1][kTopLeftCorner], geometry[g + 1][kBottomLeftCorner],
            geometry[g][kBottomLeftCorner], geometry[g][kTopLeftCorner], texture_coords.at(t + 5));      // Left
  }
}

bool PaneRenderable::shouldRenderQuad(int index,
                                      const QVector3D& location,
                                      const BlockOrientation* orientation) const {
  if (orientation == BlockOrientation::get("Running north/south") ||
      orientation == BlockOrientation::get("Running east/west")) {
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
  } else if (orientation == BlockOrientation::get("T facing south")) {
    return index > kFullWidthLeft && !(index >= kSouthHalfFront && index <= kSouthHalfLeft);
  } else if (orientation == BlockOrientation::get("T facing west")) {
    return index > kFullWidthLeft && !(index >= kWestHalfFront && index <= kWestHalfLeft);
  } else if (orientation == BlockOrientation::get("T facing north")) {
    return index > kFullWidthLeft && !(index >= kNorthHalfFront && index <= kNorthHalfLeft);
  } else if (orientation == BlockOrientation::get("T facing east")) {
    return index > kFullWidthLeft && !(index >= kEastHalfFront && index <= kEastHalfLeft);
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

Texture PaneRenderable::textureForQuad(int index, const BlockOrientation* orientation) const {
  return texture(index % 6);
}
