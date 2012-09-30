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
static const quint32 kCurrentFileFormatVersion = 0x130;

/**
  * The number of bytes we are holding reserved for future expansion.  These will all be set to zero when writing out
  * the file, so as long as zero is a reasonable default, the format will be forwards-compatible.
  */
static const int kNumReservedBytes = 256;

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

  stream->skipRawData(kNumReservedBytes);

  qint32 block_count;
  *stream >> block_count;

  BlockTransaction transaction;
  ScopedTransactionCommitter committer(this, transaction);

  // Completely nuke the document.
  foreach (const BlockInstance& block, block_map_.values()) {
    transaction.clearBlock(block);
  }

  while (!stream->atEnd()) {
    BlockInstance new_block(stream, blockManager());
    transaction.setBlock(new_block);
  }
}

void Diagram::save(QDataStream* stream) {
  stream->setVersion(QDataStream::Qt_4_7);
  stream->setFloatingPointPrecision(QDataStream::SinglePrecision);

  *stream << "mcdiagram";
  *stream << kCurrentFileFormatVersion;
  char* reserved = new char[kNumReservedBytes];
  memset(reserved, '\0', kNumReservedBytes);
  stream->writeRawData(reserved, kNumReservedBytes);
  delete[] reserved;
  *stream << static_cast<qint32>(blockCount());
  QHash<BlockPosition, BlockInstance>::const_iterator iter;
  for (iter = block_map_.constBegin(); iter != block_map_.constEnd(); ++iter) {
    const BlockInstance& instance = iter.value();
    instance.serialize(stream);
  }
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

void Diagram::copyLevel(int source_level, int dest_level) {
  QHash<BlockPosition, BlockInstance>& source_level_map = block_list_[source_level];
  QHash<BlockPosition, BlockInstance>& dest_level_map = block_list_[dest_level];

  BlockTransaction transaction;
  ScopedTransactionCommitter committer(this, transaction);

  // Remove all blocks in the dest level.
  foreach (const BlockPosition& key, dest_level_map.keys()) {
    transaction.clearBlock(dest_level_map.value(key));
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

bool Diagram::levelsAreVertical() const {
  // This will need some extra code to work correctly (we need to save it out in the file format, for one thing, and
  // adjacency calculations need to take it into account as well), so it's off for now.
  // TODO(phoenix): Make this work fully, then allow it to be enabled.
  return false;
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
  if (Q_UNLIKELY(need_to_consider_ephemeral_removals)) {
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

