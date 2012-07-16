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
#include "tool.h"

Tool::Tool() : state_(kInitial) {}
Tool::~Tool() {}

void Tool::setStateFrom(Tool* other) {
  positions_.resize(other->countPositions());
  for (int i = 0; i < other->countPositions(); ++i) {
    positions_.replace(i, other->positionAtIndex(i));
  }
  setState(kInitial);
}

void Tool::proposePosition(const BlockPosition& position) {
  if (state_ == kInitial) {
    appendPosition(position);
    setState(kProposed);
  } else if (state_ == kProposed) {
    setPositionAtIndex(countPositions() - 1, position);
  } else if (state_ == kBrushDrag) {
    appendPosition(position);
  }
}

void Tool::acceptLastPosition() {
  if (isBrush() && !wantsMorePositions()) {
    setState(Tool::kBrushDrag);
  } else {
    setState(Tool::kInitial);
  }
}

void Tool::appendPosition(const BlockPosition& position) {
  positions_.append(position);
}

void Tool::setPositionAtIndex(int index, const BlockPosition& position) {
  if (index < 0) {
    qCritical() << "Error: attempted to set position" << index << "on tool" << actionName();
    return;
  }
  if (positions_.size() <= index) {
    positions_.resize(index + 1);
  }
  positions_.replace(index, position);
}

void Tool::clear() {
  positions_.clear();
  setState(kInitial);
}

BlockPosition Tool::positionAtIndex(int index) const {
  if (positions_.size() <= index) {
    qWarning() << "Tool: Requested position index" << index << "in list only" << positions_.size() << "long.";
    return BlockPosition(0, 0, 0);
  }
  return positions_.at(index);
}

int Tool::countPositions() const {
  return positions_.size();
}
