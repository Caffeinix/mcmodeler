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

#ifndef BLOCK_TRANSACTION_H
#define BLOCK_TRANSACTION_H

#include <QList>
#include <QSet>

#include "block_type.h"

class BlockInstance;
class BlockPosition;

/**
  * Represents an atomic operation on the world that involves adding, removing, and replacing blocks.
  *
  * BlockTransaction is the basis for MCModeler's model/view separation (in the sense of model-view-controller, not
  * model-view matrix).  Rather than modifying the Diagram (which serves as the model) directly, view classes (such as
  * LevelWidget) ask the Diagram to perform operations like drawing lines, filling regions, and erasing blocks.  The
  * Diagram creates a BlockTransaction that describes the changes to the world as a result of the operation, applies
  * the BlockTransaction to its own internal model, and then broadcasts it back to all the views, which apply the same
  * transaction on their own representations.
  *
  * The manner in which the transaction is applied differs by view, but in general it goes like this:
  *
  * @code
  * foreach (const BlockInstance& block, transaction.old_blocks()) {
  *   myImaginaryRemoveBlockMethod(block);
  * }
  * foreach (const BlockInstance& block, transaction.new_blocks()) {
  *   myImaginaryAddBlockMethod(block);
  * }
  * @endcode
  *
  * Although you can call clearBlock() and setBlock() on the transaction in any order you like, the view must always
  * remove blocks \e before it adds blocks.
  *
  * BlockTransactions record both the old and new states of the world, so they can be used to undo operations as well,
  * by simply removing the new blocks and adding back the old ones.
  */
class BlockTransaction {
 public:
  /**
    * Default constructor.
    */
  BlockTransaction();

  /**
    * Records the replacement of \p old_block with \p new_block.  The blocks must have the same position.  If
    * \p old_block has a type of kBlockTypeAir, this is equivalent to setBlock().  If \p new_block has a type of
    * kBlockTypeAir, this is equivalent to clearBlock().
    *
    * Only the first replacement per position per transaction will be honored.  So in the following code:
    *
    * @code
    * BlockPrototype* dirt = BlockManager::getPrototype(0);
    * BlockPrototype* grass = BlockManager::getPrototype(1);
    * BlockInstance dirt_block(dirt, BlockPosition(0, 0, 0), dirt->defaultOrientation());
    * BlockInstance grass_block(grass, BlockPosition(0, 0, 0), grass->defaultOrientation());
    * BlockTransaction transaction;
    * transaction.replaceBlock(dirt_block, grass_block);
    * transaction.replaceBlock(grass_block, dirt_block);
    * @endcode
    *
    * The transaction will record the replacement of dirt with grass at 0, 0, 0, but will \e not record the subsequent
    * reversion of that replacement.
    */
  void replaceBlock(const BlockInstance& old_block, const BlockInstance& new_block);

  /**
    * Records the addition of \p new_block.
    *
    * If you call clearBlock() and setBlock() (in either order) for the same position in the same transaction, the
    * result will be exactly equivalent to calling replaceBlock().
    *
    * @warning Do \e not call setBlock without a corresponding clearBlock() unless you are absolutely certain that
    *    there is currently no block at the position of \p new_block.  If there is, you will corrupt the undo stack
    *    because the removal of the original block will not be recorded.  When in doubt, always use replaceBlock().
    *
    * Only the first addition per position per transaction will be honored.  So in the following code:
    *
    * @code
    * BlockPrototype* dirt = BlockManager::getPrototype(0);
    * BlockPrototype* grass = BlockManager::getPrototype(1);
    * BlockInstance dirt_block(dirt, BlockPosition(0, 0, 0), dirt->defaultOrientation());
    * BlockInstance grass_block(grass, BlockPosition(0, 0, 0), grass->defaultOrientation());
    * BlockTransaction transaction;
    * transaction.setBlock(grass_block);
    * transaction.setBlock(dirt_block);
    * @endcode
    *
    * The transaction will record the addition of the grass block at 0, 0, 0, but will \e not record the subsequent
    * overwriting of that block with dirt (luckily for the author, since otherwise it would corrupt the undo stack).
    */
  void setBlock(const BlockInstance& new_block);

  /**
    * Records the removal of \p new_block.
    *
    * If you call clearBlock() and setBlock() (in either order) for the same position in the same transaction, the
    * result will be exactly equivalent to calling replaceBlock().
    *
    * Only the position of \p old_block is used when performing the transaction normally, but the block type is used
    * when undoing the transaction, so it is important to pass accurate information about the old state of the world
    * to avoid corrupting the undo stack.
    */
  void clearBlock(const BlockInstance& old_block);

  /**
    * Returns a list of blocks that are removed from the world by this transaction.  When applying the transaction in
    * the normal (forward) manner, these blocks should be removed \e before adding the blocks returned by new_blocks().
    * When applying the transaction backwards (for undo), these blocks should be added \e after removing the blocks
    * returned by new_blocks().
    */
  const QList<BlockInstance>& old_blocks() const {
    return old_blocks_;
  }

  /**
    * Returns a list of blocks that are added to the world by this transaction.  When applying the transaction in the
    * normal (forward) manner, these blocks should be removed \e before adding the blocks returned by new_blocks().
    * When applying the transaction backwards (for undo), these blocks should be added \e after removing the blocks
    * returned by new_blocks().
    */
  const QList<BlockInstance>& new_blocks() const {
    return new_blocks_;
  }

 private:
  QSet<BlockPosition> old_positions_;
  QSet<BlockPosition> new_positions_;
  QList<BlockInstance> old_blocks_;
  QList<BlockInstance> new_blocks_;

  Q_DISABLE_COPY(BlockTransaction)
};

#endif // BLOCK_TRANSACTION_H
