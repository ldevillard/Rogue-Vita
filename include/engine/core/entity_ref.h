#pragma once

class Entity;
class World;

struct EntityRef
{
    unsigned int id = 0;

    Entity* Get(World& world) const;
};