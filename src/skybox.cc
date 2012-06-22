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

#include "skybox.h"

#include <QVector>

enum { BOTTOM_LEFT, BOTTOM_RIGHT, TOP_RIGHT, TOP_LEFT };

void Skybox::setTexture(Face face, GLuint tex_id) {
  textures_[face] = tex_id;
}

void Skybox::appendVertex(const QVector3D &vertex, const QVector3D &normal, const QVector2D &tex_coord) {
    vertices.append(vertex);
    normals.append(normal);
    texCoords.append(tex_coord);
}

void Skybox::addQuad(const QVector3D &a, const QVector3D &b,
                    const QVector3D &c, const QVector3D &d,
                    const QVector<QVector2D> &tex) {
    QVector3D norm = QVector3D::normal(a, b, c);
    appendVertex(a, norm, tex[0]);
    appendVertex(b, norm, tex[1]);
    appendVertex(c, norm, tex[2]);
    appendVertex(d, norm, tex[3]);
}

void Skybox::render() const {
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertices.constData());
  glNormalPointer(GL_FLOAT, 0, normals.constData());
  glTexCoordPointer(2, GL_FLOAT, 0, texCoords.constData());
  for (int start = 0; start < 24; start += 4) {
    glBindTexture(GL_TEXTURE_2D, textures_[start / 4]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLushort indices[4] = { start, start + 1, start + 2, start + 3 };
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, indices);
  }
}

Skybox::Skybox(float size) {
  QVector<QVector3D> verts;
  verts.resize(4);
  verts[BOTTOM_RIGHT].setX(size);
  verts[TOP_RIGHT].setX(size);
  verts[TOP_RIGHT].setY(size);
  verts[TOP_LEFT].setY(size);
  textures_.resize(6);

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
  for (int i = 0; i < 4; ++i)
      verts[i] -= QVector3D(size / 2.0f, size / 2.0f, -size / 2.0f);

  // add the front face
  addQuad(verts[BOTTOM_LEFT], verts[BOTTOM_RIGHT], verts[TOP_RIGHT], verts[TOP_LEFT], tex);

//  for (int i = 0; i < 4; ++i)
//      verts[i].setZ(size/*size / 2.0f*/);
  // back face - "extrude" verts down
  QVector<QVector3D> back(verts);
  for (int i = 0; i < 4; ++i)
      back[i].setZ(-size / 2.0f);

  // add the back face
  addQuad(back[BOTTOM_RIGHT], back[BOTTOM_LEFT], back[TOP_LEFT], back[TOP_RIGHT], tex);

  // add the sides
  addQuad(back[BOTTOM_LEFT], back[BOTTOM_RIGHT], verts[BOTTOM_RIGHT], verts[BOTTOM_LEFT], bottom_tex);       // Bottom
  addQuad(back[BOTTOM_RIGHT], back[TOP_RIGHT], verts[TOP_RIGHT], verts[BOTTOM_RIGHT], right_tex);      // Right
  addQuad(back[TOP_RIGHT], back[TOP_LEFT], verts[TOP_LEFT], verts[TOP_RIGHT], tex);                   // Top
  addQuad(back[TOP_LEFT], back[BOTTOM_LEFT], verts[BOTTOM_LEFT], verts[TOP_LEFT], left_tex);          // Left
}
