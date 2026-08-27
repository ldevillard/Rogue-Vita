#include "../unit_test.h"

#include <cstddef>

#include "dvl/math/transform.h"

DVL_TEST(TransformHasThreeAlignedSixteenByteBlocks)
{
    DVL_EXPECT_EQ(alignof(dvl::Transform), static_cast<std::size_t>(16));
    DVL_EXPECT_EQ(sizeof(dvl::Transform), static_cast<std::size_t>(48));
    DVL_EXPECT_EQ(offsetof(dvl::Transform, rotation), static_cast<std::size_t>(0));
    DVL_EXPECT_EQ(offsetof(dvl::Transform, translation), static_cast<std::size_t>(16));
    DVL_EXPECT_EQ(offsetof(dvl::Transform, scale), static_cast<std::size_t>(32));

    return true;
}

DVL_TEST(TransformDefaultValuesRepresentIdentity)
{
    const dvl::Transform transform;

    DVL_EXPECT_EQ(transform.rotation.x, 0.0f);
    DVL_EXPECT_EQ(transform.rotation.y, 0.0f);
    DVL_EXPECT_EQ(transform.rotation.z, 0.0f);
    DVL_EXPECT_EQ(transform.rotation.w, 1.0f);

    DVL_EXPECT_EQ(transform.translation.x, 0.0f);
    DVL_EXPECT_EQ(transform.translation.y, 0.0f);
    DVL_EXPECT_EQ(transform.translation.z, 0.0f);
    DVL_EXPECT_EQ(transform.translation.w, 0.0f);

    DVL_EXPECT_EQ(transform.scale.x, 1.0f);
    DVL_EXPECT_EQ(transform.scale.y, 1.0f);
    DVL_EXPECT_EQ(transform.scale.z, 1.0f);
    DVL_EXPECT_EQ(transform.scale.w, 0.0f);

    return true;
}

DVL_TEST(TransformSupportsAggregateInitialization)
{
    const dvl::Quat rotation(1.0f, 2.0f, 3.0f, 4.0f);
    const dvl::Vec4 translation(5.0f, 6.0f, 7.0f, 0.0f);
    const dvl::Vec4 scale(8.0f, 9.0f, 10.0f, 0.0f);
    const dvl::Transform transform { rotation, translation, scale };

    DVL_EXPECT_EQ(transform.rotation.w, 4.0f);
    DVL_EXPECT_EQ(transform.translation.x, 5.0f);
    DVL_EXPECT_EQ(transform.translation.y, 6.0f);
    DVL_EXPECT_EQ(transform.translation.z, 7.0f);
    DVL_EXPECT_EQ(transform.scale.x, 8.0f);
    DVL_EXPECT_EQ(transform.scale.y, 9.0f);
    DVL_EXPECT_EQ(transform.scale.z, 10.0f);

    return true;
}
