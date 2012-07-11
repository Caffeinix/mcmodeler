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

#ifndef TRACK_RENDERABLE_H
#define TRACK_RENDERABLE_H

#include "basic_renderable.h"

class TrackRenderable : public BasicRenderable {
 public:
  TrackRenderable();
  virtual ~TrackRenderable();

 protected:
  virtual Geometry createGeometry();
  virtual TextureCoords createTextureCoords(const Geometry& geometry);
  virtual Geometry moveToOrigin(const Geometry& geometry);
  virtual void addGeometry(const Geometry& geometry, const TextureCoords& texture_coords);
  virtual void applyOrientationTransform(const BlockOrientation* orientation) const;
  virtual bool shouldRenderQuad(int index, const QVector3D& location, const BlockOrientation* orientation) const;
  virtual Texture textureForQuad(int index, const BlockOrientation* orientation) const;
};

#endif // TRACK_RENDERABLE_H
