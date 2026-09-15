#pragma once

class AssetRegistry;
class World;

class AnimationSystem
{
public:
    void Update(World& world, const AssetRegistry& assetRegistry, float deltaTime) const;
};
