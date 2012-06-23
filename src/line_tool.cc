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

LineTool::LineTool(BlockOracle* oracle) : oracle_(oracle), start_(0, 0, 0), end_(0, 0, 0) {}

void LineTool::setPositionAtIndex(int index, const BlockPosition& position) {
  switch (index) {
  case 0:
    start_ = position;
    break;
  case 1:
    end_ = position;
    break;
  }
}

void LineTool::draw(BlockPrototype* prototype, BlockOrientation* orientation, BlockTransaction* transaction) {
  Q_ASSERT(start_.y() == end_.y());

  int dx = qAbs(end_.x() - start_.x());
  int dz = qAbs(end_.z() - start_.z());
  int sx = (start_.x() < end_.x() ? 1 : -1);
  int sz = (start_.z() < end_.z() ? 1 : -1);
  int err = dx - dz;
  BlockPosition pos(start_);

  forever {
    BlockInstance new_block(prototype, pos, orientation);
    transaction->replaceBlock(oracle_->blockAt(pos), new_block);

    // TODO(phoenix): There is a bug here somewhere.  Occasionally this
    // never obtains and we loop infinitely!
    if (pos == end_) {
      break;
    }

    if (err * 2 > -dz) {
      err -= dz;
      pos = BlockPosition(pos.x() + sx, pos.y(), pos.z());
    }
    if (err * 2 < dx) {
      err += dx;
      pos = BlockPosition(pos.x(), pos.y(), pos.z() + sz);
    }
  }
}
