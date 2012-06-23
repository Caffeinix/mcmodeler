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

#include "diagram.h"

#include <QDataStream>
#include <QPair>

#include "block_manager.h"
#include "block_orientation.h"
#include "block_transaction.h"
#include "line_tool.h"

class ScopedTransactionCommitter {
 public:
  ScopedTransactionCommitter(Diagram* diagram, const BlockTransaction& transaction)
      : diagram_(diagram),
        transaction_(transaction) {}
  ~ScopedTransactionCommitter() {
    diagram_->commit(transaction_);
  }
 private:
  Diagram* diagram_;
  const BlockTransaction& transaction_;
};

Diagram::Diagram(QObject* parent) : QObject(parent) {
}

void Diagram::load(QDataStream* stream) {
  Q_ASSERT(block_mgr_);
  stream->setVersion(QDataStream::Qt_4_7);
  stream->setFloatingPointPrecision(QDataStream::SinglePrecision);
  // TODO: error checking...
  char* filetype;
  quint32 version;
  *stream >> filetype;
  *stream >> version;
  QString mcdiagram_filetype = "mcdiagram";
  Q_ASSERT(strncmp(filetype, mcdiagram_filetype.toAscii(), mcdiagram_filetype.size()) == 0);
  Q_ASSERT(version == 0x0100);
  delete[] filetype;
  qint32 block_count;
  *stream >> block_count;

  BlockTransaction transaction;
  ScopedTransactionCommitter committer(this, transaction);

  // Completely nuke the document.
  foreach (const BlockInstance& block, block_map_.values()) {
    transaction.clearBlock(block);
  }

  while (!stream->atEnd()) {
    float position_x, position_y, position_z;
    *stream >> position_x;
    *stream >> position_y;
    *stream >> position_z;
    BlockPosition position(position_x, position_y, position_z);
    qint32 type_byte;
    *stream >> type_byte;
    blocktype_t type = static_cast<blocktype_t>(type_byte);
    BlockPrototype* prototype = block_mgr_->getPrototype(type);
    // TODO(phoenix): Read/write orientation data!
    BlockInstance new_block(prototype, position, prototype->defaultOrientation());
    transaction.setBlock(new_block);
  }
}

void Diagram::save(QDataStream* stream) {
  stream->setVersion(QDataStream::Qt_4_7);
  stream->setFloatingPointPrecision(QDataStream::SinglePrecision);
  static const quint32 version = 0x0100;

  *stream << "mcdiagram";
  *stream << version;
  *stream << static_cast<qint32>(blockCount());
  QHash<BlockPosition, BlockInstance>::const_iterator iter;
  for (iter = block_map_.constBegin(); iter != block_map_.constEnd(); ++iter) {
    const BlockPosition& position = iter.key();
    const QVector3D& vec = position.cornerVector();
    BlockInstance instance = iter.value();
    float position_x = vec.x();
    float position_y = vec.y();
    float position_z = vec.z();
    *stream << position_x;
    *stream << position_y;
    *stream << position_z;
    *stream << static_cast<qint32>(instance.prototype()->type());
    // TODO(phoenix): Read/write orientation data!
  }
}

void Diagram::setBlock(const BlockPosition& pos, const BlockInstance& block) {
  BlockTransaction transaction;
  ScopedTransactionCommitter committer(this, transaction);
  transaction.replaceBlock(blockAt(pos), block);
}

void Diagram::clearBlock(const BlockPosition& pos) {
  BlockTransaction transaction;
  ScopedTransactionCommitter committer(this, transaction);
  transaction.clearBlock(blockAt(pos));
}

void Diagram::addBlockInternal(const BlockInstance& block) {
  Q_ASSERT(block.prototype()->type() != kBlockTypeAir);
  const BlockPosition& position = block.position();
  block_map_.insert(position, block);
  // TODO(phoenix): This assumes top-down.  Will need to customize.
  int level = position.y();
  QHash<BlockPosition, BlockInstance>& level_map = block_list_[level];
  level_map.insert(position, block);
}

void Diagram::removeBlockInternal(const BlockPosition& position) {
  block_map_.remove(position);
  // TODO(phoenix): This assumes top-down.  Will need to customize.
  int level = position.y();
  QHash<BlockPosition, BlockInstance>& level_map = block_list_[level];
  level_map.remove(position);
}

void Diagram::commit(const BlockTransaction& transaction) {
  foreach (const BlockInstance& old_block, transaction.old_blocks()) {
    removeBlockInternal(old_block.position());
  }
  foreach (const BlockInstance& new_block, transaction.new_blocks()) {
    addBlockInternal(new_block);
  }
  emit diagramChanged(transaction);
}

void Diagram::drawLine(const BlockPosition& start_pos, const BlockPosition& end_pos, const blocktype_t& type, BlockOrientation* orientation) {
  BlockTransaction transaction;
  ScopedTransactionCommitter committer(this, transaction);
  BlockPrototype* prototype = block_mgr_->getPrototype(type);
  LineTool tool(this);
  tool.setPositionAtIndex(0, start_pos);
  tool.setPositionAtIndex(1, end_pos);
  tool.draw(prototype, orientation, &transaction);
}

void Diagram::fillBlocks(const BlockPosition& start_pos, const blocktype_t& type, BlockOrientation* orientation) {
  BlockTransaction transaction;
  ScopedTransactionCommitter committer(this, transaction);

  BlockInstance start_block = blockAt(start_pos);
  const blocktype_t source_type = start_block.prototype()->type();
  QSet<BlockPosition> filled_blocks;
  fillBlocksRecurse(start_pos, source_type, type, orientation, start_pos, 1, &filled_blocks, &transaction);
}

void Diagram::fillBlocksRecurse(const BlockPosition& pos,
                                const blocktype_t& source_type,
                                const blocktype_t& dest_type,
                                BlockOrientation* dest_orientation,
                                const BlockPosition& start_pos,
                                int depth,
                                QSet<BlockPosition>* filled_blocks,
                                BlockTransaction* transaction) {
  if (filled_blocks->contains(pos)) {
    // We've already filled this block.
    return;
  }
  if (qAbs(pos.x() - start_pos.x()) > 64 ||
      qAbs(pos.z() - start_pos.z()) > 64) {
    // This is unlikely to be a bounded area, so return to prevent stack overflow.
    return;
  }
  if (depth > 8192) {
    // Generic runaway recursion catcher.
    return;
  }
  BlockInstance old_block = blockAt(pos);
  if (old_block.prototype()->type() != source_type) {
    return;
  }
  if (dest_type == kBlockTypeAir) {
    transaction->clearBlock(old_block);
  } else {
    BlockPrototype* prototype = block_mgr_->getPrototype(dest_type);
    BlockInstance new_block(prototype, pos, dest_orientation);
    transaction->replaceBlock(old_block, new_block);
  }
  filled_blocks->insert(pos);

  // TODO: this hard-codes top-down orientation again.
  fillBlocksRecurse(pos + BlockPosition(1, 0, 0), source_type, dest_type, dest_orientation,
                    start_pos, depth, filled_blocks, transaction);
  fillBlocksRecurse(pos + BlockPosition(-1, 0, 0), source_type, dest_type, dest_orientation,
                    start_pos, depth, filled_blocks, transaction);
  fillBlocksRecurse(pos + BlockPosition(0, 0, 1), source_type, dest_type, dest_orientation,
                    start_pos, depth, filled_blocks, transaction);
  fillBlocksRecurse(pos + BlockPosition(0, 0, -1), source_type, dest_type, dest_orientation,
                    start_pos, depth, filled_blocks, transaction);
}


void Diagram::copyLevel(int source_level, int dest_level) {
  QHash<BlockPosition, BlockInstance>& source_level_map = block_list_[source_level];
  QHash<BlockPosition, BlockInstance>& dest_level_map = block_list_[dest_level];

  BlockTransaction transaction;
  ScopedTransactionCommitter committer(this, transaction);

  // Remove all blocks in the dest level.
  foreach (const BlockPosition& key, dest_level_map.keys()) {
    // TODO(phoenix): Make a default constructor and isValid() method for BlockInstance.
    BlockInstance invalid_instance(NULL, BlockPosition(0, 0, 0), NULL);
    transaction.clearBlock(dest_level_map.value(key, invalid_instance));
  }

  // Add all blocks in the source level to the dest level, adjusting their altitudes.
  foreach (const BlockPosition& source_position, source_level_map.keys()) {
    BlockPosition dest_position = source_position + BlockPosition(0, dest_level - source_level, 0);
    BlockInstance invalid_instance(NULL, BlockPosition(0, 0, 0), NULL);
    BlockInstance source_instance = source_level_map.value(source_position, invalid_instance);
    // The value() call should never return invalid_instance, since we got the key from the map
    // just before we called value() with it.
    Q_ASSERT(source_instance.prototype());
    BlockInstance dest_instance(source_instance.prototype(), dest_position, source_instance.orientation());
    // Since we already cleared the dest level, we can safely call setBlock instead of replaceBlock
    // since we know there's nothing to replace.
    transaction.setBlock(dest_instance);
  }
}

BlockInstance Diagram::blockAt(const BlockPosition& position) {
  BlockInstance default_value(block_mgr_->getPrototype(kBlockTypeAir), position, BlockOrientation::noOrientation());
  return block_map_.value(position, default_value);
}

QHash<BlockPosition, BlockInstance> Diagram::level(int level_index) {
  return block_list_.value(level_index);
}

// TODO(phoenix): This probably shouldn't be in the model.  Move it somewhere else?
void Diagram::render() {
  QVector<const BlockInstance*> transparent_blocks;
  QHash<BlockPosition, BlockInstance>::const_iterator iter;
  for (iter = block_map_.constBegin(); iter != block_map_.constEnd(); ++iter) {
    const BlockInstance& b = iter.value();
    if (b.prototype()->isTransparent()) {
      transparent_blocks.append(&b);
    } else {
      b.render();
    }
  }

  // Render transparent blocks last.
  QVector<const BlockInstance*>::const_iterator transparent_iter;
  for (transparent_iter = transparent_blocks.constBegin();
       transparent_iter != transparent_blocks.constEnd();
       ++transparent_iter ) {
    const BlockInstance* b = *transparent_iter;
    b->render();
  }
}

int Diagram::blockCount() const {
  return block_map_.size();
}

QMap<blocktype_t, int> Diagram::blockCounts() const {
  QMap<blocktype_t, int> map;
  foreach(const BlockInstance& instance, block_map_.values()) {
    int count = 0;
    if (map.contains(instance.prototype()->type())) {
      count = map.value(instance.prototype()->type());
    }
    map.insert(instance.prototype()->type(), count + 1);
  }
  return map;
}

