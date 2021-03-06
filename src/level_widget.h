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

#include <QUndoStack>
#include <QUndoView>

#include "block_type.h"
#include "block_position.h"

class Diagram;
class BlockInstance;
class BlockManager;
class BlockTransaction;
class Tool;

/**
  * The canvas widget with which the user can interact to add and remove blocks.
  *
  * The LevelWidget is a view in the sense of model/view/controller.  As such, it _never_ directly changes the state of
  * the world, but merely updates and listens for changes to the model (in this case, a Diagram).
  *
  * When the diagram changes, it emits Diagram::diagramChanged(), which is connected to updateLevel() in this class.
  * This causes the view to apply the BlockTransaction (which was very likely a consequence of its own request to the
  * Diagram), which synchronizes the view with the model.
  *
  * tl;dr: Don't mess around with the QGraphicsItems in any method other than updateLevel().
  */
class LevelWidget : public QGraphicsView {
  Q_OBJECT
 public:
  explicit LevelWidget(QWidget* parent = NULL);
  virtual ~LevelWidget();

  void setDiagram(Diagram* diagram);
  void setBlockManager(BlockManager* block_mgr);

 signals:
  /**
    * Emitted whenever the currently displayed level changes.
    * @param level The new level being displayed by the widget.
    */
  void levelChanged(int level);

 public slots:
  /**
    * Sets the level currently being rendered to \p level.
    */
  void setLevel(int level);

  /**
    * Sets the block type with which we are currently drawing to \p type.
    */
  void setBlockType(blocktype_t type);

  /**
    * Sets the tool that is selected in the tool picker to \p tool.
    *
    * @note This may not match the tool returned by currentTool() if a modifier tool is specified.
    * @sa updateTool()
    */
  void setSelectedTool(Tool* tool);

  /**
    * Copies the current level into a special clipboard-like buffer.
    */
  void copyLevel();

  /**
    * Pastes the previously copied level, replacing everything on the current level.  If there is no copied level in
    * the buffer, does nothing.
    */
  void pasteLevel();

  /**
    * Copies the current level to the next level above, overwriting whatever is there, and switches to that level.
    */
  void extrudeUpwards();

  /**
    * Copies the current level to the next level below, overwriting whatever is there, and switches to that level.
    */
  void extrudeDownwards();

  /**
    * Sets the image located at the file path \p filename to be the "template image".  It will be shown in a faded out
    * state on all levels, and blocks will be drawn on top of it.  It is not saved into the diagram.
    */
  void setTemplateImage(const QString& filename);

  /**
    * Clears the template image.
    * @sa setTemplateImage()
    */
  void clearTemplateImage();


 protected:
  /**
    * Returns the 3D position for \p point given the current level.
    */
  BlockPosition positionForPoint(const QPointF& point) const;

  /**
    * Returns the 3D position for the point at which \p event occurred.
    */
  BlockPosition positionForEvent(QMouseEvent* event) const;

  /**
    * Returns the QGraphicsItem at the 2D point corresponding to \p position in 3D space.  If \p position does not
    * represent a position on this level, returns NULL.
    */
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

  /**
    * Checks for key press events that might affect the current tool, and changes it if necessary.
    */
  void updateTool(QKeyEvent* event);

  /**
    * Checks for mouse event modifiers that might affect the current tool, and changes it if necessary.
    */
  void updateTool(QMouseEvent* event);

  /**
    * Tells the Diagram to set the block at the position of \p event, using the currently set block type.  If \p event
    * is a mouse event involving the left mouse button, then the block will be set to the currently set type unless it
    * already is that type, in which case its orientation will be cycled instead.  If \p event involves the right mouse
    * button, the block will be cleared instead.
    * @sa setBlockType()
    * @sa Diagram::setBlock()
    */
  void toggleBlock(QMouseEvent* event);

  /**
    * Synchronizes the view with the diagram by clearing all blocks and drawing them again.  This is an expensive
    * operation, and you should rarely need to call it; updateLevel() will apply incremental changes to the view when
    * the Diagram changes, so loadLevel() need only be called when the view is hopelessly out of sync.
    */
  void loadLevel();

  /**
    * Creates and returns a QGraphicsItem representing \p block in the view.  The QGraphicsItem is shown immediately.
    * @note This does not alter the Diagram, and is used internally to populate blocks into the view that already exist
    *       in the model.  If you wish to add or remove blocks in the world, call the appropriate methods on Diagram
    *       instead and let them update LevelWidget automatically.
    */
  QGraphicsItem* addBlock(const BlockInstance& block);

  /**
    * Removes the QGraphicsItem representing the block a \p position from the view.
    * @note This does not alter the Diagram, and is used internally to remove blocks from the view that have been
    *       deleted in the model.  If you wish to add or remove blocks in the world, call the appropriate methods on
    *       Diagram instead and let them update LevelWidget automatically.
    */
  void removeBlock(const BlockPosition& position);

  /**
    * Adds \p block to the LevelWidget as an ephemeral block.  The block will only be shown until the next time
    * updateEphemeralBlocks() is called, at which point it will be removed.  Ephemeral blocks are drawn on top of
    * non-ephemeral blocks, and will be drawn translucent if the current tool is not a brush.
    */
  QGraphicsItem* ephemerallyAddBlock(const BlockInstance& block);

  /**
    * Adds \p block to the LevelWidget as an ephemerally removed block.  The block will only be hidden until the next
    * time updateEphemeralBlocks() is called, at which point it will be shown again.  If a block is ephemerally
    * removed from a position and another is ephemerally added there, the added one will take precedence.
    *
    * @note Ephemerally removed blocks are implemented using a hack.  Rather than actually removing blocks from the
    * item model and adding them back when the ephemeral blocks are cleared, we simply overlay the existing block with
    * an ephemeral block painted to look like the canvas background.  This prevents us from having to remember what
    * block was there before and reinstate it later.  It also makes it possible to "partially" remove blocks, for
    * instance if the tool doing the removal is not a brush.
    */
  QGraphicsItem* ephemerallyRemoveBlock(const BlockInstance& block);

 protected slots:
  /**
    * Applies \p transaction to the current level.  Called whenever the Diagram changes.
    */
  void updateLevel(const BlockTransaction& transaction);

  /**
    * Applies \p transaction to the ephemeral blocks of the current level.  Both added and removed blocks are applied.
    * Called whenever the Diagram changes its ephemeral blocks.
    */
  void updateEphemeralBlocks(const BlockTransaction& transaction);

 protected:
  virtual void showEvent(QShowEvent* event);

 private:
  /**
    * Returns the tool the level widget should currently use to draw.  This may be either selected_tool_ or
    * modifier_tool_ depending on the circumstances.
    */
  Tool* currentTool() const;

  QHash<BlockPosition, QGraphicsItem*> item_model_;
  QVector<QGraphicsItem*> ephemeral_items_;
  QGraphicsScene* scene_;
  int level_;
  Diagram* diagram_;
  BlockManager* block_mgr_;
  bool in_pan_mode_;
  BlockPosition last_block_position_;
  QPoint last_move_event_pos_;
  blocktype_t block_type_;
  QPixmap template_image_;
  int copied_level_;

  /// The tool that is currently selected in the tool picker.
  Tool* selected_tool_;

  /// The tool, if any, that we are using instead of selected_tool_ due to modifier keys.
  QScopedPointer<Tool> modifier_tool_;

  QUndoStack undo_stack_;
  QUndoView undo_view_;
};

#endif // LEVEL_WIDGET_H
