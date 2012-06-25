#ifndef BLOCK_PICKER_H
#define BLOCK_PICKER_H

#include <QWidget>

#include "ui_block_picker.h"
#include "block_type.h"

class BlockPickerItemDelegate;
class BlockPrototype;
class QListWidget;
class QListWidgetItem;

class BlockPicker : public QWidget {
  Q_OBJECT

 public:
  explicit BlockPicker(QWidget* parent = NULL);
  ~BlockPicker();

  void addBlock(BlockPrototype* block);

 signals:
  void blockSelected(blocktype_t type);

 private slots:
  void selectBlockForItem(QListWidgetItem* item);

 private:
  static QIcon iconForSprite(const QPixmap& sprite);

  QWidget* findTab(const QString& text);
  QListWidget* createTab();

  BlockPickerItemDelegate* item_delegate_;

  Ui::BlockPicker ui;
};

#endif // BLOCK_PICKER_H
