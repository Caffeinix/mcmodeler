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

#include <QDebug>
#include <QObject>
#include <QVector>

class BlockOrientation;
class BlockPosition;
class BlockPrototype;
class BlockTransaction;

/**
  * An abstract class representing a particular method of drawing blocks given one or more positions.
  *
  * Tools are designed to support a wide variety of editing modes.  Some tools, like pencils and erasers, act as
  * "brushes" -- as soon as a block is painted (or erased), it cannot be undone until after tool's action has been
  * committed.  Other tools, like lines and rectangles, act differently: clicking defines a point, and moving the mouse
  * then repeatedly "proposes" a location for the next point until the mouse is clicked again (or in some cases
  * released) to "accept" the new point.
  *
  * To declare itself as a brush, a Tool subclass can implement isBrush() to return \c true.  Otherwise, it is assumed
  * to act like the line and rectangle tools described above.
  *
  * Either way, to use a Tool, repeatedly call proposePosition() while the user is choosing the next position for the
  * tool, and call acceptLastPosition() to accept the position that the user actually chose once you know what it was.
  * Continue doing this until the user is finished.  Brushes will automatically accept every point that is proposed, so
  * calling acceptLastPosition() has no effect.
  *
  * Some tools can only accept a certain number of positions before they become saturated; a rectangle, for example,
  * can only accept two points.  After that, a new rectangle is started.  To detect this case, you can call the
  * wantsMorePositions() method, which will return \c false if the tool has enough positions (proposed or accepted) to
  * completely draw itself, and then call clear() to re-initalize the tool.  For some tools (especially brushes) you may
  * wish to ignore this, however, and simply keep adding points until the user is finished.
  *
  * At any point while using the tool, you can obtain a BlockTransaction that describes the changes the tool would make
  * (including proposed positions) by calling draw().  Whether you should actually commit the transaction depends on
  * whether the tool wants more positions and, more generally, whether the user has signaled their intent to finish the
  * interaction.
  */
class Tool : public QObject {
 public:
  Tool();
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
    * Proposes the addition of \p position to the tool.  If draw() is called after calling this, the proposed position
    * will be drawn along with any accepted ones, but if proposePosition() is called again without calling
    * acceptPositions() first, the current proposal will be discarded and replaced with the new one.
    */
  virtual void proposePosition(const BlockPosition& position);

  /**
    * Accepts the last position that was proposed using proposePosition().  After a position is accepted, it cannot be
    * removed except by calling clear().
    */
  virtual void acceptLastPosition();

  /**
    * Removes all positions from this tool and resets the state to kInitial.
    */
  virtual void clear();

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
  virtual void draw(BlockPrototype* prototype, const BlockOrientation* orientation, BlockTransaction* transaction) = 0;

 protected:
  /**
    * Describes the state of this tool.  The state of the tool determines what happens when proposePosition() is called:
    *
    *   * If the state is kInitial, \p position will be appended and the state will change to kProposed.
    *   * If the state is kProposed, the last position in the tool will be replaced by \p position.
    *   * If the state is kBrushDrag, \p position will be appended and the state will be left alone.
    *
    * @sa proposePosition()
    */
  enum State {
    /**
      * The user is not in the process of proposing a new position to the tool yet.  This is true initially, and can
      * also be true just after each new position has been added to the tool.
      */
    kInitial,
    /**
      * The user is in the process of proposing a new position for the tool.  This typically means that they are
      * moving the mouse within the canvas.  If this tool is not a brush (see isBrush()), this may also indicate that
      * the mouse is being dragged with a button down.
      */
    kProposed,
    /**
      * If this tool is a brush (see isBrush()), the user is in the process of dragging the mouse with a button down,
      * thus "painting" with the brush.
      */
    kBrushDrag
  };

  /**
    * Sets the state of this tool to \p state.  What state the tool is in determines what happens when you call
    * proposePosition().
    * @sa proposePosition()
    */
  virtual void setState(State state) {
    state_ = state;
  }

  virtual State state() const {
    return state_;
  }

  /**
    * Returns the number of positions that have been added to this Tool.
    */
  virtual int countPositions() const;

  /**
    * Appends \p position to this tool.  The meaning of the position depends on the tool.  How many indices are
    * required or desired also depends on the tool.  Extra indices will be ignored.
    * @param position The BlockPosition to append.
    */
  virtual void appendPosition(const BlockPosition& position);

  /**
    * Sets the \p position at a particular \p index, the meaning of which depends on the tool.  How many indices are
    * required or desired also depends on the tool.  Extra indices will be ignored.
    * @param index The index, starting at 0, for which \p position should be set.
    * @param position The BlockPosition to set at this index.
    */
  virtual void setPositionAtIndex(int index, const BlockPosition& position);

  /**
    * Returns the position at \p index, or the origin position if \p index is out of bounds.
    */
  virtual BlockPosition positionAtIndex(int index) const;

 private:
  QVector<BlockPosition> positions_;
  State state_;
};

#endif // TOOL_H
