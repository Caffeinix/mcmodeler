#include "basic_renderable.h"

#include "enums.h"

BasicRenderable::BasicRenderable(const QVector3D& size)
    : size_(size) {
}

BasicRenderable::~BasicRenderable() {}

void BasicRenderable::initialize() {
  Geometry geometry = createGeometry();
  TextureCoords texture_coords = createTextureCoords(geometry);
  geometry = moveToOrigin(geometry);
  addGeometry(geometry, texture_coords);
}

void BasicRenderable::appendVertex(const QVector3D& vertex,
                                   const QVector3D& normal,
                                   const QVector2D& tex_coord) {
  vertices_.append(vertex);
  normals_.append(normal);
  tex_coords_.append(tex_coord);
}

void BasicRenderable::addQuad(const QVector3D &a, const QVector3D &b,
                              const QVector3D &c, const QVector3D &d,
                              const QVector<QVector2D> &tex) {
  QVector3D norm = QVector3D::normal(a, b, c);
  appendVertex(a, norm, tex[kBottomLeftCorner]);
  appendVertex(b, norm, tex[kBottomRightCorner]);
  appendVertex(c, norm, tex[kTopRightCorner]);
  appendVertex(d, norm, tex[kTopLeftCorner]);
}

void BasicRenderable::applyOrientationTransform(const BlockOrientation* orientation) const {}

bool BasicRenderable::shouldRenderQuad(int index) const {
  return true;
}

Texture BasicRenderable::textureForQuad(int index) const {
  return texture(index);
}

void BasicRenderable::renderAt(const QVector3D& location, const BlockOrientation* orientation) const {
  glPushMatrix();
  glTranslatef(location.x(), location.y(), location.z());

  applyOrientationTransform(orientation);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertices().constData());
  glNormalPointer(GL_FLOAT, 0, normals().constData());
  glTexCoordPointer(2, GL_FLOAT, 0, textureCoords().constData());
  for (int start = 0; start < vertices().size(); start += 4) {
    if (!shouldRenderQuad(start / 4)) {
      continue;
    }
    glBindTexture(GL_TEXTURE_2D, textureForQuad(start / 4).textureId());
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
