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

#include "flow_block_renderable.h"

#include "block_orientation.h"
#include "rectangular_prism_renderable.h"
#include "render_delegate.h"

FlowBlockRenderable::FlowBlockRenderable() : Renderable() {
}

FlowBlockRenderable::~FlowBlockRenderable() {
  qDeleteAll(renderables_.values());
}

void FlowBlockRenderable::initialize() {
}

void FlowBlockRenderable::renderAt(const QVector3D& location, const BlockOrientation* orientation) const {
  if (renderables_.isEmpty()) {
    // First time we have been rendered, so set up our delegate renderables.
    Q_ASSERT(renderDelegate() != NULL);
    QVector<BlockOrientation*> orientations = renderDelegate()->orientations();
    int len = orientations.size();
    for (int i = 0; i < len; ++i) {
      QVector3D size(1.0, 1.0, 1.0);
      // Block height varies between 15/16 and 1/16 in steps depending on the number of orientations.
      size.setY(1.0 - static_cast<qreal>(i + 1) / static_cast<qreal>(len + 1));
      RectangularPrismRenderable* renderable = new RectangularPrismRenderable(size);
      renderable->initialize();
      renderable->setRenderDelegate(renderDelegate());

      // This assumes contiguous texture ids, which we know to be true for us in particular.
      for (int tex_id = 0; tex_id < textureCount(); ++tex_id) {
        renderable->setTexture(tex_id, texture(tex_id));
      }

      renderables_.insert(renderDelegate()->orientations().at(i), renderable);
    }
  }
  Renderable* delegate_renderable = renderables_.value(orientation, NULL);
  if (delegate_renderable) {
    delegate_renderable->renderAt(location, orientation);
  } else {
    qWarning() << __PRETTY_FUNCTION__ << "No delegate renderable found for orientation" << orientation->name();
  }
}
