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

#ifndef FLOOD_FILL_TOOL_H
#define FLOOD_FILL_TOOL_H

#include "block_type.h"
#include "tool.h"

class BlockOracle;

class FloodFillTool : public Tool {
 public:
  FloodFillTool(BlockOracle* oracle);
  virtual ~FloodFillTool();

  virtual QString actionName() const;
  virtual bool wantsMorePositions();
  virtual bool isBrush() const;
  virtual void draw(BlockPrototype* prototype, BlockOrientation* orientation, BlockTransaction* transaction);

 private:
  /**
    * Recursion helper function for draw().
    * @param pos The current position we are filling.
    * @param source_type The type of the block we should replace.
    * @param dest_prototype The prototype of the block we should replace it with.
    * @param dest_orientation The orientation we should set newly created blocks to.
    * @param start_pos The point at which the initial fillBlocks began.
    * @param depth Recursive stack depth.  Used for sanity checking to avoid stack overflow.
    * @param filled_blocks The set of all blocks we have filled so far.  This is shared among all the recursive helpers.
    * @param transaction The transaction we are building which, when committed, will perform the flood fill.
    */
  void fillBlocksRecurse(const BlockPosition& pos,
                         const blocktype_t& source_type,
                         BlockPrototype* dest_prototype,
                         BlockOrientation* dest_orientation,
                         const BlockPosition& start_pos,
                         int depth,
                         QSet<BlockPosition>* filled_blocks,
                         BlockTransaction* transaction);

  BlockOracle* oracle_;
};

#endif // FLOOD_FILL_TOOL_H
