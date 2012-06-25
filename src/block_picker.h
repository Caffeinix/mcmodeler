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

#ifndef BLOCK_PICKER_H
#define BLOCK_PICKER_H

#include <QWidget>

#include "ui_block_picker.h"
#include "block_type.h"

class BlockPickerItemDelegate;
class BlockPrototype;
class QListWidget;
class QListWidgetItem;

class BlockPicker : public QWidget {
  Q_OBJECT

 public:
  explicit BlockPicker(QWidget* parent = NULL);
  ~BlockPicker();

  void addBlock(BlockPrototype* block);

 signals:
  void blockSelected(blocktype_t type);

 private slots:
  void updateSelectedBlock();
  void selectBlockForItem(QListWidgetItem* item);

 private:
  static QIcon iconForSprite(const QPixmap& sprite);

  QWidget* findTab(const QString& text);
  QListWidget* createTab();

  BlockPickerItemDelegate* item_delegate_;

  Ui::BlockPicker ui;
};

#endif // BLOCK_PICKER_H
