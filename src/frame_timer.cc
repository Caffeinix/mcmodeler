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

#include "frame_timer.h"

#include <QDebug>

FrameTimer::FrameTimer(QObject* parent) : QObject(parent), renderer_count_(0), debug_mode_(true) {
  timer_.setInterval(0);
  connect(&timer_, SIGNAL(timeout()), this, SIGNAL(timeout()));
}

void FrameTimer::pushRenderer(const QString& renderer) {
  if (debug_mode_) {
    if (renderers_.contains(renderer)) {
      renderers_.insert(renderer, renderers_.value(renderer) + 1);
    } else {
      renderers_.insert(renderer, 1);
    }
  } else {
    ++renderer_count_;
  }
  timer_.start();
}

void FrameTimer::popRenderer(const QString& renderer) {
  int count;
  if (debug_mode_) {
    if (!renderers_.contains(renderer)) {
      qWarning() << "Renderer" << renderer << "not in renderers list:" << renderers_;
    }
    if (renderers_.value(renderer) > 1) {
      renderers_.insert(renderer, renderers_.value(renderer) - 1);
    } else {
      renderers_.remove(renderer);
    }
    count = renderers_.count();
  } else {
    --renderer_count_;
    count = renderer_count_;
  }
  Q_ASSERT(count >= 0);
  if (count == 0) {
    timer_.stop();
  }
}

void FrameTimer::dumpRenderers() const {
  if (debug_mode_) {
    qDebug() << renderers_;
  } else {
    qDebug() << renderer_count_ << "renderers (enable debug mode for more info)";
  }
}
