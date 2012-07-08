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

#include "block_instance.h"

BlockInstance::BlockInstance(BlockPrototype* prototype, const BlockPosition& position, BlockOrientation* orientation)
    : prototype_(prototype), position_(position), orientation_(orientation) {}
BlockInstance::~BlockInstance() {}

BlockInstance::BlockInstance(const BlockInstance& other)
    : prototype_(other.prototype()), position_(other.position()), orientation_(other.orientation()) {}

BlockInstance& BlockInstance::operator=(const BlockInstance& other) {
  prototype_ = other.prototype();
  position_ = other.position();
  orientation_ = other.orientation();
  return *this;
}