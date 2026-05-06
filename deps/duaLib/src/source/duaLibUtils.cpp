#include <duaLibUtils.hpp>
#include <triggerFactory.h>
#include <crc.h>
#include <algorithm>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#include <setupapi.h>
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "hid.lib")
#pragma comment(lib, "winmm.lib")
#include <initguid.h>
#include <devpkey.h>
#include <hidsdi.h>
#include <codecvt>
#include <locale>
#else
#include <clocale>
#include <cstdlib>
#endif

namespace duaLibUtils {

	void setPlayerLights(duaLibUtils::controller& controller, bool oldStyle) {
		switch (controller.playerIndex) {
			case 1:
				controller.dualsenseCurOutputState.PlayerLight1 = false;
				controller.dualsenseCurOutputState.PlayerLight2 = false;
				controller.dualsenseCurOutputState.PlayerLight3 = true;
				controller.dualsenseCurOutputState.PlayerLight4 = false;
				controller.dualsenseCurOutputState.PlayerLight5 = false;
				break;

			case 2:
				controller.dualsenseCurOutputState.PlayerLight1 = false;
				controller.dualsenseCurOutputState.PlayerLight2 = true; // Simplified, as oldStyle ? true : true is always true
				controller.dualsenseCurOutputState.PlayerLight3 = false;
				controller.dualsenseCurOutputState.PlayerLight4 = oldStyle ? true : false;
				controller.dualsenseCurOutputState.PlayerLight5 = false;
				break;

			case 3:
				controller.dualsenseCurOutputState.PlayerLight1 = true;
				controller.dualsenseCurOutputState.PlayerLight2 = false;
				controller.dualsenseCurOutputState.PlayerLight3 = true;
				controller.dualsenseCurOutputState.PlayerLight4 = false;
				controller.dualsenseCurOutputState.PlayerLight5 = oldStyle ? true : false;
				break;

			case 4:
				controller.dualsenseCurOutputState.PlayerLight1 = true;
				controller.dualsenseCurOutputState.PlayerLight2 = true;
				controller.dualsenseCurOutputState.PlayerLight3 = false;
				controller.dualsenseCurOutputState.PlayerLight4 = oldStyle ? true : false;
				controller.dualsenseCurOutputState.PlayerLight5 = oldStyle ? true : false;
				break;

			default:
				controller.dualsenseCurOutputState.PlayerLight1 = false;
				controller.dualsenseCurOutputState.PlayerLight2 = false;
				controller.dualsenseCurOutputState.PlayerLight3 = false;
				controller.dualsenseCurOutputState.PlayerLight4 = false;
				controller.dualsenseCurOutputState.PlayerLight5 = false;
				break;
		}
	}

	bool letGo(hid_device* handle, uint8_t deviceType, uint8_t connectionType) {
		if (handle && deviceType == DUALSENSE && (connectionType == HID_API_BUS_USB || connectionType == HID_API_BUS_UNKNOWN)) {
			dualsenseData::ReportOut02 data = {};
			data.ReportID = 0x02;

			data.State.ResetLights = true;
			data.State.AllowLedColor = false;
			data.State.AllowAudioControl = false;
			data.State.AllowAudioControl2 = false;
			data.State.AllowAudioMute = true;
			data.State.MicMute = false;
			data.State.AllowColorLightFadeAnimation = false;
			data.State.AllowHapticLowPassFilter = false;
			data.State.AllowHeadphoneVolume = false;
			data.State.AllowLightBrightnessChange = false;
			data.State.AllowMicVolume = false;
			data.State.AllowMotorPowerLevel = false;
			data.State.AllowMuteLight = false;
			data.State.AllowPlayerIndicators = false;
			data.State.AllowSpeakerVolume = false;
			data.State.UseRumbleNotHaptics = false;
			data.State.RumbleEmulationLeft = 0;
			data.State.RumbleEmulationRight = 0;

			data.State.AllowLeftTriggerFFB = true;
			data.State.AllowRightTriggerFFB = true;
			TriggerEffectGenerator::Off(data.State.LeftTriggerFFB, 0);
			TriggerEffectGenerator::Off(data.State.RightTriggerFFB, 0);

			uint8_t res = hid_write(handle, reinterpret_cast<unsigned char*>(&data), sizeof(data));
			return true;
		}
		else if (handle && deviceType == DUALSENSE && connectionType == HID_API_BUS_BLUETOOTH) {
			dualsenseData::ReportOut31 data = {};

			data.Data.ReportID = 0x31;
			data.Data.flag = 2;
			data.Data.State.ResetLights = true;
			uint32_t crc = compute(data.CRC.Buff, sizeof(data) - 4);
			data.CRC.CRC = crc;
			uint8_t res = hid_write(handle, reinterpret_cast<unsigned char*>(&data), sizeof(data));

			data.Data.State.ResetLights = false;
			data.Data.State.AllowLedColor = true;
			data.Data.State.AllowLeftTriggerFFB = true;
			data.Data.State.AllowRightTriggerFFB = true;
			data.Data.State.LedBlue = 255;
			TriggerEffectGenerator::Off(data.Data.State.LeftTriggerFFB, 0);
			TriggerEffectGenerator::Off(data.Data.State.RightTriggerFFB, 0);
			crc = compute(data.CRC.Buff, sizeof(data) - 4);
			data.CRC.CRC = crc;
			res = hid_write(handle, reinterpret_cast<unsigned char*>(&data), sizeof(data));

			return true;
		}
		if (handle && deviceType == DUALSHOCK4 && (connectionType == HID_API_BUS_USB || connectionType == HID_API_BUS_UNKNOWN)) {
			dualshock4Data::ReportIn05 report = {};
			report.ReportID = 0x05;
			report.State.UNK_RESET1 = true;
			report.State.UNK_RESET2 = true;
			report.State.EnableRumbleUpdate = true;
			report.State.RumbleLeft = 0;
			report.State.RumbleRight = 0;
			hid_write(handle, reinterpret_cast<unsigned char*>(&report), sizeof(report));

			dualshock4Data::ReportFeatureInDongleSetAudio audioSetting = {};
			audioSetting.ReportID = 0xE0;
			audioSetting.Output = dualshock4Data::AudioOutput::Disabled;
			hid_send_feature_report(handle, reinterpret_cast<unsigned char*>(&audioSetting), sizeof(audioSetting));

			return true;
		}
		else if (handle && deviceType == DUALSHOCK4 && connectionType == HID_API_BUS_BLUETOOTH) {
			dualshock4Data::ReportOut11 report = {};
			report.Data.ReportID = 0x11;
			report.Data.EnableHID = 1;
			report.Data.AllowRed = 1;
			report.Data.AllowGreen = 1;
			report.Data.AllowBlue = 1;
			report.Data.EnableAudio = 0;
			report.Data.State.LedRed = 50;
			report.Data.State.LedGreen = 50;
			report.Data.State.LedBlue = 50;
			report.Data.State.EnableLedUpdate = true;
			report.Data.State.EnableRumbleUpdate = true;
			report.Data.State.RumbleLeft = 0;
			report.Data.State.RumbleRight = 0;

			uint32_t crc = compute(report.CRC.Buff, sizeof(report) - 4);
			report.CRC.CRC = crc;

			int res = hid_write(handle, reinterpret_cast<unsigned char*>(&report), sizeof(report));
			return true;
		}

		return false;
	}

	bool getHardwareVersion(hid_device* handle, dualsenseData::ReportFeatureInVersion& report) {
		if (!handle) return false;

		unsigned char buffer[64] = { };
		buffer[0] = 0x20;
		int res = hid_get_feature_report(handle, buffer, sizeof(buffer));

		if (res > 0) {
			const auto versionReport = *reinterpret_cast<dualsenseData::ReportFeatureInVersion*>(buffer);
			report = versionReport;
			return true;
		}

		return false;
	}

	bool getMacAddress(hid_device* handle, std::string& outMac, uint32_t deviceId, uint8_t connectionType) {
		if (!handle) return false;

		if (deviceId == DUALSENSE_DEVICE_ID || deviceId == DUALSENSE_EDGE_DEVICE_ID) {
			unsigned char buffer[20] = {};
			buffer[0] = 0x09; // Report ID
			int res = hid_get_feature_report(handle, buffer, sizeof(buffer));

			if (res > 0) {
				const auto macReport = *reinterpret_cast<dualsenseData::ReportFeatureInMacAll*>(buffer);
				char tmp[18];
				snprintf(tmp, sizeof(tmp), "%02X:%02X:%02X:%02X:%02X:%02X",
						 macReport.ClientMac[5], macReport.ClientMac[4], macReport.ClientMac[3],
						 macReport.ClientMac[2], macReport.ClientMac[1], macReport.ClientMac[0]);
				outMac = tmp;
				return true;
			}
		}
		else if ((deviceId == DUALSHOCK4_DEVICE_ID || deviceId == DUALSHOCK4V2_DEVICE_ID || deviceId == DUALSHOCK4_WIRELESS_ADAPTOR_ID) && (connectionType == HID_API_BUS_USB || connectionType == HID_API_BUS_UNKNOWN)) {
			dualshock4Data::ReportFeatureInMacAll macReport = {};
			macReport.ReportID = 0x12;
			int res = hid_get_feature_report(handle, reinterpret_cast<unsigned char*>(&macReport), sizeof(macReport));

			if (res > 0) {
				char tmp[18];
				snprintf(tmp, sizeof(tmp), "%02X:%02X:%02X:%02X:%02X:%02X",
						 macReport.ClientMac[5], macReport.ClientMac[4], macReport.ClientMac[3],
						 macReport.ClientMac[2], macReport.ClientMac[1], macReport.ClientMac[0]);
				outMac = tmp;

				return true;
			}
		}
		else if ((deviceId == DUALSHOCK4_DEVICE_ID || deviceId == DUALSHOCK4V2_DEVICE_ID || deviceId == DUALSHOCK4_WIRELESS_ADAPTOR_ID) && connectionType == HID_API_BUS_BLUETOOTH) {
			dualshock4Data::ReportFeatureInMacAllBT macReport = {};
			macReport.Data.ReportID = 0x09;
			int res = hid_get_feature_report(handle, reinterpret_cast<unsigned char*>(&macReport), sizeof(macReport));

			if (res > 0) {
				char tmp[18];
				snprintf(tmp, sizeof(tmp), "%02X:%02X:%02X:%02X:%02X:%02X",
						 macReport.Data.ClientMac[5], macReport.Data.ClientMac[4], macReport.Data.ClientMac[3],
						 macReport.Data.ClientMac[2], macReport.Data.ClientMac[1], macReport.Data.ClientMac[0]);
				outMac = tmp;

				return true;
			}
		}

		return false;
	}

	bool isValid(hid_device* handle) {
		if (!handle) return false;
		unsigned char buf[1] = {};
		return hid_read(handle, buf, 1) != -1;
	}

#if defined(_WIN32) || defined(_WIN64)
	static std::wstring Utf8ToWide(const char* utf8) {
		int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, nullptr, 0);
		if (wlen <= 0) return L"";
		std::vector<wchar_t> buf(wlen);
		MultiByteToWideChar(CP_UTF8, 0, utf8, -1, buf.data(), wlen);
		return std::wstring(buf.data());
	}
#endif

	bool GetID(const char* narrowPath, const char** ID, uint32_t* size) {
	#if defined(_WIN32) || defined(_WIN64)
		GUID hidGuid;
		GUID outContainerId;
		HidD_GetHidGuid(&hidGuid);

		HDEVINFO devs = SetupDiGetClassDevs(
			&hidGuid,
			nullptr,
			nullptr,
			DIGCF_DEVICEINTERFACE | DIGCF_PRESENT
		);
		if (devs == INVALID_HANDLE_VALUE) {
			return false;
		}

		SP_DEVICE_INTERFACE_DATA ifData = { sizeof(ifData) };
		DWORD index = 0;
		std::wstring targetPath = Utf8ToWide(narrowPath);
		std::transform(targetPath.begin(), targetPath.end(), targetPath.begin(), ::tolower);

		while (SetupDiEnumDeviceInterfaces(devs, nullptr, &hidGuid, index++, &ifData)) {
			DWORD needed = 0;
			SetupDiGetDeviceInterfaceDetailW(devs, &ifData, nullptr, 0, &needed, nullptr);
			auto detailBuf = (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)malloc(needed);
			detailBuf->cbSize = sizeof(*detailBuf);
			SP_DEVINFO_DATA devInfo = { sizeof(devInfo) };

			if (SetupDiGetDeviceInterfaceDetailW(
				devs, &ifData,
				detailBuf, needed,
				nullptr,
				&devInfo
				)) {
				if (targetPath == detailBuf->DevicePath) {
					DEVPROPTYPE propType = 0;
					DWORD cb = sizeof(GUID);
					if (SetupDiGetDevicePropertyW(
						devs,
						&devInfo,
						&DEVPKEY_Device_ContainerId,
						&propType,
						reinterpret_cast<PBYTE>(&outContainerId),
						cb,
						&cb,
						0
						)) {
						free(detailBuf);
						SetupDiDestroyDeviceInfoList(devs);

						wchar_t guidStr[39] = {};
						StringFromGUID2(outContainerId, guidStr, _countof(guidStr));

						*size = sizeof(guidStr);
						static char buffer[39] = {};
						std::wcstombs(buffer, guidStr, sizeof(buffer));
						std::transform(buffer, buffer + std::strlen(buffer), buffer, [](unsigned char c) {return std::tolower(c); });
						*ID = buffer;

						return true;
					}
				}
			}
			free(detailBuf);
		}

		SetupDiDestroyDeviceInfoList(devs);
	#endif
		return false;
	}
}