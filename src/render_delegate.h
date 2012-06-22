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

#ifndef RENDER_DELEGATE_H
#define RENDER_DELEGATE_H

#include <QVector3D>

#include "enums.h"

class Renderable;

class RenderDelegate {
 public:
  virtual bool shouldRenderFace(const Renderable* renderable, Face face, const QVector3D& location) const = 0;
};

#endif // RENDER_DELEGATE_H
