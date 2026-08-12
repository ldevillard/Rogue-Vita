#pragma once

#include <memory>
#include <vector>

#include "dvl/log/log.h"
#include "dvl/tween/tween.h"

namespace dvl
{
    class Tweener
    {
    public:
        // TODO: Return a handle and make a better ownership system for tweens to avoid dangling references
        template <typename T>
        static Tween<T>& Create(const T& from, const T& to, float duration, Easing easing = Easing::Linear, int loopCount = 1)
        {
            if (duration <= 0.0f)
                dvl::Log(dvl::LogLevel::Error, "Tween duration must be greater than zero");

            if (loopCount == 0 || loopCount < -1)
                dvl::Log(dvl::LogLevel::Error, "Tween loop count must be greater than zero or -1");

            std::unique_ptr<Tween<T>> tween = std::make_unique<Tween<T>>(from, to, duration, easing, loopCount);
            Tween<T>& result = *tween;

            _tweens.push_back(std::move(tween));

            return result;
        }

        static void Update(float deltaTime);

    private:
        static std::vector<std::unique_ptr<ITween>> _tweens;
    };
}
