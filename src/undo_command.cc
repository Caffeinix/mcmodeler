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

#include "undo_command.h"

#include "diagram.h"

UndoCommand::UndoCommand(const BlockTransaction& transaction, Diagram* diagram, QUndoCommand* parent)
    : QUndoCommand(parent),
      transaction_(transaction),
      diagram_(diagram) {
  Q_ASSERT(diagram);
}

UndoCommand::~UndoCommand() {}

void UndoCommand::undo() {
  qDebug() << "UndoCommand::undo()";
  diagram_->commit(transaction_.reversed());
}

void UndoCommand::redo() {
  qDebug() << "UndoCommand::redo()";
  diagram_->commit(transaction_);
}
