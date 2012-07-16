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

#include "block_properties.h"

#include <QDebug>
#include <QMetaProperty>

#include "enumeration.h"
#include "block_geometry.h"
#include "block_property_keys.h"

BlockProperties::BlockProperties()
    : name_(QString()),
      categories_(QStringList()),
      geometry_(BlockGeometry::kGeometryCube),
      valid_orientations_(QVector<const BlockOrientation*>()),
      tile_offsets_(QVector<QPoint>()),
      sprite_offset_(QPoint(-1, -1)),
      is_transparent_(false),
      is_biome_grass_(false),
      is_biome_tree_(false),
      is_valid_(false) {}

BlockProperties::BlockProperties(const QVariantMap& block_data) {
  foreach (QString key, block_data.keys()) {
    QVariant value = block_data.value(key);
    if (key == kBlockPropertyKeyName) {
      name_ = value.toString();
    } else if (key == kBlockPropertyKeyCategories) {
      categories_ = value.toStringList();
    } else if (key == kBlockPropertyKeyTransparent) {
      is_transparent_ = value.toBool();
    } else if (key == kBlockPropertyKeyBiomeGrass) {
      is_biome_grass_ = value.toBool();
    } else if (key == kBlockPropertyKeyBiomeTree) {
      is_biome_tree_ = value.toBool();
    } else if (key == kBlockPropertyKeyGeometry) {
      // Convert from string to BlockGeometry enum.
      geometry_ = Enumeration<BlockGeometry, BlockGeometry::Geometry>::fromString(value.toString());
    } else if (key == kBlockPropertyKeyOrientations) {
      // Convert from QVariantList of strings to QVector of BlockOrientations.
      QVector<const BlockOrientation*> orientations;
      QVariantList orientation_names = value.toList();
      foreach (QVariant orientation, orientation_names) {
        orientations << BlockOrientation::get(orientation.toString().toUtf8().constData());
      }
      valid_orientations_ = orientations;
    } else if (key == kBlockPropertyKeyTextures) {
      // Convert from QVariantList of QVariantLists representing points to a QVector of QPoints.
      QVector<QPoint> offsets;
      QVariantList offset_lists = value.toList();
      foreach (QVariant offset, offset_lists) {
        QVariantList offset_list = offset.toList();
        offsets << QPoint(offset_list.at(0).toInt(), offset_list.at(1).toInt());
      }
      tile_offsets_ = offsets;
    } else if (key == kBlockPropertyKeySpriteIndex) {
      // Convert from QVariantList representing a point to a QPoint.
      QPoint offset;
      QVariantList offset_list = value.toList();
      offset = QPoint(offset_list.at(0).toInt(), offset_list.at(1).toInt());
      sprite_offset_ = offset;
    }
  }
  is_valid_ = true;
}

QString BlockProperties::name() const {
  return name_;
}

QStringList BlockProperties::categories() const {
  return categories_;
}

BlockGeometry::Geometry BlockProperties::geometry() const {
  return geometry_;
}

QVector<const BlockOrientation*> BlockProperties::validOrientations() const {
  return valid_orientations_;
}

QVector<QPoint> BlockProperties::tileOffsets() const {
  return tile_offsets_;
}

QPoint BlockProperties::spriteOffset() const {
  return sprite_offset_;
}

bool BlockProperties::isTransparent() const {
  return is_transparent_;
}

bool BlockProperties::isBiomeGrass() const {
  return is_biome_grass_;
}

bool BlockProperties::isBiomeTree() const {
  return is_biome_tree_;
}

bool BlockProperties::isValid() const {
  return is_valid_;
}
