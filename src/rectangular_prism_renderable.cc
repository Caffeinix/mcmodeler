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
#include "enums.h"
#include "render_delegate.h"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif


RectangularPrismRenderable::Geometry RectangularPrismRenderable::createGeometry(const QVector3D& size) {
  QVector<QVector3D> verts;
  verts.resize(4);
  verts[kBottomRightCorner].setX(size.x());
  verts[kTopRightCorner].setX(size.x());
  verts[kTopRightCorner].setY(size.y());
  verts[kTopLeftCorner].setY(size.y());

  // back face - "extrude" verts down
  QVector<QVector3D> back(verts);
  for (int i = 0; i < 4; ++i) {
    back[i].setZ(-size.z());
  }
  return qMakePair(verts, back);
}

RectangularPrismRenderable::TextureCoords RectangularPrismRenderable::createTextureCoords(
    const Geometry& geometry, TextureSizing sizing) {
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

  QVector<QVector3D> verts = geometry.first;
  QVector<QVector3D> back = geometry.second;

  if (sizing == kTextureClip) {
    // Align the textures as they would appear on a 1x1x1 cube.
    top_tex[0] = QVector2D(verts[0].x(), -verts[0].z());
    top_tex[1] = QVector2D(verts[1].x(), -verts[1].z());
    top_tex[2] = QVector2D(verts[2].x(), -back[2].z());
    top_tex[3] = QVector2D(verts[3].x(), -back[3].z());

    front_tex[0] = verts[0].toVector2D();
    front_tex[1] = verts[1].toVector2D();
    front_tex[2] = verts[2].toVector2D();
    front_tex[3] = verts[3].toVector2D();

    back_tex[0] = verts[0].toVector2D();
    back_tex[1] = verts[1].toVector2D();
    back_tex[2] = verts[2].toVector2D();
    back_tex[3] = verts[3].toVector2D();

    right_tex[0] = verts[1].toVector2D();
    right_tex[1] = verts[2].toVector2D();
    right_tex[2] = verts[3].toVector2D();
    right_tex[3] = verts[0].toVector2D();

    bottom_tex[0] = QVector2D(verts[2].x(), -verts[2].z());
    bottom_tex[1] = QVector2D(verts[3].x(), -verts[3].z());
    bottom_tex[2] = QVector2D(verts[0].x(), -back[0].z());
    bottom_tex[3] = QVector2D(verts[1].x(), -back[1].z());

    left_tex[0] = verts[3].toVector2D();
    left_tex[1] = verts[0].toVector2D();
    left_tex[2] = verts[1].toVector2D();
    left_tex[3] = verts[2].toVector2D();
  } else {
    // Align the textures directly to the faces.
    top_tex[0] = QVector2D(0, 0);
    top_tex[1] = QVector2D(1, 0);
    top_tex[2] = QVector2D(1, 1);
    top_tex[3] = QVector2D(0, 1);

    front_tex[0] = QVector2D(0, 0);
    front_tex[1] = QVector2D(1, 0);
    front_tex[2] = QVector2D(1, 1);
    front_tex[3] = QVector2D(0, 1);

    back_tex[0] = QVector2D(0, 0);
    back_tex[1] = QVector2D(1, 0);
    back_tex[2] = QVector2D(1, 1);
    back_tex[3] = QVector2D(0, 1);

    right_tex[0] = QVector2D(1, 0);
    right_tex[1] = QVector2D(1, 1);
    right_tex[2] = QVector2D(0, 1);
    right_tex[3] = QVector2D(0, 0);

    bottom_tex[0] = QVector2D(1, 1);
    bottom_tex[1] = QVector2D(0, 1);
    bottom_tex[2] = QVector2D(0, 0);
    bottom_tex[3] = QVector2D(1, 0);

    left_tex[0] = QVector2D(0, 1);
    left_tex[1] = QVector2D(0, 0);
    left_tex[2] = QVector2D(1, 0);
    left_tex[3] = QVector2D(1, 1);
  }

//  To mirror horizontally:
//  left_tex[0] = verts[2].toVector2D();
//  left_tex[1] = verts[1].toVector2D();
//  left_tex[2] = verts[0].toVector2D();
//  left_tex[3] = verts[3].toVector2D();

  return TextureCoords() << front_tex << back_tex << bottom_tex << right_tex << top_tex << left_tex;
}

RectangularPrismRenderable::RectangularPrismRenderable(const QVector3D& size, TextureSizing sizing) {
  Geometry geometry = createGeometry(size);
  TextureCoords texture_coords = createTextureCoords(geometry, sizing);

  // Now move verts half cube width across so cube is centered on origin.
  // (But keep it flat on the ground below it.)
  for (int i = 0; i < 4; ++i) {
    geometry.first[i] -= QVector3D(size.x() / 2.0f, 0.5f, -size.z() / 2.0f);
    geometry.second[i] -= QVector3D(size.x() / 2.0f, 0.5f, -size.z() / 2.0f);
  }

  // add the front face
  addQuad(geometry.first[kBottomLeftCorner], geometry.first[kBottomRightCorner],
          geometry.first[kTopRightCorner], geometry.first[kTopLeftCorner], texture_coords.at(0));

  // add the back face
  addQuad(geometry.second[kBottomRightCorner], geometry.second[kBottomLeftCorner],
          geometry.second[kTopLeftCorner], geometry.second[kTopRightCorner], texture_coords.at(1));

  // add the sides
  addQuad(geometry.second[kBottomLeftCorner], geometry.second[kBottomRightCorner],
          geometry.first[kBottomRightCorner], geometry.first[kBottomLeftCorner], texture_coords.at(2));  // Bottom
  addQuad(geometry.second[kBottomRightCorner], geometry.second[kTopRightCorner],
          geometry.first[kTopRightCorner], geometry.first[kBottomRightCorner], texture_coords.at(3));    // Right
  addQuad(geometry.second[kTopRightCorner], geometry.second[kTopLeftCorner],
          geometry.first[kTopLeftCorner], geometry.first[kTopRightCorner], texture_coords.at(4));        // Top
  addQuad(geometry.second[kTopLeftCorner], geometry.second[kBottomLeftCorner],
          geometry.first[kBottomLeftCorner], geometry.first[kTopLeftCorner], texture_coords.at(5));      // Left
}

void RectangularPrismRenderable::appendVertex(const QVector3D& vertex,
                                              const QVector3D& normal,
                                              const QVector2D& tex_coord) {
  vertices_.append(vertex);
  normals_.append(normal);
  tex_coords_.append(tex_coord);
}

void RectangularPrismRenderable::addQuad(const QVector3D &a, const QVector3D &b,
                                         const QVector3D &c, const QVector3D &d,
                                         const QVector<QVector2D> &tex) {
  QVector3D norm = QVector3D::normal(a, b, c);
  appendVertex(a, norm, tex[0]);
  appendVertex(b, norm, tex[1]);
  appendVertex(c, norm, tex[2]);
  appendVertex(d, norm, tex[3]);
}

void RectangularPrismRenderable::renderAt(const QVector3D& location, const BlockOrientation* orientation) const {
  glPushMatrix();
  glTranslatef(location.x(), location.y(), location.z());
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertices_.constData());
  glNormalPointer(GL_FLOAT, 0, normals_.constData());
  glTexCoordPointer(2, GL_FLOAT, 0, tex_coords_.constData());
  for (int start = 0; start < vertices_.size(); start += 4) {
    // Front, back, bottom, right, top, left.
    if (renderDelegate() && !renderDelegate()->shouldRenderFace(this, static_cast<Face>(start / 4), location)) {
      continue;
    }
    glBindTexture(GL_TEXTURE_2D, texture(rotatedTextureIndex(start / 4, orientation)).textureId());
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLushort indices[4] = { start, start + 1, start + 2, start + 3 };
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, indices);
  }
  glPopMatrix();
}

int RectangularPrismRenderable::rotatedTextureIndex(int local_index, const BlockOrientation* orientation) const {
  if (orientation == BlockOrientation::get("Facing North")) {
    switch (local_index) {
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
  } else if (orientation == BlockOrientation::get("Facing East")) {
    switch (local_index) {
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
  } else if (orientation == BlockOrientation::get("Facing West")) {
    switch (local_index) {
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
  return local_index;
}
