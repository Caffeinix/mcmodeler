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
#include "block_manager.h"

#include <QDataStream>

BlockInstance::BlockInstance() : is_valid_(false), prototype_(NULL), orientation_(NULL) {}

BlockInstance::BlockInstance(BlockPrototype* prototype,
                             const BlockPosition& position,
                             const BlockOrientation* orientation)
    : is_valid_(true), prototype_(prototype), position_(position), orientation_(orientation) {}


BlockInstance::BlockInstance(QDataStream* stream, BlockManager* block_manager)
    : is_valid_(false), prototype_(NULL), orientation_(NULL) {
  Q_ASSERT(block_manager != NULL);
  int position_x;
  int position_y;
  int position_z;
  *stream >> position_x;
  *stream >> position_y;
  *stream >> position_z;
  qint32 type_word;
  *stream >> type_word;
  char* orientation_chars;
  *stream >> orientation_chars;

  BlockPosition position(position_x, position_y, position_z);
  blocktype_t type = static_cast<blocktype_t>(type_word);
  BlockPrototype* prototype = block_manager->getPrototype(type);
  const BlockOrientation* orientation = BlockOrientation::get(orientation_chars);

  delete[] orientation_chars;

  is_valid_ = true;
  prototype_ = prototype;
  position_ = position;
  orientation_ = orientation;
}

BlockInstance::~BlockInstance() {}

BlockInstance::BlockInstance(const BlockInstance& other)
    : is_valid_(other.isValid()),
      prototype_(other.prototype()),
      position_(other.position()),
      orientation_(other.orientation()) {}

BlockInstance& BlockInstance::operator=(const BlockInstance& other) {
  is_valid_ = other.isValid();
  prototype_ = other.prototype();
  position_ = other.position();
  orientation_ = other.orientation();
  return *this;
}

bool BlockInstance::serialize(QDataStream* stream) const {
  const BlockPosition& pos = position();
  const QVector3D& vec = pos.cornerVector();
  int position_x = vec.x();
  int position_y = vec.y();
  int position_z = vec.z();
  *stream << position_x;
  *stream << position_y;
  *stream << position_z;
  *stream << static_cast<qint32>(prototype()->type());
  *stream << orientation()->name().toAscii().constData();
  return true;
}
