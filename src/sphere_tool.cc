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
 *
 * Portions Copyright Alois Zingl, Vienna, Austria
 */

#include "sphere_tool.h"

#include "block_instance.h"
#include "block_oracle.h"
#include "block_prototype.h"
#include "block_transaction.h"

SphereTool::SphereTool(BlockOracle* oracle) : oracle_(oracle) {}

QString SphereTool::actionName() const {
  return "Draw Sphere";
}

bool SphereTool::wantsMorePositions() {
  return countPositions() < 2;
}

bool SphereTool::isBrush() const {
  return false;
}

void SphereTool::setPixel(int x, int y, int z, BlockPrototype* prototype, const BlockOrientation* orientation, BlockTransaction* transaction) {
  BlockPosition pos(x, y, z);
  BlockInstance new_block(prototype, pos, orientation);
  transaction->replaceBlock(oracle_->blockAt(pos), new_block);
}

void SphereTool::draw(BlockPrototype* prototype, const BlockOrientation* orientation, BlockTransaction* transaction) {
  if (wantsMorePositions()) {
    return;
  }

  Q_ASSERT(positionAtIndex(0).y() == positionAtIndex(1).y());

  BlockPosition start = positionAtIndex(0);
  BlockPosition end = positionAtIndex(1);
  double size = qMax(qAbs(end.x() - start.x()), qAbs(end.z() - start.z()));
  double radius = size / 2.0;
  double x_increment = (end.x() < start.x() ? -1 : 1);
  double z_increment = (end.z() < start.z() ? -1 : 1);
  QVector3D center = start.centerVector() + QVector3D(radius * x_increment, radius, radius * z_increment);
  for (int z = 0; z <= size; ++z) {
    for (int y = 0; y <= size; ++y) {
      for (int x = 0; x <= size; ++x) {
        BlockPosition pos(start.x() + x * x_increment, start.y() + y, start.z() + z * z_increment);
        QVector3D distance = pos.centerVector() - center;
        if (qAbs(radius - distance.length()) < 0.5) {
          transaction->replaceBlock(oracle_->blockAt(pos), BlockInstance(prototype, pos, orientation));
        }
      }
    }
  }
}
