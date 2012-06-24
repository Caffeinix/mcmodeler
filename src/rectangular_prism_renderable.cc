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

#include "rectangular_prism_renderable.h"

#include "block_orientation.h"
#include "render_delegate.h"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

RectangularPrismRenderable::RectangularPrismRenderable(const QVector3D& size, TextureSizing sizing)
    : BasicRenderable(size),
      sizing_(sizing) {
}

void RectangularPrismRenderable::addGeometry(const RectangularPrismRenderable::Geometry& geometry,
                                             const RectangularPrismRenderable::TextureCoords& texture_coords) {
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
}

RectangularPrismRenderable::Geometry RectangularPrismRenderable::moveToOrigin(
    const RectangularPrismRenderable::Geometry& geometry) {
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


RectangularPrismRenderable::Geometry RectangularPrismRenderable::createGeometry() {
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

RectangularPrismRenderable::TextureCoords RectangularPrismRenderable::createTextureCoords(
    const Geometry& geometry) {
  TextureSizing szng = sizing();
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

  QVector<QVector3D> verts = geometry[0];
  QVector<QVector3D> back = geometry[1];

  if (szng == kTextureClip) {
    // Align the textures as they would appear on a 1x1x1 cube.
    top_tex[kBottomLeftCorner] = QVector2D(verts[kTopRightCorner].x(), -back[kTopRightCorner].z());
    top_tex[kBottomRightCorner] = QVector2D(verts[kTopLeftCorner].x(), -back[kTopLeftCorner].z());
    top_tex[kTopRightCorner] = QVector2D(verts[kBottomLeftCorner].x(), -verts[kBottomLeftCorner].z());
    top_tex[kTopLeftCorner] = QVector2D(verts[kBottomRightCorner].x(), -verts[kBottomRightCorner].z());

    qDebug() << "Top tex coords:" << top_tex;

    front_tex[kBottomLeftCorner] = verts[kBottomLeftCorner].toVector2D();
    front_tex[kBottomRightCorner] = verts[kBottomRightCorner].toVector2D();
    front_tex[kTopRightCorner] = verts[kTopRightCorner].toVector2D();
    front_tex[kTopLeftCorner] = verts[kTopLeftCorner].toVector2D();

    back_tex[kBottomLeftCorner] = verts[kBottomLeftCorner].toVector2D();
    back_tex[kBottomRightCorner] = verts[kBottomRightCorner].toVector2D();
    back_tex[kTopRightCorner] = verts[kTopRightCorner].toVector2D();
    back_tex[kTopLeftCorner] = verts[kTopLeftCorner].toVector2D();

    right_tex[kBottomLeftCorner] = QVector2D(-back[kBottomRightCorner].z(), verts[kBottomRightCorner].y());
    right_tex[kBottomRightCorner] = QVector2D(-back[kTopRightCorner].z(), verts[kTopRightCorner].y());
    right_tex[kTopRightCorner] = QVector2D(-verts[kTopLeftCorner].z(), verts[kTopLeftCorner].y());
    right_tex[kTopLeftCorner] = QVector2D(-verts[kBottomLeftCorner].z(), verts[kBottomLeftCorner].y());

    bottom_tex[kBottomLeftCorner] = QVector2D(verts[kTopRightCorner].x(), -verts[kTopRightCorner].z());
    bottom_tex[kBottomRightCorner] = QVector2D(verts[kTopLeftCorner].x(), -verts[kTopLeftCorner].z());
    bottom_tex[kTopRightCorner] = QVector2D(verts[kBottomLeftCorner].x(), -back[kBottomLeftCorner].z());
    bottom_tex[kTopLeftCorner] = QVector2D(verts[kBottomRightCorner].x(), -back[kBottomRightCorner].z());

    left_tex[kBottomLeftCorner] = QVector2D(-verts[kTopLeftCorner].z(), verts[kTopLeftCorner].y());
    left_tex[kBottomRightCorner] = QVector2D(-verts[kBottomLeftCorner].z(), verts[kBottomLeftCorner].y());
    left_tex[kTopRightCorner] = QVector2D(-back[kBottomRightCorner].z(), verts[kBottomRightCorner].y());
    left_tex[kTopLeftCorner] = QVector2D(-back[kTopRightCorner].z(), verts[kTopRightCorner].y());
  } else {
    // Align the textures directly to the faces.
    top_tex[kBottomLeftCorner] = QVector2D(1, 1);
    top_tex[kBottomRightCorner] = QVector2D(0, 1);
    top_tex[kTopRightCorner] = QVector2D(0, 0);
    top_tex[kTopLeftCorner] = QVector2D(1, 0);

    front_tex[kBottomLeftCorner] = QVector2D(0, 0);
    front_tex[kBottomRightCorner] = QVector2D(1, 0);
    front_tex[kTopRightCorner] = QVector2D(1, 1);
    front_tex[kTopLeftCorner] = QVector2D(0, 1);

    back_tex[kBottomLeftCorner] = QVector2D(0, 0);
    back_tex[kBottomRightCorner] = QVector2D(1, 0);
    back_tex[kTopRightCorner] = QVector2D(1, 1);
    back_tex[kTopLeftCorner] = QVector2D(0, 1);

    right_tex[kBottomLeftCorner] = QVector2D(1, 0);
    right_tex[kBottomRightCorner] = QVector2D(1, 1);
    right_tex[kTopRightCorner] = QVector2D(0, 1);
    right_tex[kTopLeftCorner] = QVector2D(0, 0);

    bottom_tex[kBottomLeftCorner] = QVector2D(1, 1);
    bottom_tex[kBottomRightCorner] = QVector2D(0, 1);
    bottom_tex[kTopRightCorner] = QVector2D(0, 0);
    bottom_tex[kTopLeftCorner] = QVector2D(1, 0);

    left_tex[kBottomLeftCorner] = QVector2D(0, 1);
    left_tex[kBottomRightCorner] = QVector2D(0, 0);
    left_tex[kTopRightCorner] = QVector2D(1, 0);
    left_tex[kTopLeftCorner] = QVector2D(1, 1);
  }

  return TextureCoords() << front_tex << back_tex << bottom_tex << right_tex << top_tex << left_tex;
}

void RectangularPrismRenderable::applyOrientationTransform(const BlockOrientation* orientation) const {
  if (orientation == BlockOrientation::get("Facing north")) {
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
  } else if (orientation == BlockOrientation::get("Facing east")) {
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
  } else if (orientation == BlockOrientation::get("Facing west")) {
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
  }
}

bool RectangularPrismRenderable::shouldRenderQuad(int index,
                                                  const QVector3D& location,
                                                  const BlockOrientation* orientation) const {
  // Rectangular prisms have one quad per face, so we can just check the delegate.
  Face face = mapToDefaultOrientation(static_cast<Face>(index), orientation);
  return !(renderDelegate() && !renderDelegate()->shouldRenderFace(this, face, location));
}

Face RectangularPrismRenderable::mapToDefaultOrientation(Face local_face, const BlockOrientation* orientation) const {
  if (orientation == BlockOrientation::get("Facing north")) {
    switch (local_face) {
    case kFrontFace:
      return kBackFace;
    case kBackFace:
      return kFrontFace;
    case kRightFace:
      return kLeftFace;
    case kLeftFace:
      return kRightFace;
    default:
      break;
    }
  } else if (orientation == BlockOrientation::get("Facing west")) {
    switch (local_face) {
    case kFrontFace:
      return kLeftFace;
    case kBackFace:
      return kRightFace;
    case kRightFace:
      return kFrontFace;
    case kLeftFace:
      return kBackFace;
    default:
      break;
    }
  } else if (orientation == BlockOrientation::get("Facing east")) {
    switch (local_face) {
    case kFrontFace:
      return kRightFace;
    case kBackFace:
      return kLeftFace;
    case kRightFace:
      return kBackFace;
    case kLeftFace:
      return kFrontFace;
    default:
      break;
    }
  }
  return local_face;
}
