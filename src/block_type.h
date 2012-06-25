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

/**
  * Represents a block type, aka a block ID.  These do not correspond to the Minecraft block IDs, but are instead
  * determined by the order in which blocks are read from the JSON.
  * @todo This may soon change to make save files more compatible.  Minecraft's block IDs serve as a good reference
  *       that will not change.
  */
typedef int blocktype_t;

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
