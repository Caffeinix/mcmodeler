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

#include "tool_picker_item_delegate.h"

#include <QDebug>

#include <QPainter>

ToolPickerItemDelegate::ToolPickerItemDelegate(QObject* parent) : QItemDelegate(parent) {
}

void ToolPickerItemDelegate::drawDecoration(QPainter* painter, const QStyleOptionViewItem& option,
                                            const QRect& rect, const QPixmap& pixmap) const {
  if (option.state & QStyle::State_Selected) {
    painter->setRenderHint(QPainter::Antialiasing, true);
    QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
    gradient.setColorAt(0, option.palette.highlight().color().lighter(150));
    gradient.setColorAt(1, option.palette.highlight().color().lighter(110));
    QBrush brush(gradient);
    painter->setBrush(brush);
    painter->setPen(QPen(option.palette.highlight(), 1.0f));
    QRectF rect = option.rect;
    rect.adjust(0.5, 0.5, -0.5, -0.5);
    painter->drawRect(rect);
  }
  QItemDelegate::drawDecoration(painter, option, rect, pixmap);
}
