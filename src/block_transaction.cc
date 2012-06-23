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

#include "block_transaction.h"

#include "block_instance.h"
#include "block_position.h"

BlockTransaction::BlockTransaction() {
}

void BlockTransaction::setBlock(const BlockInstance& new_block) {
  if (new_block.prototype()->type() != kBlockTypeAir &&
      !new_positions_.contains(new_block.position())) {
    new_blocks_.append(new_block);
    new_positions_.insert(new_block.position());
  }
}

void BlockTransaction::replaceBlock(const BlockInstance& old_block, const BlockInstance& new_block) {
  Q_ASSERT(old_block.position() == new_block.position());
  clearBlock(old_block);
  setBlock(new_block);
}

void BlockTransaction::clearBlock(const BlockInstance& old_block) {
  if (old_block.prototype()->type() != kBlockTypeAir &&
      !old_positions_.contains(old_block.position())) {
    old_blocks_.append(old_block);
    old_positions_.insert(old_block.position());
  }
}
