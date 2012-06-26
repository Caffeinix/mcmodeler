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

#ifndef BLOCK_MANAGER_H
#define BLOCK_MANAGER_H

#include <QHash>
#include <QScopedPointer>

#include "block_type.h"
#include "texture_pack.h"

class BlockOracle;
class BlockPrototype;
class TexturePack;
class QGLWidget;

/**
  * Manages BlockPrototype objects.  To the the prototype for a particular block type, call getPrototype().
  * @warning There should only be one BlockManager in the application.  If you create more than one, you will end up
  * with duplicate prototypes for different block types and lots of things will break.  The BlockManager is owned by
  * the Application.
  */
class BlockManager {
 public:
  /**
    * Constructs a new BlockManager.  Do not create multiple BlockManagers; the canonical instance is owned by
    * Application.
    */
  BlockManager(BlockOracle* oracle, QGLWidget* widget);

  ~BlockManager();

  /**
    * Gets the BlockPrototype for \p type.  The same pointer will be returned each time, and will be created on demand
    * if it does not exist.
    * @note This method does _not_ pass ownership of the prototype to the caller, so don't delete it!
    */
  BlockPrototype* getPrototype(blocktype_t type) const;

 private:
  mutable QHash<blocktype_t, BlockPrototype*> blocks_;
  BlockOracle* oracle_;
  QGLWidget* widget_;
  QScopedPointer<TexturePack> default_texture_pack_;
};

#endif // BLOCK_MANAGER_H
