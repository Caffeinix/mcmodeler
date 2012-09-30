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

#ifndef TOOL_PICKER_H
#define TOOL_PICKER_H

#include <QWidget>

class QListWidgetItem;

class BlockManager;
class Diagram;
class Tool;

namespace Ui {
class ToolPicker;
}

class ToolPicker : public QWidget {
  Q_OBJECT

 public:
  explicit ToolPicker(QWidget* parent = NULL);
  virtual ~ToolPicker();

  void addTool(Tool* tool, QString label, QIcon icon);

 signals:
  void currentToolChanged(Tool* new_tool);

 private slots:
  void updateCurrentTool(QListWidgetItem* old_item, QListWidgetItem* new_item);

 private:
  Ui::ToolPicker* ui;
  Diagram* diagram_;
  BlockManager* block_mgr_;
};

#endif // TOOL_PICKER_H
