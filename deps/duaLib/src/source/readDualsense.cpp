#include <duaLib.h>
#include <duaLibUtils.hpp>
#include <crc.h>
#include <readDualsense.hpp>
#include <algorithm>

bool ReadDualsense(duaLibUtils::controller& controller)
{
    bool isBt = controller.connectionType == HID_API_BUS_BLUETOOTH ? true : false;

    dualsenseData::ReportIn01USB inputUsb = {};
    dualsenseData::ReportIn31 inputBt = {};

    int32_t res = -1;

    if (isBt)
        res = hid_read_timeout(controller.handle, reinterpret_cast<unsigned char *>(&inputBt), sizeof(inputBt), 0);
    else
        res = hid_read_timeout(controller.handle, reinterpret_cast<unsigned char *>(&inputUsb), sizeof(inputUsb), 0);

    dualsenseData::USBGetStateData inputData = isBt ? inputBt.Data.State.StateData : inputUsb.State;

    if (controller.failedReadCount >= 15)
    {
        controller.valid = false;
    }

    if (res == -1)
    {
        controller.failedReadCount++;
        return false;
    }
    else if (res > 0)
    {
        controller.failedReadCount = 0;

        if (!inputData.ButtonMute && controller.dualsenseCurInputState.ButtonMute)
        {
            controller.dualsenseCurOutputState.AllowAudioMute = true;
            controller.isMicMuted = !controller.isMicMuted;
            controller.dualsenseCurOutputState.MuteLightMode = controller.isMicMuted ? dualsenseData::MuteLight::On : dualsenseData::MuteLight::Off;
            controller.dualsenseCurOutputState.MicMute = controller.isMicMuted;
            controller.dualsenseCurOutputState.AllowMuteLight = true;
        }
        else
        {
            controller.dualsenseCurOutputState.AllowAudioMute = false;
            controller.dualsenseCurOutputState.AllowMuteLight = false;
        }

        if (controller.dualsenseCurOutputState.LedRed != controller.dualsenseLastOutputState.LedRed ||
            controller.dualsenseCurOutputState.LedGreen != controller.dualsenseLastOutputState.LedGreen ||
            controller.dualsenseCurOutputState.LedBlue != controller.dualsenseLastOutputState.LedBlue ||
            controller.wasDisconnected)
        {
            controller.dualsenseCurOutputState.AllowLedColor = true;
        }
        else
        {
            controller.dualsenseCurOutputState.AllowLedColor = false;
        }

        bool oldStyle = ((controller.versionReport.HardwareInfo & 0x00FFFF00) < 0x00000400);
        duaLibUtils::setPlayerLights(controller, oldStyle);

        if (controller.dualsenseCurOutputState.lightBrightness != controller.dualsenseLastOutputState.lightBrightness || controller.wasDisconnected)
        {
            controller.dualsenseCurOutputState.AllowLightBrightnessChange = true;
        }
        else
        {
            controller.dualsenseCurOutputState.AllowLightBrightnessChange = false;
        }

        if ((controller.dualsenseCurOutputState.PlayerLight1 != controller.dualsenseLastOutputState.PlayerLight1 ||
             controller.dualsenseCurOutputState.PlayerLight2 != controller.dualsenseLastOutputState.PlayerLight2 ||
             controller.dualsenseCurOutputState.PlayerLight3 != controller.dualsenseLastOutputState.PlayerLight3 ||
             controller.dualsenseCurOutputState.PlayerLight4 != controller.dualsenseLastOutputState.PlayerLight4) ||
            controller.wasDisconnected)
        {
            controller.dualsenseCurOutputState.AllowPlayerIndicators = true;
        }
        else
        {
            controller.dualsenseCurOutputState.AllowPlayerIndicators = true; // keep on true because it doesn't always light up
        }

        if (controller.wasDisconnected)
        {
            controller.dualsenseCurOutputState.MicMute = controller.isMicMuted;
            controller.dualsenseCurOutputState.AllowMuteLight = true;
        }

        if (controller.dualsenseCurOutputState.OutputPathSelect != controller.dualsenseLastOutputState.OutputPathSelect ||
            controller.wasDisconnected)
        {
            controller.dualsenseCurOutputState.AllowAudioControl = true;
        }
        else
        {
            controller.dualsenseCurOutputState.AllowAudioControl = false;
        }

        if (controller.dualsenseCurOutputState.VolumeSpeaker != controller.dualsenseLastOutputState.VolumeSpeaker ||
            controller.wasDisconnected)
        {
            controller.dualsenseCurOutputState.AllowSpeakerVolume = true;
        }
        else
        {
            controller.dualsenseCurOutputState.AllowSpeakerVolume = false;
        }

        if (controller.dualsenseCurOutputState.VolumeMic != controller.dualsenseLastOutputState.VolumeMic ||
            controller.wasDisconnected)
        {
            controller.dualsenseCurOutputState.AllowMicVolume = true;
        }
        else
        {
            controller.dualsenseCurOutputState.AllowMicVolume = false;
        }

        if (controller.dualsenseCurOutputState.VolumeHeadphones != controller.dualsenseLastOutputState.VolumeHeadphones ||
            controller.wasDisconnected)
        {
            controller.dualsenseCurOutputState.AllowHeadphoneVolume = true;
        }
        else
        {
            controller.dualsenseCurOutputState.AllowHeadphoneVolume = false;
        }

        if (controller.triggerMask & SCE_PAD_TRIGGER_EFFECT_TRIGGER_MASK_L2 || controller.wasDisconnected)
        {
            controller.dualsenseCurOutputState.AllowLeftTriggerFFB = true;
            for (int i = 0; i < 11; i++)
            {
                controller.dualsenseCurOutputState.LeftTriggerFFB[i] = controller.L2.force[i];
            }
        }
        else
        {
            controller.dualsenseCurOutputState.AllowLeftTriggerFFB = false;
        }

        if (controller.triggerMask & SCE_PAD_TRIGGER_EFFECT_TRIGGER_MASK_R2 || controller.wasDisconnected)
        {
            controller.dualsenseCurOutputState.AllowRightTriggerFFB = true;
            for (int i = 0; i < 11; i++)
            {
                controller.dualsenseCurOutputState.RightTriggerFFB[i] = controller.R2.force[i];
            }
        }
        else
        {
            controller.dualsenseCurOutputState.AllowRightTriggerFFB = false;
        }

        controller.dualsenseCurOutputState.HostTimestamp = controller.dualsenseCurInputState.SensorTimestamp;
        controller.triggerMask = 0;
        res = -1;

        if (controller.connectionType == HID_API_BUS_USB || controller.connectionType == HID_API_BUS_UNKNOWN)
        {
            dualsenseData::ReportOut02 usbOutput = {};

            usbOutput.ReportID = 0x02;
            usbOutput.State = controller.dualsenseCurOutputState;

            if ((controller.dualsenseCurOutputState != controller.dualsenseLastOutputState) || controller.wasDisconnected)
            {
                res = hid_write(controller.handle, reinterpret_cast<unsigned char *>(&usbOutput), sizeof(usbOutput));
            }
        }
        else if (controller.connectionType == HID_API_BUS_BLUETOOTH)
        {
            dualsenseData::ReportOut31 btOutput = {};

            btOutput.Data.ReportID = 0x31;
            btOutput.Data.flag = 2;
            btOutput.Data.State = controller.dualsenseCurOutputState;

            uint32_t crc = compute(btOutput.CRC.Buff, sizeof(btOutput) - 4);
            btOutput.CRC.CRC = crc;
            if ((controller.dualsenseCurOutputState != controller.dualsenseLastOutputState) || controller.wasDisconnected)
            {
                res = hid_write(controller.handle, reinterpret_cast<unsigned char *>(&btOutput), sizeof(btOutput));
            }
        }

        if (res > 0)
        {
            std::shared_lock guard(controller.lock);
            controller.wasDisconnected = false;
            // std::cout << "Controller idx " << controller.sceHandle << " path=" << controller.macAddress << " connType=" << (int)controller.connectionType << std::endl;
        }

        {
            std::shared_lock guard(controller.lock);
            controller.dualsenseLastOutputState = controller.dualsenseCurOutputState;
            controller.dualsenseCurInputState = inputData;
        }
    }

    return true;
}