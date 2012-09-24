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

#include <stdlib.h>
#include <time.h>

#include "block_instance.h"
#include "block_oracle.h"
#include "block_manager.h"
#include "block_transaction.h"

double sampleNormal() {
  double u = static_cast<double>(qrand()) / static_cast<double>(RAND_MAX) * 2.0 - 1.0;
  double v = static_cast<double>(qrand()) / static_cast<double>(RAND_MAX) * 2.0 - 1.0;
  double r = u * u + v * v;
  if (r == 0 || r > 1) return sampleNormal();
  double c = sqrt(-2 * log(r) / r);
  return u * c;
}

// Evaluates to a random number uniformly distributed in the range [min, max] (inclusive).
#define UNI_RANGE(min, max) (qrand() % (((max) + 1) - (min)) + (min))

// Evaluates to a random number normally distributed in the range [min, max] (inclusive).
#define NORM_RANGE(min, max) \
        qBound(min, qRound((sampleNormal() * ((max) - (min)) / 2) + (((min) + ((max) - (min)) / 2))), max)

TreeTool::TreeTool(BlockOracle* oracle, BlockManager* block_manager)
    : oracle_(oracle), block_manager_(block_manager) {
  randomizeTree();
}

QString TreeTool::actionName() const {
  return "Plant tree";
}
bool TreeTool::wantsMorePositions() {
  return countPositions() == 0;
}

bool TreeTool::isBrush() const {
  return false;
}

void TreeTool::clear() {
  Tool::clear();
  randomizeTree();
}

void TreeTool::draw(BlockPrototype* prototype, const BlockOrientation* orientation, BlockTransaction* transaction) {
  if (wantsMorePositions()) {
    return;
  }

  qsrand(foliage_seed_);

  // Draw trunk.
  for (int i = 0; i < trunk_height_; ++i) {
    BlockPosition point = positionAtIndex(0) + BlockPosition(0, i, 0);
    BlockInstance trunk_block(prototype, point, orientation);
    transaction->replaceBlock(oracle_->blockAt(point), trunk_block);
  }

  blocktype_t leaf_type = 0x12;  // Oak leaves by default.
  if ((prototype->type() & 0xFFFF) == 0x11 || (prototype->type() & 0xFFFF) == 0x05) {  // This tree is made of wood.
    leaf_type = (prototype->type() & 0xFF0000) + 0x12;  // Select the leaves for that type of wood.
  }
  BlockPrototype* leaf_prototype = block_manager_->getPrototype(leaf_type);

  BlockPosition canopy_center = positionAtIndex(0) + BlockPosition(0, trunk_height_ - canopy_offset_, 0);
  for (int x = canopy_center.x() - clip_radius_; x <= canopy_center.x() + clip_radius_; ++x) {
    for (int y = canopy_center.y(); y <= canopy_center.y() + canopy_radius_; ++y) {
      for (int z = canopy_center.z() - clip_radius_; z <= canopy_center.z() + clip_radius_; ++z) {
        BlockPosition point(x, y, z);
        // Don't overwrite existing blocks.
        if (!oracle_->blockAt(point).prototype()->type() == kBlockTypeAir) {
          continue;
        }
        float squared_distance = (point.centerVector() - canopy_center.centerVector()).lengthSquared();
        if (squared_distance <= canopy_radius_ * canopy_radius_) {
          // Only compute actual distance if we might draw the leaves.
          float distance = (point.centerVector() - canopy_center.centerVector()).length();
          if (distance < canopy_radius_ || qrand() % 2 != 0) {
            // If the leaf block is on the periphery of the tree, only draw it with 50% probability.
            BlockInstance leaf_block(leaf_prototype, point, orientation);
            transaction->replaceBlock(oracle_->blockAt(point), leaf_block);
          }
        }
      }
    }
  }
  qsrand(time(NULL));
}

void TreeTool::randomizeTree() {
  qDebug() << sampleNormal();
  trunk_height_ = NORM_RANGE(3, 5);
  canopy_radius_ = NORM_RANGE(2, 4) + (trunk_height_ - 3);
  clip_radius_ = canopy_radius_ - 1;
  canopy_offset_ = NORM_RANGE(1, 2);
  foliage_seed_ = time(NULL);
}
