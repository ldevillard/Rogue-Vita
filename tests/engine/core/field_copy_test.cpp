#include "unit_test.h"

#include "engine/component/component.h"
#include "engine/core/entity_ref.h"
#include "engine/core/field_copy.h"

DVL_TEST(ReferenceMappingPreservesZeroAndUnmappedIds)
{
    const std::unordered_map<unsigned int, unsigned int> mapping = {{4, 12}};

    DVL_EXPECT_EQ(ReferenceMapping::RemapId(mapping, 0), 0u);
    DVL_EXPECT_EQ(ReferenceMapping::RemapId(mapping, 4), 12u);
    DVL_EXPECT_EQ(ReferenceMapping::RemapId(mapping, 9), 9u);

    return true;
}

DVL_TEST(CopyFieldRemapsEntityReferences)
{
    ReferenceMapping mapping;
    mapping.entityIds[3] = 8;
    const EntityRef source{3};
    EntityRef destination;

    CopyField(source, destination, mapping);

    DVL_EXPECT_EQ(destination.id, 8u);

    return true;
}

DVL_TEST(CopyFieldRemapsComponentReferences)
{
    ReferenceMapping mapping;
    mapping.componentIds[7] = 15;
    const ComponentRef<Component> source{7};
    ComponentRef<Component> destination;

    CopyField(source, destination, mapping);

    DVL_EXPECT_EQ(destination.id, 15u);

    return true;
}
