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

#include "block_picker.h"

#include "block_picker_item_delegate.h"
#include "block_prototype.h"

BlockPicker::BlockPicker(QWidget* parent)
    : QWidget(parent) {
  ui.setupUi(this);
  ui.tab_widget_->setAttribute(Qt::WA_MacSmallSize, true);
  item_delegate_ = new BlockPickerItemDelegate(this);
  connect(ui.tab_widget_, SIGNAL(currentChanged(int)), SLOT(updateSelectedBlock()));
}

BlockPicker::~BlockPicker() {
}

void BlockPicker::setUpSelection() {
  ui.tab_widget_->setCurrentIndex(0);
  updateSelectedBlock();
}

QWidget* BlockPicker::findTab(const QString& text) {
  for (int i = 0; i < ui.tab_widget_->count(); ++i) {
    if (ui.tab_widget_->tabText(i) == text) {
      return ui.tab_widget_->widget(i);
    }
  }
  return NULL;
}

QIcon BlockPicker::iconForSprite(const QPixmap& sprite) {
  QIcon icon(sprite);
  icon.addPixmap(sprite, QIcon::Active);
  icon.addPixmap(sprite, QIcon::Selected);
  return icon;
}

QListWidget* BlockPicker::createTab() {
  QListWidget* list = new QListWidget(this);
  list->setAttribute(Qt::WA_MacShowFocusRect, false);
  list->setViewMode(QListWidget::IconMode);
  list->setMovement(QListWidget::Static);
  list->setResizeMode(QListWidget::Adjust);
  list->setSelectionMode(QListWidget::SingleSelection);
  list->setUniformItemSizes(true);
  list->setItemDelegate(item_delegate_);
  connect(list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
          SLOT(selectBlockForItem(QListWidgetItem*)));
  return list;
}

void BlockPicker::updateSelectedBlock() {
  QListWidget* list = qobject_cast<QListWidget*>(ui.tab_widget_->currentWidget());
  if (!list) {
    qWarning() << "Couldn't update selected block.  This tab wasn't a QListWidget, it was a"
               << ui.tab_widget_->currentWidget()->metaObject()->className();
    return;
  }

  // If the newly selected tab doesn't have a selected item, just leave the previous tab's selected block as the
  // active block.  It's a bit strange, but it'll be more intuitive for the user than selecting a null block.
  QListWidgetItem* item = list->currentItem();
  if (item) {
    selectBlockForItem(item);
  }
}

void BlockPicker::selectBlockForItem(QListWidgetItem* item) {
  emit blockSelected(item->data(Qt::UserRole).toInt());
}

void BlockPicker::addBlock(BlockPrototype* block) {
  foreach (const QString& raw_category, QStringList() << "All blocks" << block->categories()) {
    QString category = raw_category;
    category[0] = category[0].toUpper();
    QWidget* tab = findTab(category);
    QListWidget* list = NULL;
    if (!tab) {
      list = createTab();
      ui.tab_widget_->addTab(list, category);
    } else {
      list = qobject_cast<QListWidget*>(tab);
    }
    if (!list) {
      qWarning() << "Couldn't add block for category" << category
                 << "-- this tab wasn't a QListWidget, it was a" << tab->metaObject()->className();
      continue;
    }
    QListWidgetItem* item = new QListWidgetItem(
          iconForSprite(block->sprite(BlockOrientation::paletteOrientation())), QString());
    item->setSizeHint(QSize(24, 24));
    item->setData(Qt::UserRole, block->type());
    item->setToolTip(block->name());
    list->addItem(item);
    list->setCurrentRow(0);
  }
}
