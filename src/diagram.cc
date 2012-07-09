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

/**
  * The current version of the MCModeler file format.  This must be increased whenever a backwards-incompatible change
  * is made.  The nybbles roughly correspond to major, minor, and maintenance version numbers.
  */
static const quint32 kCurrentFileFormatVersion = 0x110;

/**
  * An RAII class that automatically calls Diagram::commit() on a diagram with its transaction when it is destroyed.
  */
class ScopedTransactionCommitter {
 public:
  /**
    * Creates a ScopedTransactionCommitter.
    * @param diagram The diagram to which \p transaction will be committed when this object is destroyed.
    * @param transaction The transaction that will be committed once this object is destroyed.
    * @note There is no easy way to cancel committing a transaction once a ScopedTransactionCommitter is in play.
    *       Use caution.
    */
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

Diagram::Diagram(QObject* parent) : QObject(parent), block_mgr_(NULL) {
}

BlockManager* Diagram::blockManager() const {
  Q_ASSERT_X(block_mgr_, __PRETTY_FUNCTION__, "No BlockManager has been set. Call setBlockManager() first.");
  return block_mgr_;
}

void Diagram::load(QDataStream* stream) {
  stream->setVersion(QDataStream::Qt_4_7);
  stream->setFloatingPointPrecision(QDataStream::SinglePrecision);
  // TODO: error checking...
  char* filetype;
  quint32 version;
  *stream >> filetype;
  *stream >> version;
  QString mcdiagram_filetype = "mcdiagram";
  if (strncmp(filetype, mcdiagram_filetype.toAscii(), mcdiagram_filetype.size()) != 0) {
    QMessageBox* error_dialog = new QMessageBox();
    error_dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    error_dialog->setWindowTitle(qAppName());
    error_dialog->setText("The file you have selected could not be opened.");
    error_dialog->setInformativeText("The file does not appear to be an MCModeler diagram.");
    error_dialog->setIcon(QMessageBox::Critical);
    error_dialog->exec();
    return;
  }
  if (version != kCurrentFileFormatVersion) {
    QMessageBox* error_dialog = new QMessageBox();
    error_dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    error_dialog->setWindowTitle(qAppName());
    if (version < kCurrentFileFormatVersion) {
      error_dialog->setText("The diagram you have selected was created using an older version of MCModeler.");
      error_dialog->setInformativeText("The save file format has changed in an incompatible way since then. "
                                       "You can try to open it anyway, but it will almost certainly display "
                                       "incorrectly, and may even crash.\n\n"
                                       "Do you want to try to open the diagram?");
    } else {
      error_dialog->setText("The diagram you have selected was created using a newer version of MCModeler.");
      error_dialog->setInformativeText("You can try to open it anyway, but the save file format has changed in an "
                                       "incompatible way, so this version will almost certainly display the diagram "
                                       "incorrectly. It may even crash.\n\n"
                                       "For best results, update MCModeler and try again with that version.\n\n"
                                       "Do you want to try to open the diagram?");
    }
    error_dialog->addButton("&Open Anyway", QMessageBox::DestructiveRole);
    error_dialog->addButton("&Cancel", QMessageBox::AcceptRole);
    error_dialog->setIcon(QMessageBox::Warning);
    int result = error_dialog->exec();
    if (result == QMessageBox::Accepted) {
      return;
    }
  }
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
    BlockPrototype* prototype = blockManager()->getPrototype(type);
    // TODO(phoenix): Read/write orientation data!
    BlockInstance new_block(prototype, position, prototype->defaultOrientation());
    transaction.setBlock(new_block);
  }
}

void Diagram::save(QDataStream* stream) {
  stream->setVersion(QDataStream::Qt_4_7);
  stream->setFloatingPointPrecision(QDataStream::SinglePrecision);
  static const quint32 version = 0x0110;

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

void Diagram::ephemerallyAddBlockInternal(const BlockInstance& block) {
  Q_ASSERT(block.prototype()->type() != kBlockTypeAir);
  const BlockPosition& position = block.position();
  ephemeral_blocks_.insert(position, block);
}

void Diagram::ephemerallyRemoveBlockInternal(const BlockInstance& block) {
  Q_ASSERT(block.prototype()->type() != kBlockTypeAir);
  const BlockPosition& position = block.position();
  ephemeral_block_removals_.insert(position, block);
}

void Diagram::removeBlockInternal(const BlockPosition& position) {
  block_map_.remove(position);
  // TODO(phoenix): This assumes top-down.  Will need to customize.
  int level = position.y();
  QHash<BlockPosition, BlockInstance>& level_map = block_list_[level];
  level_map.remove(position);
}

void Diagram::commit(const BlockTransaction& transaction) {
  ephemeral_blocks_.clear();
  ephemeral_block_removals_.clear();
  foreach (const BlockInstance& old_block, transaction.old_blocks()) {
    removeBlockInternal(old_block.position());
  }
  foreach (const BlockInstance& new_block, transaction.new_blocks()) {
    addBlockInternal(new_block);
  }
  emit ephemeralBlocksChanged(transaction);
  emit diagramChanged(transaction);
}

void Diagram::commitEphemeral(const BlockTransaction& transaction) {
  ephemeral_blocks_.clear();
  ephemeral_block_removals_.clear();
  foreach (const BlockInstance& old_block, transaction.old_blocks()) {
    ephemerallyRemoveBlockInternal(old_block);
  }
  foreach (const BlockInstance& new_block, transaction.new_blocks()) {
    ephemerallyAddBlockInternal(new_block);
  }
  emit ephemeralBlocksChanged(transaction);
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
    BlockPrototype* prototype = blockManager()->getPrototype(dest_type);
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

BlockInstance Diagram::blockAt(const BlockPosition& position, BlockOracle::Mode mode) {
  BlockInstance default_value(blockManager()->getPrototype(kBlockTypeAir), position, BlockOrientation::noOrientation());
  if (mode == kPhysicalOrEphemeralBlocks) {
    if (ephemeral_block_removals_.contains(position)) {
      return default_value;
    } else if (ephemeral_blocks_.contains(position)) {
      return ephemeral_blocks_.value(position, default_value);
    }
  }
  return block_map_.value(position, default_value);
}

QHash<BlockPosition, BlockInstance> Diagram::level(int level_index) {
  return block_list_.value(level_index);
}

// TODO(phoenix): This probably shouldn't be in the model.  Move it somewhere else?
void Diagram::render() {
  QVector<const BlockInstance*> transparent_blocks;
  QHash<BlockPosition, BlockInstance>::const_iterator iter;

  // Try to give the compiler as much opportunity to optimize this branch out as possible.
  bool need_to_consider_ephemeral_removals = (ephemeral_block_removals_.size() > 0);
  if (need_to_consider_ephemeral_removals) {
    // Slow path: ephemeral removals to consider.
    for (iter = block_map_.constBegin(); iter != block_map_.constEnd(); ++iter) {
      if (ephemeral_block_removals_.contains(iter.key())) {
        continue;
      }
      const BlockInstance& b = iter.value();
      if (b.prototype()->isTransparent()) {
        transparent_blocks.append(&b);
      } else {
        b.render();
      }
    }
  } else {
    // Fast path: no ephemeral removals.
    for (iter = block_map_.constBegin(); iter != block_map_.constEnd(); ++iter) {
      const BlockInstance& b = iter.value();
      if (b.prototype()->isTransparent()) {
        transparent_blocks.append(&b);
      } else {
        b.render();
      }
    }
  }

  for (iter = ephemeral_blocks_.constBegin(); iter != ephemeral_blocks_.constEnd(); ++iter) {
    const BlockInstance& b = iter.value();
    b.render();
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

