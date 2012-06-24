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
