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

#ifndef LEVEL_WIDGET_H
#define LEVEL_WIDGET_H

#include <QGraphicsColorizeEffect>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScopedPointer>

#include "block_type.h"
#include "block_position.h"

class Diagram;
class BlockInstance;
class BlockManager;
class BlockTransaction;

class LevelWidget : public QGraphicsView {
  Q_OBJECT
 public:
  explicit LevelWidget(QWidget* parent = NULL);
  void setDiagram(Diagram* diagram);
  void setBlockManager(BlockManager* block_mgr);

 signals:
  void levelChanged(int level);

 public slots:
  void setLevel(int level);

  void setBlockType(BlockType type);

  void copyLevel();
  void pasteLevel();

  void extrudeUpwards();
  void extrudeDownwards();

  void setTemplateImage(const QString& filename);
  void clearTemplateImage();


 protected:
  BlockPosition positionForPoint(const QPointF& point) const;
  BlockPosition positionForEvent(QMouseEvent* event) const;
  QGraphicsItem* itemAtPosition(const BlockPosition& position) const;

  void drawBackground(QPainter* painter, const QRectF& rect);
  void drawForeground(QPainter* painter, const QRectF& rect);

  bool event(QEvent* event);
  void keyPressEvent(QKeyEvent* event);
  void keyReleaseEvent(QKeyEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void mouseDoubleClickEvent(QMouseEvent* event);

  void toggleBlock(QMouseEvent* event);
  void drawLine(QMouseEvent* event);
  void fillBlocks(QMouseEvent* event);

  void loadLevel();

  QGraphicsItem* addBlock(const BlockInstance& block);
  void removeBlock(const BlockPosition& position);

 protected slots:
  void updateLevel(const BlockTransaction& transaction);

 private:
  QHash<BlockPosition, QGraphicsItem*> item_model_;
  QGraphicsScene* scene_;
  int level_;
  Diagram* diagram_;
  BlockManager* block_mgr_;
  bool in_pan_mode_;
  BlockPosition last_block_position_;
  QPoint last_move_event_pos_;
  BlockType block_type_;
  QPixmap template_image_;
  int copied_level_;
};

#endif // LEVEL_WIDGET_H
