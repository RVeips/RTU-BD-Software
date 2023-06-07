#pragma once
#include <CFXS/Base/Function.hpp>

class Buttons {
    static constexpr auto DEBOUNCE_TIME = 3; // 3ms

public:
    enum Key { LEFT, RIGHT, UP, DOWN, OK, _COUNT };
    using StateCallbackFunction = CFXS::Function<void(void* user_data, Key key, bool state)>;

public:
    static void Initialize();

    static bool GetState(Key key);

    static void RegisterStateCallback(const StateCallbackFunction& cb);

private:
    static void Process(void* task_data);
};