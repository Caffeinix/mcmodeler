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

#include "tool_picker.h"
#include "ui_tool_picker.h"

#include <QIcon>

#include "block_manager.h"
#include "diagram.h"
#include "pencil_tool.h"
#include "line_tool.h"
#include "rectangle_tool.h"
#include "qvariant_ptr.h"
#include "tool_picker_item_delegate.h"

ToolPicker::ToolPicker(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::ToolPicker) {
  ui->setupUi(this);
  ui->list_widget_->setItemDelegate(new ToolPickerItemDelegate(this));
  connect(ui->list_widget_, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
          SLOT(updateCurrentTool(QListWidgetItem*, QListWidgetItem*)));
}

void ToolPicker::setDiagram(Diagram* diagram) {
  QListWidgetItem* item = new QListWidgetItem();
  item->setToolTip("Pencil");
  item->setIcon(QIcon(":/icons/pencil_tool.png"));
  item->setData(Qt::UserRole, QVariantPtr<Tool>::asVariant(new PencilTool(diagram)));
  ui->list_widget_->addItem(item);

  item = new QListWidgetItem();
  item->setToolTip("Line");
  item->setIcon(QIcon(":/icons/line_tool.png"));
  item->setData(Qt::UserRole, QVariantPtr<Tool>::asVariant(new LineTool(diagram)));
  ui->list_widget_->addItem(item);

  item = new QListWidgetItem();
  item->setToolTip("Rectangle");
  item->setIcon(QIcon(":/icons/rectangle_tool.png"));
  item->setData(Qt::UserRole, QVariantPtr<Tool>::asVariant(new RectangleTool(diagram)));
  ui->list_widget_->addItem(item);

  // Select the pencil tool.
  ui->list_widget_->setCurrentRow(0);
  updateCurrentTool(ui->list_widget_->item(0), NULL);
}

void ToolPicker::updateCurrentTool(QListWidgetItem* new_item, QListWidgetItem* old_item) {
  if (!new_item) {
    return;
  }
  Tool* new_tool = QVariantPtr<Tool>::asPointer(new_item->data(Qt::UserRole));
  emit currentToolChanged(new_tool);
}

ToolPicker::~ToolPicker() {
  delete ui;
}
