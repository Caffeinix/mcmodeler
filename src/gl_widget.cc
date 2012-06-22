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

#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "gl_widget.h"

#include "block_manager.h"
#include "cube.h"
#include "diagram.h"
#include "matrix.h"
#include "texture.h"

#ifndef GL_MULTISAMPLE_ARB
#define GL_MULTISAMPLE_ARB 0x809D
#endif

#ifndef GL_SAMPLE_ALPHA_TO_COVERAGE_ARB
#define GL_SAMPLE_ALPHA_TO_COVERAGE_ARB 0x809E
#endif

#define DEG_TO_RAD(x) ((x) * M_PI / 180.0f)

GLWidget::GLWidget(QWidget* parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
      diagram_(NULL),
      block_mgr_(NULL),
      frame_rate_enabled_(false),
      frame_rate_(-1.0f),
      scene_dirty_(true) {
  setFocusPolicy(Qt::WheelFocus);
  QGLFormat f = format();
  f.setSwapInterval(1);
  setFormat(f);
  connect(&frame_timer_, SIGNAL(timeout()), SLOT(updateGL()));
#ifdef NDEBUG
  frame_timer_.setDebugMode(false);
#endif
}

GLWidget::~GLWidget() {
}

void GLWidget::setDiagram(Diagram *diagram) {
  diagram_ = diagram;
  connect(diagram_, SIGNAL(diagramChanged(BlockTransaction)), SLOT(setSceneDirty()));
}

void GLWidget::setBlockManager(BlockManager *block_mgr) {
  block_mgr_ = block_mgr;
}

QSize GLWidget::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize GLWidget::sizeHint() const {
  return QSize(600, 400);
}

void GLWidget::setSceneDirty(bool dirty) {
  scene_dirty_ = dirty;
  if (dirty) {
    updateGL();
  }
}

void GLWidget::initializeGL() {
  qglClearColor(QColor(128, 192, 255));

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_MULTISAMPLE_ARB);
  glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_FOG);
  glFogf(GL_FOG_MODE, GL_LINEAR);
  static GLfloat fog_color[] = { 0.5f, 0.75f, 1.0f, 1.0f };
  glFogfv(GL_FOG_COLOR, fog_color);
  glFogf(GL_FOG_START, 50);
  glFogf(GL_FOG_END, 100);
//  glEnable(GL_LIGHT1);
  static GLfloat light_position[4] = { 0.5, 0.0, 0.0, 1.0 };
  static GLfloat light_ambient_intensity[4] = { 0.0, 0.0, 0.0, 1.0 };
  static GLfloat light_diffuse_intensity[4] = { 2.0, 2.0, 2.0, 1.0 };
  glLightfv(GL_LIGHT1, GL_POSITION, light_position);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient_intensity);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse_intensity);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.1f);

  GLfloat global_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

  ground_plane_display_list_ = glGenLists(1);
  scene_display_list_ = glGenLists(2);
  glNewList(ground_plane_display_list_, GL_COMPILE);
  glPushMatrix();
  glTranslatef(-50.0f, 0.005, -50.0f);
  for (int i = 0; i < 100; ++i) {
    glBegin(GL_QUAD_STRIP);
    glVertex3f(i, 0, 0);
    glVertex3f(i + 1.0f, 0, 0);
    for (int j = 0; j < 100; ++j) {
      glVertex3f(i, 0, j + 1.0f);
      glVertex3f(i + 1.0f, 0, j + 1.0f);
    }
    glEnd();
  }
  glPopMatrix();
  glEndList();

  camera_.translate(QVector3D(0.5, 1, 5));

//  grass_.reset(new Block(Block::kGrass, diagram_.data(), this));
//  sand_.reset(new Block(Block::kSand, diagram_.data(), this));
//  dirt_.reset(new Block(Block::kDirt, diagram_.data(), this));
//  stone_.reset(new Block(Block::kStone, diagram_.data(), this));
//  chest_.reset(new Block(Block::kChest, diagram_.data(), this));
//  furnace_.reset(new Block(Block::kFurnace, diagram_.data(), this));
//  workbench_.reset(new Block(Block::kWorkbench, diagram_.data(), this));

//  static const int kTestExtent = 15;
//  for (int i = -kTestExtent; i < kTestExtent; ++i) {
//    for (int j = -kTestExtent; j < kTestExtent; ++j) {
//      for (int k = -kTestExtent; k < kTestExtent; ++k) {
//        if (random() % 4 == 0) {
//          diagram_->addBlock(grass_.data(), QVector3D(i + 0.5, j + 0.5, k + 0.5));
//        }
//      }
//    }
//  }

//  for (int i = 0; i < 500; ++i) {
//    diagram_->addBlock(sand_.data(), QVector3D(-25 + (i % 50 + 0.5), i / 250 + 0.5, -(i / 50) - 0.5));
//  }
//  for (int i = 500; i < 2000; ++i) {
//    diagram_->addBlock(grass_.data(), QVector3D(-25 + (i % 50 + 0.5), i / 250 + 0.5, -(i / 50) - 0.5));
//  }
//  static const int size = 15;
//  for (int i = -size/2; i < size/2; ++i) {
//    for (int j = 0; j < size; ++j) {
//      for (int k = -size; k < 0; ++k) {
//        diagram_->addBlock(grass_.data(), QVector3D(i + 0.5, j + 0.5, k + 0.5));
//      }
//    }
//  }
//  for (int x = -8; x < 8; ++x) {
//    for (int z = 0; z < 16; ++z) {
//      diagram_->addBlock(stone_.data(), QVector3D(x + 0.5, 0.5, -z + 0.5));
//    }
//  }

//  diagram_->addBlock(chest_.data(), QVector3D(-6.5, 1.5, -13.5));
//  diagram_->addBlock(furnace_.data(), QVector3D(-5.5, 1.5, -13.5));
//  diagram_->addBlock(furnace_.data(), QVector3D(-4.5, 1.5, -13.5));
//  diagram_->addBlock(workbench_.data(), QVector3D(-3.5, 1.5, -13.5));

//  for (int y = 1; y < 8; ++y) {
//    diagram_->addBlock(stone_.data(), QVector3D(-7.5, y + 0.5, 0.5));
//    diagram_->addBlock(stone_.data(), QVector3D(7.5, y + 0.5, 0.5));
//    for (int z = 1; z < 15; ++z) {
//      diagram_->addBlock(dirt_.data(), QVector3D(7.5, y + 0.5, -z + 0.5));
//    }
//    diagram_->addBlock(stone_.data(), QVector3D(-7.5, y + 0.5, -14.5));
//    for (int x = -7; x < 7; ++x) {
//      diagram_->addBlock(dirt_.data(), QVector3D(x + 0.5, y + 0.5, -14.5));
//    }
//    diagram_->addBlock(stone_.data(), QVector3D(7.5, y + 0.5, -14.5));
//    for (int z = 1; z < 15; ++z) {
//      diagram_->addBlock(dirt_.data(), QVector3D(-7.5, y + 0.5, -z + 0.5));
//    }

//    for (int x = -9; x < 9; ++x) {
//      for (int z = -1; z < 17; ++z) {
//        for (int y = 8; y < 17; ++y) {
//          int level = y - 8;
//          if (x > 8 - level || x < -9 + level) {
//            continue;
//          }
//          if (z > 16 - level || z < level) {
//            continue;
//          }
//          if (level == 0) {
//            diagram_->addBlock(stone_.data(), QVector3D(x + 0.5, y + 0.5, -z + 0.5));
//          } else {
//            diagram_->addBlock(grass_.data(), QVector3D(x + 0.5, y + 0.5, -z + 0.5));
//          }
//        }
//      }
//    }
//  }

//  QFile file("/Users/phoenix/Desktop/test.mcdiagram");
//  file.open(QIODevice::WriteOnly);
//  QDataStream ostream(&file);
//  diagram_->save(&ostream);
//  file.close();

//  QFile file("/Users/phoenix/Desktop/test.mcdiagram");
//  file.open(QIODevice::ReadOnly);
//  QDataStream istream(&file);
//  diagram_.reset(new Diagram(&istream, this));
//  file.close();
}

void GLWidget::drawSkybox() {
  Cube skybox(10.0f);
  Texture skybox_up = Texture(this, ":/skybox_up.png");
  Texture skybox_out = Texture(this, ":/skybox_east.png");
  Texture skybox_down = Texture(this, ":/skybox_down.png");
  skybox.setTexture(kFrontFace, skybox_out);
  skybox.setTexture(kLeftFace, skybox_out);
  skybox.setTexture(kRightFace, skybox_out);
  skybox.setTexture(kBackFace, skybox_out);
  skybox.setTexture(kTopFace, skybox_up);
  skybox.setTexture(kBottomFace, skybox_down);
  glPushMatrix();
  glLoadIdentity();
  camera_.applyRotation();
  glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BITS | GL_FOG_BIT | GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);
  glDisable(GL_FOG);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  skybox.renderAt(QVector3D(0, 0, 0));
  glPopAttrib();
  glPopMatrix();
}

void GLWidget::updateScene() {
  if (!diagram_) {
    return;
  }

  glNewList(scene_display_list_, GL_COMPILE_AND_EXECUTE);
  // Draw the ground plane.
  glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT);
  glDisable(GL_CULL_FACE);
  GLfloat color[] = {0, 1, 0, 0.5};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
  glCallList(ground_plane_display_list_);
  glDisable(GL_LIGHTING);
  glDisable(GL_MULTISAMPLE_ARB);
  glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);
  glBindTexture(GL_TEXTURE_2D, NULL);
  glColor4f(1, 1, 1, 0.1);
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glPushMatrix();
  glTranslatef(0, 0.01f, 0);
  glCallList(ground_plane_display_list_);
  glPopMatrix();
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glPopAttrib();

  diagram_->render();

  glEndList();
}

void GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  applyPressedKeys();

  // Handle frame rate display.
  if (frame_rate_enabled_) {
    static const int kFrameRateSampleCount = 60;
    float elapsed_time = time_since_last_frame_.elapsed() / 1000.0f;  // seconds
    frame_rate_queue_.enqueue(elapsed_time);
    while (frame_rate_queue_.size() > kFrameRateSampleCount) {
      frame_rate_queue_.dequeue();
    }
    float average_frame_time = 0.0f;
    foreach(float sample, frame_rate_queue_) {
      average_frame_time += sample;
    }
    average_frame_time /= frame_rate_queue_.size();

    frame_rate_ = 1.0f / average_frame_time;
    emit frameRateChanged(QString::number(frame_rate_, 'g', 4));
  }

  time_since_last_frame_.start();
  camera_.apply();

  static GLfloat light_position[4] = { 0.0, 10.0, 5.0, 0.0 };
  static GLfloat light_ambient_intensity[4] = { 3, 3, 3, 1.0 };
  static GLfloat light_diffuse_intensity[4] = { 0.8, 0.8, 0.8, 1.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient_intensity);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_intensity);

  // Can't put this in the display list or it doesn't rotate correctly.
  drawSkybox();

  if (scene_dirty_) {
    updateScene();
    scene_dirty_ = false;
  } else {
    glCallList(scene_display_list_);
  }

  // Handle frame stats.
  if (diagram_) {
    emit frameStatsChanged(QString("%1 blocks").arg(diagram_->blockCount()));
  } else {
    emit frameStatsChanged(QString("0 blocks"));
  }
}

void GLWidget::resizeGL(int width, int height) {
  const float aspect = float(width) / float(height);
  const float near_clip = 0.01;
  const float far_clip = 100;

  const float tan30 = tanf(DEG_TO_RAD(30));

  float top = tan30 * near_clip;
  float bottom = -top;

  float right = tan30 * near_clip * aspect;
  float left = -right;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(left, right, bottom, top, near_clip, far_clip);
  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 0, width, height);
}

void GLWidget::mousePressEvent(QMouseEvent* event) {
  lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent* event) {
  static const float kCameraSensitivity = 4.0f;
  float dx = static_cast<float>(event->x() - lastPos.x()) / height();
  float dy = static_cast<float>(event->y() - lastPos.y()) / height();

  if (event->buttons() & Qt::LeftButton) {
    camera_.rotateX(dx * kCameraSensitivity);
    camera_.rotateY(dy * kCameraSensitivity);
  }
  lastPos = event->pos();
  updateGL();
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
  if (!event->isAutoRepeat()) {
    pressed_keys_.insert(event->key());
    time_since_last_frame_.start();
    frame_timer_.pushRenderer("Key press handler");
  }
}

void GLWidget::keyReleaseEvent(QKeyEvent* event) {
  if (!event->isAutoRepeat()) {
    pressed_keys_.remove(event->key());
    frame_timer_.popRenderer("Key press handler");
  }
}

void GLWidget::applyPressedKeys() {
  static const QVector3D right(1, 0,  0);
  static const QVector3D    up(0, 1,  0);
  static const QVector3D  look(0, 0, -1);

  float elapsed_time = time_since_last_frame_.elapsed() / 1000.0f;  // seconds.
  float desired_velocity = 10.0f;  // meters per second.
  float distance = elapsed_time * desired_velocity;  // meters.

  foreach (int key, pressed_keys_) {
    switch (key) {
    case Qt::Key_W:
      camera_.translate(QVector3D(0, 0, -distance));
      break;
    case Qt::Key_S:
      camera_.translate(QVector3D(0, 0, distance));
      break;
    case Qt::Key_A:
      camera_.translate(QVector3D(-distance, 0, 0));
      break;
    case Qt::Key_D:
      camera_.translate(QVector3D(distance, 0, 0));
      break;
    case Qt::Key_E:
      camera_.translateWorld(QVector3D(0, distance, 0));
      break;
    case Qt::Key_C:
      camera_.translateWorld(QVector3D(0, -distance, 0));
      break;
    default:
      break;
    }
  }
}

void GLWidget::enableFrameRate(bool enable) {
  if (enable == frame_rate_enabled_) {
    return;
  }
  if (enable) {
    frame_timer_.pushRenderer("Frame rate");
  } else {
    frame_timer_.popRenderer("Frame rate");
    frame_rate_ = -1.0f;
  }
  frame_rate_enabled_ = enable;
}
