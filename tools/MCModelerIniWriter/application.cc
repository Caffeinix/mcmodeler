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

const char kNameKey[] = "name";
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

const char kGeometryBed[] = "bed";
const char kGeometryCube[] = "cube";
const char kGeometrySlab[] = "slab";
const char kGeometryCactus[] = "cactus";
const char kGeometryChest[] = "chest";
const char kGeometryDoor[] = "door";
const char kGeometryStairs[] = "stairs";
const char kGeometryPressurePlate[] = "pressure_plate";

const char kOrientationNone[] = "";
const char kOrientationFacingSouth[] = "Facing south";
const char kOrientationFacingWest[] = "Facing west";
const char kOrientationFacingNorth[] = "Facing north";
const char kOrientationFacingEast[] = "Facing east";

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

QVariantMap block(const QString& name,
                  const QStringList& types,
                  const char* geometry,
                  const QPoint& sprite_index,
                  const QVariantList& orientations,
                  const QVariantList& textures,
                  bool transparent,
                  bool biome_grass = false,
                  bool biome_tree = false) {
  QVariantMap block;
  block.insert(kNameKey, name);
  block.insert(kCategoriesKey, types);
  block.insert(kGeometryKey, geometry);
  block.insert(kSpriteIndexKey, toArray(sprite_index));
  block.insert(kOrientationsKey, orientations);
  block.insert(kTexturesKey, textures);
  block.insert(kTransparentKey, transparent);
  block.insert(kBiomeGrassKey, biome_grass);
  block.insert(kBiomeTreeKey, biome_tree);
  return block;
}

Application::Application(int argc, char *argv[]) :
    QCoreApplication(argc, argv) {
  QVariantList blocks;

  blocks << block("Wooden Plank",
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(4, 0),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(4, 0)),
                  false);

  blocks << block("Wood",
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(4, 1),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(4, 1) << QPoint(4, 1) << QPoint(5, 1) << QPoint(4, 1) << QPoint(5, 1) << QPoint(4, 1)),
                  false);
  blocks << block("Wood (Birch)",
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(5, 7),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(5, 7) << QPoint(5, 7) << QPoint(5, 1) << QPoint(5, 7) << QPoint(5, 1) << QPoint(5, 7)),
                  false);
  blocks << block("Wood (Pine)",
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(4, 7),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(4, 7) << QPoint(4, 7) << QPoint(5, 1) << QPoint(4, 7) << QPoint(5, 1) << QPoint(4, 7)),
                  false);
  blocks << block("Leaves",
                  QStringList() << kCategoryBasic << kCategoryVegetation,
                  kGeometryCube,
                  QPoint(4, 3),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(4, 3)),
                  false, false, true);
  blocks << block("Leaves (Birch)",
                  QStringList() << kCategoryBasic << kCategoryVegetation,
                  kGeometryCube,
                  QPoint(5, 3),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(5, 3)),
                  false, false, true);
  blocks << block("Leaves (Pine)",
                  QStringList() << kCategoryBasic << kCategoryVegetation,
                  kGeometryCube,
                  QPoint(5, 8),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(5, 8)),
                  false, false, true);
  blocks << block("Grass",
                  QStringList() << kCategoryBasic << kCategoryVegetation,
                  kGeometryCube,
                  QPoint(0, 0),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(3, 0) << QPoint(3, 0) << QPoint(2, 0) << QPoint(3, 0) << QPoint(0, 0) << QPoint(3, 0)),
                  false, true);
  blocks << block("Dirt",
                  QStringList() << kCategoryBasic,
                  kGeometryCube,
                  QPoint(2, 0),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 0)),
                  false);
  blocks << block("Stone",
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(1, 0),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 0)),
                  false);
  blocks << block("Glass",
                  QStringList() << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(1, 3),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 3)),
                  true);
  blocks << block("Cobblestone",
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(0, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(0, 1)),
                  false);
  blocks << block("Sand",
                  QStringList() << kCategoryBasic,
                  kGeometryCube,
                  QPoint(2, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 1)),
                  false);
  blocks << block("Crafting Table",
                  QStringList() << kCategoryBasic << kCategoryTools,
                  kGeometryCube,
                  QPoint(11, 2),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(11, 3) << QPoint(11, 3) << QPoint(5, 0) << QPoint(12, 3) << QPoint(11, 2) << QPoint(12, 3)),
                  false);
  blocks << block("Furnace",
                  QStringList() << kCategoryBasic << kCategoryTools,
                  kGeometryCube,
                  QPoint(12, 2),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(12, 2) << QPoint(13, 2) << QPoint(6, 0) << QPoint(13, 2) << QPoint(14, 3) << QPoint(13, 2)),
                  false);
  blocks << block("Chest",
                  QStringList() << kCategoryBasic << kCategoryTools,
                  kGeometryChest,
                  QPoint(11, 1),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(11, 1) << QPoint(10, 1) << QPoint(9, 1) << QPoint(10, 1) << QPoint(9, 1) << QPoint(10, 1)),
                  false);
  blocks << block("Water (Source)",
                  QStringList() << kCategoryBasic,
                  kGeometryCube,
                  QPoint(13, 12),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(13, 12)),
                  true);
  blocks << block("Lava (Source)",
                  QStringList() << kCategoryBasic,
                  kGeometryCube,
                  QPoint(13, 14),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(13, 14)),
                  true);
  blocks << block("Wool",
                  QStringList() << kCategoryBasic << kCategoryWool,
                  kGeometryCube,
                  QPoint(0, 4),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(0, 4)),
                  false);
  blocks << block("Light Gray Wool",
                  QStringList() << kCategoryWool,
                  kGeometryCube,
                  QPoint(1, 14),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 14)),
                  false);
  blocks << block("Gray Wool",
                  QStringList() << kCategoryWool,
                  kGeometryCube,
                  QPoint(2, 7),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 7)),
                  false);
  blocks << block("Black Wool",
                  QStringList() << kCategoryWool,
                  kGeometryCube,
                  QPoint(1, 7),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 7)),
                  false);
  blocks << block("Red Wool",
                  QStringList() << kCategoryWool,
                  kGeometryCube,
                  QPoint(1, 8),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 8)),
                  false);
  blocks << block("Pink Wool",
                  QStringList() << kCategoryWool,
                  kGeometryCube,
                  QPoint(2, 8),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 8)),
                  false);
  blocks << block("Green Wool",
                  QStringList() << kCategoryWool,
                  kGeometryCube,
                  QPoint(1, 9),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 9)),
                  false);
  blocks << block("Light Green Wool",
                  QStringList() << kCategoryWool,
                  kGeometryCube,
                  QPoint(2, 9),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 9)),
                  false);
  blocks << block("Brown Wool",
                  QStringList() << kCategoryWool,
                  kGeometryCube,
                  QPoint(1, 10),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 10)),
                  false);
  blocks << block("Yellow Wool",
                  QStringList() << kCategoryWool,
                  kGeometryCube,
                  QPoint(2, 10),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 10)),
                  false);
  blocks << block("Blue Wool",
                  QStringList() << kCategoryWool,
                  kGeometryCube,
                  QPoint(1, 11),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 11)),
                  false);
  blocks << block("Light Blue Wool",
                  QStringList() << kCategoryWool,
                  kGeometryCube,
                  QPoint(2, 11),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 11)),
                  false);
  blocks << block("Purple Wool",
                  QStringList() << kCategoryWool,
                  kGeometryCube,
                  QPoint(1, 12),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 12)),
                  false);
  blocks << block("Magenta Wool",
                  QStringList() << kCategoryWool,
                  kGeometryCube,
                  QPoint(2, 12),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 12)),
                  false);
  blocks << block("Cyan Wool",
                  QStringList() << kCategoryWool,
                  kGeometryCube,
                  QPoint(1, 13),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 13)),
                  false);
  blocks << block("Orange Wool",
                  QStringList() << kCategoryWool,
                  kGeometryCube,
                  QPoint(2, 13),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 13)),
                  false);
  blocks << block("Cobblestone Slab",
                  QStringList() << kCategoryConstruction,
                  kGeometrySlab,
                  QPoint(0, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(0, 1)),
                  true);
  blocks << block("Double Cobblestone Slab",
                  QStringList() << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(0, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(0, 1)),
                  false);
  blocks << block("Stone Slab",
                  QStringList() << kCategoryConstruction,
                  kGeometrySlab,
                  QPoint(6, 0),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(5, 0) << QPoint(5, 0) << QPoint(6, 0) << QPoint(5, 0) << QPoint(6, 0) << QPoint(5, 0)),
                  true);
  blocks << block("Double Stone Slab",
                  QStringList() << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(6, 0),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(5, 0) << QPoint(5, 0) << QPoint(6, 0) << QPoint(5, 0) << QPoint(6, 0) << QPoint(5, 0)),
                  false);
  blocks << block("Wooden Slab",
                  QStringList() << kCategoryConstruction,
                  kGeometrySlab,
                  QPoint(4, 0),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(4, 0)),
                  true);
  blocks << block("Double Wooden Slab",
                  QStringList() << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(4, 0),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(4, 0)),
                  false);
  blocks << block("Brick",
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(7, 0),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(7, 0)),
                  false);
  blocks << block("TNT",
                  QStringList() << kCategoryRedstone,
                  kGeometryCube,
                  QPoint(8, 0),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(8, 0) << QPoint(8, 0) << QPoint(10, 0) << QPoint(8, 0) << QPoint(9, 0) << QPoint(8, 0)),
                  false);
  blocks << block("Bedrock",
                  QStringList() << kCategoryBasic,
                  kGeometryCube,
                  QPoint(1, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 1)),
                  false);
  blocks << block("Gravel",
                  QStringList() << kCategoryBasic,
                  kGeometryCube,
                  QPoint(3, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(3, 1)),
                  false);
  blocks << block("Iron Block",
                  QStringList() << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(6, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(6, 1)),
                  false);
  blocks << block("Gold Block",
                  QStringList() << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(7, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(7, 1)),
                  false);
  blocks << block("Diamond Block",
                  QStringList() << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(8, 1),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(8, 1)),
                  false);
  blocks << block("Lapis Lazuli Block",
                  QStringList() << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(0, 9),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(0, 9)),
                  false);
  blocks << block("Double Chest (Left Half)",
                  QStringList() << kCategoryBasic << kCategoryTools,
                  kGeometryCube,
                  QPoint(9, 2),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(9, 2) << QPoint(9, 3) << QPoint(9, 1) << QPoint(10, 1) << QPoint(9, 1) << QPoint(10, 1)),
                  false);
  blocks << block("Double Chest (Right Half)",
                  QStringList() << kCategoryBasic << kCategoryTools,
                  kGeometryCube,
                  QPoint(10, 2),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(10, 2) << QPoint(10, 3) << QPoint(9, 1) << QPoint(10, 1) << QPoint(9, 1) << QPoint(10, 1)),
                  false);
  blocks << block("Gold Ore",
                  QStringList() << kCategoryMining,
                  kGeometryCube,
                  QPoint(0, 2),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(0, 2)),
                  false);
  blocks << block("Iron Ore",
                  QStringList() << kCategoryMining,
                  kGeometryCube,
                  QPoint(1, 2),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 2)),
                  false);
  blocks << block("Coal Ore",
                  QStringList() << kCategoryMining,
                  kGeometryCube,
                  QPoint(2, 2),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 2)),
                  false);
  blocks << block("Bookcase",
                  QStringList() << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(3, 2),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(3, 2) << QPoint(4, 0) << QPoint(4, 0) << QPoint(4, 0) << QPoint(4, 0) << QPoint(4, 0)),
                  false);
  blocks << block("Mossy Cobblestone",
                  QStringList() << kCategoryConstruction << kCategoryMining,
                  kGeometryCube,
                  QPoint(4, 2),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(4, 2)),
                  false);
  blocks << block("Obsidian",
                  QStringList() << kCategoryConstruction << kCategoryMining,
                  kGeometryCube,
                  QPoint(5, 2),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(5, 2)),
                  false);
  blocks << block("Dispenser",
                  QStringList() << kCategoryTools,
                  kGeometryCube,
                  QPoint(14, 2),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(14, 2) << QPoint(13, 2) << QPoint(6, 0) << QPoint(13, 2) << QPoint(14, 3) << QPoint(13, 2)),
                  false);
  blocks << block("Sponge",
                  QStringList() << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(0, 3),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(0, 3)),
                  false);
  blocks << block("Diamond Ore",
                  QStringList() << kCategoryMining,
                  kGeometryCube,
                  QPoint(2, 3),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(2, 3)),
                  false);
  blocks << block("Redstone Ore",
                  QStringList() << kCategoryMining << kCategoryRedstone,
                  kGeometryCube,
                  QPoint(3, 3),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(3, 3)),
                  false);
  blocks << block("Lapis Lazuli Ore",
                  QStringList() << kCategoryMining,
                  kGeometryCube,
                  QPoint(0, 10),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(0, 10)),
                  false);
  blocks << block("Monster Spawner",
                  QStringList() << kCategoryConstruction << kCategoryMining,
                  kGeometryCube,
                  QPoint(1, 4),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(1, 4)),
                  true);
  blocks << block("Snow",
                  QStringList() << kCategoryBasic,
                  kGeometryCube,
                  QPoint(2, 4),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(4, 4) << QPoint(4, 4) << QPoint(2, 0) << QPoint(4, 4) << QPoint(2, 4) << QPoint(4, 4)),
                  false);
  blocks << block("Ice",
                  QStringList() << kCategoryBasic,
                  kGeometryCube,
                  QPoint(3, 4),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(3, 4)),
                  true);
  blocks << block("Cactus",
                  QStringList() << kCategoryBasic << kCategoryVegetation,
                  kGeometryCactus,
                  QPoint(6, 4),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(6, 4) << QPoint(6, 4) << QPoint(7, 4) << QPoint(6, 4) << QPoint(5, 4) << QPoint(6, 4)),
                  true);
  blocks << block("Clay",
                  QStringList() << kCategoryBasic,
                  kGeometryCube,
                  QPoint(8, 4),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(8, 4)),
                  false);
  blocks << block("Record Player",
                  QStringList() << kCategoryTools,
                  kGeometryCube,
                  QPoint(11, 4),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(10, 4) << QPoint(10, 4) << QPoint(9, 1) << QPoint(10, 4) << QPoint(11, 4) << QPoint(10, 4)),
                  false);
  blocks << block("Field",
                  QStringList() << kCategoryBasic,
                  kGeometryCube,
                  QPoint(7, 5),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(2, 0) << QPoint(2, 0) << QPoint(2, 0) << QPoint(2, 0) << QPoint(7, 5) << QPoint(2, 0)),
                  false);
  blocks << block("Fertile Field",
                  QStringList() << kCategoryBasic,
                  kGeometryCube,
                  QPoint(6, 5),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(2, 0) << QPoint(2, 0) << QPoint(2, 0) << QPoint(2, 0) << QPoint(6, 5) << QPoint(2, 0)),
                  false);
  blocks << block("Pumpkin",
                  QStringList() << kCategoryVegetation,
                  kGeometryCube,
                  QPoint(7, 7),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(7, 7) << QPoint(6, 7) << QPoint(6, 7) << QPoint(6, 7) << QPoint(6, 6) << QPoint(6, 7)),
                  false);
  blocks << block("Netherrack",
                  QStringList() << kCategoryBasic << kCategoryNether,
                  kGeometryCube,
                  QPoint(7, 6),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(7, 6)),
                  false);
  blocks << block("Soul Sand",
                  QStringList() << kCategoryBasic << kCategoryNether,
                  kGeometryCube,
                  QPoint(8, 6),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(8, 6)),
                  false);
  blocks << block("Glowstone",
                  QStringList() << kCategoryBasic << kCategoryNether,
                  kGeometryCube,
                  QPoint(9, 6),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(9, 6)),
                  false);
  blocks << block("Glowstone Lamp (Off)",
                  QStringList() << kCategoryRedstone,
                  kGeometryCube,
                  QPoint(3, 13),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(3, 13)),
                  false);
  blocks << block("Glowstone Lamp (On)",
                  QStringList() << kCategoryRedstone,
                  kGeometryCube,
                  QPoint(4, 13),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(4, 13)),
                  false);
  blocks << block("Sandstone",
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(0, 12),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(0, 12) << QPoint(0, 12) << QPoint(0, 13) << QPoint(0, 12) << QPoint(0, 11) << QPoint(0, 12)),
                  false);
  blocks << block("Sandstone (Chiseled)",
                  QStringList() << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(6, 14),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(6, 14) << QPoint(6, 14) << QPoint(0, 13) << QPoint(6, 14) << QPoint(0, 11) << QPoint(6, 14)),
                  false);
  blocks << block("Sandstone (Heiroglyphs)",
                  QStringList() << kCategoryConstruction,
                  kGeometryCube,
                  QPoint(5, 14),
                  QVariantList() << kOrientationNone,
                  textures(QList<QPoint>() << QPoint(5, 14) << QPoint(5, 14) << QPoint(0, 13) << QPoint(5, 14) << QPoint(0, 11) << QPoint(5, 14)),
                  false);
  blocks << block("Note Block",
                  QStringList() << kCategoryRedstone,
                  kGeometryCube,
                  QPoint(10, 4),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(10, 4)),
                  false);
  blocks << block("Bed (Bottom Half)",
                  QStringList() << kCategoryBasic,
                  kGeometryBed,
                  QPoint(6, 9),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  // FRONT BACK BOTTOM RIGHT TOP LEFT
                  textures(QList<QPoint>() << QPoint(6, 9) << QPoint(6, 9) << QPoint(4, 0) << QPoint(5, 9) << QPoint(6, 8) << QPoint(5, 11)),
                  true);
  blocks << block("Bed (Top Half)",
                  QStringList() << kCategoryBasic,
                  kGeometryBed,
                  QPoint(7, 9),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QList<QPoint>() << QPoint(7, 9) << QPoint(7, 9) << QPoint(4, 0) << QPoint(5, 11) << QPoint(7, 8) << QPoint(8, 9)),
                  true);
  blocks << block("Wooden Pressure Plate",
                  QStringList() << kCategoryRedstone,
                  kGeometryPressurePlate,
                  QPoint(4, 0),
                  QVariantList() << kOrientationNone,
                  textures(QPoint(4, 0)),
                  true);
  blocks << block("Wooden Door (Bottom Half)",
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  kGeometryDoor,
                  QPoint(1, 6),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QPoint(1, 6)),
                  true);
  blocks << block("Wooden Door (Top Half)",
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  kGeometryDoor,
                  QPoint(1, 5),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QPoint(1, 5)),
                  true);
  blocks << block("Cobblestone Stairs",
                  QStringList() << kCategoryBasic << kCategoryConstruction,
                  kGeometryStairs,
                  QPoint(0, 1),
                  QVariantList() << kOrientationFacingSouth << kOrientationFacingWest << kOrientationFacingNorth << kOrientationFacingEast,
                  textures(QPoint(0, 1)),
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
