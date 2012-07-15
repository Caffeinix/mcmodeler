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

#include "enumeration.h"

#include <QMetaEnum>

template <typename Class, typename Enum>
Enumeration<Class, Enum>::Enumeration() {}

template <typename Class, typename Enum>
Enum Enumeration<Class, Enum>::fromString(const QString& str) {
  QMetaObject obj = Class::staticMetaObject;
  if (obj.enumeratorCount() < 1) {
    qWarning() << __PRETTY_FUNCTION__
               << QString("Class %1 defines no enum types; returning 0.")
                  .arg(obj.className());
    return static_cast<Enum>(0);
  } else if (obj.enumeratorCount() > 1) {
    qWarning() << __PRETTY_FUNCTION__
               << QString("Class %1 defines multiple enum types; using \"%2\".")
                  .arg(obj.className(), obj.enumerator(0).name());
  }
  QMetaEnum e = obj.enumerator(0);
  for (int i = 0; i < e.keyCount(); ++i) {
    if (e.key(i) == str) {
      return static_cast<Enum>(e.value(i));
    }
  }

  qWarning() << __PRETTY_FUNCTION__
             << QString("Enum \"%1\" in %2 has no value \"%3\"; returning %4 by default.")
                .arg(e.name(), obj.className(), str, e.key(0));
  return static_cast<Enum>(e.value(0));
}

template <typename Class, typename Enum>
QString Enumeration<Class, Enum>::toString(Enum value) {
  QMetaObject obj = Class::staticMetaObject;
  if (obj.enumeratorCount() < 1) {
    qWarning() << __PRETTY_FUNCTION__
               << QString("Class %1 defines no enum types; returning empty string.")
                  .arg(obj.className());
    return QString();
  } else if (obj.enumeratorCount() > 1) {
    qWarning() << __PRETTY_FUNCTION__
               << QString("Class %1 defines multiple enum types; using \"%2\".")
                  .arg(obj.className(), obj.enumerator(0).name());
  }
  QMetaEnum e = obj.enumerator(0);
  for (int i = 0; i < e.keyCount(); ++i) {
    if (e.value(i) == value) {
      return QString::fromAscii(e.key(i));
    }
  }

  qWarning() << __PRETTY_FUNCTION__
             << QString("Enum \"%1\" in %2 has no value %3; returning empty string.")
                .arg(e.name(), obj.className()).arg(value);
  return QString();
}
