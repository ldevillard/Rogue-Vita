#pragma once

#include <type_traits>
#include <unordered_map>

#include "engine/component/component_ref.h"

class EntityRef;

struct ReferenceMapping
{
    std::unordered_map<unsigned int, unsigned int> entityIds;
    std::unordered_map<unsigned int, unsigned int> componentIds;

    static unsigned int RemapId(const std::unordered_map<unsigned int, unsigned int>& table, unsigned int id);
};

// Simple copy
template <typename T>
void CopyField(const T& source, T& destination, const ReferenceMapping&)
{
    static_assert(!std::is_pointer_v<T>, "Use EntityRef or ComponentRef instead of raw pointers in COMPONENT_FIELDS");

    destination = source;
}

// Copy with entity reference remaping
void CopyField(const EntityRef& source, EntityRef& destination, const ReferenceMapping& remap);

// Copy with component reference remaping
template <typename T>
void CopyField(const ComponentRef<T>& source, ComponentRef<T>& destination, const ReferenceMapping& remap)
{
    destination.id = ReferenceMapping::RemapId(remap.componentIds, source.id);
}

// Implementation method to select the correct copy method
template <typename Source, typename Destination, std::size_t... I>
void CopyFieldsImpl(const Source& source, Destination& destination, const ReferenceMapping& remap, std::index_sequence<I...>)
{
    (CopyField(std::get<I>(destination), std::get<I>(source), remap), ...);
}

// The generic call that will forward source and destination to the implementation method
template <typename Source, typename Destination>
void CopyFields(Destination destination, const Source& source, const ReferenceMapping& remap)
{
    static_assert(std::tuple_size_v<Destination> == std::tuple_size_v<Source>, "Field lists must have the same size");

    CopyFieldsImpl(source, destination, remap, std::make_index_sequence<std::tuple_size_v<Source>>{});
}