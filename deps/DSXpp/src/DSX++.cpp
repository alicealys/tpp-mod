/*
 * Copyright (C) 2024 Thanasis Petsas <thanpetsas@gmail.com>
 * Licence: MIT Licence
 */

#include "DSX++.h"
#include "UDPClient.h"
#include <iostream>

#define DSX_SERVER "127.0.0.1"  // UDP Server of DSX (localhost)
#define DSX_SERVER_PORT 6969    // The default DSX Server port

using json=nlohmann::json;

// Implementations of some DSX structures' functions
// (declared in DSXProtocol)

json DSXInstruction::toJson()
{
    json j = {
        { "type", type },
        { "parameters", parameters }
    };
    return j;
}

json Payload::toJson()
{
    std::vector<json> jsonInstructions;
    for (auto instruction : instructions) {
        jsonInstructions.push_back( instruction.toJson() );
    }
    json j = {
        { "instructions", jsonInstructions}
    };
    return j;
}

// Internal functions

/**
 * Adds an adaptive trigger instruction to the provided payload
 * This instruction configures the trigger mode and parameters
 * for the adaptive trigger
 * @param payload  The payload to receive the instruction
 * @param controllerIndex    The index of the controller to receive the trigger
 * change
 * @param trigger  The trigger (e.g., L2 or R2) to be configured
 * @param triggerMode The mode to set for the adaptive trigger
 * @param extras (optional) Additional parameters required by the trigger
 * */
void addAdaptiveTriggerToPayload(Payload &payload, int controllerIndex,
        Trigger trigger, TriggerMode triggerMode, std::vector<int> extras)
{
    // TODO: add bound checks for all the parameters
    std::vector<int> parameters = {
        controllerIndex,
        trigger,
        triggerMode
    };

    if (!extras.empty()) {
        parameters.insert(parameters.end(), extras.begin(), extras.end());
    }

    DSXInstruction instruction(TriggerUpdate, parameters);
    payload.instructions.push_back(instruction);
}


/**
 * Adds a custom adaptive trigger instruction to the provided payload
 * This instruction configures the trigger mode and parameters
 * for the adaptive trigger
 * @param payload  The payload to receive the instruction
 * @param controllerIndex    The index of the controller to receive the trigger
 * change
 * @param trigger  The trigger (e.g., L2 or R2) to be configured
 * @param triggerMode  The mode to set for the adaptive trigger
 * @param customMode   The custom value mode for more detailed trigger
 * @param extras (optional) Additional parameters required by the trigger
 * */
void addCustomAdaptiveTriggerToPayload(Payload &payload, int controllerIndex,
                                Trigger trigger,
                                CustomTriggerValueMode customMode,
                                std::vector<int> extras)
{
    // TODO: add bound checks for all the parameters
    std::vector<int> parameters = {
        controllerIndex,
        trigger,
        CustomTriggerValue,
        customMode
    };

    if (!extras.empty()) {
        parameters.insert(parameters.end(), extras.begin(), extras.end());
    }

    DSXInstruction instruction(TriggerUpdate, parameters);
    payload.instructions.push_back(instruction);
}


/**
 * Adds an RGB LEDs update to the provided payload
 * @param payload  The payload to receive the instruction
 * @param controllerIndex  The index of the controller to receive the RGB
 * change
 * @param red     The red (R) component of the color
 * @param green   The green (G) component of the color
 * @param blue    The blue (B) component of the color
 * @param brightness    The brightness level of the LEDs
 */
void addRGBToPayload(Payload &payload, int controllerIndex,
                                            int red, int green,
                                                int blue, int brightness)
{

    // TODO: add bound checks for all the parameters
    std::vector<int> parameters = {
        controllerIndex,
        red,
        green,
        blue,
        brightness
    };

    DSXInstruction instruction(RGBUpdate, parameters);
    payload.instructions.push_back(instruction);
}

/**
 * Adds a player LED indicative color update to the provided payload
 * @param payload  The payload to receive the instruction
 * @param controllerIndex  The index of the controller to receive the LED
 * change
 * @param playerLED   player LED configuration to apply
 */
void addPlayerLEDToPayload(Payload &payload, int controllerIndex,
                                            PlayerLEDNewRevision playerLED)
{
    // TODO: add bound checks for all the parameters
    std::vector<int> parameters = {
        controllerIndex,
        playerLED
    };

    DSXInstruction instruction(PlayerLEDRevisionUpdate, parameters);
    payload.instructions.push_back(instruction);
}

/**
 * Adds a microphone LED update (mute/unmute) to the provided payload
 * @param micLED  The microphone LED configuration to apply
 */
void addMicLEDToPayload(Payload &payload, int controllerIndex,
                                                    MicLEDMode micLED)
{
    // TODO: add bound checks for all the parameters
    std::vector<int> parameters = {
        controllerIndex,
        micLED
    };

    DSXInstruction instruction(MicLED, parameters);
    payload.instructions.push_back(instruction);
}

/**
 * Adds a reset setting instruction to the provided payload to reset the
 * settings back to the controller's setting to user's predefined settings
 * configured in the DSX app for the current controller's payload
 */
 void addResetToPayload(Payload &payload, int controllerIndex)
 {
    // TODO: add bound checks for all the parameters
    std::vector<int> parameters = {
        controllerIndex
    };

    DSXInstruction instruction(ResetToUserSettings, parameters);
    payload.instructions.push_back(instruction);
 }

/**
 * Adds a request to get the DSX status (list of devices) to the provided
 * payload
 */
void addGetDSXStatus(Payload &payload, int controllerIndex)
{
    // TODO: add bound checks for all the parameters
    std::vector<int> parameters = {
        controllerIndex
    };

    DSXInstruction instruction(GetDSXStatus, parameters);
    payload.instructions.push_back(instruction);
}


namespace DSX
{

    Payload payload;
    extern const int Success = 0;

    int init(void)
    {
        int res = UDPClient::init(DSX_SERVER, DSX_SERVER_PORT);
        if (res != UDPClient::Success) {
            std::cerr << "* UDPClient failed to initialize! (err: " <<
                        res << ")" << std::endl;
            return UDPClientInitError;
        }
        payload.instructions.clear();
        return Success;
    }

    void setGetDSXStatus(void)
    {
        addGetDSXStatus(payload, 0);
    }

    int sendPayload(void)
    {
        if (payload.instructions.empty())
            return EmptyPayloadError;

        std::string payloadStr = payload.toJson().dump();

        int res = UDPClient::send(payloadStr);
        if (res != UDPClient::Success) {
            std::cerr << "* UDPClient failed to send payload! (err: " <<
                        res << ")" << std::endl;
            return SendPayloadError;
        }
        return Success;
    }

    int terminate(void)
    {
        payload.instructions.clear();
        int res = UDPClient::terminate();
        if (res != UDPClient::Success) {
            std::cerr << "* UDPClient failed to terminate! (err: " <<
                        res << ")" << std::endl;
            return UDPClientTerminateError;
        }
        return Success;
    }

    void clearPayload(void)
    {
        payload.instructions.clear();
    }

    // NOTE: extras is an optional parameter (check DSX++.h for mor info)

    void setLeftTrigger(TriggerMode triggerMode, std::vector<int> extras)
    {
        addAdaptiveTriggerToPayload(payload, 0, Left, triggerMode, extras);
    }

    void setRightTrigger(TriggerMode triggerMode, std::vector<int> extras)
    {
        addAdaptiveTriggerToPayload(payload, 0, Right, triggerMode, extras);
    }

    void setLeftCustomTrigger(CustomTriggerValueMode customMode,
                                                    std::vector<int> extras)
    {
        addCustomAdaptiveTriggerToPayload(payload, 0, Left,
                                                    customMode, extras);
    }

    void setRightCustomTrigger(CustomTriggerValueMode customMode,
                                                    std::vector<int> extras)
    {
        addCustomAdaptiveTriggerToPayload(payload, 0, Right,
                                                    customMode, extras);
    }

    void setRGB(int red, int green, int blue, int brightness)
    {
        addRGBToPayload(payload, 0, red, green, blue,
                                                            brightness);
    }

    void setPlayerLED(PlayerLEDNewRevision playerLED)
    {
        addPlayerLEDToPayload(payload, 0, playerLED);
    }

    void setMicLED(MicLEDMode micLED)
    {
        addMicLEDToPayload(payload, 0, micLED);
    }

    void reset(void)
    {
        addResetToPayload(payload, 0);
    }

    // controllerIndex overloads

    void setLeftTrigger(int controllerIndex, TriggerMode triggerMode, std::vector<int> extras)
    {
        addAdaptiveTriggerToPayload(payload, controllerIndex, Left, triggerMode, extras);
    }

    void setRightTrigger(int controllerIndex, TriggerMode triggerMode, std::vector<int> extras)
    {
        addAdaptiveTriggerToPayload(payload, controllerIndex, Right, triggerMode, extras);
    }

    void setLeftCustomTrigger(int controllerIndex, CustomTriggerValueMode customMode,
                                                    std::vector<int> extras)
    {
        addCustomAdaptiveTriggerToPayload(payload, controllerIndex,
                                                    Left, customMode, extras);
    }
    void setRightCustomTrigger(int controllerIndex, CustomTriggerValueMode customMode,
                                                    std::vector<int> extras)
    {
        addCustomAdaptiveTriggerToPayload(payload, controllerIndex,
                                                    Right, customMode, extras);
    }
    void setRGB(int controllerIndex, int red, int green, int blue, int brightness)
    {
        addRGBToPayload(payload, controllerIndex, red, green, blue,
                                                            brightness);
    }

    void setPlayerLED(int controllerIndex, PlayerLEDNewRevision playerLED)
    {
        addPlayerLEDToPayload(payload, controllerIndex, playerLED);
    }

    void setMicLED(int controllerIndex, MicLEDMode micLED)
    {
        addMicLEDToPayload(payload, controllerIndex, micLED);
    }

    void reset(int controllerIndex)
    {
        addResetToPayload(payload, controllerIndex);
    }
}
