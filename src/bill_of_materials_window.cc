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

#include "bill_of_materials_window.h"

#include <QList>
#include <QMap>
#include <QString>

#include "block_prototype.h"
#include "block_type.h"
#include "diagram.h"

BillOfMaterialsWindow::BillOfMaterialsWindow(Diagram* diagram, QWidget* parent)
    : QWidget(parent), diagram_(diagram) {
  ui.setupUi(this);
  ui.bill_of_materials_text_->setAttribute(Qt::WA_MacSmallSize);
  this->connect(diagram, SIGNAL(diagramChanged(BlockTransaction)), SLOT(updateBillOfMaterials()));
}

BillOfMaterialsWindow::~BillOfMaterialsWindow() {}

void BillOfMaterialsWindow::showEvent(QShowEvent* evt) {
  updateBillOfMaterials();
}

void BillOfMaterialsWindow::updateBillOfMaterials() {
  if (!isVisible()) {
    return;
  }
  QMap<BlockType, int> block_counts = diagram_->blockCounts();
  QList<BlockType> sorted_types = block_counts.keys();
  qSort(sorted_types);
  ui.bill_of_materials_text_->clear();
  foreach (BlockType type, sorted_types) {
    QString line = QString("%1: %2 (%3 stacks)")
                   .arg(BlockPrototype::nameOfType(type))
                   .arg(block_counts.value(type))
                   .arg(block_counts.value(type) / 64 + 1);
    ui.bill_of_materials_text_->append(line);
  }
}
