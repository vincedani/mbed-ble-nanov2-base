// Copyright (c) 2017-2019 elecro
//
// Licensed under the BSD 3-Clause License
// <LICENSE.md or https://opensource.org/licenses/BSD-3-Clause>.
// This file may not be copied, modified, or distributed except
// according to those terms.

#include <mbed.h>
#include <ble/BLE.h>

#include "blue.hpp"

DigitalOut led1(LED1, 1);
Serial usb(USBTX, USBRX);

EventQueue queue;

void blink_led(void)
{
    led1 = !led1;
}

static void blue_InitDone(BLE::InitializationCompleteCallbackContext* params)
{
    BLE& ble = params->ble;
    ble_error_t error = params->error;

    /* Make sure there was no error when initializing the BLE interface. */
    if (error != BLE_ERROR_NONE) {
        usb.printf("[BLUE] failed to initialize the BLE. (%d)\r\n", error);
        return;
    }

    /* Make sure that the BLE instance is the default instance. */
    if (ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        usb.printf("[BLUE] invalid BLE instance detected.\r\n");
        return;
    }

    blue_ComputeUnitService(ble, queue);
}

/* All events from the BLE should be queue on our own event queue. */
static void blue_ScheduleEventsProcessing(BLE::OnEventsToProcessCallbackContext* context)
{
    BLE& ble = BLE::Instance();
    queue.call(Callback<void()>(&ble, &BLE::processEvents));
}

int main()
{
    blink_led();
    wait(4);
    blink_led();

    /* Initialize BLE */
    BLE& ble = BLE::Instance();

    ble.onEventsToProcess(blue_ScheduleEventsProcessing);
    ble.init(blue_InitDone);

    queue.call_every(500, blink_led);
    queue.dispatch_forever();

    return 0;
}
