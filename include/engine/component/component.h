#pragma once

class Component
{
public:
    virtual ~Component() = default;

    virtual bool IsValid() const = 0;
};
