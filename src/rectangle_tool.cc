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

#include "rectangle_tool.h"

#include "block_instance.h"
#include "block_oracle.h"
#include "block_prototype.h"
#include "block_transaction.h"

RectangleTool::RectangleTool(BlockOracle* oracle) : oracle_(oracle) {}

QString RectangleTool::actionName() const {
  return "Draw Rectangle";
}

bool RectangleTool::wantsMorePositions() {
  return countPositions() < 2;
}

bool RectangleTool::isBrush() const {
  return false;
}

void RectangleTool::draw(BlockPrototype* prototype, const BlockOrientation* orientation, BlockTransaction* transaction) {
  if (wantsMorePositions()) {
    return;
  }

  Q_ASSERT(positionAtIndex(0).y() == positionAtIndex(1).y());

  BlockPosition start(qMin(positionAtIndex(0).x(), positionAtIndex(1).x()),
                      positionAtIndex(0).y(),
                      qMin(positionAtIndex(0).z(), positionAtIndex(1).z()));
  BlockPosition end(qMax(positionAtIndex(0).x(), positionAtIndex(1).x()),
                    positionAtIndex(0).y(),
                    qMax(positionAtIndex(0).z(), positionAtIndex(1).z()));

  for (int x = start.x(); x <= end.x(); ++x) {
    {
      BlockPosition pos(x, start.y(), start.z());
      BlockInstance new_block(prototype, pos, orientation);
      transaction->replaceBlock(oracle_->blockAt(pos), new_block);
    }

    BlockPosition pos(x, start.y(), end.z());
    BlockInstance new_block(prototype, pos, orientation);
    transaction->replaceBlock(oracle_->blockAt(pos), new_block);
  }

  for (int z = start.z(); z <= end.z(); ++z) {
    {
      BlockPosition pos(start.x(), start.y(), z);
      BlockInstance new_block(prototype, pos, orientation);
      transaction->replaceBlock(oracle_->blockAt(pos), new_block);
    }

    BlockPosition pos(end.x(), start.y(), z);
    BlockInstance new_block(prototype, pos, orientation);
    transaction->replaceBlock(oracle_->blockAt(pos), new_block);
  }
}
