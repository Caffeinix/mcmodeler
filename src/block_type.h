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

#ifndef BLOCK_TYPE_H
#define BLOCK_TYPE_H

/**
  * Enum constants for all known Minecraft block types.  The constants themselves should be self-explanatory.
  * There are two special block types, kBlockTypeAir and kBlockTypeUnknown, which should not be added to the Diagram
  * but can be used in a view-dependent manner.
  */
enum BlockType {
  kBlockTypeGrass,
  kBlockTypeDirt,
  kBlockTypeStone,
  kBlockTypeGlass,
  kBlockTypeWoodPlank,
  kBlockTypeCobblestoneSlab,
  kBlockTypeDoubleCobblestoneSlab,
  kBlockTypeStoneSlab,
  kBlockTypeDoubleStoneSlab,
  kBlockTypeWoodSlab,
  kBlockTypeDoubleWoodSlab,
  kBlockTypeClay,
  kBlockTypeBrick,
  kBlockTypeTNT,
  kBlockTypeYellowFlower,
  kBlockTypeRedFlower,
  kBlockTypeSapling,
  kBlockTypeCobblestone,
  kBlockTypeBedrock,
  kBlockTypeSand,
  kBlockTypeGravel,
  kBlockTypeSandstone,
  kBlockTypeWoodTrunk,
  kBlockTypeWoodTrunkBirch,
  kBlockTypeWoodTrunkPine,
  kBlockTypeIron,
  kBlockTypeGold,
  kBlockTypeDiamond,
  kBlockTypeLapisLazuli,
  kBlockTypeCoalOre,
  kBlockTypeIronOre,
  kBlockTypeGoldOre,
  kBlockTypeRedstoneOre,
  kBlockTypeDiamondOre,
  kBlockTypeLapisLazuliOre,
  kBlockTypeBookcase,
  kBlockTypeMossyCobblestone,
  kBlockTypeObsidian,
  kBlockTypeWorkbench,
  kBlockTypeFurnace,
  kBlockTypeFurnaceLit,
  kBlockTypeDispenser,
  kBlockTypeChest,
  kBlockTypeDoubleChestLeft,
  kBlockTypeDoubleChestRight,
  kBlockTypeRedMushroom,
  kBlockTypeBrownMushroom,
  kBlockTypeWhiteWool,
  kBlockTypeLightGrayWool,
  kBlockTypeDarkGrayWool,
  kBlockTypeBlackWool,
  kBlockTypeRedWool,
  kBlockTypeOrangeWool,
  kBlockTypeBrownWool,
  kBlockTypeYellowWool,
  kBlockTypeLightGreenWool,
  kBlockTypeGreenWool,
  kBlockTypeLightBlueWool,
  kBlockTypeCyanWool,
  kBlockTypeBlueWool,
  kBlockTypePurpleWool,
  kBlockTypeMagentaWool,
  kBlockTypePinkWool,
  kBlockTypeSponge,
  kBlockTypeSpawner,
  kBlockTypeSnow,
  kBlockTypeIce,
  kBlockTypeCactus,
  kBlockTypeReeds,
  kBlockTypeLeaves,
  kBlockTypeLeavesBirch,
  kBlockTypeLeavesPine,
  kBlockTypeJukebox,
  kBlockTypeNoteBlock,
  kBlockTypeCake,
  kBlockTypeTorch,
  kBlockTypeWoodenDoor,
  kBlockTypeIronDoor,
  kBlockTypeLadder,
  kBlockTypeRedstoneDust,
  kBlockTypeFertileField,
  kBlockTypeField,
  kBlockTypeWheat,
  kBlockTypeLever,
  kBlockTypeButton,
  kBlockTypeWoodenPressurePlate,
  kBlockTypeStonePressurePlate,
  kBlockTypePumpkin,
  kBlockTypeNetherrack,
  kBlockTypeSoulSand,
  kBlockTypeGlowstone,
  kBlockTypeMinecartTrack,
  kBlockTypeWaterSource,
  kBlockTypeWaterFlow,
  kBlockTypeLavaSource,
  kBlockTypeLavaFlow,
  kBlockTypeAir,
  kBlockTypeUnknown,
  kBlockTypeLastBlock = kBlockTypeLavaFlow
};

#endif // BLOCK_TYPE_H
