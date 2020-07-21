#pragma once

#include "Message.h"
#include <Structs/GameId.h>
#include <Structs/ActionEvent.h>
#include <Structs/Tints.h>
#include <Structs/Vector3_NetQuantize.h>
#include <Structs/Rotator2_NetQuantize.h>
#include <Buffer.hpp>
#include <Stl.hpp>

using TiltedPhoques::String;

struct CharacterSpawnRequest final : ServerMessage
{
    CharacterSpawnRequest()
        : ServerMessage(kCharacterSpawnRequest)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const CharacterSpawnRequest& acRhs) const noexcept
    {
        return
            ServerId == acRhs.ServerId &&
            FormId == acRhs.FormId &&
            BaseId == acRhs.BaseId &&
            Position == acRhs.Position &&
            Rotation == acRhs.Rotation &&
            ChangeFlags == acRhs.ChangeFlags &&
            AppearanceBuffer == acRhs.AppearanceBuffer &&
            InventoryBuffer == acRhs.InventoryBuffer &&
            FaceTints == acRhs.FaceTints &&
            GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t ServerId{};
    GameId FormId{};
    GameId BaseId{};
    Vector3_NetQuantize Position{};
    Rotator2_NetQuantize Rotation{};
    uint32_t ChangeFlags{};
    String AppearanceBuffer{};
    String InventoryBuffer{};
    ActionEvent LatestAction{};
    Tints FaceTints{};
};
