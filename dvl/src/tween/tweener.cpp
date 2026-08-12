#include "dvl/tween/tweener.h"

namespace dvl
{
    std::vector<std::unique_ptr<ITween>> Tweener::_tweens;

    void Tweener::Update(float deltaTime)
    {
        const std::size_t tweenCount = _tweens.size();

        for (std::size_t i = 0; i < tweenCount; ++i)
        {
            _tweens[i]->Update(deltaTime);
        }

        // Remove finished tweens
        for (auto it = _tweens.begin(); it != _tweens.end();)
        {
            if ((*it)->IsFinished())
            {
                it = _tweens.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}
