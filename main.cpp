#include "openvr.h"

#include <atomic>
#include <iostream>
#include <string>
#include <sstream>

#include <thread>
#include <mutex>

#include <filesystem>
#include <vector>

int main(int argc, char *argv[]) {
    vr::EVRInitError err = vr::VRInitError_None;
    vr::VR_Init(&err, vr::VRApplication_Scene);

    if (err != vr::VRInitError_None) {
        std::cout << "failed to init openvr! " << vr::VR_GetVRInitErrorAsEnglishDescription(err) << std::endl;
        return 1;
    }

    std::filesystem::path cwd = std::filesystem::current_path() / "actions.json";

    vr::EVRInputError inputError;
    inputError = vr::VRInput()->SetActionManifestPath(cwd.string().c_str());

    if(inputError != vr::VRInputError_None) {
        std::cout << "action manifest error " << inputError << std::endl;
    }

    vr::VRActionSetHandle_t actionSet;
    vr::VRInput()->GetActionSetHandle("/actions/default", &actionSet);

    vr::VRActionHandle_t handleTrigger;
    vr::VRInput()->GetActionHandle("/actions/default/in/trigger", &handleTrigger);

    vr::VRActionHandle_t handleGrip;
    vr::VRInput()->GetActionHandle("/actions/default/in/grip", &handleGrip);
    
    vr::VRInputValueHandle_t userChest;
    vr::VRInput()->GetInputSourceHandle("/user/chest", &userChest);

    while (true) {
        vr::VREvent_t vEvent;
        while (vr::VRSystem()->PollNextEvent(&vEvent, sizeof(vr::VREvent_t))) {
            std::cout << "Event: " << vr::VRSystem()->GetEventTypeNameFromEnum((vr::EVREventType)vEvent.eventType) << std::endl;
        }

        vr::VRActiveActionSet_t activeActionSet = { 0 };
        activeActionSet.ulActionSet = actionSet;
        if (vr::EVRInputError err = vr::VRInput()->UpdateActionState(&activeActionSet, sizeof(activeActionSet), 1)) {
            std::cout << "UpdateActionState error: " << err << std::endl;
        }

        vr::InputDigitalActionData_t triggerData{};
        if (vr::EVRInputError err = vr::VRInput()->GetDigitalActionData(handleTrigger, &triggerData, sizeof(vr::InputDigitalActionData_t), vr::k_ulInvalidInputValueHandle)) {
            std::cout << "Trigger GetDigitalActionData error: " << err << std::endl;
        }

        std::cout << "Trigger state: " << (triggerData.bState ? "On" : "Off") << std::endl;

        vr::InputDigitalActionData_t gripData{};
        if (vr::EVRInputError err = vr::VRInput()->GetDigitalActionData(handleGrip, &gripData, sizeof(vr::InputDigitalActionData_t), vr::k_ulInvalidInputValueHandle)) {
            std::cout << "Grip GetDigitalActionData error: " << err << std::endl;
        }

        std::cout << "Grip state: " << (gripData.bState ? "On" : "Off") << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}
