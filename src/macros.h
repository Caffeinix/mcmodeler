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

#ifndef MACROS_H
#define MACROS_H

/**
  * Attempts to return the size of an array.  This only works if the argument \p lhs is actually an array, _not_ merely
  * a pointer.  (Arrays cannot be passed through method arguments, for example; if you try, it will demote to a pointer
  * instead.)
  */
template <typename T, size_t N>
inline int arraysize(const T (&lhs)[N]) {
  Q_UNUSED(lhs);
  return N;
}

#endif // MACROS_H
