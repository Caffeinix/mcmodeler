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

#ifndef BLOCK_PICKER_ITEM_DELEGATE_H
#define BLOCK_PICKER_ITEM_DELEGATE_H

#include <QItemDelegate>

class BlockPickerItemDelegate : public QItemDelegate {
  Q_OBJECT
 public:
  explicit BlockPickerItemDelegate(QObject* parent = NULL);

 protected:
  void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QPixmap &pixmap) const;
};

#endif // BLOCK_PICKER_ITEM_DELEGATE_H
