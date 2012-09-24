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

#include "stairs_renderable.h"

#include "block_orientation.h"
#include "render_delegate.h"

StairsRenderable::StairsRenderable(const QVector3D& size)
    : BasicRenderable(size) {
}


void StairsRenderable::addGeometry(const StairsRenderable::Geometry& geometry,
                                   const StairsRenderable::TextureCoords& texture_coords) {
  for (int front = 0, back = 1, coords = 0; back <= 3; front += 2, back += 2, coords += 6) {
    // add the front face
    addQuad(geometry[front][kBottomLeftCorner], geometry[front][kBottomRightCorner],
            geometry[front][kTopRightCorner], geometry[front][kTopLeftCorner], texture_coords.at(coords));

    // add the back face
    addQuad(geometry[back][kBottomRightCorner], geometry[back][kBottomLeftCorner],
            geometry[back][kTopLeftCorner], geometry[back][kTopRightCorner], texture_coords.at(coords + 1));

    // add the sides
    addQuad(geometry[back][kBottomLeftCorner], geometry[back][kBottomRightCorner],                             // Bottom
            geometry[front][kBottomRightCorner], geometry[front][kBottomLeftCorner], texture_coords.at(coords + 2));
    addQuad(geometry[back][kBottomRightCorner], geometry[back][kTopRightCorner],                               // Right
            geometry[front][kTopRightCorner], geometry[front][kBottomRightCorner], texture_coords.at(coords + 3));
    addQuad(geometry[back][kTopRightCorner], geometry[back][kTopLeftCorner],                                   // Top
            geometry[front][kTopLeftCorner], geometry[front][kTopRightCorner], texture_coords.at(coords + 4));
    addQuad(geometry[back][kTopLeftCorner], geometry[back][kBottomLeftCorner],                                 // Left
            geometry[front][kBottomLeftCorner], geometry[front][kTopLeftCorner], texture_coords.at(coords + 5));
  }
}

StairsRenderable::Geometry StairsRenderable::moveToOrigin(const StairsRenderable::Geometry& geometry) {
  // Move move verts half cube width across so cube is centered on origin.
  // (But keep it flat on the ground below it.)
  QVector3D sz = size();
  Geometry out_geometry = geometry;
  for (int quad = 0; quad < 4; ++quad) {
    for (int i = 0; i < 4; ++i) {
      out_geometry[quad][i] -= QVector3D(sz.x() / 2.0f, 0.5f, -sz.z() / 2.0f);
    }
  }
  return out_geometry;
}


StairsRenderable::Geometry StairsRenderable::createGeometry() {
  QVector3D sz = size();
  QVector<QVector3D> verts1;
  verts1.resize(4);
  verts1[kBottomRightCorner].setX(sz.x());
  verts1[kTopRightCorner].setX(sz.x());
  verts1[kTopRightCorner].setY(sz.y() / 2);
  verts1[kTopLeftCorner].setY(sz.y() / 2);

  // back face - "extrude" verts down
  QVector<QVector3D> back1(verts1);
  for (int i = 0; i < 4; ++i) {
    back1[i].setZ(-sz.z() / 2);
  }

  // Repeat for other block
  QVector<QVector3D> verts2;
  verts2.resize(4);
  verts2[kBottomLeftCorner].setZ(-sz.z() / 2);
  verts2[kBottomRightCorner].setZ(-sz.z() / 2);
  verts2[kTopRightCorner].setZ(-sz.z() / 2);
  verts2[kTopLeftCorner].setZ(-sz.z() / 2);
  verts2[kBottomRightCorner].setX(sz.x());
  verts2[kTopRightCorner].setX(sz.x());
  verts2[kTopRightCorner].setY(sz.y());
  verts2[kTopLeftCorner].setY(sz.y());

  // back face - "extrude" verts down
  QVector<QVector3D> back2(verts2);
  for (int i = 0; i < 4; ++i) {
    back2[i].setZ(-sz.z());
  }

  return Geometry() << verts1 << back1 << verts2 << back2;
}

StairsRenderable::TextureCoords StairsRenderable::createTextureCoords(const Geometry& geometry) {
  TextureCoords front_coords = createTextureCoordsForBlock(geometry[0], geometry[1]);
  TextureCoords back_coords = createTextureCoordsForBlock(geometry[2], geometry[3]);

  // See http://goo.gl/OSz2U.
  front_coords[kLeftFace][kBottomLeftCorner] += QVector2D(0.5, 0);
  front_coords[kLeftFace][kBottomRightCorner] += QVector2D(0.5, 0);
  front_coords[kLeftFace][kTopRightCorner] += QVector2D(0.5, 0);
  front_coords[kLeftFace][kTopLeftCorner] += QVector2D(0.5, 0);

  back_coords[kLeftFace][kBottomLeftCorner] += QVector2D(-0.5, 0);
  back_coords[kLeftFace][kBottomRightCorner] += QVector2D(-0.5, 0);
  back_coords[kLeftFace][kTopRightCorner] += QVector2D(-0.5, 0);
  back_coords[kLeftFace][kTopLeftCorner] += QVector2D(-0.5, 0);

  return TextureCoords() << front_coords << back_coords;
}

StairsRenderable::TextureCoords StairsRenderable::createTextureCoordsForBlock(QVector<QVector3D> front,
                                                                              QVector<QVector3D> back) {
  QVector<QVector2D> top_tex;
  top_tex.resize(4);
  QVector<QVector2D> front_tex;
  front_tex.resize(4);
  QVector<QVector2D> back_tex;
  back_tex.resize(4);
  QVector<QVector2D> right_tex;
  right_tex.resize(4);
  QVector<QVector2D> bottom_tex;
  bottom_tex.resize(4);
  QVector<QVector2D> left_tex;
  left_tex.resize(4);

  top_tex[kBottomLeftCorner] = QVector2D(front[kTopLeftCorner].x(), -back[kTopLeftCorner].z());
  top_tex[kBottomRightCorner] = QVector2D(front[kTopRightCorner].x(), -back[kTopRightCorner].z());
  top_tex[kTopRightCorner] = QVector2D(front[kBottomRightCorner].x(), -front[kBottomRightCorner].z());
  top_tex[kTopLeftCorner] = QVector2D(front[kBottomLeftCorner].x(), -front[kBottomLeftCorner].z());

  front_tex[kBottomLeftCorner] = front[kBottomLeftCorner].toVector2D();
  front_tex[kBottomRightCorner] = front[kBottomRightCorner].toVector2D();
  front_tex[kTopRightCorner] = front[kTopRightCorner].toVector2D();
  front_tex[kTopLeftCorner] = front[kTopLeftCorner].toVector2D();

  back_tex[kBottomLeftCorner] = front[kBottomLeftCorner].toVector2D();
  back_tex[kBottomRightCorner] = front[kBottomRightCorner].toVector2D();
  back_tex[kTopRightCorner] = front[kTopRightCorner].toVector2D();
  back_tex[kTopLeftCorner] = front[kTopLeftCorner].toVector2D();

  right_tex[kBottomLeftCorner] = QVector2D(-back[kBottomRightCorner].z(), front[kBottomRightCorner].y());
  right_tex[kBottomRightCorner] = QVector2D(-back[kTopRightCorner].z(), front[kTopRightCorner].y());
  right_tex[kTopRightCorner] = QVector2D(-front[kTopLeftCorner].z(), front[kTopLeftCorner].y());
  right_tex[kTopLeftCorner] = QVector2D(-front[kBottomLeftCorner].z(), front[kBottomLeftCorner].y());

  bottom_tex[kBottomLeftCorner] = QVector2D(front[kTopRightCorner].x(), -front[kTopRightCorner].z());
  bottom_tex[kBottomRightCorner] = QVector2D(front[kTopLeftCorner].x(), -front[kTopLeftCorner].z());
  bottom_tex[kTopRightCorner] = QVector2D(front[kBottomLeftCorner].x(), -back[kBottomLeftCorner].z());
  bottom_tex[kTopLeftCorner] = QVector2D(front[kBottomRightCorner].x(), -back[kBottomRightCorner].z());

  left_tex[kBottomLeftCorner] = QVector2D(-front[kTopLeftCorner].z(), front[kTopLeftCorner].y());
  left_tex[kBottomRightCorner] = QVector2D(-front[kBottomLeftCorner].z(), front[kBottomLeftCorner].y());
  left_tex[kTopRightCorner] = QVector2D(-back[kBottomRightCorner].z(), front[kBottomRightCorner].y());
  left_tex[kTopLeftCorner] = QVector2D(-back[kTopRightCorner].z(), front[kTopRightCorner].y());

  return TextureCoords() << front_tex << back_tex << bottom_tex << right_tex << top_tex << left_tex;
}

void StairsRenderable::applyOrientationTransform(const BlockOrientation* orientation) const {
  if (orientation == BlockOrientation::get("Facing north")) {
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
  } else if (orientation == BlockOrientation::get("Facing east")) {
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
  } else if (orientation == BlockOrientation::get("Facing west")) {
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
  } else if (orientation == BlockOrientation::get("Facing north, inverted")) {
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
  } else if (orientation == BlockOrientation::get("Facing east, inverted")) {
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
  } else if (orientation == BlockOrientation::get("Facing west, inverted")) {
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
  } else if (orientation == BlockOrientation::get("Facing south, inverted")) {
    glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
  }
}

Texture StairsRenderable::textureForQuad(int index, const BlockOrientation* orientation) const {
  return texture(index % 6);
}
