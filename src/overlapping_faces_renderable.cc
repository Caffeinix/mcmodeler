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

OverlappingFacesRenderable::OverlappingFacesRenderable(float size, QVector3D overlap) {
  QVector<QVector3D> verts;
  verts.resize(4);
  verts[kBottomRightCorner].setX(size);
  verts[kTopRightCorner].setX(size);
  verts[kTopRightCorner].setY(size);
  verts[kTopLeftCorner].setY(size);

  // these vert numbers are good for the tex-coords
  QVector<QVector2D> tex;
  tex.resize(4);
  for (int i = 0; i < 4; ++i) {
    tex[i] = verts[i].toVector2D();
    if (tex[i].x() != 0.0f) {
      tex[i].setX(1.0f);
    }
    if (tex[i].y() != 0.0f) {
      tex[i].setY(1.0f);
    }
  }

  QVector<QVector2D> right_tex;
  right_tex.resize(4);
  right_tex[0] = verts[1].toVector2D();
  right_tex[1] = verts[2].toVector2D();
  right_tex[2] = verts[3].toVector2D();
  right_tex[3] = verts[0].toVector2D();
  for (int i = 0; i < 4; ++i) {
    if (right_tex[i].x() != 0.0f) {
      right_tex[i].setX(1.0f);
    }
    if (right_tex[i].y() != 0.0f) {
      right_tex[i].setY(1.0f);
    }
  }

  QVector<QVector2D> bottom_tex;
  bottom_tex.resize(4);
  bottom_tex[0] = verts[2].toVector2D();
  bottom_tex[1] = verts[3].toVector2D();
  bottom_tex[2] = verts[0].toVector2D();
  bottom_tex[3] = verts[1].toVector2D();
  for (int i = 0; i < 4; ++i) {
    if (bottom_tex[i].x() != 0.0f) {
      bottom_tex[i].setX(1.0f);
    }
    if (bottom_tex[i].y() != 0.0f) {
      bottom_tex[i].setY(1.0f);
    }
  }


  QVector<QVector2D> left_tex;
  left_tex.resize(4);
  left_tex[0] = verts[3].toVector2D();
  left_tex[1] = verts[0].toVector2D();
  left_tex[2] = verts[1].toVector2D();
  left_tex[3] = verts[2].toVector2D();
  for (int i = 0; i < 4; ++i) {
    if (left_tex[i].x() != 0.0f) {
      left_tex[i].setX(1.0f);
    }
    if (left_tex[i].y() != 0.0f) {
      left_tex[i].setY(1.0f);
    }
  }

  // now move verts half cube width across so cube is centered on origin
  for (int i = 0; i < 4; ++i) {
    verts[i] -= QVector3D(size / 2.0f, size / 2.0f, -size / 2.0f);
  }

  // add the front face
  addQuad(verts[kBottomLeftCorner] + QVector3D(0, 0, -overlap.z()),
          verts[kBottomRightCorner] + QVector3D(0, 0, -overlap.z()),
          verts[kTopRightCorner] + QVector3D(0, 0, -overlap.z()),
          verts[kTopLeftCorner] + QVector3D(0, 0, -overlap.z()), tex);

  // back face - "extrude" verts down
  QVector<QVector3D> back(verts);
  for (int i = 0; i < 4; ++i) {
    back[i].setZ(-size / 2.0f);
  }

  // add the back face
  addQuad(back[kBottomRightCorner] + QVector3D(0, 0, overlap.z()),
          back[kBottomLeftCorner] + QVector3D(0, 0, overlap.z()),
          back[kTopLeftCorner] + QVector3D(0, 0, overlap.z()),
          back[kTopRightCorner] + QVector3D(0, 0, overlap.z()), tex);

  // add the sides
  addQuad(back[kBottomLeftCorner] + QVector3D(0, overlap.y(), 0),
          back[kBottomRightCorner] + QVector3D(0, overlap.y(), 0),
          verts[kBottomRightCorner] + QVector3D(0, overlap.y(), 0),
          verts[kBottomLeftCorner] + QVector3D(0, overlap.y(), 0),
          bottom_tex); // Bottom
  addQuad(back[kBottomRightCorner] + QVector3D(-overlap.x(), 0, 0),
          back[kTopRightCorner] + QVector3D(-overlap.x(), 0, 0),
          verts[kTopRightCorner] + QVector3D(-overlap.x(), 0, 0),
          verts[kBottomRightCorner] + QVector3D(-overlap.x(), 0, 0),
          right_tex);      // Right
  addQuad(back[kTopRightCorner] + QVector3D(0, -overlap.y(), 0),
          back[kTopLeftCorner] + QVector3D(0, -overlap.y(), 0),
          verts[kTopLeftCorner] + QVector3D(0, -overlap.y(), 0),
          verts[kTopRightCorner] + QVector3D(0, -overlap.y(), 0),
          tex);                    // Top
  addQuad(back[kTopLeftCorner] + QVector3D(overlap.x(), 0, 0),
          back[kBottomLeftCorner] + QVector3D(overlap.x(), 0, 0),
          verts[kBottomLeftCorner] + QVector3D(overlap.x(), 0, 0),
          verts[kTopLeftCorner] + QVector3D(overlap.x(), 0, 0),
          left_tex);           // Left
}

void OverlappingFacesRenderable::appendVertex(const QVector3D &vertex, const QVector3D &normal, const QVector2D &tex_coord) {
  vertices_.append(vertex);
  normals_.append(normal);
  tex_coords_.append(tex_coord);
}

void OverlappingFacesRenderable::addQuad(const QVector3D &a, const QVector3D &b,
                    const QVector3D &c, const QVector3D &d,
                    const QVector<QVector2D> &tex) {
  QVector3D norm = QVector3D::normal(a, b, c);
  appendVertex(a, norm, tex[0]);
  appendVertex(b, norm, tex[1]);
  appendVertex(c, norm, tex[2]);
  appendVertex(d, norm, tex[3]);
}

void OverlappingFacesRenderable::renderAt(const QVector3D& location, const BlockOrientation* orientation) const {
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
    glBindTexture(GL_TEXTURE_2D, texture(start / 4).textureId());
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
