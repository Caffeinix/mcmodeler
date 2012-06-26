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

#include "block_manager.h"

#include "block_prototype.h"
#include "block_type.h"

BlockManager::BlockManager(BlockOracle* oracle, QGLWidget* widget)
    : oracle_(oracle), widget_(widget) {
  // Load textures.
  default_texture_pack_.reset(TexturePack::createDefaultTexturePack());
}

BlockManager::~BlockManager() {
  qDeleteAll(blocks_);
}

BlockPrototype* BlockManager::getPrototype(blocktype_t type) const {
  if (type >= BlockPrototype::blockCount()) {
    qWarning() << "Tried to create a block prototype for block type" << type
               << "but we only know about" << BlockPrototype::blockCount() << "blocks!";
    type = 0;
  }
  BlockPrototype* block = blocks_.value(type);
  if (block) {
    return block;
  } else {
    block = new BlockPrototype(type, default_texture_pack_.data(), oracle_, widget_);
    blocks_.insert(type, block);
    return block;
  }
}
