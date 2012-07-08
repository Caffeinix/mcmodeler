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

#include "level_widget.h"

#include "block_manager.h"
#include "block_position.h"
#include "block_transaction.h"
#include "diagram.h"
#include "line_tool.h"
#include "macros.h"
#include "pencil_tool.h"
#include "rectangle_tool.h"

static const int kSpriteWidth = 16;
static const int kSpriteHeight = 16;

static const int kCanvasWidth = 10000;
static const int kCanvasHeight = 10000;

static const int kGhostLevelOffsets[] = {-1, 0};

LevelWidget::LevelWidget(QWidget* parent) :
    QGraphicsView(parent),
    scene_(new QGraphicsScene(this)),
    level_(0),
    diagram_(NULL),
    block_mgr_(NULL),
    in_pan_mode_(false),
    last_block_position_(0, 0, 0),
    block_type_(kBlockTypeUnknown),
    copied_level_(-1),
    current_tool_(NULL),
    state_(kStateInitial) {
  setScene(scene_);
  setBackgroundBrush(QBrush(QPixmap(":/grid_background.png")));
  setSceneRect(QRectF(-kCanvasWidth / 2, -kCanvasHeight / 2, kCanvasWidth, kCanvasHeight));
  setTransformationAnchor(QGraphicsView::NoAnchor);
  setMouseTracking(true);
  setCursor(QCursor(Qt::CrossCursor));
}

LevelWidget::~LevelWidget() {}

void LevelWidget::setBlockManager(BlockManager* block_mgr) {
  block_mgr_ = block_mgr;
}

void LevelWidget::setDiagram(Diagram* diagram) {
  diagram_ = diagram;
  connect(diagram, SIGNAL(diagramChanged(BlockTransaction)), SLOT(updateLevel(BlockTransaction)));
  connect(diagram, SIGNAL(ephemeralBlocksChanged(BlockTransaction)), SLOT(updateEphemeralBlocks(BlockTransaction)));
  setLevel(0);
}

void LevelWidget::updateLevel(const BlockTransaction& transaction) {
  qDebug() << "Diagram changed.";
  QElapsedTimer timer;
  timer.start();
  foreach (const BlockInstance& old_block, transaction.old_blocks()) {
    removeBlock(old_block.position());
  }
  foreach (const BlockInstance& new_block, transaction.new_blocks()) {
    addBlock(new_block);
  }
  // QGraphicsScene doesn't notify the view that things have changed until the next event loop
  // cycle.  Qt apparently optimizes mouse moves so that they don't return control to the event loop
  // while the mouse is in motion, which causes the view to freeze.  This hack fixes the problem.
  qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

void LevelWidget::updateEphemeralBlocks(const BlockTransaction& transaction) {
  // Remove all ephemeral items from the view.
  foreach(QGraphicsItem* item, ephemeral_item_model_.values()) {
    scene()->removeItem(item);
  }
  ephemeral_item_model_.clear();

  foreach (const BlockInstance& new_block, transaction.new_blocks()) {
    addEphemeralBlock(new_block);
  }
  // QGraphicsScene doesn't notify the view that things have changed until the next event loop
  // cycle.  Qt apparently optimizes mouse moves so that they don't return control to the event loop
  // while the mouse is in motion, which causes the view to freeze.  This hack fixes the problem.
  qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

void LevelWidget::loadLevel() {
  scene()->clear();
  item_model_.clear();
  if (!diagram_) {
    return;
  }
  QGraphicsView::ViewportUpdateMode previous_mode = viewportUpdateMode();
  setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
  for (int i = 0; i < arraysize(kGhostLevelOffsets); ++i) {
    foreach (const BlockInstance& block, diagram_->level(level_ + kGhostLevelOffsets[i])) {
      addBlock(block);
    }
  }
  setViewportUpdateMode(previous_mode);
}

bool LevelWidget::event(QEvent* event) {
  if (event->type() == QEvent::PolishRequest) {
    translate(viewport()->width() / 2, viewport()->height() / 2);
  }
  return QGraphicsView::event(event);
}

void LevelWidget::updateTool(QKeyEvent* event) {
//  if (event->type() == QEvent::KeyPress) {
//    if (event->key() == Qt::Key_Shift) {
//      Tool* line_tool = new LineTool(diagram_);
//      line_tool->setStateFrom(current_tool_.data());
//      current_tool_.reset(line_tool);
//    }
//  } else {  // Key release.
//    Tool* pencil_tool = new PencilTool(diagram_);
//    pencil_tool->setStateFrom(current_tool_.data());
//    current_tool_.reset(pencil_tool);
//  }
}

void LevelWidget::updateTool(QMouseEvent* event) {
//  if (event->modifiers() & Qt::ShiftModifier) {
//    Tool* line_tool = new LineTool(diagram_);
//    line_tool->setStateFrom(current_tool_.data());
//    current_tool_.reset(line_tool);
//  } else {
//    Tool* pencil_tool = new PencilTool(diagram_);
//    pencil_tool->setStateFrom(current_tool_.data());
//    current_tool_.reset(pencil_tool);
//  }
}

void LevelWidget::setState(LevelWidget::State state) {
  state_ = state;
}

void LevelWidget::keyPressEvent(QKeyEvent* event) {
  updateTool(event);
}

void LevelWidget::keyReleaseEvent(QKeyEvent* event) {
  updateTool(event);
}

void LevelWidget::mousePressEvent(QMouseEvent* event) {
  updateTool(event);

  if (current_tool_->isBrush() && !current_tool_->wantsMorePositions()) {
    setState(kStateBrushDrag);
  } else {
    setState(kStateInitial);
  }

  BlockTransaction transaction;
  BlockPrototype* prototype = block_mgr_->getPrototype(block_type_);
  current_tool_->draw(prototype, prototype->defaultOrientation(), &transaction);
  diagram_->commitEphemeral(transaction);
}

void LevelWidget::mouseReleaseEvent(QMouseEvent* event) {
  updateTool(event);
  BlockPosition pos = positionForPoint(mapToScene(event->pos()));

  if (!current_tool_->wantsMorePositions()) {
    // Commit the current transaction for real.
    BlockTransaction transaction;
    BlockPrototype* prototype = block_mgr_->getPrototype(block_type_);
    current_tool_->draw(prototype, prototype->defaultOrientation(), &transaction);
    diagram_->commit(transaction);
    current_tool_->clear();
    setState(kStateInitial);
  }
}

void LevelWidget::mouseDoubleClickEvent(QMouseEvent* event) {
  mousePressEvent(event);
  mouseReleaseEvent(event);
}

void LevelWidget::mouseMoveEvent(QMouseEvent* event) {
  updateTool(event);

  BlockPosition pos = positionForPoint(mapToScene(event->pos()));
  if (state_ == kStateInitial) {
    // Add a new point to the tool and draw it ephemerally.
    current_tool_->appendPosition(pos);
    setState(kStateUnsatisfied);
  } else if (state_ == kStateUnsatisfied) {
    // Reset last point to current position and draw it ephemerally.
    current_tool_->setPositionAtIndex(current_tool_->countPositions() - 1, pos);
  } else if (state_ == kStateBrushDrag) {
    // Brush-like tool, mouse is down.  Append current position to the brush stroke.
    current_tool_->appendPosition(pos);
  }
  BlockTransaction transaction;
  BlockPrototype* prototype = block_mgr_->getPrototype(block_type_);
  current_tool_->draw(prototype, prototype->defaultOrientation(), &transaction);
  diagram_->commitEphemeral(transaction);
}

QGraphicsItem* LevelWidget::itemAtPosition(const BlockPosition& position) const {
  return item_model_.value(position, NULL);
}

BlockPosition LevelWidget::positionForEvent(QMouseEvent* event) const {
  return positionForPoint(mapToScene(event->pos()));
}

void LevelWidget::toggleBlock(QMouseEvent* event) {
  if (block_type_ == kBlockTypeUnknown) {
    return;
  }

  BlockPosition position = positionForEvent(event);
  if (event->buttons() & Qt::RightButton) {
    diagram_->clearBlock(position);
  } else {
    BlockPrototype* prototype = block_mgr_->getPrototype(block_type_);
    const BlockInstance& old_block = diagram_->blockAt(position);
    if (block_type_ == old_block.prototype()->type()) {
      // Only change orientation on mouse down; otherwise we get a bunch of orientation changes during mouse move.
      if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick) {
        BlockOrientation* old_orientation = old_block.orientation();
        const QVector<BlockOrientation*>& orientations = old_block.prototype()->orientations();
        int orientation_index = orientations.indexOf(old_orientation);
        if (orientation_index == orientations.size() - 1) {
          orientation_index = 0;
        } else {
          ++orientation_index;
        }
        qDebug() << "Using orientation" << orientations.at(orientation_index)->name();
        BlockInstance new_block(prototype, position, orientations.at(orientation_index));
        diagram_->setBlock(position, new_block);
      }
    } else {
      BlockInstance new_block(prototype, position, prototype->defaultOrientation());
      diagram_->setBlock(position, new_block);
    }
  }
}

void LevelWidget::drawLine(QMouseEvent* event, bool commit) {
  if (block_type_ == kBlockTypeUnknown) {
    return;
  }

  BlockPosition position = positionForEvent(event);
  if (event->buttons() & Qt::RightButton) {
    return;
  }

  const BlockPrototype* prototype = block_mgr_->getPrototype(block_type_);
  diagram_->drawLine(last_block_position_, position, block_type_, prototype->defaultOrientation());
}

void LevelWidget::fillBlocks(QMouseEvent* event) {
  if (block_type_ == kBlockTypeUnknown) {
    return;
  }

  BlockPosition position = positionForEvent(event);
  if (event->buttons() & Qt::RightButton) {
    diagram_->fillBlocks(position, kBlockTypeAir, BlockOrientation::noOrientation());
  } else {
    const BlockPrototype* prototype = block_mgr_->getPrototype(block_type_);
    diagram_->fillBlocks(position, block_type_, prototype->defaultOrientation());
  }
}

BlockPosition LevelWidget::positionForPoint(const QPointF& point) const {
  BlockPosition ret(qRound(point.x() / kSpriteWidth), level_, qRound(point.y() / kSpriteHeight));
  return ret;
}

void LevelWidget::drawBackground(QPainter* painter, const QRectF& rect) {
  QGraphicsView::drawBackground(painter, rect);  // Draw background brush (grid lines)
  painter->setOpacity(0.5);
  painter->drawPixmap(0 - template_image_.width() / 2, 0 - template_image_.height() / 2,
                      template_image_.width(), template_image_.height(), template_image_);
}

void LevelWidget::drawForeground(QPainter* painter, const QRectF& rect) {
  painter->drawPixmap(3, 3, 11, 11, QPixmap(":/origin.png"));
}

QGraphicsItem* LevelWidget::addEphemeralBlock(const BlockInstance& block) {
  if (!block_mgr_ || !diagram_) {
    return NULL;
  }

  const BlockPosition& position = block.position();

  BlockPrototype* prototype = block.prototype();
  QGraphicsPixmapItem* item = scene()->addPixmap(prototype->sprite(block.orientation()));
  item->setOffset(-0.5 * kSpriteWidth, -0.5 * kSpriteHeight);
  item->setPos(position.x() * kSpriteWidth, position.z() * kSpriteHeight);
  item->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
  item->setData(0, prototype->type());
  item->setZValue(position.y() + 64.0);  // Stack on top of normal blocks.
  if (!current_tool_->isBrush()) {
    item->setOpacity(0.25);
  }
  ephemeral_item_model_.insert(position, item);
  return item;
}

QGraphicsItem* LevelWidget::addBlock(const BlockInstance& block) {
  if (!block_mgr_ || !diagram_) {
    return NULL;
  }

  const BlockPosition& position = block.position();

  // Determine whether this block appears on our diagram (either in the current level or
  // as a ghost of a nearby level).
  bool is_in_view = false;
  for (int i = 0; i < arraysize(kGhostLevelOffsets); ++i) {
    if (position.y() == level_ + kGhostLevelOffsets[i]) {
      is_in_view = true;
    }
  }
  if (!is_in_view) {
    return NULL;
  }

  BlockPrototype* prototype = block.prototype();
  QGraphicsPixmapItem* item = scene()->addPixmap(prototype->sprite(block.orientation()));
  item->setOffset(-0.5 * kSpriteWidth, -0.5 * kSpriteHeight);
  item->setPos(position.x() * kSpriteWidth, position.z() * kSpriteHeight);
  item->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
  item->setData(0, prototype->type());
  item->setZValue(position.y());
  if (position.y() != level_) {
    item->setOpacity(0.25);
    // TODO(phoenix): Reuse this object.
    QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect();
    effect->setStrength(1.0);
    item->setGraphicsEffect(effect);
  }
  item_model_.insert(position, item);
  return item;
}

void LevelWidget::removeBlock(const BlockPosition& position) {
  if (!block_mgr_ || !diagram_) {
    return;
  }
  QGraphicsItem* item = itemAtPosition(position);
  if (!item) {
    return;
  }
  item_model_.remove(position);
  scene_->removeItem(item);
  delete item;
}

void LevelWidget::setLevel(int level) {
  level_ = level;
  loadLevel();
  emit levelChanged(level);
}

void LevelWidget::setBlockType(blocktype_t type) {
  block_type_ = type;
}

void LevelWidget::setCurrentTool(Tool* tool) {
  qDebug() << "Setting current tool to" << tool;
  current_tool_ = tool;
  current_tool_->clear();
  setState(kStateInitial);
}

void LevelWidget::copyLevel() {
  copied_level_ = level_;
}

void LevelWidget::pasteLevel() {
  if (copied_level_ >= 0 && copied_level_ != level_) {
    diagram_->copyLevel(copied_level_, level_);
  }
}

void LevelWidget::extrudeUpwards() {
  diagram_->copyLevel(level_, level_ + 1);
  setLevel(level_ + 1);
}

void LevelWidget::extrudeDownwards() {
  diagram_->copyLevel(level_, level_ - 1);
  setLevel(level_ - 1);
}

void LevelWidget::setTemplateImage(const QString& filename) {
  if (!filename.isEmpty()) {
    template_image_ = QPixmap(filename);
    update();
  }
}

void LevelWidget::clearTemplateImage() {
  template_image_ = QPixmap();
  update();
}
