#include "block_picker_item_delegate.h"

#include <QDebug>

#include <QPainter>

BlockPickerItemDelegate::BlockPickerItemDelegate(QObject* parent) : QItemDelegate(parent) {
}

void BlockPickerItemDelegate::drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QPixmap &pixmap) const {
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
    painter->drawRoundedRect(rect, 1, 1);
  }
  QRect offset_rect = rect.translated(0, 2);
  QItemDelegate::drawDecoration(painter, option, offset_rect, pixmap);
}
