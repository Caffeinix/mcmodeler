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

#include "application.h"

#include "gl_preview_window.h"
#include "main_window.h"

#include "block_prototype.h"

#include <QDebug>

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv), block_mgr_(NULL) {
  setApplicationName("MCModeler");
  setApplicationVersion("0.1 dev 2");

  BlockPrototype::setupBlockProperties();

  MainWindow* main_window = new MainWindow(NULL);
  GLPreviewWindow* gl_preview_window = new GLPreviewWindow(NULL);

  diagram_.reset(new Diagram());
  block_mgr_.reset(new BlockManager(diagram_.data(), gl_preview_window->glWidget()));
  diagram_->setBlockManager(block_mgr_.data());

  gl_preview_window->setDiagram(diagram_.data());
  gl_preview_window->setBlockManager(block_mgr_.data());

  main_window->setDiagram(diagram_.data());
  main_window->setBlockManager(block_mgr_.data());

  main_window->show();
  gl_preview_window->show();
}

Application::~Application() {
}
