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

#ifndef BLOCK_ORIENTATION_H
#define BLOCK_ORIENTATION_H

#include <QtCore/QHash>

class QString;

/**
  * Represents a possible valid orientation of a block.
  *
  * BlockOrientations cannot be constructed directly.  Instead, they are initialized by means of the get() method,
  * which takes a uniquely identifying string, or the noOrientation() method which returns a default instance.  This
  * ensures that there will be exactly one BlockOrientation object per distinct string.
  *
  * BlockOrientations are similar to enum constants in that they are useful purely for comparison to other
  * BlockOrientations, but they are true objects instead of integers under the covers.  This allows a descriptive name
  * to be associated with each instance that can be retrieved by calling the name() method.
  */
class BlockOrientation {
 public:
  /**
    * Returns a BlockOrientation object representing no orientation.  This is the default orientation for blocks like
    * dirt, stone, and gravel that look the same from every direction, and for blocks like grass and snow that cannot
    * be rotated out of their default orientation.
    *
    * @note This function does \e not transfer ownership of the pointer to the caller, so you should \e not delete it!
    */
  static BlockOrientation* noOrientation();

  /**
    * Returns a BlockOrientation object for the given string.  This is guaranteed to return the same pointer every time
    * it is called with an equal string (that is, if you call get() with two strings for which strcmp() would return 0,
    * you will get back the same pointer for both strings).
    *
    * @note This function does \e not transfer ownership of the pointer to the caller, so you should \e not delete it!
    */
  static BlockOrientation* get(const char* name);

  /**
    * Returns whether this BlockOrientation is equal to \p other.
    */
  inline bool operator==(const BlockOrientation& other) const {
    return (this == &other);
  }

  /**
    * Returns the name of this orientation, which is the same as the string used to obtain it.
    */
  QString name() const {
    return name_;
  }

 private:
  BlockOrientation(const QString& name);
  static QHash<QString, BlockOrientation*> s_known_orientations_;
  QString name_;
  Q_DISABLE_COPY(BlockOrientation)
};

#endif // BLOCK_ORIENTATION_H
