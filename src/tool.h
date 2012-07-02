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

#ifndef TOOL_H
#define TOOL_H

#include <QVector>

class BlockOrientation;
class BlockPosition;
class BlockPrototype;
class BlockTransaction;

/**
  * An abstract class representing a particular method of drawing blocks given one or more positions.
  * To use a Tool, repeatedly call setPositionAtIndex() until the tool has enough points to work with (how many points
  * this is depends on the tool: for a line tool, the required number is two, whereas a flood fill tool would only need
  * one).  Then, call draw() and pass the block information describing what you want to draw, and a BlockTransaction
  * you want the tool to populate.
  */
class Tool : public QObject {
 public:
  virtual void setStateFrom(Tool* other);

  virtual void appendPosition(const BlockPosition& position);

  virtual void clear();

  virtual ~Tool();

  /**
    * Sets the \p position at a particular \p index, the meaning of which depends on the tool.  How many indices are
    * required or desired also depends on the tool.  Extra indices will be ignored.
    * @param index The index, starting at 0, for which \p position should be set.
    * @param position The BlockPosition to set at this index.
    */
  virtual void setPositionAtIndex(int index, const BlockPosition& position);

  virtual bool wantsMorePositions() = 0;

  /**
    * Modifies the BlockTransaction passed as \p transaction so that it reflects the results of applying the tool to
    * the indices and positions that have been set using setPositionAtIndex().
    * @param prototype The prototype for the block the tool should use when drawing.
    * @param orientation The orientation that added blocks should have.
    * @param[in,out] transaction The transaction that should be modified with the results of applying the tool.
    */
  virtual void draw(BlockPrototype* prototype, BlockOrientation* orientation, BlockTransaction* transaction) = 0;

  virtual BlockPosition positionAtIndex(int index) const;

  virtual int countPositions() const;

private:
  QVector<BlockPosition> positions_;
};

#endif // TOOL_H
