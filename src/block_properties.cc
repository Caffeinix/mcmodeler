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

BlockProperties::BlockProperties()
    : name_(QString()),
      categories_(QStringList()),
      geometry_(kBlockGeometryCube),
      valid_orientations_(QVector<BlockOrientation*>()),
      tile_offsets_(QVector<QPoint>()),
      sprite_offset_(QPoint(-1, -1)),
      is_transparent_(false),
      is_biome_grass_(false),
      is_biome_tree_(false),
      is_valid_(false) {}

BlockProperties::BlockProperties(const QVariantMap& block_data) {
  // TODO(phoenix): It might be cleaner to create a BlockPropertiesJson class which has read/write Q_PROPERTIES for
  // all of these, then use QJson's QObject helper to construct one for each block.  Then it should be possible to
  // read out the properties directly without having to do string matches (which are prone to typos).
  foreach (QString key, block_data.keys()) {
    QVariant value = block_data.value(key);
    if (key == "name") {
      name_ = value.toString();
    } else if (key == "categories") {
      categories_ = value.toStringList();
    } else if (key == "isTransparent") {
      is_transparent_ = value.toBool();
    } else if (key == "isBiomeGrass") {
      is_biome_grass_ = value.toBool();
    } else if (key == "isBiomeTree") {
      is_biome_tree_ = value.toBool();
    } else if (key == "geometry") {
      // Convert from string to BlockGeometry enum.
      // TODO(phoenix): Implement this for real.
      if (value.toString() == "bed") {
        geometry_ = kBlockGeometryBed;
      } else if (value.toString() == "slab") {
        geometry_ = kBlockGeometrySlab;
      } else if (value.toString() == "cactus") {
        geometry_ = kBlockGeometryCactus;
      } else if (value.toString() == "chest") {
        geometry_ = kBlockGeometryChest;
      } else if (value.toString() == "ladder") {
        geometry_ = kBlockGeometryLadder;
      } else if (value.toString() == "pressure_plate") {
        geometry_ = kBlockGeometryPressurePlate;
      } else if (value.toString() == "door") {
        geometry_ = kBlockGeometryDoor;
      } else if (value.toString() == "stairs") {
        geometry_ = kBlockGeometryStairs;
      } else {
        geometry_ = kBlockGeometryCube;
      }
    } else if (key == "validOrientations") {
      // Convert from QVariantList of strings to QVector of BlockOrientations.
      QVector<BlockOrientation*> orientations;
      QVariantList orientation_names = value.toList();
      foreach (QVariant orientation, orientation_names) {
        orientations << BlockOrientation::get(orientation.toString().toUtf8().constData());
      }
      valid_orientations_ = orientations;
    } else if (key == "tileOffsets") {
      // Convert from QVariantList of QVariantLists representing points to a QVector of QPoints.
      QVector<QPoint> offsets;
      QVariantList offset_lists = value.toList();
      foreach (QVariant offset, offset_lists) {
        QVariantList offset_list = offset.toList();
        offsets << QPoint(offset_list.at(0).toInt(), offset_list.at(1).toInt());
      }
      tile_offsets_ = offsets;
    } else if (key == "spriteOffset") {
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

BlockGeometry BlockProperties::geometry() const {
  return geometry_;
}

QVector<BlockOrientation*> BlockProperties::validOrientations() const {
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
