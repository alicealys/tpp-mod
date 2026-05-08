/*
 * Copyright (C) 2024 Thanasis Petsas <thanpetsas@gmail.com>
 * Licence: MIT Licence
 */

#include <iostream>
#include "DSX++.h"

int main (void)
{
    if ( DSX::init() != DSX::Success ) {
        std::cerr << "* DSX++ client failed to initialize!" << std::endl;
        return -1;
    }
    std::cout << "* DSX++ client initialized successfully!" << std::endl;

#ifdef DSX_V3
    DSX::setLeftTrigger(Weapon, {2, 6, 8});
    DSX::setRightTrigger(Vibration, {2, 8, 20});
#else
    DSX::setLeftTrigger(GameCube);
    DSX::setRightTrigger(Hard);
#endif

    if (DSX::sendPayload() != DSX::Success) {
        std::cerr << "* DSX++ client failed to send data!" << std::endl;
        return -2;
    }
    std::cout << "* DSX++ client sent the data successfully!" << std::endl;

    if ( DSX::terminate() != DSX::Success ) {
        std::cerr << "* DSX++ client failed to terminate!" << std::endl;
        return -3;
    }
    std::cout << "* DSX++ client terminated successfully!" << std::endl;

    return 0;
}
