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

#ifndef PENCIL_TOOL_H
#define PENCIL_TOOL_H

#include "block_position.h"
#include "tool.h"

class BlockOracle;

/**
  * A Tool that draws individual blocks.
  */
class PencilTool : public Tool {
 public:
  explicit PencilTool(BlockOracle* oracle);
  virtual QString actionName() const;
  virtual bool wantsMorePositions();
  virtual bool isBrush() const;
  virtual void draw(BlockPrototype* prototype, const BlockOrientation* orientation, BlockTransaction* transaction);

 private:
  BlockOracle* oracle_;
};

#endif // PENCIL_TOOL_H
