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

#include "track_renderable.h"

#include "block_orientation.h"
#include "enums.h"

TrackRenderable::TrackRenderable() : BasicRenderable(QVector3D(1.0, 1.0, 1.0)) {}

TrackRenderable::~TrackRenderable() {}

TrackRenderable::Geometry TrackRenderable::createGeometry() {
  static const double kYFloat = 1.0 / 256.0;
  Geometry geometry;

  // Add a flat quad on the ground for the non-ascending orientations.
  QVector<QVector3D> verts;
  verts.resize(4);
  verts[kBottomRightCorner].setX(1.0);
  verts[kTopRightCorner].setX(1.0);
  verts[kTopRightCorner].setZ(1.0);
  verts[kTopLeftCorner].setZ(1.0);
  for (int i = 0; i < 4; ++i) {
    // Raise the quad a tiny bit above the ground to avoid z-fighting.
    verts[i].setY(kYFloat);
  }
  geometry << verts;

  // Now hike up one edge of the quad for the ascending orientations.
  // Shockingly, we use a value greater than 1 here so the next level's track matches up visually.
  // This will cause a tiny bit of clipping if we put a solid block on top, but who'll notice?
  verts[kBottomLeftCorner].setY(1.0 + kYFloat);
  verts[kBottomRightCorner].setY(1.0 + kYFloat);
  geometry << verts;

  return geometry;
}

TrackRenderable::TextureCoords TrackRenderable::createTextureCoords(const Geometry& geometry) {
  TextureCoords texture_coords;
  QVector<QVector2D> tex;
  tex.resize(4);

  QVector<QVector3D> flat_quad = geometry[0];
  QVector<QVector3D> ascending_quad = geometry[1];

  tex[kBottomLeftCorner] = QVector2D(flat_quad[kBottomLeftCorner].x(), flat_quad[kBottomLeftCorner].z());
  tex[kBottomRightCorner] = QVector2D(flat_quad[kBottomRightCorner].x(), flat_quad[kBottomRightCorner].z());
  tex[kTopRightCorner] = QVector2D(flat_quad[kTopRightCorner].x(), flat_quad[kTopRightCorner].z());
  tex[kTopLeftCorner] = QVector2D(flat_quad[kTopLeftCorner].x(), flat_quad[kTopLeftCorner].z());
  texture_coords << tex;

  tex[kBottomLeftCorner] = QVector2D(ascending_quad[kBottomLeftCorner].x(), ascending_quad[kBottomLeftCorner].z());
  tex[kBottomRightCorner] = QVector2D(ascending_quad[kBottomRightCorner].x(), ascending_quad[kBottomRightCorner].z());
  tex[kTopRightCorner] = QVector2D(ascending_quad[kTopRightCorner].x(), ascending_quad[kTopRightCorner].z());
  tex[kTopLeftCorner] = QVector2D(ascending_quad[kTopLeftCorner].x(), ascending_quad[kTopLeftCorner].z());
  texture_coords << tex;

  tex[kBottomLeftCorner] = QVector2D(flat_quad[kBottomRightCorner].x(), flat_quad[kBottomRightCorner].z());
  tex[kBottomRightCorner] = QVector2D(flat_quad[kBottomLeftCorner].x(), flat_quad[kBottomLeftCorner].z());
  tex[kTopRightCorner] = QVector2D(flat_quad[kTopLeftCorner].x(), flat_quad[kTopLeftCorner].z());
  tex[kTopLeftCorner] = QVector2D(flat_quad[kTopRightCorner].x(), flat_quad[kTopRightCorner].z());
  texture_coords << tex;

  return texture_coords;
}

TrackRenderable::Geometry TrackRenderable::moveToOrigin(const Geometry& geometry) {
  Geometry out_geometry = geometry;
  for (int i = 0; i < 4; ++i) {
    out_geometry[0][i] -= QVector3D(0.5f, 0.5f, 0.5f);
    out_geometry[1][i] -= QVector3D(0.5f, 0.5f, 0.5f);
  }
  return out_geometry;
}

void TrackRenderable::addGeometry(const Geometry& geometry, const TextureCoords& texture_coords) {
  // Flat quad, top side.
  addQuad(geometry[0][kTopLeftCorner], geometry[0][kTopRightCorner],
          geometry[0][kBottomRightCorner], geometry[0][kBottomLeftCorner], texture_coords[0]);

  // Flat quad, bottom side.
  addQuad(geometry[0][kBottomLeftCorner], geometry[0][kBottomRightCorner],
          geometry[0][kTopRightCorner], geometry[0][kTopLeftCorner], texture_coords[0]);

  // Ascending quad, top side.
  addQuad(geometry[1][kTopLeftCorner], geometry[1][kTopRightCorner],
          geometry[1][kBottomRightCorner], geometry[1][kBottomLeftCorner], texture_coords[1]);

  // Ascending quad, bottom side.
  addQuad(geometry[1][kBottomLeftCorner], geometry[1][kBottomRightCorner],
          geometry[1][kTopRightCorner], geometry[1][kTopLeftCorner], texture_coords[1]);

  // Flat quad, top side, reversed textures.
  addQuad(geometry[0][kTopLeftCorner], geometry[0][kTopRightCorner],
          geometry[0][kBottomRightCorner], geometry[0][kBottomLeftCorner], texture_coords[2]);

  // Flat quad, bottom side, reversed textures.
  addQuad(geometry[0][kBottomLeftCorner], geometry[0][kBottomRightCorner],
          geometry[0][kTopRightCorner], geometry[0][kTopLeftCorner], texture_coords[2]);
}

void TrackRenderable::applyOrientationTransform(const BlockOrientation* orientation) const {
  if (orientation == BlockOrientation::get("Running east/west")) {
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
  } else if (orientation == BlockOrientation::get("Ascending east")) {
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
  } else if (orientation == BlockOrientation::get("Ascending south")) {
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
  } else if (orientation == BlockOrientation::get("Ascending west")) {
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
  } else if (orientation == BlockOrientation::get("Southeast corner") ||
             orientation == BlockOrientation::get("Southwest corner")) {
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
  }
}

bool TrackRenderable::shouldRenderQuad(int index,
                                       const QVector3D& location,
                                       const BlockOrientation* orientation) const {
  switch (index) {
    case 0:  // Normal flat quad.
    case 1:
      return !orientation->name().contains("Ascending") &&
             !(orientation == BlockOrientation::get("Southwest corner") ||
               orientation == BlockOrientation::get("Northeast corner"));
    case 2:  // Ascending quad, top.
    case 3:  // Ascending quad, bottom.
      return orientation->name().contains("Ascending");
    case 4:  // Reversed flat quad.
    case 5:
      return orientation == BlockOrientation::get("Southwest corner") ||
             orientation == BlockOrientation::get("Northeast corner");
    default:
      return false;
  }
}

Texture TrackRenderable::textureForQuad(int index, const BlockOrientation* orientation) const {
  if (orientation->name().contains("corner")) {
    return texture(1);
  } else {
    return texture(0);
  }
}

