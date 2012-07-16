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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QScopedPointer>

#include "block_manager.h"
#include "diagram.h"

/**
  * Represents the application.  Since Qt guarantees that there will only ever be one of these, it is a convenient owner
  * for other effectively global objects such as the BlockManager and Diagram.  If you ever think you need a global
  * object or a singleton, what you really want to do is add another member to Application.
  */
class Application : public QApplication {
  Q_OBJECT
 public:
  explicit Application(int& argc, char** argv);
  virtual ~Application();

  /**
    * Returns the application instance.  This is equivalent to `static_cast<Application*>(qApp)`.
    */
  static Application* instance();

  /**
    * Returns the application settings.
    */
  QSettings* settings() const;

 private:
  QScopedPointer<Diagram> diagram_;
  QScopedPointer<BlockManager> block_mgr_;
  QScopedPointer<QSettings> settings_;
};

#endif // APPLICATION_H
