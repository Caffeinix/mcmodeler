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

#ifndef BILL_OF_MATERIALS_WINDOW_H
#define BILL_OF_MATERIALS_WINDOW_H

#include "ui_bill_of_materials_window.h"

class Diagram;

class BillOfMaterialsWindow : public QWidget {
  Q_OBJECT

 public:
  explicit BillOfMaterialsWindow(Diagram* diagram, QWidget* parent = NULL);
  virtual ~BillOfMaterialsWindow();

 private slots:
  void updateBillOfMaterials();

 protected:
  virtual void showEvent(QShowEvent* evt);

 private:
  Diagram* diagram_;
  Ui::BillOfMaterialsWindow ui;
};

#endif // BILL_OF_MATERIALS_WINDOW_H
