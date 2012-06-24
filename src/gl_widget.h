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

#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include <QGLWidget>
#include <QSet>
#include <QTime>

#include "frame_timer.h"
#include "matrix.h"
#include "mouselook_cam.h"

class Diagram;
class BlockPrototype;
class BlockManager;
class Renderable;

class GLWidget : public QGLWidget {
  Q_OBJECT

 public:
  GLWidget(QWidget* parent = NULL);
  ~GLWidget();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

  float frameRate() const {
    return frame_rate_;
  }

  void setDiagram(Diagram* diagram);
  void setBlockManager(BlockManager* block_mgr);

 public slots:
  void enableFrameRate(bool enable);
  void setSceneDirty(bool dirty = true);

 signals:
  void frameRateChanged(const QString& frame_rate);
  void frameStatsChanged(const QString& frame_stats);

 protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);
  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void keyPressEvent(QKeyEvent* event);
  void keyReleaseEvent(QKeyEvent* event);

 private:
  void applyPressedKeys();
  void drawSkybox();
  void updateScene();

 private:
  Diagram* diagram_;
  BlockManager* block_mgr_;

  QScopedPointer<Renderable> skybox_;

  QPoint lastPos;
  GLuint ground_plane_display_list_;
  GLuint scene_display_list_;
  QSet<int> pressed_keys_;
  QTime time_since_last_frame_;
  QQueue<float> frame_rate_queue_;
  FrameTimer frame_timer_;
  bool frame_rate_enabled_;
  float frame_rate_;

  MouselookCam camera_;

  bool scene_dirty_;

  BlockPrototype* grass_;
  BlockPrototype* sand_;
  BlockPrototype* dirt_;
  BlockPrototype* stone_;
  BlockPrototype* chest_;
  BlockPrototype* furnace_;
  BlockPrototype* workbench_;
};

#endif
