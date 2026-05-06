#pragma once
#include <dataStructures.h>
#include <mutex>
#include <hidapi.h>
#include <shared_mutex>
#include <string>
#include <cstdint>
#include <duaLib.h>
#include <vector>

#define UNKNOWN 0
#define DUALSHOCK4 1
#define DUALSENSE 2
#define DEVICE_COUNT 4
#define MAX_CONTROLLER_COUNT 4
#define VENDOR_ID 0x54c
#define DUALSENSE_DEVICE_ID 0x0ce6
#define DUALSENSE_EDGE_DEVICE_ID 0x0df2
#define DUALSHOCK4_DEVICE_ID 0x05c4
#define DUALSHOCK4V2_DEVICE_ID 0x09cc
#define DUALSHOCK4_WIRELESS_ADAPTOR_ID 0xba0

namespace duaLibUtils {
	struct trigger {
		uint8_t force[11] = {};
	};

	struct controller {
		std::shared_mutex lock{};
		hid_device* handle = 0;
		uint32_t sceHandle = 0;
		uint8_t playerIndex = 0;
		uint8_t deviceType = UNKNOWN;
		uint16_t productID = 0;
		uint8_t seqNo = 0;
		uint8_t connectionType = 0;
		bool opened = false;
		bool isMicMuted = false;
		bool wasDisconnected = false;
		bool valid = false;
		uint32_t failedReadCount = 0;
		dualsenseData::USBGetStateData dualsenseCurInputState = {};
		dualsenseData::SetStateData dualsenseLastOutputState = {};
		dualsenseData::SetStateData dualsenseCurOutputState = {};
		dualsenseData::ReportFeatureInVersion versionReport = {};
		dualshock4Data::USBGetStateData dualshock4CurInputState = {};
		dualshock4Data::BTSetStateData dualshock4LastOutputState = {};
		dualshock4Data::BTSetStateData dualshock4CurOutputState = {};
		dualshock4Data::ReportFeatureInDongleSetAudio dualshock4CurAudio = { 0xE0, 0, dualshock4Data::AudioOutput::Disabled };
		dualshock4Data::ReportFeatureInDongleSetAudio dualshock4LastAudio = { 0xE0, 0, dualshock4Data::AudioOutput::Speaker };
		std::string macAddress = "";
		std::string systemIdentifier = "";
		std::string lastPath = "";
		std::string id = "";
		uint32_t idSize = 0;
		trigger L2 = {};
		trigger R2 = {};
		uint8_t triggerMask = 0;
		uint32_t lastSensorTimestamp = 0;
		bool velocityDeadband = false;
		bool motionSensorState = true;
		bool tiltCorrection = false;
		s_SceFQuaternion orientation = { 0.0f,0.0f,0.0f,1.0f };
		s_SceFVector3 lastAcceleration = { 0.0f,0.0f,0.0f };
		float eInt[3] = { 0.0f, 0.0f, 0.0f };
		std::chrono::steady_clock::time_point lastUpdate = {};
		float deltaTime = 0.0f;
		uint8_t touch1Count = 0;
		uint8_t touch2Count = 0;
		uint8_t touch1LastCount = 0;
		uint8_t touch2LastCount = 0;
		uint8_t touch1LastIndex = 0;
		uint8_t touch2LastIndex = 0;
		bool started = false;
	};

    void setPlayerLights(duaLibUtils::controller& controller, bool oldStyle);
    bool letGo(hid_device* handle, uint8_t deviceType, uint8_t connectionType);
    bool getHardwareVersion(hid_device* handle, dualsenseData::ReportFeatureInVersion& report);
    bool getMacAddress(hid_device* handle, std::string& outMac, uint32_t deviceId, uint8_t connectionType);
    bool isValid(hid_device* handle);
    bool GetID(const char* narrowPath, const char** ID, uint32_t* size);
}