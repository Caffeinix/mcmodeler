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
  * To use a Tool, repeatedly call appendPosition() or setPositionAtIndex() until the tool returns \c false from
  * wantsMorePositions(), indicating that it has enough points to work with (how many points this is depends on the
  * tool: for a line tool, the required number is two, whereas a flood fill tool would only need one).  Then, call
  * draw() and pass the block information describing what you want to draw, and a BlockTransaction you want the tool to
  * populate.
  */
class Tool : public QObject {
 public:
  virtual ~Tool();

  /**
    * Returns the name of the action this tool performs.  For example, the Rectangle tool might have an actionName of
    * "Draw Rectangle".
    */
  virtual QString actionName() const = 0;

  /**
    * Copies the positions that have been recorded so far by \p other and re-interprets them for this tool instead.
    */
  virtual void setStateFrom(Tool* other);

  /**
    * Appends \p position to this tool.  The meaning of the position depends on the tool.  How many indices are
    * required or desired also depends on the tool.  Extra indices will be ignored.
    * @param position The BlockPosition to append.
    */
  virtual void appendPosition(const BlockPosition& position);

  /**
    * Removes all positions from this tool.
    */
  virtual void clear();

  /**
    * Sets the \p position at a particular \p index, the meaning of which depends on the tool.  How many indices are
    * required or desired also depends on the tool.  Extra indices will be ignored.
    * @param index The index, starting at 0, for which \p position should be set.
    * @param position The BlockPosition to set at this index.
    */
  virtual void setPositionAtIndex(int index, const BlockPosition& position);

  /**
    * Returns \c true if this Tool requires more than its current number of positions in order to fully perform its
    * action.  A \c false return value does not imply that no more positions will be accepted, however; many tools such
    * as brushes can accept an arbitrary number of positions, but only actually need one or two.
    */
  virtual bool wantsMorePositions() = 0;

  /**
    * Returns \c true if this Tool represents a brush.  Brushes must return \c false from wantsMorePositions() after
    * receiving a single position, and once a position being added results in a particular block being filled, that
    * block must always be filled thereafter regardless of what other positions are added to the Tool.
    */
  virtual bool isBrush() const = 0;

  /**
    * Modifies the BlockTransaction passed as \p transaction so that it reflects the results of applying the tool to
    * the indices and positions that have been set using setPositionAtIndex().
    * @param prototype The prototype for the block the tool should use when drawing.
    * @param orientation The orientation that added blocks should have.
    * @param[in,out] transaction The transaction that should be modified with the results of applying the tool.
    */
  virtual void draw(BlockPrototype* prototype, BlockOrientation* orientation, BlockTransaction* transaction) = 0;

  /**
    * Returns the number of positions that have been added to this Tool.
    */
  virtual int countPositions() const;

 protected:
  /**
    * Returns the position at \p index, or the origin position if \p index is out of bounds.
    */
  virtual BlockPosition positionAtIndex(int index) const;

 private:
  QVector<BlockPosition> positions_;
};

#endif // TOOL_H
