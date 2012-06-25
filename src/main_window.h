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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui_main_window.h"

class Diagram;
class BlockManager;

#include "bill_of_materials_window.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = NULL);

  void setDiagram(Diagram* diagram);
  void setBlockManager(BlockManager* block_mgr);

 private slots:
  void about();

  void open();
  void save();
  void saveAs();
  void quit();

  void setDocumentModified();

  void setTemplateImage();

  void saveToFile(const QString& filename);
  void openFile(const QString& filename);
  void onSavePromptClosed(QAbstractButton* button);

  void showBillOfMaterials();

 protected:
  void closeEvent(QCloseEvent* event);

 private:
  void doOpen();
  void maybeSave();

  void performPendingAction();

  void setupToolbox();

  Ui::MainWindow ui;
  Diagram* diagram_;
  BlockManager* block_mgr_;
  bool toolbox_initialized_;
  QAction* pending_action_;
  QScopedPointer<BillOfMaterialsWindow> bill_of_materials_window_;
};

#endif // MAIN_WINDOW_H
