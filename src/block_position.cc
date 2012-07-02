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

#include "block_position.h"

#include <qmath.h>

QDebug operator<<(QDebug dbg, const BlockPosition& position) {
  dbg.nospace() << "(" << position.x() << ", " << position.y() << ", " << position.z() << ")";
  return dbg.space();
}

uint qHash(const BlockPosition& position) {
  quint64 hash_seed = static_cast<quint64>(position.x()) +
                      (static_cast<quint64>(position.y()) << 16) +
                      (static_cast<quint64>(position.z()) << 32);
  return qHash(hash_seed);
}

BlockPosition::BlockPosition() : x_(0), y_(0), z_(0), vectors_known_(false) {

}

BlockPosition::BlockPosition(int x, int y, int z)
    : x_(x), y_(y), z_(z), vectors_known_(false) {
}

BlockPosition::BlockPosition(const BlockPosition& other) {
  x_ = other.x_;
  y_ = other.y_;
  z_ = other.z_;
  vectors_known_ = other.vectors_known_;
  corner_vector_ = other.corner_vector_;
  center_vector_ = other.center_vector_;
}

BlockPosition::BlockPosition(const QVector3D& vector) {
  // + 0.25 is to ensure that floating-point errors won't cause, e.g., 1000 to be represented as 999.999999999 and
  // floor()'d down to 999, while still ensuring that 1000.5 gets rounded correctly.
  x_ = qFloor(vector.x() + 0.25f);
  y_ = qFloor(vector.y() + 0.25f);
  z_ = qFloor(vector.z() + 0.25f);
}

BlockPosition& BlockPosition::operator=(const BlockPosition& other) {
  x_ = other.x_;
  y_ = other.y_;
  z_ = other.z_;
  vectors_known_ = other.vectors_known_;
  corner_vector_ = other.corner_vector_;
  center_vector_ = other.center_vector_;
  return *this;
}

bool BlockPosition::operator==(const BlockPosition& other) const {
  return x_ == other.x_ && y_ == other.y_ && z_ == other.z_;
}

BlockPosition BlockPosition::operator+(const BlockPosition& other) const {
  return BlockPosition(x_ + other.x_, y_ + other.y_, z_ + other.z_);
}

const QVector3D& BlockPosition::centerVector() const {
  if (!vectors_known_) {
    computeVectors();
  }
  return center_vector_;
}

const QVector3D& BlockPosition::cornerVector() const {
  if (!vectors_known_) {
    computeVectors();
  }
  return corner_vector_;
}

void BlockPosition::computeVectors() const {
  corner_vector_ = QVector3D(static_cast<qreal>(x_),
                             static_cast<qreal>(y_),
                             static_cast<qreal>(z_)),
  center_vector_ = QVector3D(static_cast<qreal>(x_) + 0.5f,
                             static_cast<qreal>(y_) + 0.5f,
                             static_cast<qreal>(z_) + 0.5f);
  vectors_known_ = true;
}

