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

#include "eraser_tool.h"

#include "block_instance.h"
#include "block_oracle.h"
#include "block_prototype.h"
#include "block_transaction.h"

EraserTool::EraserTool(BlockOracle* oracle) : oracle_(oracle) {}

QString EraserTool::actionName() const {
  return "Erase Blocks";
}

bool EraserTool::wantsMorePositions() {
  return countPositions() == 0;
}

bool EraserTool::isBrush() const {
  return true;
}

void EraserTool::draw(BlockPrototype* prototype, BlockOrientation* orientation, BlockTransaction* transaction) {
  if (wantsMorePositions()) {
    return;
  }

  for (int i = 0; i < countPositions(); ++i) {
    // TODO(phoenix): Should eventually draw lines between these points.
    const BlockPosition& point = positionAtIndex(i);
    transaction->clearBlock(oracle_->blockAt(point));
  }
}
