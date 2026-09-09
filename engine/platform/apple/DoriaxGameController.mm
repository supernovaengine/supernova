// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#import "DoriaxGameController.h"

#import <GameController/GameController.h>

#include <math.h>
#include <string.h>

#include "Engine.h"
#include "Input.h"

#define DORIAX_MAX_GAMEPADS 8

@implementation DoriaxGameController

// _slots[i] holds the GCController in slot i, or NSNull for a free slot. Slots
// stay stable across disconnects so a controller keeps its gamepad index.
static NSMutableArray* _slots = nil;
static bool  _buttonState[DORIAX_MAX_GAMEPADS][D_GAMEPAD_BUTTON_LAST + 1];
static float _axisState[DORIAX_MAX_GAMEPADS][D_GAMEPAD_AXIS_LAST + 1];
static bool  _swapFaceButtons[DORIAX_MAX_GAMEPADS];
static bool  _started = false;

// GameController reports Nintendo pads by the labels printed on them (A on the
// right, B at the bottom), while every other backend reports face buttons by
// position. Detect them so the diamond can be put back in positional order.
static bool hasNintendoFaceLayout(GCController* controller) {
    NSString* category = controller.productCategory;
    if (category == nil)
        return false;
    return [category containsString:@"Switch"] || [category containsString:@"Joy-Con"];
}

+ (void)start {
    if (_started)
        return;
    _started = true;

    _slots = [NSMutableArray arrayWithCapacity:DORIAX_MAX_GAMEPADS];
    for (int i = 0; i < DORIAX_MAX_GAMEPADS; i++)
        [_slots addObject:[NSNull null]];

    NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
    [nc addObserver:self
           selector:@selector(controllerConnected:)
               name:GCControllerDidConnectNotification
             object:nil];
    [nc addObserver:self
           selector:@selector(controllerDisconnected:)
               name:GCControllerDidDisconnectNotification
             object:nil];

    for (GCController* controller in [GCController controllers])
        [self registerController:controller];
}

+ (int)slotForController:(GCController*)controller {
    for (int i = 0; i < DORIAX_MAX_GAMEPADS; i++) {
        if (_slots[i] == controller)
            return i;
    }
    return -1;
}

+ (void)registerController:(GCController*)controller {
    // Only standard (extended) controllers; skip Siri-remote micro gamepads.
    if (controller.extendedGamepad == nil)
        return;
    if ([self slotForController:controller] != -1)
        return;

    int slot = -1;
    for (int i = 0; i < DORIAX_MAX_GAMEPADS; i++) {
        if (_slots[i] == [NSNull null]) {
            slot = i;
            break;
        }
    }
    if (slot == -1)
        return;

    _slots[slot] = controller;
    controller.playerIndex = (GCControllerPlayerIndex)slot;
    _swapFaceButtons[slot] = hasNintendoFaceLayout(controller);

    memset(_buttonState[slot], 0, sizeof(_buttonState[slot]));
    for (int a = 0; a <= D_GAMEPAD_AXIS_LAST; a++)
        _axisState[slot][a] = 0.0f;
    // triggers rest at -1 in the engine's [-1, 1] convention
    _axisState[slot][D_GAMEPAD_AXIS_LEFT_TRIGGER] = -1.0f;
    _axisState[slot][D_GAMEPAD_AXIS_RIGHT_TRIGGER] = -1.0f;

    NSString* name = controller.vendorName ?: @"Gamepad";
    doriax::Engine::systemGamepadConnect(slot, [name UTF8String]);

    int capturedSlot = slot;
    controller.extendedGamepad.valueChangedHandler =
        ^(GCExtendedGamepad* gamepad, GCControllerElement*) {
            [DoriaxGameController handleState:gamepad slot:capturedSlot];
        };

    // Emit the initial state so scripts see buttons already held on connect.
    [self handleState:controller.extendedGamepad slot:slot];
}

+ (void)controllerConnected:(NSNotification*)note {
    [self registerController:(GCController*)note.object];
}

+ (void)controllerDisconnected:(NSNotification*)note {
    int slot = [self slotForController:(GCController*)note.object];
    if (slot == -1)
        return;
    _slots[slot] = [NSNull null];
    doriax::Engine::systemGamepadDisconnect(slot);
}

static void processButton(int slot, int button, bool pressed) {
    if (pressed != _buttonState[slot][button]) {
        _buttonState[slot][button] = pressed;
        if (pressed)
            doriax::Engine::systemGamepadButtonDown(slot, button);
        else
            doriax::Engine::systemGamepadButtonUp(slot, button);
    }
}

static void processAxis(int slot, int axis, float value) {
    if (fabsf(value - _axisState[slot][axis]) > 0.001f) {
        _axisState[slot][axis] = value;
        doriax::Engine::systemGamepadAxisMove(slot, axis, value);
    }
}

+ (void)handleState:(GCExtendedGamepad*)gp slot:(int)slot {
    // A is the bottom button and X the left one, whatever the pad prints there
    bool swap = _swapFaceButtons[slot];
    processButton(slot, D_GAMEPAD_BUTTON_A, (swap ? gp.buttonB : gp.buttonA).pressed);
    processButton(slot, D_GAMEPAD_BUTTON_B, (swap ? gp.buttonA : gp.buttonB).pressed);
    processButton(slot, D_GAMEPAD_BUTTON_X, (swap ? gp.buttonY : gp.buttonX).pressed);
    processButton(slot, D_GAMEPAD_BUTTON_Y, (swap ? gp.buttonX : gp.buttonY).pressed);
    processButton(slot, D_GAMEPAD_BUTTON_LEFT_BUMPER, gp.leftShoulder.pressed);
    processButton(slot, D_GAMEPAD_BUTTON_RIGHT_BUMPER, gp.rightShoulder.pressed);
    processButton(slot, D_GAMEPAD_BUTTON_DPAD_UP, gp.dpad.up.pressed);
    processButton(slot, D_GAMEPAD_BUTTON_DPAD_RIGHT, gp.dpad.right.pressed);
    processButton(slot, D_GAMEPAD_BUTTON_DPAD_DOWN, gp.dpad.down.pressed);
    processButton(slot, D_GAMEPAD_BUTTON_DPAD_LEFT, gp.dpad.left.pressed);

    // buttonMenu (start) and buttonOptions (back/view) exist from the iOS 13 /
    // macOS 10.15 deployment floor; buttonOptions is still nullable per device.
    if (gp.buttonMenu)
        processButton(slot, D_GAMEPAD_BUTTON_START, gp.buttonMenu.pressed);
    if (gp.buttonOptions)
        processButton(slot, D_GAMEPAD_BUTTON_BACK, gp.buttonOptions.pressed);

    // Thumbstick clicks: nullable (not every controller has them).
    if (gp.leftThumbstickButton)
        processButton(slot, D_GAMEPAD_BUTTON_LEFT_THUMB, gp.leftThumbstickButton.pressed);
    if (gp.rightThumbstickButton)
        processButton(slot, D_GAMEPAD_BUTTON_RIGHT_THUMB, gp.rightThumbstickButton.pressed);

    // Home/guide button is newer (iOS 14 / macOS 11).
    if (@available(iOS 14.0, macOS 11.0, tvOS 14.0, *)) {
        if (gp.buttonHome)
            processButton(slot, D_GAMEPAD_BUTTON_GUIDE, gp.buttonHome.pressed);
    }

    // Sticks: GameController reports Y up-positive; the engine follows the GLFW
    // convention (down-positive), so invert Y.
    processAxis(slot, D_GAMEPAD_AXIS_LEFT_X, gp.leftThumbstick.xAxis.value);
    processAxis(slot, D_GAMEPAD_AXIS_LEFT_Y, -gp.leftThumbstick.yAxis.value);
    processAxis(slot, D_GAMEPAD_AXIS_RIGHT_X, gp.rightThumbstick.xAxis.value);
    processAxis(slot, D_GAMEPAD_AXIS_RIGHT_Y, -gp.rightThumbstick.yAxis.value);

    // Triggers: GameController reports 0..1; the engine uses [-1, 1] resting at -1.
    processAxis(slot, D_GAMEPAD_AXIS_LEFT_TRIGGER, gp.leftTrigger.value * 2.0f - 1.0f);
    processAxis(slot, D_GAMEPAD_AXIS_RIGHT_TRIGGER, gp.rightTrigger.value * 2.0f - 1.0f);
}

@end
