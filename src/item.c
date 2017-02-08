#include <assert.h>
#include "item.h"

int item_type_slot(int item_type) {
  assert(0 <= item_type && item_type < NUM_ITEM_TYPES);

  int slot = EQUIPMENT_NO_SLOT;
  switch (item_type) {
    case ITEM_SWORD:
      slot = EQUIPMENT_WEAPON_LEFT | EQUIPMENT_WEAPON_RIGHT;
      break;
    case ITEM_AXE:
      slot = EQUIPMENT_WEAPON_LEFT | EQUIPMENT_WEAPON_RIGHT;
      break;
    case ITEM_MACE:
      slot = EQUIPMENT_WEAPON_LEFT | EQUIPMENT_WEAPON_RIGHT;
      break;
    case ITEM_SPEAR:
      slot = EQUIPMENT_WEAPON_LEFT | EQUIPMENT_WEAPON_RIGHT;
      break;
    case ITEM_BOW:
      slot = EQUIPMENT_WEAPON_LEFT | EQUIPMENT_WEAPON_RIGHT;
      break;
    case ITEM_STAFF:
      slot = EQUIPMENT_WEAPON_LEFT | EQUIPMENT_WEAPON_RIGHT;
      break;
    case ITEM_SHIELD:
      slot = EQUIPMENT_WEAPON_RIGHT;
      break;
    case ITEM_HELM:
      slot = EQUIPMENT_HELM;
      break;
    case ITEM_ARMOR:
      slot = EQUIPMENT_ARMOR;
      break;
    case ITEM_PANTS:
      slot = EQUIPMENT_PANTS;
      break;
    case ITEM_GLOVE:
      slot = EQUIPMENT_GLOVES;
      break;
    case ITEM_BOOT:
      slot = EQUIPMENT_BOOTS;
      break;
    case ITEM_WING:
      slot = EQUIPMENT_WING;
      break;
    case ITEM_PET:
      slot = EQUIPMENT_PET;
      break;
    case ITEM_RING:
      slot = EQUIPMENT_RING_LEFT | EQUIPMENT_RING_RIGHT;
      break;
    case ITEM_AMULET:
      slot = EQUIPMENT_RING_LEFT | EQUIPMENT_RING_RIGHT;
      break;
    case ITEM_POTION:
      slot = EQUIPMENT_NO_SLOT;
      break;
    case ITEM_OTHER:
      slot = EQUIPMENT_NO_SLOT;
      break;
  }
  return slot;
}
