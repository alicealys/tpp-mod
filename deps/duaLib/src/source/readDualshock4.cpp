#include <duaLib.h>
#include <duaLibUtils.hpp>
#include <crc.h>
#include <readDualshock4.hpp>
#include <algorithm>

bool ReadDualshock4(duaLibUtils::controller &controller)
{
    bool isBt = controller.connectionType == HID_API_BUS_BLUETOOTH ? true : false;

    dualshock4Data::ReportIn01USB inputUsb = {};
    dualshock4Data::ReportIn01BT inputBt = {};

    int32_t res = -1;

    if (isBt)
        res = hid_read_timeout(controller.handle, reinterpret_cast<unsigned char *>(&inputBt), sizeof(inputBt), 0);
    else
        res = hid_read_timeout(controller.handle, reinterpret_cast<unsigned char *>(&inputUsb), sizeof(inputUsb), 0);

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

        if (controller.dualshock4CurOutputState.LedRed != controller.dualshock4LastOutputState.LedRed ||
            controller.dualshock4CurOutputState.LedGreen != controller.dualshock4LastOutputState.LedGreen ||
            controller.dualshock4CurOutputState.LedBlue != controller.dualshock4LastOutputState.LedBlue ||
            controller.wasDisconnected)
        {
            controller.dualshock4CurOutputState.EnableLedUpdate = true;
        }
        else
        {
            controller.dualshock4CurOutputState.EnableLedUpdate = true;
        }

        if (controller.dualshock4CurAudio.Output != controller.dualshock4LastAudio.Output || controller.wasDisconnected)
        {
            controller.dualshock4CurAudio.ReportID = 0xE0;
            hid_send_feature_report(controller.handle, reinterpret_cast<unsigned char *>(&controller.dualshock4CurAudio), sizeof(controller.dualshock4CurAudio));
            controller.dualshock4LastAudio.Output = controller.dualshock4CurAudio.Output;
        }

        if (controller.dualshock4CurOutputState.VolumeSpeaker != controller.dualshock4LastOutputState.VolumeSpeaker || controller.wasDisconnected)
            controller.dualshock4CurOutputState.EnableVolumeSpeakerUpdate = true;
        else
            controller.dualshock4CurOutputState.EnableVolumeSpeakerUpdate = false;

        if (controller.dualshock4CurOutputState.VolumeMic != controller.dualshock4LastOutputState.VolumeMic || controller.wasDisconnected)
            controller.dualshock4CurOutputState.EnableVolumeMicUpdate = true;
        else
            controller.dualshock4CurOutputState.EnableVolumeMicUpdate = false;

        if (controller.dualshock4CurOutputState.VolumeLeft != controller.dualshock4LastOutputState.VolumeLeft || controller.wasDisconnected)
            controller.dualshock4CurOutputState.EnableVolumeLeftUpdate = true;
        else
            controller.dualshock4CurOutputState.EnableVolumeLeftUpdate = false;

        if (controller.dualshock4CurOutputState.VolumeRight != controller.dualshock4LastOutputState.VolumeRight || controller.wasDisconnected)
            controller.dualshock4CurOutputState.EnableVolumeRightUpdate = true;
        else
            controller.dualshock4CurOutputState.EnableVolumeRightUpdate = false;

        if ((controller.dualshock4CurOutputState.RumbleLeft != controller.dualshock4LastOutputState.RumbleLeft) || (controller.dualshock4CurOutputState.RumbleRight != controller.dualshock4LastOutputState.RumbleRight) || controller.wasDisconnected)
            controller.dualshock4CurOutputState.EnableRumbleUpdate = true;
        else
            controller.dualshock4CurOutputState.EnableRumbleUpdate = false;

        controller.triggerMask = 0;
        res = -1;

        if (controller.connectionType == HID_API_BUS_USB || controller.connectionType == HID_API_BUS_UNKNOWN)
        {
            dualshock4Data::ReportIn05 usbOutput = {};

            usbOutput.ReportID = 0x05;
            usbOutput.State = controller.dualshock4CurOutputState;

            if ((controller.dualshock4CurOutputState != controller.dualshock4LastOutputState) || controller.wasDisconnected)
            {
                res = hid_write(controller.handle, reinterpret_cast<unsigned char *>(&usbOutput), sizeof(usbOutput));
            }
        }
        else if (controller.connectionType == HID_API_BUS_BLUETOOTH)
        {
            dualshock4Data::ReportOut11 report = {};
            report.Data.ReportID = 0x11;
            report.Data.EnableHID = 1;
            report.Data.AllowRed = controller.dualshock4CurOutputState.LedRed > 0 ? 1 : 0;
            report.Data.AllowGreen = controller.dualshock4CurOutputState.LedGreen > 0 ? 1 : 0;
            report.Data.AllowBlue = controller.dualshock4CurOutputState.LedBlue > 0 ? 1 : 0;
            report.Data.EnableAudio = 0;
            report.Data.State = controller.dualshock4CurOutputState;

            uint32_t crc = compute(report.CRC.Buff, sizeof(report) - 4);
            report.CRC.CRC = crc;

            int res = hid_write(controller.handle, reinterpret_cast<unsigned char *>(&report), sizeof(report));
        }

        if (res > 0)
        {
            controller.wasDisconnected = false;
            // std::cout << "Controller idx " << controller.sceHandle << " path=" << controller.macAddress << " connType=" << (int)controller.connectionType << std::endl;
        }

        {
            std::shared_lock guard(controller.lock);
            controller.dualshock4LastOutputState = controller.dualshock4CurOutputState;
            controller.dualshock4CurInputState = isBt ? inputBt.State : inputUsb.State;
        }
    }

    return true;
}