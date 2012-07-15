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

#ifndef ENUMERATION_H
#define ENUMERATION_H

#include <QString>

/**
  * A static utility class that can map between strings and enum constants.  Sounds like magic, right?  Well, it is,
  * but there's a requirement: the enum in question must be defined inside a class which subclasses QObject, and it must
  * be declared using the Q_ENUMS macro.  The class containing the enum need not be instantiable, and in fact it
  * probably shouldn't be; the point is that we need to have run the meta-object compiler on it.
  * See the BlockGeometry class for an implementation example.
  *
  * To use this class (which cannot be instantiated, since its only methods are static), simply pass it template
  * parameters for the QObject subclass and enum type you wish to map to or from, then call either fromString or
  * toString depending on what you want to get.
  *
  * @warning If your QObject subclass declares more than one enum using Q_ENUMS, only the first declared enum will be
  * searched when mapping.  If more than one is found, a warning message will be printed.
  */
template <typename Class, typename Enum>
class Enumeration {
 public:

  /**
    * Returns the value of \p Enum whose constant name is exactly equal to \p str.  If \p Enum contains no constant
    * with that name, the first value in the enum is returned instead and a warning is printed.
    */
  static Enum fromString(const QString& str);

  /**
    * Returns a string containing the constant name of the value of \p Enum equal to \p value.  If \p value is not
    * a member of \p Enum (for example if you coerce an arbitrary \c int to the enum type), an empty string is returned
    * instead and a warning is printed.
    */
  static QString toString(Enum value);

 private:
  /**
    * Constructor.  Private because Enumeration is not instantiable.
    */
  Enumeration();
};

#include "enumeration_impl.h"

#endif // ENUMERATION_H
