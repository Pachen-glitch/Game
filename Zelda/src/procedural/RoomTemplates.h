#pragma once

#include "RoomType.h"

#include <cstdint>
#include <string>
#include <vector>

// Bitmask for allowed / required openings on a room edge.
enum RoomOpeningMask : uint8_t {
    OpenNone  = 0,
    OpenNorth = 1 << 0,
    OpenSouth = 1 << 1,
    OpenEast  = 1 << 2,
    OpenWest  = 1 << 3,
    OpenAll   = OpenNorth | OpenSouth | OpenEast | OpenWest
};

struct RoomTemplateDef {
    const char* id;
    RoomType category;
    uint8_t allowedOpenings;
    const std::vector<std::string>* layout;
};

uint8_t openingMaskForSide(DoorSide side);
uint8_t requiredOpeningMask(const std::vector<struct RoomConnection>& connections);

const RoomTemplateDef* pickRoomTemplate(
    RoomType type,
    uint8_t requiredOpenings,
    int salt
);
