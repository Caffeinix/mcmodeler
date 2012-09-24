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

#include "tree_tool.h"

#include "block_oracle.h"
#include "block_manager.h"
#include "block_transaction.h"

TreeTool::TreeTool(BlockOracle* oracle, BlockManager* block_manager)
    : oracle_(oracle), block_manager_(block_manager) {}

QString TreeTool::actionName() const {
  return "Plant tree";
}
bool TreeTool::wantsMorePositions() {
  return countPositions() == 0;
}

bool TreeTool::isBrush() const {
  return false;
}

void TreeTool::draw(BlockPrototype* prototype, const BlockOrientation* orientation, BlockTransaction* transaction) {
  if (wantsMorePositions()) {
    return;
  }

  BlockPosition point = positionAtIndex(0);
  BlockInstance new_block(prototype, point, orientation);
  transaction->replaceBlock(oracle_->blockAt(point), new_block);

  point = point + BlockPosition(0, 1, 0);
  new_block = BlockInstance(prototype, point, orientation);
  transaction->replaceBlock(oracle_->blockAt(point), new_block);

  point = point + BlockPosition(0, 1, 0);
  new_block = BlockInstance(prototype, point, orientation);
  transaction->replaceBlock(oracle_->blockAt(point), new_block);

  point = point + BlockPosition(0, 1, 0);
  new_block = BlockInstance(prototype, point, orientation);
  transaction->replaceBlock(oracle_->blockAt(point), new_block);

  blocktype_t leaf_type = 0x12;  // Oak leaves by default.
  if ((prototype->type() & 0xFFFF) == 0x11 || (prototype->type() & 0xFFFF) == 0x05) {  // This tree is made of wood.
    leaf_type = (prototype->type() & 0xFF0000) + 0x12;  // Select the leaves for that type of wood.
  }
  BlockPrototype* leaf_prototype = block_manager_->getPrototype(leaf_type);

  BlockPosition leaf_point = point + BlockPosition(1, 0, 0);
  BlockInstance leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(-1, 0, 0);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(0, 0, 1);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(0, 0, -1);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  point = point + BlockPosition(0, 1, 0);
  new_block = BlockInstance(prototype, point, orientation);
  transaction->replaceBlock(oracle_->blockAt(point), new_block);

  leaf_point = point + BlockPosition(1, 0, 0);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(-1, 0, 0);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(0, 0, 1);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(0, 0, -1);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(1, 0, 1);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(1, 0, -1);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(-1, 0, 1);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(-1, 0, -1);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(2, 0, 0);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(-2, 0, 0);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(0, 0, 2);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(0, 0, -2);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  point = point + BlockPosition(0, 1, 0);
  leaves = BlockInstance(leaf_prototype, point, orientation);
  transaction->replaceBlock(oracle_->blockAt(point), leaves);

  leaf_point = point + BlockPosition(1, 0, 0);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(-1, 0, 0);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(0, 0, 1);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

  leaf_point = point + BlockPosition(0, 0, -1);
  leaves = BlockInstance(leaf_prototype, leaf_point, orientation);
  transaction->replaceBlock(oracle_->blockAt(leaf_point), leaves);

}
