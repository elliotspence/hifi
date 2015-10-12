//
//  Created by Bradley Austin Davis 2015/10/09
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once

namespace Controllers {

    // Needs to match order and values of SDL_GameControllerButton
    enum StandardButtonChannel {
        // Button quad
        A = 0,
        B,
        X,
        Y,
        // Center buttons
        BACK,
        GUIDE,
        START,
        // Stick press
        LS,
        RS,
        // Bumper press
        LB,
        RB,
        // DPad
        DU,
        DD,
        DL,
        DR
    };

    // Needs to match order and values of SDL_GameControllerAxis
    enum StandardAxisChannel {
        // Left Analog stick
        LX = 0,
        LY,
        // Right Analog stick
        RX,
        RY,
        // Triggers
        LT,
        RT
    };

    // No correlation to SDL
    enum StandardPoseChannel {
        LeftPose = 0,
        RightPose
    };

}
