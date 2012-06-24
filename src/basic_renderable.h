#ifndef BASIC_RENDERABLE_H
#define BASIC_RENDERABLE_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>

#include "renderable.h"

class BasicRenderable : public Renderable {
 public:
  explicit BasicRenderable(const QVector3D& size);
  virtual ~BasicRenderable();

  virtual void initialize();

 protected:
  typedef QVector< QVector<QVector3D> > Geometry;
  typedef QVector< QVector<QVector2D> > TextureCoords;

  virtual Geometry createGeometry() = 0;
  virtual TextureCoords createTextureCoords(const Geometry& geometry) = 0;
  virtual Geometry moveToOrigin(const Geometry& geometry) = 0;
  virtual void addGeometry(const Geometry& geometry, const TextureCoords& texture_coords) = 0;

  const QVector3D& size() const {
    return size_;
  }

  void appendVertex(const QVector3D &vertex,
                    const QVector3D &normal,
                    const QVector2D &tex_coord);

  void addQuad(const QVector3D &a, const QVector3D &b,
               const QVector3D &c, const QVector3D &d,
               const QVector<QVector2D> &tex);

  const QVector<QVector3D>& vertices() const {
    return vertices_;
  }
  const QVector<QVector3D>& normals() const {
    return normals_;
  }
  const QVector<QVector2D>& textureCoords() const {
    return tex_coords_;
  }

 private:
  QVector3D size_;
  QVector<QVector3D> vertices_;
  QVector<QVector3D> normals_;
  QVector<QVector2D> tex_coords_;
};

#endif // BASIC_RENDERABLE_H
