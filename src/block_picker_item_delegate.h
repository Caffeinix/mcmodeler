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
