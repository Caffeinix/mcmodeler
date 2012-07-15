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

#include "torch_renderable.h"

#include "block_orientation.h"
#include "enums.h"

TorchRenderable::TorchRenderable(const QVector3D& size) : BasicRenderable(size) {
}

TorchRenderable::Geometry TorchRenderable::createGeometry() {
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
  return Geometry() << verts << back;
}

TorchRenderable::TextureCoords TorchRenderable::createTextureCoords(const Geometry& geometry) {
  QVector<QVector2D> top_tex;
  top_tex.resize(4);
  QVector<QVector2D> front_tex;
  front_tex.resize(4);
  QVector<QVector2D> back_tex;
  back_tex.resize(4);
  QVector<QVector2D> right_tex;
  right_tex.resize(4);
  QVector<QVector2D> left_tex;
  left_tex.resize(4);

  top_tex[kBottomLeftCorner] = QVector2D(9./16., 10./16.);
  top_tex[kBottomRightCorner] = QVector2D(7./16., 10./16.);
  top_tex[kTopRightCorner] = QVector2D(7./16., 8./16.);
  top_tex[kTopLeftCorner] = QVector2D(9./16., 8./16.);

  front_tex[kBottomLeftCorner] = QVector2D(7./16., 0);
  front_tex[kBottomRightCorner] = QVector2D(9./16., 0);
  front_tex[kTopRightCorner] = QVector2D(9./16., 10./16.);
  front_tex[kTopLeftCorner] = QVector2D(7./16., 10./16.);

  back_tex[kBottomLeftCorner] = QVector2D(7./16., 0);
  back_tex[kBottomRightCorner] = QVector2D(9./16., 0);
  back_tex[kTopRightCorner] = QVector2D(9./16., 10./16.);
  back_tex[kTopLeftCorner] = QVector2D(7./16., 10./16.);

  right_tex[kBottomLeftCorner] = QVector2D(9./16., 0);
  right_tex[kBottomRightCorner] = QVector2D(9./16., 10./16.);
  right_tex[kTopRightCorner] = QVector2D(7./16., 10./16.);
  right_tex[kTopLeftCorner] = QVector2D(7./16., 0);

  left_tex[kBottomLeftCorner] = QVector2D(7./16., 10./16.);
  left_tex[kBottomRightCorner] = QVector2D(7./16., 0);
  left_tex[kTopRightCorner] = QVector2D(9./16., 0);
  left_tex[kTopLeftCorner] = QVector2D(9./16., 10./16.);

  return TextureCoords() << front_tex << back_tex << right_tex << top_tex << left_tex;
}

TorchRenderable::Geometry TorchRenderable::moveToOrigin(const Geometry& geometry) {
  // Now move verts half cube width across so cube is centered on origin.
  // (But keep it flat on the ground below it.)
  const QVector3D& sz = size();
  Geometry out_geometry = geometry;
  for (int i = 0; i < 4; ++i) {
    out_geometry[0][i] -= QVector3D(sz.x() / 2.0f, 0.5f, -sz.z() / 2.0f);
    out_geometry[1][i] -= QVector3D(sz.x() / 2.0f, 0.5f, -sz.z() / 2.0f);
  }
  return out_geometry;
}

void TorchRenderable::addGeometry(const Geometry& geometry, const TextureCoords& texture_coords) {
  // add the front face
  addQuad(geometry[0][kBottomLeftCorner], geometry[0][kBottomRightCorner],
          geometry[0][kTopRightCorner], geometry[0][kTopLeftCorner], texture_coords.at(0));

  // add the back face
  addQuad(geometry[1][kBottomRightCorner], geometry[1][kBottomLeftCorner],
          geometry[1][kTopLeftCorner], geometry[1][kTopRightCorner], texture_coords.at(1));

  // add the sides
  addQuad(geometry[1][kBottomRightCorner], geometry[1][kTopRightCorner],
          geometry[0][kTopRightCorner], geometry[0][kBottomRightCorner], texture_coords.at(2));    // Right
  addQuad(geometry[1][kTopRightCorner], geometry[1][kTopLeftCorner],
          geometry[0][kTopLeftCorner], geometry[0][kTopRightCorner], texture_coords.at(3));        // Top
  addQuad(geometry[1][kTopLeftCorner], geometry[1][kBottomLeftCorner],
          geometry[0][kBottomLeftCorner], geometry[0][kTopLeftCorner], texture_coords.at(4));      // Left

  // Do it again, but this time at an angle.

  Geometry slanted_geometry = geometry;
  slanted_geometry[0][kBottomLeftCorner].setX(-0.5 - 1./16.);
  slanted_geometry[0][kBottomRightCorner].setX(-0.5 + 1./16.);
  slanted_geometry[0][kTopLeftCorner].setX(-0.5 + 3./16.);
  slanted_geometry[0][kTopRightCorner].setX(-0.5 + 5./16.);
  slanted_geometry[0][kBottomLeftCorner].setY(-0.5 + 3./16.);
  slanted_geometry[0][kBottomRightCorner].setY(-0.5 + 3./16.);
  slanted_geometry[0][kTopLeftCorner].setY(0.5 - 3./16.);
  slanted_geometry[0][kTopRightCorner].setY(0.5 - 3./16.);

  slanted_geometry[1][kBottomLeftCorner].setX(-0.5 - 1./16.);
  slanted_geometry[1][kBottomRightCorner].setX(-0.5 + 1./16.);
  slanted_geometry[1][kTopLeftCorner].setX(-0.5 + 3./16.);
  slanted_geometry[1][kTopRightCorner].setX(-0.5 + 5./16.);
  slanted_geometry[1][kBottomLeftCorner].setY(-0.5 + 3./16.);
  slanted_geometry[1][kBottomRightCorner].setY(-0.5 + 3./16.);
  slanted_geometry[1][kTopLeftCorner].setY(0.5 - 3./16.);
  slanted_geometry[1][kTopRightCorner].setY(0.5 - 3./16.);

  // add the front face
  addQuad(slanted_geometry[0][kBottomLeftCorner], slanted_geometry[0][kBottomRightCorner],
          slanted_geometry[0][kTopRightCorner], slanted_geometry[0][kTopLeftCorner], texture_coords.at(0));

  // add the back face
  addQuad(slanted_geometry[1][kBottomRightCorner], slanted_geometry[1][kBottomLeftCorner],
          slanted_geometry[1][kTopLeftCorner], slanted_geometry[1][kTopRightCorner], texture_coords.at(1));

  // add the sides
  addQuad(slanted_geometry[1][kBottomRightCorner], slanted_geometry[1][kTopRightCorner],
          slanted_geometry[0][kTopRightCorner], slanted_geometry[0][kBottomRightCorner], texture_coords.at(2)); // Right
  addQuad(slanted_geometry[1][kTopRightCorner], slanted_geometry[1][kTopLeftCorner],
          slanted_geometry[0][kTopLeftCorner], slanted_geometry[0][kTopRightCorner], texture_coords.at(3));     // Top
  addQuad(slanted_geometry[1][kTopLeftCorner], slanted_geometry[1][kBottomLeftCorner],
          slanted_geometry[0][kBottomLeftCorner], slanted_geometry[0][kTopLeftCorner], texture_coords.at(4));   // Left
}

void TorchRenderable::applyOrientationTransform(const BlockOrientation* orientation) const {
  if (orientation == BlockOrientation::get("On north wall")) {
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
  } else if (orientation == BlockOrientation::get("On east wall")) {
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
  } else if (orientation == BlockOrientation::get("On south wall")) {
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
  }
}

bool TorchRenderable::shouldRenderQuad(int index, const QVector3D& location,
                                       const BlockOrientation* orientation) const {
  if (orientation == BlockOrientation::get("On floor")) {
    return index < 5;
  } else {
    return index >= 5;
  }
}

Texture TorchRenderable::textureForQuad(int index, const BlockOrientation* orientation) const {
  return texture(0);
}
