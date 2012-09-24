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
 *
 * Portions Copyright Alois Zingl, Vienna, Austria
 */

#include "circle_tool.h"

#include "block_instance.h"
#include "block_oracle.h"
#include "block_prototype.h"
#include "block_transaction.h"

CircleTool::CircleTool(BlockOracle* oracle) : oracle_(oracle) {}

QString CircleTool::actionName() const {
  return "Draw Circle";
}

bool CircleTool::wantsMorePositions() {
  return countPositions() < 2;
}

bool CircleTool::isBrush() const {
  return false;
}

void CircleTool::setPixel(int x, int y, int z, BlockPrototype* prototype, const BlockOrientation* orientation, BlockTransaction* transaction) {
  BlockPosition pos(x, y, z);
  BlockInstance new_block(prototype, pos, orientation);
  transaction->replaceBlock(oracle_->blockAt(pos), new_block);
}

void CircleTool::draw(BlockPrototype* prototype, const BlockOrientation* orientation, BlockTransaction* transaction) {
  if (wantsMorePositions()) {
    return;
  }

  Q_ASSERT(positionAtIndex(0).y() == positionAtIndex(1).y());

  BlockPosition start(qMin(positionAtIndex(0).x(), positionAtIndex(1).x()),
                      positionAtIndex(0).y(),
                      qMin(positionAtIndex(0).z(), positionAtIndex(1).z()));
  BlockPosition end(qMax(positionAtIndex(0).x(), positionAtIndex(1).x()),
                    positionAtIndex(0).y(),
                    qMax(positionAtIndex(0).z(), positionAtIndex(1).z()));

  int x0 = start.x();
  int x1 = end.x();
  int y0 = start.z();
  int y1 = end.z();

  int a = abs(x1-x0), b = abs(y1-y0), b1 = b&1; /* values of diameter */
  long dx = 4*(1-a)*b*b, dy = 4*(b1+1)*a*a; /* error increment */
  long err = dx+dy+b1*a*a, e2; /* error of 1.step */

  if (x0 > x1) { x0 = x1; x1 += a; } /* if called with swapped points */
  if (y0 > y1) y0 = y1; /* .. exchange them */
  y0 += (b+1)/2; y1 = y0-b1;   /* starting pixel */
  a *= 8*a; b1 = 8*b*b;

  do {
    setPixel(x1, start.y(), y0, prototype, orientation, transaction); /*   I. Quadrant */
    setPixel(x0, start.y(), y0, prototype, orientation, transaction); /*  II. Quadrant */
    setPixel(x0, start.y(), y1, prototype, orientation, transaction); /* III. Quadrant */
    setPixel(x1, start.y(), y1, prototype, orientation, transaction); /*  IV. Quadrant */
    e2 = 2*err;
    if (e2 <= dy) { y0++; y1--; err += dy += a; }  /* y step */
    if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; } /* x step */
  } while (x0 <= x1);

  while (y0-y1 < b) {  /* too early stop of flat ellipses a=1 */
    setPixel(x0-1, start.y(), y0, prototype, orientation, transaction); /* -> finish tip of ellipse */
    setPixel(x1+1, start.y(), y0++, prototype, orientation, transaction);
    setPixel(x0-1, start.y(), y1, prototype, orientation, transaction);
    setPixel(x1+1, start.y(), y1--, prototype, orientation, transaction);
  }
}
