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

#include "flood_fill_tool.h"

#include "block_oracle.h"
#include "block_transaction.h"

static const int kMaxFillDistance = 64;
static const int kPreviewMaxFillDistance = 32;

FloodFillTool::FloodFillTool(BlockOracle* oracle) : oracle_(oracle) {
}

FloodFillTool::~FloodFillTool() {}

QString FloodFillTool::actionName() const {
  return "Flood Fill";
}

bool FloodFillTool::wantsMorePositions() {
  return countPositions() < 1;
}

bool FloodFillTool::isBrush() const {
  return false;
}

void FloodFillTool::draw(BlockPrototype* prototype, const BlockOrientation* orientation, BlockTransaction* transaction) {
  if (countPositions() < 1) {
    return;
  }

  BlockPosition start_pos = positionAtIndex(0);
  BlockInstance start_block = oracle_->blockAt(start_pos);
  const blocktype_t source_type = start_block.prototype()->type();
  QSet<BlockPosition> filled_blocks;
  fillBlocksRecurse(start_pos, source_type, prototype, orientation, start_pos, 1, &filled_blocks, transaction);
}

void FloodFillTool::fillBlocksRecurse(const BlockPosition& pos,
                                      const blocktype_t& source_type,
                                      BlockPrototype* dest_prototype,
                                      const BlockOrientation* dest_orientation,
                                      const BlockPosition& start_pos,
                                      int depth,
                                      QSet<BlockPosition>* filled_blocks,
                                      BlockTransaction* transaction) {
  if (filled_blocks->contains(pos)) {
    // We've already filled this block.
    return;
  }
  int fill_distance = kMaxFillDistance;
  if (state() == kProposed) {
    fill_distance = kPreviewMaxFillDistance;
  }
  if (qAbs(pos.x() - start_pos.x()) > fill_distance ||
      qAbs(pos.z() - start_pos.z()) > fill_distance) {
    // This is unlikely to be a bounded area, so return to prevent stack overflow.
    return;
  }
  if (depth > 8192) {
    // Generic runaway recursion catcher.
    return;
  }
  BlockInstance old_block = oracle_->blockAt(pos);
  if (old_block.prototype()->type() != source_type) {
    return;
  }
  if (dest_prototype->type() == kBlockTypeAir) {
    transaction->clearBlock(old_block);
  } else {
    BlockInstance new_block(dest_prototype, pos, dest_orientation);
    transaction->replaceBlock(old_block, new_block);
  }
  filled_blocks->insert(pos);

  // TODO: this hard-codes top-down orientation again.
  fillBlocksRecurse(pos + BlockPosition(1, 0, 0), source_type, dest_prototype, dest_orientation,
                    start_pos, depth, filled_blocks, transaction);
  fillBlocksRecurse(pos + BlockPosition(-1, 0, 0), source_type, dest_prototype, dest_orientation,
                    start_pos, depth, filled_blocks, transaction);
  fillBlocksRecurse(pos + BlockPosition(0, 0, 1), source_type, dest_prototype, dest_orientation,
                    start_pos, depth, filled_blocks, transaction);
  fillBlocksRecurse(pos + BlockPosition(0, 0, -1), source_type, dest_prototype, dest_orientation,
                    start_pos, depth, filled_blocks, transaction);
}
