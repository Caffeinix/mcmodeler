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

#include "pencil_tool.h"

#include "block_instance.h"
#include "block_oracle.h"
#include "block_prototype.h"
#include "block_transaction.h"

PencilTool::PencilTool(BlockOracle* oracle) : oracle_(oracle) {}

bool PencilTool::wantsMorePositions() {
  return countPositions() == 0;
}

void PencilTool::draw(BlockPrototype* prototype, BlockOrientation* orientation, BlockTransaction* transaction) {
  if (wantsMorePositions()) {
    return;
  }

  for (int i = 0; i < countPositions(); ++i) {
    // TODO(phoenix): Should eventually draw lines between these points.
    const BlockPosition& point = positionAtIndex(i);
    BlockInstance new_block(prototype, point, orientation);
    transaction->replaceBlock(oracle_->blockAt(point), new_block);
  }
}
