#pragma once

#include <algorithm>
#include <functional>
#include <utility>

#include "dvl/tween/easing.h"

namespace dvl
{
    // Interface to allow polymorphic storage of Tween<T> in Tweener
    class ITween
    {
    public:
        virtual ~ITween() = default;

        virtual void Update(float deltaTime) = 0;
        virtual void Kill() = 0;
        virtual bool IsFinished() const = 0;
    };

    template <typename T>
    class Tween : public ITween
    {
    public:
        using Callback = std::function<void()>;
        using UpdateCallback = std::function<void(const T&)>;

        Tween(const T& from, const T& to, float duration, Easing easing, int loopCount = 1)
            : _from(from), _to(to), _duration(std::max(duration, 0.0f)), _easing(easing), _loopCount(loopCount)
        {
        }

        Tween& OnStart(Callback callback)
        {
            _onStart = std::move(callback);
            return *this;
        }

        Tween& OnUpdate(UpdateCallback callback)
        {
            _onUpdate = std::move(callback);
            return *this;
        }

        Tween& OnComplete(Callback callback)
        {
            _onComplete = std::move(callback);
            return *this;
        }

        void Kill() override
        {
            _finished = true;
        }

    private:
        void Update(float deltaTime) override
        {
            if (_finished)
                return;

            if (!_started)
            {
                _started = true;

                if (_onStart)
                    _onStart();

                // Check if there was a call to Kill() in the OnStart callback
                if (_finished)
                    return;
            }

            _elapsedTime = std::min(_elapsedTime + std::max(deltaTime, 0.0f), _duration);

            const float time = _duration > 0.0f ? _elapsedTime / _duration : 1.0f;

            const float easedTime = ApplyEasing(_easing, time);
            const T value = _from + (_to - _from) * easedTime;

            if (_onUpdate)
                _onUpdate(value);

            // Check if there was a call to Kill() in the OnUpdate callback
            if (_finished)
                return;

            if (time >= 1.0f)
            {
                if (_loopCount == -1)
                {
                    _elapsedTime = 0.0f;
                    return;
                }

                _completedLoops++;

                if (_completedLoops < _loopCount)
                {
                    _elapsedTime = 0.0f;
                    return;
                }

                _finished = true;
                if (_onComplete)
                    _onComplete();
            }
        }

        bool IsFinished() const override
        {
            return _finished;
        }

        T _from;
        T _to;

        float _duration;
        float _elapsedTime = 0.0f;

        Easing _easing;

        int _loopCount;
        int _completedLoops = 0;

        bool _started = false;
        bool _finished = false;

        Callback _onStart;
        UpdateCallback _onUpdate;
        Callback _onComplete;
    };
}
