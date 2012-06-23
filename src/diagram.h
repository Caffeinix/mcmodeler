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

class Diagram : public QObject, public BlockOracle {
  Q_OBJECT
 public:
  Diagram(QObject* parent = NULL);

  void setBlockManager(BlockManager* block_mgr) {
    block_mgr_ = block_mgr;
  }

  virtual BlockInstance blockAt(const BlockPosition& position);
  void render();
  void save(QDataStream* stream);
  void load(QDataStream* stream);

  void setBlock(const BlockPosition& pos, const BlockInstance& block);
  void clearBlock(const BlockPosition& pos);
  void fillBlocks(const BlockPosition& start_pos, const blocktype_t& type, BlockOrientation* orientation);
  void drawLine(const BlockPosition& start_pos, const BlockPosition& end_pos, const blocktype_t& type, BlockOrientation* orientation);

  void commit(const BlockTransaction& transaction);

  void copyLevel(int source_level, int dest_level);

  int blockCount() const;

  QMap<blocktype_t, int> blockCounts() const;

  QHash<BlockPosition, BlockInstance> level(int level_index);

 signals:
  void diagramChanged(const BlockTransaction& transaction);

 private:
  void addBlockInternal(const BlockInstance& block);
  void removeBlockInternal(const BlockPosition& position);

  void fillBlocksRecurse(const BlockPosition& pos,
                         const blocktype_t& source_type,
                         const blocktype_t& dest_type,
                         BlockOrientation* dest_orientation,
                         const BlockPosition& start_pos,
                         int depth,
                         QSet<BlockPosition>* filled_blocks,
                         BlockTransaction* transaction);

  QHash<BlockPosition, BlockInstance> block_map_;
  QHash< int, QHash<BlockPosition, BlockInstance> > block_list_;
  BlockManager* block_mgr_;
};

#endif // DIAGRAM_H
