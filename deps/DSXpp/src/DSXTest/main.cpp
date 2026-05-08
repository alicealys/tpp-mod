/*
 * Copyright (C) 2024 Thanasis Petsas <thanpetsas@gmail.com>
 * Licence: MIT Licence
 */

#include <iostream>
#include "DSX++.h"

#include <string>  // for basic_string, string, allocator
#include <vector>  // for vector
#include <unordered_map>

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Dropdown, Horizontal, Vertical
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/component/component_options.hpp"   // for ButtonOption
#include "ftxui/dom/elements.hpp"  // for gauge, separator, text, vbox, operator|, Element, border
#include "ftxui/screen/color.hpp"  // for Color, Color::Blue, Color::Green, Color::Red

using namespace ftxui;

const std::vector<std::string> modes = {
    "Normal",
    "GameCube",
    "VerySoft",
    "Soft",
    "Hard",
    "VeryHard",
    "Hardest",
    "Rigid",
    "VibrateTrigger",
    "Choppy",
    "Medium",
    "VibrateTriggerPulse",
    "CustomTriggerValue",
    "Resistance",
    "Bow",
    "Galloping",
    "SemiAutomaticGun",
    "AutomaticGun",
    "Machine",

    // DSX V3.1+

    "VibrateTrigger10Hz",
    "TriggerOff",
    "Feedback",
    "Weapon",
    "Vibration",
    "SlopeFeedback",
    "MultiplePositionFeedback",
    "MultiplePositionVibration",
};


const std::vector<std::string> customModes  = {
    "OFF",
    "Rigid",
    "RigidA",
    "RigidB",
    "RigidAB",
    "Pulse",
    "PulseA",
    "PulseB",
    "PulseAB",
    "VibrateResistance",
    "VibrateResistanceA",
    "VibrateResistanceB",
    "VibrateResistanceAB",
    "VibratePulse",
    "VibratePulseA",
    "VibratePulsB",
    "VibratePulseAB",
 };

struct Extra
{
    const std::string name;
    Component component;
    int &value;

    Extra(std::string name, Component component, int &value) :
    name(name), component(component), value(value) {}
};

#define DEFAULT_FORCE 128
#define FORCES 7

#define DEFAULT_REGION 4
#define REGIONS 10

struct TriggerContext
{
    std::string name;
    int freq = 100;
    int start = 4;
    int end = 4;
    int force = 4;
    int snapforce = 4;
    int customMode = 0;

    std::array<int, FORCES> forces;
    std::vector<Component> forceSliders;

    int startFoot = 4;
    int secondFoot = 4;

    int strengthA = 3;
    int strengthB = 3;

    // v3.1+
    int startPosition = 5;
    int resistanceStrength = 4;

    int weaponStart = 3;
    int weaponEnd = 5;
    int amplitude = 4;
    int vibrationFreq = 20;

    int slopeStart = 4;
    int slopeEnd = 5;
    int slopeResistanceStart = 4;
    int slopeResistanceEnd = 4;

    int triggerMode = 0;

    std::array<int, REGIONS> regions;
    std::vector<Component> regionSliders;

    Component menu;
    Component frequencySlider;
    Component startSlider;
    Component endSlider;
    Component startFootSlider;
    Component secondFootSlider;
    Component forceSlider;
    Component snapforceSlider;
    Component strengthASlider;
    Component strengthBSlider;
    Component startPositionSlider;
    Component resistanceStrengthSlider;
    Component weaponStartSlider;
    Component weaponEndSlider;
    Component amplitudeSlider;
    Component vibrationFreqSlider;
    Component slopeStartSlider;
    Component slopeEndSlider;
    Component slopeResistanceStartSlider;
    Component slopeResistanceEndSlider;

    std::unordered_map<std::string, std::vector<Extra>> modeExtras;

    std::vector<Component> components;
    Component extrasContainer;
    Component extrasWindow;
};

// reset trigger extras to a value near the middle of their range
void resetTriggerContext(TriggerContext &ctx)
{
    ctx.freq = 100;
    ctx.start = 4;
    ctx.end = 4;
    ctx.force = 4;
    ctx.snapforce = 4;
    ctx.customMode = 0;
    ctx.startFoot = 4;
    ctx.secondFoot = 4;
    ctx.strengthA = 3;
    ctx.strengthB = 3;
    ctx.startPosition = 5;
    ctx.resistanceStrength = 4;
    ctx.weaponStart = 3;
    ctx.weaponEnd = 5;
    ctx.amplitude = 4;
    ctx.vibrationFreq = 20;
    ctx.slopeStart = 4;
    ctx.slopeEnd = 5;
    ctx.slopeResistanceStart = 4;
    ctx.slopeResistanceEnd = 4;
    ctx.triggerMode = 0;
    ctx.forces.fill(DEFAULT_FORCE);
    ctx.regions.fill(DEFAULT_REGION);

}

bool currentModeHasExtras(TriggerContext ctx)
{
    return ctx.modeExtras.find( modes[ctx.triggerMode] ) != ctx.modeExtras.end();
}

void resetTriggers(TriggerContext &L2, TriggerContext &R2)
{
    resetTriggerContext(L2);
    resetTriggerContext(R2);
    DSX::clearPayload();
}

std::vector<int> getExtras(TriggerContext &ctx)
{
    std::vector<int> extras;
    if (!currentModeHasExtras(ctx))
        return extras;
    auto extrasVector = ctx.modeExtras.at( modes[ctx.triggerMode] );
    for (const auto &extra : extrasVector) {
        extras.push_back(extra.value);
    }
    return extras;
}

void sendTriggers(TriggerContext &L2, TriggerContext &R2)
{
    DSX::clearPayload();

    if (modes[L2.triggerMode] == "CustomTriggerValue")
        DSX::setLeftCustomTrigger((CustomTriggerValueMode) L2.customMode, getExtras(L2));
    else
        DSX::setLeftTrigger((TriggerMode) L2.triggerMode, getExtras(L2));

    if (modes[R2.triggerMode] == "CustomTriggerValue")
        DSX::setRightCustomTrigger((CustomTriggerValueMode) R2.customMode, getExtras(R2));
    else
        DSX::setRightTrigger((TriggerMode) R2.triggerMode, getExtras(R2));

    if (DSX::sendPayload() != DSX::Success)
        std::cerr << "* DSX++ client failed to send data!" << std::endl;
}

void setupTriggerContext(TriggerContext &ctx, std::string name)
{
    ctx.name = name;
    ctx.frequencySlider = Slider("Frequency: ", &ctx.freq, 0, 256, 1);
    ctx.startSlider = Slider("Start: ", &ctx.start, 0, 8, 1);
    ctx.endSlider = Slider("End: ", &ctx.end, 0, 8, 1);
    ctx.startFootSlider = Slider("StartFoot: ", &ctx.startFoot, 0, 8, 1);
    ctx.secondFootSlider = Slider("secondFoot: ", &ctx.secondFoot, 0, 8, 1);
    ctx.forceSlider = Slider("Force: ", &ctx.force, 0, 8, 1);
    ctx.snapforceSlider = Slider("SnapForce: ", &ctx.snapforce, 0, 8, 1);
    ctx.strengthASlider = Slider("Strength A: ", &ctx.strengthA, 0, 7, 1);
    ctx.strengthBSlider = Slider("Strength B: ", &ctx.strengthB, 0, 7, 1);
    ctx.startPositionSlider = Slider("StartPos: ", &ctx.startPosition, 1, 9, 1);
    ctx.resistanceStrengthSlider = Slider("Resistance: ",
                                            &ctx.resistanceStrength, 1, 8, 1);
    ctx.weaponStartSlider = Slider("Start: ", &ctx.weaponStart, 2, 7, 1);
    ctx.weaponEndSlider = Slider("End: ", &ctx.weaponEnd, 3, 8, 1);
    ctx.amplitudeSlider = Slider("Amplitude: ", &ctx.amplitude, 1, 8, 1);
    ctx.vibrationFreqSlider = Slider("Frequency: ",
                                                &ctx.vibrationFreq, 1, 40, 1);
    ctx.slopeStartSlider = Slider("Start: ", &ctx.slopeStart, 1, 8, 1);
    ctx.slopeEndSlider = Slider("End: ", &ctx.slopeEnd, 2, 9, 1);
    ctx.slopeResistanceStartSlider = Slider("Resistance Start: ",
                                            &ctx.slopeResistanceStart, 1, 8, 1);
    ctx.slopeResistanceEndSlider = Slider("Resistance End: ",
                                                &ctx.slopeResistanceEnd, 2, 9, 1);


    ctx.forces.fill(DEFAULT_FORCE);
    for (int i=0; i<ctx.forces.size(); ++i) {
        ctx.forceSliders.push_back (
            Slider("Force #" + std::to_string(i+1) + ": ", &ctx.forces[i],
                                                                0, 256, 1)
        );
    }
    ctx.regions.fill(DEFAULT_REGION);
    for (int i=0; i<ctx.regions.size(); ++i) {
        ctx.regionSliders.push_back (
            Slider("Region #" + std::to_string(i+1) + ": ",
                                                &ctx.regions[i], 0, 8, 1)
        );
    }

    std::vector<Extra> automaticGun =
    {
        Extra("Start", ctx.startSlider, ctx.start),
        Extra("End", ctx.endSlider, ctx.end),
        Extra("Force", ctx.forceSlider, ctx.force),
    };

    std::vector<Extra> forceExtras;
    for (int i = 0; i < ctx.forces.size(); ++i) {
        forceExtras.push_back(
            Extra("Force", ctx.forceSliders[i], ctx.forces[i])
        );
    }

    std::vector<Extra> regionExtras;
    for (int i = 0; i < ctx.regions.size(); ++i) {
        regionExtras.push_back(
            Extra("Region", ctx.regionSliders[i], ctx.regions[i])
        );
    }

    std::vector<Extra> multivibrationExtras;
    multivibrationExtras.push_back (
        Extra("Frequency", ctx.vibrationFreqSlider, ctx.vibrationFreq)
    );
    for (int i = 0; i < ctx.regions.size(); ++i) {
        multivibrationExtras.push_back(
            Extra("Region", ctx.regionSliders[i], ctx.regions[i])
        );
    }

    ctx.modeExtras = {
        {
            "VibrateTrigger", {
                Extra("Frequency", ctx.frequencySlider, ctx.freq)
            }
        },
        {
            "Resistance", {
                Extra("Start", ctx.startSlider, ctx.start),
                Extra("Force", ctx.forceSlider, ctx.force),
            }
        },
        {
            "CustomTriggerValue", forceExtras
        },
        {
            "Bow", {
                Extra("Start", ctx.startSlider, ctx.start),
                Extra("End", ctx.endSlider, ctx.end),
                Extra("Force", ctx.forceSlider, ctx.force),
                Extra("SnapForce", ctx.snapforceSlider, ctx.snapforce),
            }
        },
        {
            "Galloping", {
                Extra("Start", ctx.startSlider, ctx.start),
                Extra("End", ctx.endSlider, ctx.end),
                Extra("StartFoot", ctx.startFootSlider, ctx.startFoot),
                Extra("SecondFoot", ctx.secondFootSlider, ctx.secondFoot),
                Extra("Frequency", ctx.frequencySlider, ctx.freq)
            }
        },
        // NOTE: the following is used for SemiAutomaticGun too
        {
            "AutomaticGun", automaticGun
        },
        {
            "SemiAutomaticGun", automaticGun
        },
        {
            "Machine", {
                Extra("Start", ctx.startSlider, ctx.start),
                Extra("End", ctx.endSlider, ctx.end),
                Extra("StrengthA", ctx.strengthASlider, ctx.strengthA),
                Extra("StrengthB", ctx.strengthBSlider, ctx.strengthB),
                Extra("Frequency", ctx.frequencySlider, ctx.freq)
            }
        },
        {
            "Feedback", {
                Extra("StartPos", ctx.startPositionSlider, ctx.startPosition),
                Extra("Resistance", ctx.resistanceStrengthSlider,
                                                    ctx.resistanceStrength),
            }
        },
        {
            "Weapon", {
                Extra("WeaponStart", ctx.weaponStartSlider, ctx.weaponStart),
                Extra("WeaponEnd", ctx.weaponEndSlider, ctx.weaponEnd),
                Extra("Resistance", ctx.resistanceStrengthSlider,
                                                    ctx.resistanceStrength),
            }
        },
        {
            "Vibration", {
                Extra("StartPos", ctx.startPositionSlider, ctx.startPosition),
                Extra("amplitude", ctx.amplitudeSlider, ctx.amplitude),
                Extra("Frequency", ctx.vibrationFreqSlider, ctx.vibrationFreq),
            }
        },
        {
            "SlopeFeedback", {
                Extra("Start", ctx.slopeStartSlider, ctx.slopeStart),
                Extra("End", ctx.slopeEndSlider, ctx.slopeEnd),
                Extra("Resistance Start", ctx.slopeResistanceStartSlider, ctx.slopeResistanceStart),
                Extra("Resistance Start", ctx.slopeResistanceEndSlider, ctx.slopeResistanceEnd),
            }
        },
        {
            "MultiplePositionFeedback", regionExtras
        },
        {
            "MultiplePositionVibration", multivibrationExtras
        },
    };


	ctx.menu = Menu(&customModes, &ctx.customMode, MenuOption());

    ctx.components = {
        ctx.menu,
        ctx.frequencySlider,
        ctx.startSlider,
        ctx.endSlider,
        ctx.strengthASlider,
        ctx.strengthBSlider,
        ctx.startFootSlider,
        ctx.secondFootSlider,
        ctx.forceSlider,
        ctx.snapforceSlider,
        ctx.startPositionSlider,
        ctx.weaponStartSlider,
        ctx.weaponEndSlider,
        ctx.resistanceStrengthSlider,
        ctx.amplitudeSlider,
        ctx.vibrationFreqSlider,
        ctx.slopeStartSlider,
        ctx.slopeEndSlider,
        ctx.slopeResistanceStartSlider,
        ctx.slopeResistanceEndSlider,
    };

    ctx.components.insert (
        ctx.components.end(),
        ctx.forceSliders.begin(),
        ctx.forceSliders.end()
    );

    ctx.components.insert (
        ctx.components.end(),
        ctx.regionSliders.begin(),
        ctx.regionSliders.end()
    );

    ctx.extrasContainer = Container::Vertical(ctx.components);
    ctx.extrasWindow = Renderer(ctx.extrasContainer, [&] {

        if (currentModeHasExtras(ctx)) {
            std::vector<Element> elements;

            if (modes[ctx.triggerMode] == "CustomTriggerValue") {
                elements.push_back(text("Custom Mode:"));
                elements.push_back(ctx.menu->Render());
            }

            elements.push_back(text(modes[ctx.triggerMode] + " extras:          "));
            elements.push_back(text(""));
            auto extrasVector = ctx.modeExtras.at( modes[ctx.triggerMode] );
            for (const auto &extra : extrasVector) {
                elements.push_back (
                    hbox(extra.component->Render(),
                    text("(" + std::to_string(extra.value) +")"))

                );
            }
            elements.push_back(text(""));
            return window(text(ctx.name + " Extras"),
                    vbox(elements)
            ) | flex;
        }

        return window(text(ctx.name + " Extras"),
            vbox({
                text(modes[ctx.triggerMode] + " ( no extras )"),
            })
        ) | flex;
    });
}

void resetExtras (std::vector<int> &extras)
{
    extras.clear();
}

int main (void)
{
    using namespace ftxui;

    TriggerContext L2;
    TriggerContext R2;
    setupTriggerContext(L2, "L2");
    setupTriggerContext(R2, "R2");

    if ( DSX::init() != DSX::Success ) {
        std::cerr << "* DSX++ client failed to initialize!" << std::endl;
        return -1;
    }

    auto settings = Renderer([&] {
        std::vector<Element> elements;
        elements.push_back( text("⯍ L2 addaptive trigger  = " + modes[L2.triggerMode]) );
        if (modes[L2.triggerMode] == "CustomTriggerValue") {
            elements.push_back(text("   ⯌ custom mode: " + customModes[L2.customMode]));
        }

        if (currentModeHasExtras(L2)) {
            std::string extrasAllValues = "   ⯌ extras: {";
            auto extrasVector = L2.modeExtras.at( modes[L2.triggerMode] );
            for (const auto &extra : extrasVector) {
                extrasAllValues += std::to_string(extra.value);
                if (&extra != &extrasVector.back())
                    extrasAllValues += ", ";
            }
            extrasAllValues += "}";
            elements.push_back(text(extrasAllValues.c_str()));
        }

        elements.push_back( text("⯍ R2 addaptive trigger  = " + modes[R2.triggerMode]) );

        if (modes[R2.triggerMode] == "CustomTriggerValue") {
            elements.push_back(text("   ⯌ custom mode: " + customModes[R2.customMode]));
        }
        if (currentModeHasExtras(R2)) {
            std::string extrasAllValues = "   ⯌ extras: {";
            auto extrasVector = R2.modeExtras.at( modes[R2.triggerMode] );
            for (const auto &extra : extrasVector) {
                extrasAllValues += std::to_string(extra.value);
                if (&extra != &extrasVector.back())
                    extrasAllValues += ", ";
            }
            extrasAllValues += "}";
            elements.push_back(text(extrasAllValues.c_str()));
        }

        return window(text("Settings"),
            vbox(elements)
        ) | flex;
    });

    auto layout = Container::Vertical({
        Container::Horizontal({
            Container::Vertical({
                Renderer([]{ return text(L"");}),
                Renderer([]{ return text(L"");}),
                Renderer([]{ return text(L"");}),
                Renderer([]{ return text(L"              🎮 DSX++ Adaptive Triggers Test 🎮");}),
                Renderer([]{ return text(L"");}),
                Renderer([]{ return text(L"👉🏽 Use the dropdown menus on the left to choose the adaptive triggers");}),
                Renderer([]{ return text(L"you want to send to controller and then click on the button on the right");}),
                Renderer([]{ return text(L"");}),
            }),
        }),
        Container::Horizontal({
            Dropdown(&modes, &L2.triggerMode),
            L2.extrasWindow,
            Dropdown(&modes, &R2.triggerMode),
            R2.extrasWindow,
            settings,
            Button("🚀 Send", [&] { sendTriggers(L2, R2); }, ButtonOption::Animated(Color::Blue)),
            Button("💥 Reset", [&] { resetTriggers(L2, R2); }, ButtonOption::Animated(Color::Red)),
        }),
        Renderer([]{ return text(L"");}),
        Renderer([]{ return text(L"");}),
    });

    //auto screen = ScreenInteractive::Fullscreen();
    auto screen = ScreenInteractive::FitComponent();
    screen.Loop(layout);

    if ( DSX::terminate() != DSX::Success ) {
        std::cerr << "* DSX++ client failed to terminate!" << std::endl;
        return -3;
    }

    return 0;
}
