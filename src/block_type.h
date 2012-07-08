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

#ifndef BLOCK_TYPE_H
#define BLOCK_TYPE_H

#include <QtGlobal>  // For quint32

/**
  * Represents a block type, aka a block ID.  These mostly correspond to the Minecraft block IDs, but are 32 bits long.
  * The bottom 16 bits represent the Minecraft block ID (though most of those are unused in Minecraft at present).
  * The next four bits represent the Minecraft block data.  The next four bits represent MCModeler-specific block
  * data which does not exist in Minecraft (for instance whether the block is the top or bottom half of a bed).
  * The upper eight bits are currently unused and should always be 0.
  */
typedef qint32 blocktype_t;

/**
  * Special block type ID for "air", or empty space.  This is used whenever it is necessary to generate a BlockPrototype
  * for a block that does not exist.
  */
const blocktype_t kBlockTypeAir = -1;

/**
  * Special block type ID for the "unknown block".  This is used to indicate that no block type is currently selected
  * in the BlockPicker.
  */
const blocktype_t kBlockTypeUnknown = -2;

#endif // BLOCK_TYPE_H
