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

#ifndef FRAME_TIMER_H
#define FRAME_TIMER_H

#include <QObject>
#include <QSet>
#include <QTimer>

/**
  * Ensures that the view continues rendering constantly as long as there is at least one renderer attempting to update
  * the scene.  This is useful for example for the frame rate counter, which needs to keep the renderer updating at all
  * times to display an accurate frame count.
  */
class FrameTimer : public QObject {
  Q_OBJECT
 public:
  explicit FrameTimer(QObject* parent = NULL);

 signals:
  void timeout();

 public slots:
  void pushRenderer(const QString& renderer);
  void popRenderer(const QString& renderer);
  void setDebugMode(bool debug) {
    debug_mode_ = debug;
  }
  void dumpRenderers() const;

 private:
  QTimer timer_;
  QHash<QString, int> renderers_;
  int renderer_count_;
  bool debug_mode_;
};

#endif // FRAME_TIMER_H
