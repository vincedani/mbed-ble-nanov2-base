// Copyright (c) 2019 elecro
//
// Licensed under the BSD 3-Clause License
// <LICENSE.md or https://opensource.org/licenses/BSD-3-Clause>.
// This file may not be copied, modified, or distributed except
// according to those terms.
#ifndef BLUE_HPP
#define BLUE_HPP

#include <mbed.h>
#include <ble/BLE.h>

void blue_AddConnectionCallbacks(BLE& ble);
void blue_ComputeUnitService(BLE& ble, EventQueue& queue);

#endif
