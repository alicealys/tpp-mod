/*
 * Copyright (C) 2024 Thanasis Petsas <thanpetsas@gmail.com>
 * Licence: MIT Licence
 */

#ifndef DSXPROTOCOL_H
#define DSXPROTOCOL_H

#include "nlohmann/json.hpp"
#include <string>
#include <vector>

enum TriggerMode
{
    Normal = 0,
    GameCube = 1,
    VerySoft = 2,
    Soft = 3,
    Hard = 4,
    VeryHard = 5,
    Hardest = 6,
    Rigid = 7,
    VibrateTrigger = 8,
    Choppy = 9,
    Medium = 10,
    VibrateTriggerPulse = 11,
    CustomTriggerValue = 12,
    Resistance = 13,
    Bow = 14,
    Galloping = 15,
    SemiAutomaticGun = 16,
    AutomaticGun = 17,
    Machine = 18,
#ifdef DSX_V3
    VibrateTrigger10Hz = 19,
    TriggerOff = 20,
    Feedback = 21,
    Weapon = 22,
    Vibration = 23,
    SlopeFeedback = 24,
    MultiplePositionFeedback = 25,
    MultiplePositionVibration = 26
#endif
};

enum CustomTriggerValueMode
{
    OFF = 0,
    Rigid0 = 1,
    RigidA = 2,
    RigidB = 3,
    RigidAB = 4,
    Pulse0 = 5,
    PulseA = 6,
    PulseB = 7,
    PulseAB = 8,
    VibrateResistance = 9,
    VibrateResistanceA = 10,
    VibrateResistanceB = 11,
    VibrateResistanceAB = 12,
    VibratePulse = 13,
    VibratePulseA = 14,
    VibratePulsB = 15,
    VibratePulseAB = 16
};

enum PlayerLEDNewRevision
{
    One = 0,
    Two = 1,
    Three = 2,
    Four = 3,
    Five = 4, // Five is Also All On
    AllOff = 5
};

enum MicLEDMode
{
    On = 0,
    Pulse = 1,
    Off = 2
};

enum Trigger
{
    Invalid,
    Left,
    Right
};

enum InstructionType
{
    GetDSXStatus,
    TriggerUpdate,
    RGBUpdate,
    PlayerLED,
    TriggerThreshold,
    MicLED,
    PlayerLEDRevisionUpdate,
    ResetToUserSettings
};

struct DSXInstruction
{
    InstructionType type;
    std::vector<int> parameters;

    DSXInstruction() : type(GetDSXStatus), parameters({}) {}

    DSXInstruction(InstructionType type, std::vector<int> parameters) :
        type(type), parameters(parameters) {}

    // implemented in DSX++.cpp
    nlohmann::json toJson();
};

struct Payload
{
    std::vector<DSXInstruction> instructions;

    Payload() : instructions({}) {}
    
    Payload(std::vector<DSXInstruction> instructions) :
        instructions(instructions) {}

    // implemented in DSX++.cpp
    nlohmann::json toJson();

};

struct ServerResponse
{
    std::string status;
    std::string timeReceived;
    bool isControllerConnected;
    int batteryLevel;
};

#ifdef DSX_V3
enum ConnectionType
{
    Usb,
    Bluetooth,
    Dongle
};

enum DeviceType
{
    Dualsense,
    DualsenseEdge,
    DualshockV1,
    DualshockV2,
    DualshockDongle,
    PSVR2LeftController,
    PSVR2RightController,
    AccessVontroller
};
#endif

#endif // DSXPROTOCOL_H
