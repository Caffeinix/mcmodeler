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

#include "gl_preview_window.h"

#include "gl_widget.h"

GLPreviewWindow::GLPreviewWindow(QWidget* parent)
    : QMainWindow(parent) {
  ui.setupUi(this);

  ui.frame_rate_check_box_->setAttribute(Qt::WA_MacSmallSize);
  ui.status_bar_->addPermanentWidget(ui.frame_rate_check_box_);
}

void GLPreviewWindow::setDiagram(Diagram* diagram) {
  ui.gl_widget_->setDiagram(diagram);
}

void GLPreviewWindow::setBlockManager(BlockManager* block_mgr) {
  ui.gl_widget_->setBlockManager(block_mgr);
}

GLWidget* GLPreviewWindow::glWidget() const {
  return ui.gl_widget_;
}
