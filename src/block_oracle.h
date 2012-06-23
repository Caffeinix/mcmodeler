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

#ifndef BLOCK_ORACLE_H
#define BLOCK_ORACLE_H

#include "block_instance.h"

class BlockPrototype;
class BlockInstance;

/**
  * An interface describing a class that can return a BlockInstance for any BlockPosition in the world.
  */
class BlockOracle {
 public:
  /**
    * Returns the BlockInstance at \p position.  If there is no block there, the block type of the returned instance
    * will be kBlockTypeAir.  The position of the instance will always equal \p position.
    */
  virtual BlockInstance blockAt(const BlockPosition& position) = 0;
};

#endif // BLOCK_ORACLE_H
