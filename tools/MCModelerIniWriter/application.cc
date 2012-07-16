#include "application.h"

#include <QDebug>
#include <QFile>
#include <QPoint>
#include <QStringList>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>

#define QJSON_VERBOSE_DEBUG_OUTPUT
#include <QJson/Serializer>

#include "../../src/block_geometry.h"
#include "../../src/enumeration.h"

const char kNameKey[] = "name";
const char kIdKey[] = "id";
const char kCategoriesKey[] = "categories";
const char kGeometryKey[] = "geometry";
const char kSpriteIndexKey[] = "spriteOffset";
const char kOrientationsKey[] = "validOrientations";
const char kTexturesKey[] = "tileOffsets";
const char kTransparentKey[] = "isTransparent";
const char kBiomeGrassKey[] = "isBiomeGrass";
const char kBiomeTreeKey[] = "isBiomeTree";

const char kCategoryBasic[] = "basic";
const char kCategoryConstruction[] = "construction";
const char kCategoryWool[] = "wool";
const char kCategoryTools[] = "tools";
const char kCategoryRedstone[] = "redstone";
const char kCategoryMining[] = "mining";
const char kCategoryVegetation[] = "vegetation";
const char kCategoryNether[] = "nether";

const char kOrientationNone[] = "";
const char kOrientationFacingSouth[] = "Facing south";
const char kOrientationFacingWest[] = "Facing west";
const char kOrientationFacingNorth[] = "Facing north";
const char kOrientationFacingEast[] = "Facing east";
const char kOrientationRunningNorthSouth[] = "Running north/south";
const char kOrientationRunningEastWest[] = "Running east/west";
const char kOrientationNorthwestCorner[] = "Northwest corner";
const char kOrientationSouthwestCorner[] = "Southwest corner";
const char kOrientationNortheastCorner[] = "Northeast corner";
const char kOrientationSoutheastCorner[] = "Southeast corner";
const char kOrientationAscendingSouth[] = "Ascending south";
const char kOrientationAscendingWest[] = "Ascending west";
const char kOrientationAscendingNorth[] = "Ascending north";
const char kOrientationAscendingEast[] = "Ascending east";
const char kOrientationNorthHalf[] = "North half";
const char kOrientationSouthHalf[] = "South half";
const char kOrientationEastHalf[] = "East half";
const char kOrientationWestHalf[] = "West half";
const char kOrientationTFacingSouth[] = "T facing south";
const char kOrientationTFacingWest[] = "T facing west";
const char kOrientationTFacingNorth[] = "T facing north";
const char kOrientationTFacingEast[] = "T facing east";
const char kOrientationCross[] = "Cross";
const char kOrientationOnFloor[] = "On floor";
const char kOrientationOnNorthWall[] = "On north wall";
const char kOrientationOnEastWall[] = "On east wall";
const char kOrientationOnSouthWall[] = "On south wall";
const char kOrientationOnWestWall[] = "On west wall";
const char kOrientationOneBlockFromSource[] = "One block from source";
const char kOrientationTwoBlocksFromSource[] = "Two blocks from source";
const char kOrientationThreeBlocksFromSource[] = "Three blocks from source";
const char kOrientationFourBlocksFromSource[] = "Four blocks from source";
const char kOrientationFiveBlocksFromSource[] = "Five blocks from source";
const char kOrientationSixBlocksFromSource[] = "Six blocks from source";
const char kOrientationSevenBlocksFromSource[] = "Seven blocks from source";


// Anatomy of a 32-bit MCModeler block ID:
// XXXXXXXX|UUUU|BBBB|RRRRRRRR|MMMMMMMM
// X = Unused
// U = User data
// B = Minecraft block data
// R = Reserved for when Minecraft inevitably starts using 16-bit block IDs.
// M = Minecraft block ID
//
// So:
// 0x    11 = oak wood
// 0x 10011 = spruce wood
// 0x    02 = grass
// 0x100002 = grass with snow side and top textures
//
// Get it?

// The four bits of block data are stored above the 16-bit block ID.
const unsigned int kBlockDataShift = 16;

// Four bits of user data (data used by MCModeler, but that does not appear in Minecraft)
// is stored above that.
const unsigned int kUserDataShift = 20;

QVariantList toArray(const QPoint& point) {
  QVariantList list;
  list << point.x() << point.y();
  return list;
}

// Texture face ordering for most cubic geometries:
// FRONT, BACK, BOTTOM, RIGHT, TOP, LEFT
QVariantList textures(const QList<QPoint>& indices) {
  QVariantList textures;
  foreach (const QPoint& point, indices) {
    textures << QVariant(toArray(point));
  }
  return textures;
}

QVariantList textures(const QPoint& index) {
  QVariantList textures;
  QVariant point(toArray(index));
  for (int i = 0; i < 6; ++i) {
    textures << point;
  }
  return textures;
}

QMap<quint32, QString> s_used_block_ids_;

QVariantMap block(const QString& name,
                  const quint32 id,
                  const QStringList& types,
                  BlockGeometry::Geometry geometry,
                  const QPoint& sprite_index,
                  const QVariantList& orientations,
                  const QVariantList& textures,
                  bool transparent,
                  bool biome_grass = false,
                  bool biome_tree = false) {
  // Sanity checking for IDs:
  if (s_used_block_ids_.contains(id)) {
    qFatal("Error: block ID 0x%x is already used by block %s and cannot be assigned to block %s.",
           id, qPrintable(s_used_block_ids_.value(id)), qPrintable(name));
  }

  QVariantMap block;
  block.insert(kNameKey, name);
  block.insert(kIdKey, id);
  block.insert(kCategoriesKey, types);
  block.insert(kGeometryKey, Enumeration<BlockGeometry, BlockGeometry::Geometry>::toString(geometry));
  block.insert(kSpriteIndexKey, toArray(sprite_index));
  block.insert(kOrientationsKey, orientations);
  block.insert(kTexturesKey, textures);
  block.insert(kTransparentKey, transparent);
  block.insert(kBiomeGrassKey, biome_grass);
  block.insert(kBiomeTreeKey, biome_tree);
  return block;
}

Application::Application(int argc, char* argv[]) :
    QCoreApplication(argc, argv) {
  QVariantList blocks;

  blocks << block("Oak Wood Planks",
                  0x05,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(4, 0),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(4, 0)),
                  false);
  blocks << block("Spruce Wood Planks",
                  0x05 + (0x1 << kBlockDataShift),
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(6, 12),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(6, 12)),
                  false);
  blocks << block("Birch Wood Planks",
                  0x05 + (0x2 << kBlockDataShift),
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(6, 13),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(6, 13)),
                  false);
  blocks << block("Jungle Wood Planks",
                  0x05 + (0x3 << kBlockDataShift),
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(7, 12),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(7, 12)),
                  false);

  blocks << block("Oak Wood",
                  0x11,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(4, 1),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(4, 1) << QPoint(4, 1) << QPoint(5, 1) << QPoint(4, 1) << QPoint(5, 1) << QPoint(4, 1)),
                  false);
  blocks << block("Birch Wood",
                  0x11 + (0x2 << kBlockDataShift),
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(5, 7),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(5, 7) << QPoint(5, 7) << QPoint(5, 1) << QPoint(5, 7) << QPoint(5, 1) << QPoint(5, 7)),
                  false);
  blocks << block("Spruce Wood",
                  0x11 + (0x1 << kBlockDataShift),
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(4, 7),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(4, 7) << QPoint(4, 7) << QPoint(5, 1) << QPoint(4, 7) << QPoint(5, 1) << QPoint(4, 7)),
                  false);
  blocks << block("Jungle Wood",
                  0x11 + (0x3 << kBlockDataShift),
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(9, 9),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(9, 9) << QPoint(9, 9) << QPoint(5, 1) << QPoint(9, 9) << QPoint(5, 1) << QPoint(9, 9)),
                  false);

  blocks << block("Oak Leaves",
                  0x12,
                  QStringList() << kCategoryBasic << kCategoryVegetation,
                  BlockGeometry::kGeometryLeaves,
                  QPoint(4, 3),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(4, 3)),
                  true, false, true);
  blocks << block("Birch Leaves",
                  0x12 + (0x2 << kBlockDataShift),
                  QStringList() << kCategoryBasic << kCategoryVegetation,
                  BlockGeometry::kGeometryLeaves,
                  QPoint(5, 3),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(5, 3)),
                  true, false, true);
  blocks << block("Spruce Leaves",
                  0x12 + (0x1 << kBlockDataShift),
                  QStringList() << kCategoryBasic << kCategoryVegetation,
                  BlockGeometry::kGeometryLeaves,
                  QPoint(5, 8),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(5, 8)),
                  true, false, true);

  blocks << block("Grass",
                  0x02,
                  QStringList() << kCategoryBasic << kCategoryVegetation,
                  BlockGeometry::kGeometryCube,
                  QPoint(0, 0),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(3, 0) << QPoint(3, 0) << QPoint(2, 0) << QPoint(3, 0) << QPoint(0, 0) << QPoint(3, 0)),
                  false, true);
  blocks << block("Dirt",
                  0x03,
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometryCube,
                  QPoint(2, 0),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 0)),
                  false);
  blocks << block("Stone",
                  0x01,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(1, 0),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 0)),
                  false);
  blocks << block("Glass",
                  0x66,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(1, 3),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 3)),
                  true);
  blocks << block("Glass Pane",
                  0x14,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryPane,
                  QPoint(1, 3),
                  QVariantList() << kOrientationRunningNorthSouth << kOrientationRunningEastWest
                                 << kOrientationNorthHalf << kOrientationSouthHalf
                                 << kOrientationEastHalf << kOrientationWestHalf
                                 << kOrientationNorthwestCorner << kOrientationSouthwestCorner
                                 << kOrientationNortheastCorner << kOrientationSoutheastCorner
                                 << kOrientationTFacingSouth << kOrientationTFacingWest
                                 << kOrientationTFacingNorth << kOrientationTFacingEast
                                 << kOrientationCross,
                  textures(QPoint(1, 3)),
                  true);
  blocks << block("Iron Bars",
                  0x65,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryPane,
                  QPoint(5, 5),
                  QVariantList() << kOrientationRunningNorthSouth << kOrientationRunningEastWest
                                 << kOrientationNorthHalf << kOrientationSouthHalf
                                 << kOrientationEastHalf << kOrientationWestHalf
                                 << kOrientationNorthwestCorner << kOrientationSouthwestCorner
                                 << kOrientationNortheastCorner << kOrientationSoutheastCorner
                                 << kOrientationTFacingSouth << kOrientationTFacingWest
                                 << kOrientationTFacingNorth << kOrientationTFacingEast
                                 << kOrientationCross,
                  textures(QPoint(5, 5)),
                  true);
  blocks << block("Cobblestone",
                  0x04,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(0, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(0, 1)),
                  false);
  blocks << block("Mossy Cobblestone",
                  0x30,
                  QStringList() << kCategoryConstruction << kCategoryMining,
                  BlockGeometry::kGeometryCube,
                  QPoint(4, 2),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(4, 2)),
                  false);

  blocks << block("Stone Brick",
                  0x04,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(6, 3),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(6, 3)),
                  false);
  blocks << block("Mossy Stone Brick",
                  0x04 + (0x1 << kBlockDataShift),
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(4, 6),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(4, 6)),
                  false);
  blocks << block("Cracked Stone Brick",
                  0x04 + (0x2 << kBlockDataShift),
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(5, 6),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(5, 6)),
                  false);
  blocks << block("Chiseled Stone Brick",
                  0x04 + (0x3 << kBlockDataShift),
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(5, 13),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(5, 13)),
                  false);

  blocks << block("Sand",
                  0x0C,
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometryCube,
                  QPoint(2, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 1)),
                  false);
  blocks << block("Crafting Table",
                  0x3A,
                  QStringList() << kCategoryBasic << kCategoryTools,
                  BlockGeometry::kGeometryCube,
                  QPoint(11, 2),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(11, 3) << QPoint(11, 3) << QPoint(5, 0) << QPoint(12, 3) << QPoint(11, 2) << QPoint(12, 3)),
                  false);
  blocks << block("Furnace",
                  0x3D,
                  QStringList() << kCategoryBasic << kCategoryTools,
                  BlockGeometry::kGeometryCube,
                  QPoint(12, 2),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(12, 2) << QPoint(13, 2) << QPoint(6, 0) << QPoint(13, 2) << QPoint(14, 3) << QPoint(13, 2)),
                  false);
  blocks << block("Chest",
                  0x36,
                  QStringList() << kCategoryBasic << kCategoryTools,
                  BlockGeometry::kGeometryChest,
                  QPoint(11, 1),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(11, 1) << QPoint(10, 1) << QPoint(9, 1) << QPoint(10, 1) << QPoint(9, 1) << QPoint(10, 1)),
                  false);
  blocks << block("Water (Source)",
                  0x08,
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometryCube,
                  QPoint(13, 12),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(13, 12)),
                  true);
  blocks << block("Water (Flow)",
                  0x08 + (0x0F << kBlockDataShift),
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometryFlow,
                  QPoint(13, 12),
                  QVariantList() << kOrientationOneBlockFromSource << kOrientationTwoBlocksFromSource
                                 << kOrientationThreeBlocksFromSource << kOrientationFourBlocksFromSource
                                 << kOrientationFiveBlocksFromSource << kOrientationSixBlocksFromSource
                                 << kOrientationSevenBlocksFromSource,
                  textures(QPoint(13, 12)),
                  true);
  blocks << block("Lava (Source)",
                  0x0A,
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometryCube,
                  QPoint(13, 14),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(13, 14)),
                  true);
  blocks << block("Lava (Flow)",
                  0x0A + (0x0E << kBlockDataShift),
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometryFlow,
                  QPoint(13, 14),
                  QVariantList() << kOrientationOneBlockFromSource << kOrientationTwoBlocksFromSource
                                 << kOrientationThreeBlocksFromSource,
                  textures(QPoint(13, 14)),
                  true);
  blocks << block("Wool",
                  0x23,
                  QStringList() << kCategoryBasic << kCategoryWool,
                  BlockGeometry::kGeometryCube,
                  QPoint(0, 4),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(0, 4)),
                  false);
  blocks << block("Light Gray Wool",
                  0x23 + (0x8 << kBlockDataShift),
                  QStringList() << kCategoryWool,
                  BlockGeometry::kGeometryCube,
                  QPoint(1, 14),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 14)),
                  false);
  blocks << block("Gray Wool",
                  0x23 + (0x7 << kBlockDataShift),
                  QStringList() << kCategoryWool,
                  BlockGeometry::kGeometryCube,
                  QPoint(2, 7),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 7)),
                  false);
  blocks << block("Black Wool",
                  0x23 + (0xF << kBlockDataShift),
                  QStringList() << kCategoryWool,
                  BlockGeometry::kGeometryCube,
                  QPoint(1, 7),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 7)),
                  false);
  blocks << block("Red Wool",
                  0x23 + (0xE << kBlockDataShift),
                  QStringList() << kCategoryWool,
                  BlockGeometry::kGeometryCube,
                  QPoint(1, 8),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 8)),
                  false);
  blocks << block("Pink Wool",
                  0x23 + (0x6 << kBlockDataShift),
                  QStringList() << kCategoryWool,
                  BlockGeometry::kGeometryCube,
                  QPoint(2, 8),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 8)),
                  false);
  blocks << block("Green Wool",
                  0x23 + (0xD << kBlockDataShift),
                  QStringList() << kCategoryWool,
                  BlockGeometry::kGeometryCube,
                  QPoint(1, 9),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 9)),
                  false);
  blocks << block("Lime Wool",
                  0x23 + (0x5 << kBlockDataShift),
                  QStringList() << kCategoryWool,
                  BlockGeometry::kGeometryCube,
                  QPoint(2, 9),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 9)),
                  false);
  blocks << block("Brown Wool",
                  0x23 + (0xC << kBlockDataShift),
                  QStringList() << kCategoryWool,
                  BlockGeometry::kGeometryCube,
                  QPoint(1, 10),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 10)),
                  false);
  blocks << block("Yellow Wool",
                  0x23 + (0x4 << kBlockDataShift),
                  QStringList() << kCategoryWool,
                  BlockGeometry::kGeometryCube,
                  QPoint(2, 10),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 10)),
                  false);
  blocks << block("Blue Wool",
                  0x23 + (0xB << kBlockDataShift),
                  QStringList() << kCategoryWool,
                  BlockGeometry::kGeometryCube,
                  QPoint(1, 11),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 11)),
                  false);
  blocks << block("Light Blue Wool",
                  0x23 + (0x3 << kBlockDataShift),
                  QStringList() << kCategoryWool,
                  BlockGeometry::kGeometryCube,
                  QPoint(2, 11),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 11)),
                  false);
  blocks << block("Purple Wool",
                  0x23 + (0xA << kBlockDataShift),
                  QStringList() << kCategoryWool,
                  BlockGeometry::kGeometryCube,
                  QPoint(1, 12),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 12)),
                  false);
  blocks << block("Magenta Wool",
                  0x23 + (0x2 << kBlockDataShift),
                  QStringList() << kCategoryWool,
                  BlockGeometry::kGeometryCube,
                  QPoint(2, 12),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 12)),
                  false);
  blocks << block("Cyan Wool",
                  0x23 + (0x9 << kBlockDataShift),
                  QStringList() << kCategoryWool,
                  BlockGeometry::kGeometryCube,
                  QPoint(1, 13),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 13)),
                  false);
  blocks << block("Orange Wool",
                  0x23 + (0x1 << kBlockDataShift),
                  QStringList() << kCategoryWool,
                  BlockGeometry::kGeometryCube,
                  QPoint(2, 13),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 13)),
                  false);

  blocks << block("Cobblestone Slab",
                  0x44 + (0x3 << kBlockDataShift),
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometrySlab,
                  QPoint(0, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(0, 1)),
                  true);
  blocks << block("Double Cobblestone Slab",
                  0x2B + (0x3 << kBlockDataShift),
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(0, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(0, 1)),
                  false);
  blocks << block("Stone Slab",
                  0x44,
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometrySlab,
                  QPoint(6, 0),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(5, 0) << QPoint(5, 0) << QPoint(6, 0) << QPoint(5, 0) << QPoint(6, 0) << QPoint(5, 0)),
                  true);
  blocks << block("Double Stone Slab",
                  0x2B,
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(6, 0),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(5, 0) << QPoint(5, 0) << QPoint(6, 0) << QPoint(5, 0) << QPoint(6, 0) << QPoint(5, 0)),
                  false);
  blocks << block("Oak Wood Slab",
                  0x7E,
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometrySlab,
                  QPoint(4, 0),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(4, 0)),
                  true);
  blocks << block("Double Oak Wood Slab",
                  0x7D,
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(4, 0),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(4, 0)),
                  false);
  blocks << block("Spruce Wood Slab",
                  0x7E + (0x1 << kBlockDataShift),
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometrySlab,
                  QPoint(6, 12),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(6, 12)),
                  true);
  blocks << block("Double Spruce Wood Slab",
                  0x7D + (0x1 << kBlockDataShift),
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(6, 12),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(6, 12)),
                  false);
  blocks << block("Birch Wood Slab",
                  0x7E + (0x2 << kBlockDataShift),
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometrySlab,
                  QPoint(6, 13),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(6, 13)),
                  true);
  blocks << block("Double Birch Wood Slab",
                  0x7D + (0x2 << kBlockDataShift),
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(6, 13),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(6, 13)),
                  false);
  blocks << block("Jungle Wood Slab",
                  0x7E + (0x3 << kBlockDataShift),
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometrySlab,
                  QPoint(7, 12),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(7, 12)),
                  true);
  blocks << block("Double Jungle Wood Slab",
                  0x7D + (0x3 << kBlockDataShift),
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(7, 12),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(7, 12)),
                  false);

  blocks << block("Brick",
                  0x2D,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(7, 0),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(7, 0)),
                  false);
  blocks << block("TNT",
                  0x2E,
                  QStringList() << kCategoryRedstone,
                  BlockGeometry::kGeometryCube,
                  QPoint(8, 0),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(8, 0) << QPoint(8, 0) << QPoint(10, 0) << QPoint(8, 0) << QPoint(9, 0) << QPoint(8, 0)),
                  false);
  blocks << block("Bedrock",
                  0x07,
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometryCube,
                  QPoint(1, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 1)),
                  false);
  blocks << block("Gravel",
                  0x0D,
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometryCube,
                  QPoint(3, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(3, 1)),
                  false);
  blocks << block("Iron Block",
                  0x2A,
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(6, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(6, 1)),
                  false);
  blocks << block("Gold Block",
                  0x29,
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(7, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(7, 1)),
                  false);
  blocks << block("Diamond Block",
                  0x39,
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(8, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(8, 1)),
                  false);
  blocks << block("Lapis Lazuli Block",
                  0x16,
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(0, 9),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(0, 9)),
                  false);
  blocks << block("Emerald Block",
                  0x85,
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(9, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(9, 1)),
                  false);
  blocks << block("Double Chest (Left Half)",
                  0x36 + (0x1 << kUserDataShift),
                  QStringList() << kCategoryBasic << kCategoryTools,
                  BlockGeometry::kGeometryCube,
                  QPoint(9, 2),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(9, 2) << QPoint(9, 3) << QPoint(9, 1) << QPoint(10, 1) << QPoint(9, 1) << QPoint(10, 1)),
                  false);
  blocks << block("Double Chest (Right Half)",
                  0x36 + (0x2 << kUserDataShift),
                  QStringList() << kCategoryBasic << kCategoryTools,
                  BlockGeometry::kGeometryCube,
                  QPoint(10, 2),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(10, 2) << QPoint(10, 3) << QPoint(9, 1) << QPoint(10, 1) << QPoint(9, 1) << QPoint(10, 1)),
                  false);
  blocks << block("Gold Ore",
                  0x0E,
                  QStringList() << kCategoryMining,
                  BlockGeometry::kGeometryCube,
                  QPoint(0, 2),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(0, 2)),
                  false);
  blocks << block("Iron Ore",
                  0x0F,
                  QStringList() << kCategoryMining,
                  BlockGeometry::kGeometryCube,
                  QPoint(1, 2),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 2)),
                  false);
  blocks << block("Coal Ore",
                  0x10,
                  QStringList() << kCategoryMining,
                  BlockGeometry::kGeometryCube,
                  QPoint(2, 2),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 2)),
                  false);
  blocks << block("Bookshelf",
                  0x2F,
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(3, 2),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(3, 2) << QPoint(4, 0) << QPoint(4, 0) << QPoint(4, 0) << QPoint(4, 0) << QPoint(4, 0)),
                  false);
  blocks << block("Obsidian",
                  0x31,
                  QStringList() << kCategoryConstruction << kCategoryMining,
                  BlockGeometry::kGeometryCube,
                  QPoint(5, 2),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(5, 2)),
                  false);
  blocks << block("Dispenser",
                  0x23,
                  QStringList() << kCategoryTools,
                  BlockGeometry::kGeometryCube,
                  QPoint(14, 2),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(14, 2) << QPoint(13, 2) << QPoint(6, 0) << QPoint(13, 2) << QPoint(14, 3) << QPoint(13, 2)),
                  false);
  blocks << block("Sponge",
                  0x13,
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(0, 3),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(0, 3)),
                  false);
  blocks << block("Diamond Ore",
                  0x38,
                  QStringList() << kCategoryMining,
                  BlockGeometry::kGeometryCube,
                  QPoint(2, 3),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 3)),
                  false);
  blocks << block("Redstone Ore",
                  0x49,
                  QStringList() << kCategoryMining << kCategoryRedstone,
                  BlockGeometry::kGeometryCube,
                  QPoint(3, 3),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(3, 3)),
                  false);
  blocks << block("Lapis Lazuli Ore",
                  0x15,
                  QStringList() << kCategoryMining,
                  BlockGeometry::kGeometryCube,
                  QPoint(0, 10),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(0, 10)),
                  false);
  blocks << block("Monster Spawner",
                  0x34,
                  QStringList() << kCategoryConstruction << kCategoryMining,
                  BlockGeometry::kGeometryCube,
                  QPoint(1, 4),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 4)),
                  true);
  blocks << block("Snow",
                  0x02 + (0x1 << kUserDataShift),
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometryCube,
                  QPoint(2, 4),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(4, 4) << QPoint(4, 4) << QPoint(2, 0) << QPoint(4, 4) << QPoint(2, 4) << QPoint(4, 4)),
                  false);
  blocks << block("Ice",
                  0x4F,
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometryCube,
                  QPoint(3, 4),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(3, 4)),
                  true);
  blocks << block("Cactus",
                  0x51,
                  QStringList() << kCategoryBasic << kCategoryVegetation,
                  BlockGeometry::kGeometryCactus,
                  QPoint(6, 4),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(6, 4) << QPoint(6, 4) << QPoint(7, 4) << QPoint(6, 4) << QPoint(5, 4) << QPoint(6, 4)),
                  true);
  blocks << block("Clay",
                  0x52,
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometryCube,
                  QPoint(8, 4),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(8, 4)),
                  false);
  blocks << block("Record Player",
                  0x54,
                  QStringList() << kCategoryTools,
                  BlockGeometry::kGeometryCube,
                  QPoint(11, 4),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(10, 4) << QPoint(10, 4) << QPoint(9, 1) << QPoint(10, 4) << QPoint(11, 4) << QPoint(10, 4)),
                  false);

  blocks << block("Field",
                  0x3C,
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometryCube,
                  QPoint(7, 5),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(2, 0) << QPoint(2, 0) << QPoint(2, 0) << QPoint(2, 0) << QPoint(7, 5) << QPoint(2, 0)),
                  false);
  blocks << block("Fertile Field",
                  0x3C + (0x1 << kBlockDataShift),
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometryCube,
                  QPoint(6, 5),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(2, 0) << QPoint(2, 0) << QPoint(2, 0) << QPoint(2, 0) << QPoint(6, 5) << QPoint(2, 0)),
                  false);

  blocks << block("Pumpkin",
                  0x56,
                  QStringList() << kCategoryVegetation,
                  BlockGeometry::kGeometryCube,
                  QPoint(7, 7),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(7, 7) << QPoint(6, 7) << QPoint(6, 7) << QPoint(6, 7) << QPoint(6, 6) << QPoint(6, 7)),
                  false);
  blocks << block("Jack o' Lantern",
                  0x5B,
                  QStringList() << kCategoryVegetation,
                  BlockGeometry::kGeometryCube,
                  QPoint(8, 7),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(8, 7) << QPoint(6, 7) << QPoint(6, 7) << QPoint(6, 7) << QPoint(6, 6) << QPoint(6, 7)),
                  false);

  blocks << block("Netherrack",
                  0x57,
                  QStringList() << kCategoryBasic << kCategoryNether,
                  BlockGeometry::kGeometryCube,
                  QPoint(7, 6),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(7, 6)),
                  false);
  blocks << block("Soul Sand",
                  0x58,
                  QStringList() << kCategoryBasic << kCategoryNether,
                  BlockGeometry::kGeometryCube,
                  QPoint(8, 6),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(8, 6)),
                  false);
  blocks << block("Glowstone",
                  0x59,
                  QStringList() << kCategoryBasic << kCategoryNether,
                  BlockGeometry::kGeometryCube,
                  QPoint(9, 6),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(9, 6)),
                  false);

  blocks << block("Glowstone Lamp (Off)",
                  0x7B,
                  QStringList() << kCategoryRedstone,
                  BlockGeometry::kGeometryCube,
                  QPoint(3, 13),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(3, 13)),
                  false);
  blocks << block("Glowstone Lamp (On)",
                  0x7C,
                  QStringList() << kCategoryRedstone,
                  BlockGeometry::kGeometryCube,
                  QPoint(4, 13),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(4, 13)),
                  false);

  blocks << block("Sandstone",
                  0x18,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(0, 12),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(0, 12) << QPoint(0, 12) << QPoint(0, 13) << QPoint(0, 12) << QPoint(0, 11) << QPoint(0, 12)),
                  false);
  blocks << block("Chiseled Sandstone",
                  0x18 + (0x1 << kBlockDataShift),
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(6, 14),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(6, 14) << QPoint(6, 14) << QPoint(0, 13) << QPoint(6, 14) << QPoint(0, 11) << QPoint(6, 14)),
                  false);
  blocks << block("Smooth Sandstone",
                  0x18 + (0x2 << kBlockDataShift),
                  QStringList() << kCategoryConstruction,
                  BlockGeometry::kGeometryCube,
                  QPoint(5, 14),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(5, 14) << QPoint(5, 14) << QPoint(0, 13) << QPoint(5, 14) << QPoint(0, 11) << QPoint(5, 14)),
                  false);

  blocks << block("Note Block",
                  0x19,
                  QStringList() << kCategoryRedstone,
                  BlockGeometry::kGeometryCube,
                  QPoint(10, 4),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(10, 4)),
                  false);

  blocks << block("Bed (Bottom Half)",
                  0x1A,
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometryBed,
                  QPoint(6, 9),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  // FRONT BACK BOTTOM RIGHT TOP LEFT
                  textures(QList<QPoint>() << QPoint(6, 9) << QPoint(6, 9) << QPoint(4, 0) << QPoint(5, 9) << QPoint(6, 8) << QPoint(5, 11)),
                  true);
  blocks << block("Bed (Top Half)",
                  0x1A + (0x1 << kUserDataShift),
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometryBed,
                  QPoint(7, 9),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(7, 9) << QPoint(7, 9) << QPoint(4, 0) << QPoint(5, 11) << QPoint(7, 8) << QPoint(8, 9)),
                  true);

  blocks << block("Wooden Pressure Plate",
                  0x48,
                  QStringList() << kCategoryRedstone,
                  BlockGeometry::kGeometryPressurePlate,
                  QPoint(4, 0),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(4, 0)),
                  true);

  blocks << block("Wooden Door (Bottom Half)",
                  0x40,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryDoor,
                  QPoint(1, 6),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QPoint(1, 6)),
                  true);
  blocks << block("Wooden Door (Top Half)",
                  0x40 + (0x8 << kBlockDataShift),
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryDoor,
                  QPoint(1, 5),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QPoint(1, 5)),
                  true);
  blocks << block("Iron Door (Bottom Half)",
                  0x47,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryDoor,
                  QPoint(2, 6),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QPoint(2, 6)),
                  true);
  blocks << block("Iron Door (Top Half)",
                  0x47 + (0x8 << kBlockDataShift),
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryDoor,
                  QPoint(2, 5),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QPoint(2, 5)),
                  true);

  blocks << block("Cobblestone Stairs",
                  0x43,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryStairs,
                  QPoint(0, 1),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QPoint(0, 1)),
                  true);
  blocks << block("Brick Stairs",
                  0x6C,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryStairs,
                  QPoint(7, 0),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QPoint(7, 0)),
                  true);
  blocks << block("Stone Brick Stairs",
                  0x6D,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryStairs,
                  QPoint(6, 3),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QPoint(6, 3)),
                  true);
  blocks << block("Nether Brick Stairs",
                  0x72,
                  QStringList() << kCategoryBasic << kCategoryConstruction << kCategoryNether,
                  BlockGeometry::kGeometryStairs,
                  QPoint(0, 15),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QPoint(0, 15)),
                  true);
  blocks << block("Sandstone Stairs",
                  0x72,
                  QStringList() << kCategoryBasic << kCategoryConstruction << kCategoryNether,
                  BlockGeometry::kGeometryStairs,
                  QPoint(0, 13),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QPoint(0, 13)),
                  true);
  blocks << block("Oak Wood Stairs",
                  0x35,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryStairs,
                  QPoint(4, 0),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QPoint(4, 0)),
                  true);
  blocks << block("Spruce Wood Stairs",
                  0x86,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryStairs,
                  QPoint(6, 12),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QPoint(6, 12)),
                  true);
  blocks << block("Birch Wood Stairs",
                  0x86,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryStairs,
                  QPoint(6, 13),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QPoint(6, 13)),
                  true);
  blocks << block("Jungle Wood Stairs",
                  0x86,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryStairs,
                  QPoint(7, 12),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QPoint(7, 12)),
                  true);

  blocks << block("Ladder",
                  0x41,
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  BlockGeometry::kGeometryLadder,
                  QPoint(3, 5),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(3, 5)),
                  true);
  blocks << block("Minecart Track",
                  0x42,
                  QStringList() << kCategoryConstruction << kCategoryRedstone,
                  BlockGeometry::kGeometryTrack,
                  QPoint(0, 8),
                  QVariantList() << kOrientationRunningNorthSouth << kOrientationRunningEastWest
                                 << kOrientationAscendingSouth << kOrientationAscendingWest
                                 << kOrientationAscendingNorth << kOrientationAscendingEast
                                 << kOrientationNorthwestCorner << kOrientationSouthwestCorner
                                 << kOrientationNortheastCorner << kOrientationSoutheastCorner,
                  textures(QList<QPoint>() << QPoint(0, 8) << QPoint(0, 7)),
                  true);
  blocks << block("Vines",
                  0x6A,
                  QStringList() << kCategoryBasic << kCategoryVegetation,
                  BlockGeometry::kGeometryLadder,
                  QPoint(15, 8),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(15, 8)),
                  true, false, true);
  blocks << block("Snow Cover",
                  0x4E,
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometrySnow,
                  QPoint(2, 4),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 4)),
                  true);
  blocks << block("Torch",
                  0x32,
                  QStringList() << kCategoryBasic,
                  BlockGeometry::kGeometryTorch,
                  QPoint(0, 5),
                  QVariantList() << kOrientationOnFloor << kOrientationOnSouthWall << kOrientationOnWestWall
                                 << kOrientationOnNorthWall << kOrientationOnEastWall,
                  textures(QList<QPoint>() << QPoint(0, 5) << QPoint(0, 5) << QPoint(0, 5) << QPoint(0, 5)),
                  true);

  QJson::Serializer serializer;
  QByteArray json = serializer.serialize(blocks);

  QFile f("blocks.json");
  f.open(QFile::ReadWrite | QFile::Truncate);
  f.write(json);
  f.close();

  printf("Wrote %d bytes to blocks.json\n", f.size());
  printf("All done.\n");

  QTimer::singleShot(2000, this, SLOT(quit()));
}
