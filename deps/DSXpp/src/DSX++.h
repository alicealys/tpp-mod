/*
 * Copyright (C) 2024 Thanasis Petsas <thanpetsas@gmail.com>
 * Licence: MIT Licence
 */

#ifndef DSXPP_H
#define DSXPP_H

#include "DSXProtocol.h"

#include <string>
#include <vector>


namespace DSX {

    enum ErrorCode {
        UDPClientInitError = 1,
        UDPClientTerminateError,
        EmptyPayloadError,
        SendPayloadError
    };

    extern const int Success;

    /**
     * Initializes a DSX++ client for sending dualsense features requests
     * @return 0 if the client was successfully initialized,
     *         or an error code otherwise (see enum class ErrorCode)
     */
    int init(void);

    /**
     * Sets a request to get the DSX status (list of devices) to the current
     * controller's payload
     */
    void setGetDSXStatus(void);

    /**
     * Initializes a DSX++ client
     * @return 0 if the client was successfully terminated,
     *         or an error code otherwise (see enum class ErrorCode)
     */
    int terminate(void);

    /**
     * Clears the current payload of instruction of DSX++ client so
     * that a new set of instructions can be added
     */
    void clearPayload(void);

    /**
     * Sends the currently constucted payload of instructions to DSX server
     * in order to be applied to the controller
     */
    int sendPayload(void);

    // Main DSX++ API

    // NOTE: The controller index used in the following functions is always 0.
    // There is a set of overloads at the bottom that let you specify the
    // desired controllerIndex if needed.
    //
    // For details about setting fields like extras, brightnetss, etc.,
    // check the examples here:
    // https://github.com/Paliverse/DSX/tree/main/Mod%20System%20(DSX%20v3)#adaptive-triggers

    /**
    * Sets an adaptive trigger mode to the left trigger (i.e., L2)
    * @param triggerMode The mode to set for the adaptive trigger
    * @param extras (optional) Additional parameters required by the trigger
    */
    void setLeftTrigger(TriggerMode triggerMode, std::vector<int> extras={});

    /**
    * Sets an adaptive trigger mode to the right trigger (i.e., R2)
    * @param   triggerMode The mode to set for the adaptive trigger
    * @param   extras (optional) Additional parameters required by the trigger
    */
    void setRightTrigger(TriggerMode triggerMode, std::vector<int> extras={});

    /**
     * Sets an custom adaptive trigger mode to the left trigger (i.e., L2)
     *
     * NOTE: This allows for more complex trigger configurations, including a
     * custom value mode and additional parameters
     *
     * @param customMode   The custom value mode for more detailed trigger
     * control
     * @param extras   Additional parameters required by the custom trigger
     * mode
     */
    void setLeftCustomTrigger(CustomTriggerValueMode customMode,
                                                    std::vector<int> extras);
    /**
     * Sets an custom adaptive trigger mode to the right trigger (i.e., R2)
     *
     * NOTE: This allows for more complex trigger configurations, including a
     * custom value mode and additional parameters
     *
     * @param customMode   The custom value mode for more detailed trigger
     * control
     * @param extras   Additional parameters required by the custom trigger
     * mode
     */
    void setRightCustomTrigger(CustomTriggerValueMode customMode,
                                                    std::vector<int> extras);
    /**
     * Sets an RGB LEDs update to the current controller's payload
     * @param red     The red (R) component of the color
     * @param green   The green (G) component of the color
     * @param blue    The blue (B) component of the color
     * @param brightness    The brightness level of the LEDs
     */
    void setRGB(int red, int green, int blue, int brightness);

    /**
     * Sets a player LED indicative color update to the current controller's
     * payload
     * playerLED   player LED configuration to apply
     */
    void setPlayerLED(PlayerLEDNewRevision playerLED);

    /**
     * Sets a microphone LED update (mute/unmute) to the current controller's
     * payload
     * micLED  The microphone LED configuration to apply
     */
    void setMicLED(MicLEDMode micLED);

    /**
     * Resets the controller's setting to user's predefined settings configured
     * in the DSX app for the current controller's payload
     */
     void reset(void);

    // controllerIndex overloads

    /**
    * Sets an adaptive trigger mode to the left trigger (i.e., L2)
    * @param controllerIndex    The index of the controller to receive the trigger
    * @param triggerMode The mode to set for the adaptive trigger
    * @param extras (optional) Additional parameters required by the trigger
    */
    void setLeftTrigger(int controllerIndex, TriggerMode triggerMode, std::vector<int> extras={});

    /**
    * Sets an adaptive trigger mode to the right trigger (i.e., R2)
    * @param controllerIndex    The index of the controller to receive the trigger
    * @param   triggerMode The mode to set for the adaptive trigger
    * @param   extras (optional) Additional parameters required by the trigger
    */
    void setRightTrigger(int controllerIndex, TriggerMode triggerMode, std::vector<int> extras={});

    /**
     * Sets an custom adaptive trigger mode to the left trigger (i.e., L2)
     *
     * NOTE: This allows for more complex trigger configurations, including a
     * custom value mode and additional parameters
     *
     * @param controllerIndex    The index of the controller to receive the trigger
     * @param customMode   The custom value mode for more detailed trigger
     * control
     * @param extras   Additional parameters required by the custom trigger
     * mode
     */
    void setLeftCustomTrigger(int controllerIndex, CustomTriggerValueMode customMode,
                                                    std::vector<int> extras);
    /**
     * Sets an custom adaptive trigger mode to the right trigger (i.e., R2)
     *
     * NOTE: This allows for more complex trigger configurations, including a
     * custom value mode and additional parameters
     * @param controllerIndex    The index of the controller to receive the trigger
     * @param customMode   The custom value mode for more detailed trigger
     * control
     * @param extras   Additional parameters required by the custom trigger
     * mode
     */
    void setRightCustomTrigger(int controllerIndex, CustomTriggerValueMode customMode,
                                                    std::vector<int> extras);
    /**
     * Sets an RGB LEDs update to the current controller's payload
     * @param controllerIndex    The index of the controller to receive the trigger
     * @param red     The red (R) component of the color
     * @param green   The green (G) component of the color
     * @param blue    The blue (B) component of the color
     * @param brightness    The brightness level of the LEDs
     */
    void setRGB(int controllerIndex, int red, int green, int blue, int brightness);

    /**
     * Sets a player LED indicative color update to the current controller's
     * payload
     * @param controllerIndex    The index of the controller to receive the trigger
     * @param playerLED   player LED configuration to apply
     */
    void setPlayerLED(int controllerIndex, PlayerLEDNewRevision playerLED);

    /**
     * Sets a microphone LED update (mute/unmute) to the current controller's
     * payload
     * @param controllerIndex    The index of the controller to receive the trigger
     * @param micLED  The microphone LED configuration to apply
     */
    void setMicLED(int controllerIndex, MicLEDMode micLED);

    /**
     * Resets the controller's setting to user's predefined settings configured
     * in the DSX app for the current controller's payload
     * @param controllerIndex    The index of the controller to receive the trigger
     */
     void reset(int controllerIndex);
}

#endif // DSXPP_H
