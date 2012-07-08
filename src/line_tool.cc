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

#include "line_tool.h"

#include "block_instance.h"
#include "block_oracle.h"
#include "block_prototype.h"
#include "block_transaction.h"

LineTool::LineTool(BlockOracle* oracle) : oracle_(oracle) {}

bool LineTool::isBrush() const {
  return true;
}

bool LineTool::wantsMorePositions() {
  return countPositions() < 2;
}

void LineTool::draw(BlockPrototype* prototype, BlockOrientation* orientation, BlockTransaction* transaction) {
  if (wantsMorePositions()) {
    return;
  }

  const BlockPosition& start = positionAtIndex(0);
  const BlockPosition& end = positionAtIndex(1);

  Q_ASSERT(start.y() == end.y());

  // What follows is an implementation of Bresenham's line algorithm.  See
  // http://en.wikipedia.org/wiki/Bresenham's_line_algorithm.

  int x1 = start.x();
  int y1 = start.z();

  int x2 = end.x();
  int y2 = end.z();

  // Flip start and end if line extends to the left.
  if (start.x() > end.x()) {
    x1 = end.x();
    y1 = end.z();

    x2 = start.x();
    y2 = start.z();
  }

  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);

  // Increment if line extends downwards, otherwise decrement.
  int inc_dec = (y2 >= y1 ? 1 : -1);

  BlockPosition pos(start);

  // Different algorithm depending on whether slope is less or greater than 1.
  if (dx > dy) {
    // Multiply everything by two to avoid non-integral values.
    int two_dy = 2 * dy;
    int two_dy_dx = 2 * (dy - dx);
    int diff = (2 * dy) - dx;

    int x = x1;
    int y = y1;

    pos = BlockPosition(x, pos.y(), y);
    BlockInstance new_block(prototype, pos, orientation);
    transaction->replaceBlock(oracle_->blockAt(pos), new_block);

    while (x < x2) {
      ++x;

      if (diff < 0) {
        // Midpoint is above line.
        diff += two_dy;
      } else {
        // Midpoint is below line.
        y += inc_dec;
        diff += two_dy_dx;
      }

      pos = BlockPosition(x, pos.y(), y);
      BlockInstance new_block(prototype, pos, orientation);
      transaction->replaceBlock(oracle_->blockAt(pos), new_block);
    }
  } else {
    // Multiply everything by two to avoid non-integral values.
    int two_dx = 2 * dx;
    int two_dx_dy = 2 * (dx - dy);
    int diff = (2 * dx) - dy;

    int x = x1;
    int y = y1;

    pos = BlockPosition(x, pos.y(), y);
    BlockInstance new_block(prototype, pos, orientation);
    transaction->replaceBlock(oracle_->blockAt(pos), new_block);

    while (y != y2) {  // This is safe since y will never change by more than one block per iteration.
      y += inc_dec;

      if (diff < 0) {
        // Midpoint is above line.
        diff += two_dx;
      } else {
        // Midpoint is below line.
        ++x;
        diff += two_dx_dy;
      }

      pos = BlockPosition(x, pos.y(), y);
      BlockInstance new_block(prototype, pos, orientation);
      transaction->replaceBlock(oracle_->blockAt(pos), new_block);
    }
  }
}
