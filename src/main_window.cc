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

#include "main_window.h"

#include <QtGui/QApplication>

#include "about_box.h"
#include "block_manager.h"
#include "block_picker.h"
#include "block_prototype.h"
#include "block_type.h"
#include "diagram.h"
#include "tool_picker.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      diagram_(NULL),
      block_mgr_(NULL),
      toolbox_initialized_(false),
      pending_action_(NULL),
      bill_of_materials_window_(NULL) {
  ui.setupUi(this);

  move(12, 12);
#ifdef Q_OS_MACX
  ui.menu_bar_->setParent(NULL);
  move(12, 12 + ui.menu_bar_->height());
#endif

  ui.level_widget_->setLevel(ui.level_slider_->value());
  ui.tool_picker_->setAttribute(Qt::WA_MacShowFocusRect, false);
  connect(ui.tool_picker_, SIGNAL(currentToolChanged(Tool*)), ui.level_widget_, SLOT(setSelectedTool(Tool*)));
}

void MainWindow::setDiagram(Diagram* diagram) {
  diagram_ = diagram;
  ui.level_widget_->setDiagram(diagram);
  ui.tool_picker_->setDiagram(diagram);
  bill_of_materials_window_.reset(new BillOfMaterialsWindow(diagram));
  connect(diagram_, SIGNAL(diagramChanged(BlockTransaction)), SLOT(setDocumentModified()));
}

void MainWindow::setBlockManager(BlockManager* block_mgr) {
  block_mgr_ = block_mgr;
  ui.level_widget_->setBlockManager(block_mgr);
  ui.tool_picker_->setBlockManager(block_mgr);

  setupToolbox();
}

void MainWindow::setupToolbox() {
  BlockTypeIterator iter = BlockPrototype::blockIterator();
  connect(ui.block_picker_, SIGNAL(blockSelected(blocktype_t)),
          ui.level_widget_, SLOT(setBlockType(blocktype_t)));
  while (iter.hasNext()) {
    BlockPrototype* block = block_mgr_->getPrototype(iter.next());
    ui.block_picker_->addBlock(block);
  }
  ui.block_picker_->setUpSelection();
}

void MainWindow::setTemplateImage() {
  QFileDialog* open_dialog = new QFileDialog(this);
  open_dialog->setFileMode(QFileDialog::ExistingFile);
  open_dialog->setAcceptMode(QFileDialog::AcceptOpen);
  open_dialog->open(ui.level_widget_, SLOT(setTemplateImage(QString)));
}

void MainWindow::maybeSave() {
  QMessageBox* save_prompt = new QMessageBox(this);
  save_prompt->setIcon(QMessageBox::Question);
  save_prompt->setWindowTitle("MCModeler - Save Changes");
  save_prompt->setText("Would you like to save the changes you made to this diagram?");
  save_prompt->setInformativeText("Your changes will be lost if you don't save them.");
  save_prompt->addButton(QMessageBox::Save);
  save_prompt->addButton(QMessageBox::Cancel);
  save_prompt->addButton(QMessageBox::Discard);
  save_prompt->open(this, SLOT(onSavePromptClosed(QAbstractButton*)));
}

void MainWindow::onSavePromptClosed(QAbstractButton* button) {
  QMessageBox* prompt = qobject_cast<QMessageBox*>(sender());
  QMessageBox::ButtonRole role = prompt->buttonRole(button);
  if (role == QMessageBox::AcceptRole) {
    save();
  } else if (role == QMessageBox::DestructiveRole) {
    performPendingAction();
  }
}

void MainWindow::about() {
  AboutBox* about_box = new AboutBox(NULL);
  connect(about_box, SIGNAL(finished(int)), about_box, SLOT(deleteLater()));
  about_box->show();
}

void MainWindow::open() {
  pending_action_ = ui.action_open_;
  if (isWindowModified()) {
    maybeSave();
  } else {
    performPendingAction();
  }
}

void MainWindow::doOpen() {
  QFileDialog* open_dialog = new QFileDialog(this);
  open_dialog->setFileMode(QFileDialog::ExistingFile);
  open_dialog->setAcceptMode(QFileDialog::AcceptOpen);
  open_dialog->open(this, SLOT(openFile(QString)));
}

void MainWindow::save() {
  if (windowFilePath().isEmpty()) {
    saveAs();
  } else {
    saveToFile(windowFilePath());
  }
}

void MainWindow::saveAs() {
  QFileDialog* save_dialog = new QFileDialog(this);
  save_dialog->setFileMode(QFileDialog::AnyFile);
  save_dialog->setAcceptMode(QFileDialog::AcceptSave);
  save_dialog->setDefaultSuffix("mcdiagram");
  save_dialog->open(this, SLOT(saveToFile(QString)));
}

void MainWindow::saveToFile(const QString& filename) {
  QFileDialog* dlg = qobject_cast<QFileDialog*>(sender());
  if (dlg) {
    dlg->deleteLater();
  }
  if (!filename.isEmpty()) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
      // TODO(phoenix): Handle disappearing/unwritable files.
      return;
    }
    QDataStream ostream(&file);
    diagram_->save(&ostream);
    file.close();
    setWindowFilePath(filename);
    setWindowTitle(QFileInfo(filename).fileName());
    setWindowModified(false);
    performPendingAction();
  } else {
    // User canceled the save, so don't do whatever it was we were going to do.
    return;
  }
}

void MainWindow::openFile(const QString& filename) {
  QFileDialog* dlg = qobject_cast<QFileDialog*>(sender());
  if (dlg) {
    dlg->deleteLater();
  }
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    // TODO(phoenix): Handle unreadable files.
    return;
  }
  QDataStream istream(&file);
  diagram_->load(&istream);
  file.close();
  setWindowFilePath(filename);
  setWindowTitle(QFileInfo(filename).fileName());
  setWindowModified(false);
}

void MainWindow::showBillOfMaterials() {
  if (bill_of_materials_window_.isNull()) {
    return;
  }
  bill_of_materials_window_->setVisible(true);
}

void MainWindow::quit() {
  pending_action_ = ui.action_quit_;
  if (isWindowModified()) {
    maybeSave();
  } else {
    performPendingAction();
  }
}

void MainWindow::closeEvent(QCloseEvent* event) {
  quit();
  event->ignore();
}

void MainWindow::performPendingAction() {
  if (pending_action_ == ui.action_quit_) {
    qApp->quit();
  } else if (pending_action_ == ui.action_open_) {
    doOpen();
  }
  pending_action_ = NULL;
}

void MainWindow::setDocumentModified() {
  setWindowModified(true);
}
