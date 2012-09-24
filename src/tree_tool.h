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

#ifndef TREE_TOOL_H
#define TREE_TOOL_H

#include "tool.h"

class BlockManager;
class BlockOracle;

class TreeTool : public Tool {
 public:
  explicit TreeTool(BlockOracle* oracle, BlockManager* block_manager);

  virtual QString actionName() const;
  virtual bool wantsMorePositions();
  virtual bool isBrush() const;
  virtual void clear();
  virtual void draw(BlockPrototype* prototype, const BlockOrientation* orientation, BlockTransaction* transaction);

 private:
  void randomizeTree();
  BlockOracle* oracle_;
  BlockManager* block_manager_;

  int trunk_height_;
  int canopy_radius_;
  int clip_radius_;
  int canopy_offset_;
  int foliage_seed_;
};

#endif // TREE_TOOL_H
