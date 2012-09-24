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

#ifndef DIAGRAM_H
#define DIAGRAM_H

#include <QMap>
#include <QObject>
#include <QVector>
#include <QVector3D>

#include "block_instance.h"
#include "block_oracle.h"
#include "block_position.h"
#include "block_prototype.h"
#include "block_type.h"

class BlockManager;
class BlockOrientation;
class BlockTransaction;

/**
  * Represents a diagram containing block data for the world.
  *
  * The diagram is the model class responsible for keeping track of what blocks exist at what locations, and for
  * serializing and deserializing that data upon request.  It provides some convenience methods for performing batch
  * operations on blocks, such as flood filling and drawing lines.
  *
  * All operations on the Diagram take place within the scope of a BlockTransaction.  To make changes to the diagram,
  * create a BlockTransaction that describes the change you'd like to make and then call commit() on the Diagram
  * yourself.  Whenever a change is made to the diagram, the diagramChanged() signal is emitted with the transaction
  * that was performed.
  *
  * Diagram treats the world as horizontal slices, each corresponding to a level in the LevelWidget.  You can get a
  * map of a given level by calling the level() method.  You can also look up the block at a particular 3D location by
  * calling the blockAt() method.
  */
class Diagram : public QObject, public BlockOracle {
  Q_OBJECT
 public:
  Diagram(QObject* parent = NULL);

  /**
    * Sets the block manager for this diagram.  The block manager is used to get the prototypes for blocks in the map.
    * Diagram cannot function without a block manager, so make sure to set one before you call any other methods.
    */
  void setBlockManager(BlockManager* block_mgr) {
    block_mgr_ = block_mgr;
  }

  /**
    * @inheritDoc
    * @sa BlockOracle::blockAt()
    */
  virtual BlockInstance blockAt(const BlockPosition& position, BlockOracle::Mode mode = kPhysicalBlocksOnly);

  /**
    * @inheritDoc
    * @sa BlockOracle::levelsAreVertical();
    */
  virtual bool levelsAreVertical() const;

  /**
    * Tells all blocks in the diagram to render themselves.
    * @todo This probably does not belong in the Diagram class, but it's unclear where it should go instead.
    */
  void render();

  /**
    * Saves all blocks in the diagram out to \p stream.  The save format is versioned, so incompatible changes should
    * probably occasion a change to the version number.
    */
  void save(QDataStream* stream);

  /**
    * Populates the diagram with blocks deserialized from \p stream.
    */
  void load(QDataStream* stream);

  /**
    * Creates and commits a BlockTransaction which copies all blocks on \p source_level to \p dest_level.
    * @note A "level" is currently defined to be the set of blocks sharing a particular _y_ coordinate.
    */
  void copyLevel(int source_level, int dest_level);

  /**
    * Applies \p transaction to the diagram.  This is the method to call to make changes to the diagram.
    */
  void commit(const BlockTransaction& transaction);

  /**
    * Applies \p transaction to the diagram ephemerally.  Ephemeral commits will be temporarily reflected in the UI, but
    * will not actually affect the underlying model until committed for real using commit().
    * @note For technical reasons, although the parameter to ephemeralBlocksChanged() will be exactly \p transaction,
    * ephemeral \i removals in particular will not be shown in the 3D preview.
    */
  void commitEphemeral(const BlockTransaction& transaction);

  /**
    * Returns the number of blocks in the diagram.
    */
  int blockCount() const;

  /**
    * Returns a dictionary of counts for every block type that appears at least once in the diagram.
    */
  QMap<blocktype_t, int> blockCounts() const;

  /**
    * Returns all the blocks on the level \p level_index.  Each BlockInstance in the returned dictionary will have a
    * _y_ coordinate of \p level_index, and will be keyed on its own position for easy lookup.
    */
  QHash<BlockPosition, BlockInstance> level(int level_index);

 signals:
  /**
    * Emitted when the diagram changes.
    * @param transaction The BlockTransaction that caused the change.
    */
  void diagramChanged(const BlockTransaction& transaction);

  /**
    * Emitted when the diagram's ephemeral blocks change.
    * @param transaction The BlockTransaction that caused the change.
    */
  void ephemeralBlocksChanged(const BlockTransaction& transaction);

 private:
  /**
    * Returns the block manager, or NULL if it's not set.  This method exists mainly to fire an assert if it is called
    * while the BlockManager is NULL.  For that reason, you should always call this instead of accessing block_mgr_
    * directly.
    */
  BlockManager* blockManager() const;

  /**
    * Directly adds a block to the diagram, replacing whatever block was there.  This should only be called from
    * commit() unless you know what you're doing, since it will neither fire diagramChanged() nor create a
    * BlockTransaction for the change.
    */
  void addBlockInternal(const BlockInstance& block);

  /**
    * Directly removes a block from the diagram.  This should only be called from commit() unless you know what you're
    * doing, since it will neither fire diagramChanged() nor create a BlockTransaction for the change.
    */
  void removeBlockInternal(const BlockPosition& position);

  /**
    * Ephemerally adds a block to the diagram.  This should only be called from commitEphemeral() unless you know what
    * you're doing, since it will neither fire ephemeralBlocksChanged() nor create a BlockTransaction for the change.
    */
  void ephemerallyAddBlockInternal(const BlockInstance& block);

  /**
    * Ephemerally removes a block from the diagram.  This should only be called from commitEphemeral() unless you know
    * what you're doing, since it will neither fire ephemeralBlocksChanged() nor create a BlockTransaction for the
    * change.
    */
  void ephemerallyRemoveBlockInternal(const BlockInstance& block);

  /**
    * A map of all blocks in the diagram, regardless of level.  Used mainly for rendering.
    */
  QHash<BlockPosition, BlockInstance> block_map_;

  /**
    * A map from level to the blocks at that level.  This allows us to display a given level without having to perform
    * an incremental search for blocks with the right y-coordinate.
    */
  QHash< int, QHash<BlockPosition, BlockInstance> > block_list_;

  /**
    * A map of the ephemeral blocks in the diagram.
    */
  QHash<BlockPosition, BlockInstance> ephemeral_blocks_;
  QHash<BlockPosition, BlockInstance> ephemeral_block_removals_;

  /**
    * The block manager set using setBlockManager.  Can technically be NULL, but shouldn't be by the time any other
    * methods are called.  Don't access this directly -- use blockManager() instead.
    */
  BlockManager* block_mgr_;
};

#endif // DIAGRAM_H
