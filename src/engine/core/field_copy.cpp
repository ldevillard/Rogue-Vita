#include "engine/core/field_copy.h"

#include "engine/core/entity_ref.h"

unsigned int ReferenceMapping::RemapId(const std::unordered_map<unsigned int, unsigned int>& table, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }

    const auto it = table.find(id);
    return it != table.end() ? it->second : id;
}

void CopyField(const EntityRef& source, EntityRef& destination, const ReferenceMapping& remap)
{
    destination.id = ReferenceMapping::RemapId(remap.entityIds, source.id);
}