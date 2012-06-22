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

class BlockOracle;
class BlockPrototype;
class QGLWidget;

class BlockManager {
 public:
  BlockManager(BlockOracle* oracle, QGLWidget* widget);
  ~BlockManager();

  BlockPrototype* getPrototype(BlockType type) const;

 private:
  mutable QHash<BlockType, BlockPrototype*> blocks_;
  BlockOracle* oracle_;
  QGLWidget* widget_;
};

#endif // BLOCK_MANAGER_H
