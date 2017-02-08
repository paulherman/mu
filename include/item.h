#ifndef ITEM_H
#define ITEM_H

#include <stdint.h>
#include <stdbool.h>

enum item_type {
  ITEM_SWORD,
  ITEM_AXE,
  ITEM_MACE,
  ITEM_SPEAR,
  ITEM_BOW,
  ITEM_STAFF,
  ITEM_SHIELD,
  ITEM_HELM,
  ITEM_ARMOR,
  ITEM_PANTS,
  ITEM_GLOVE,
  ITEM_BOOT,
  ITEM_WING,
  ITEM_PET,
  ITEM_RING,
  ITEM_AMULET,
  ITEM_POTION,
  ITEM_OTHER,
  NUM_ITEM_TYPES
};

enum equipment_slot {
  EQUIPMENT_NO_SLOT = -1,
  EQUIPMENT_WEAPON_RIGHT = 1,
  EQUIPMENT_WEAPON_LEFT = 2,
  EQUIPMENT_HELM = 4,
  EQUIPMENT_ARMOR = 8,
  EQUIPMENT_PANTS = 16,
  EQUIPMENT_GLOVES = 32,
  EQUIPMENT_BOOTS = 64,
  EQUIPMENT_WING = 128,
  EQUIPMENT_PET = 256,
  EQUIPMENT_AMULET = 512,
  EQUIPMENT_RING_RIGHT = 1024,
  EQUIPMENT_RING_LEFT = 2048,
  EQUIPMENT_GOLD = 4096,
  NUM_EQUIPMENT_SLOTS = 12
};

struct item {
  char name[32];
  uint8_t type;
  uint8_t level;
  uint8_t width;
  uint8_t height;
  bool two_handed;
  uint32_t required_strength;
  uint32_t required_agility;
  uint32_t required_energy;
  uint32_t required_vitality;
  uint32_t required_leadership;
  uint16_t required_level;
  uint32_t value;
};

int item_type_slot(int item_type);

#endif
