// Copyright (c) 2019 elecro
//
// Licensed under the BSD 3-Clause License
// <LICENSE.md or https://opensource.org/licenses/BSD-3-Clause>.
// This file may not be copied, modified, or distributed except
// according to those terms.

#include <mbed.h>
#include "blue.hpp"

#include "ComputeUnitService.hpp"

/* BLE connection callback
 *
 * Note: ATM only one connection can be done in parallel.
 */
void blue_ConnectedCallback(const Gap::ConnectionCallbackParams_t* params)
{
    const BLEProtocol::AddressBytes_t& peer_addr = params->peerAddr;

    /* Address is in little endian, thus print it in reverse order. */
    printf("Connection form: [%02x:%02x:%02x:%02x:%02x:%02x]",
           peer_addr[5], peer_addr[4], peer_addr[3],
           peer_addr[2], peer_addr[1], peer_addr[0]);

    /* In a connection a device has its own role:
     *  * Central: This device can search for services/characteristics and access them.
     *  * Peripheral: This device is the "data provider", it should have it's own service(s)/characteristic(s).
     */
    switch (params->role) {
        case Gap::CENTRAL: printf(" and I'm a CENTRAL device"); break;
        case Gap::PERIPHERAL: printf(" and I'm a PERIPHERAL device"); break;
        default: break;
    }
    printf("\r\n");
}

void blue_DisconnectedCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    printf("Disconnected\r\n");
    /* After disconnection the advertisement and scanning could be restarted. */
    BLE::Instance().gap().startAdvertising();
}

void blue_AddConnectionCallbacks(BLE& ble)
{
    Gap& gap = ble.gap();
    gap.onConnection(blue_ConnectedCallback);
    gap.onDisconnection(blue_DisconnectedCallback);
}

static ComputeUnitService* computeService;


void blue_ComputeUnitService(BLE& ble, EventQueue& queue) {
    Gap& gap = ble.gap();

    /* Create the service(s) */
    computeService = new ComputeUnitService(ble);

    /* Add the 16 bit service UUID entries into the advertisment data. */
    const uint16_t service_uuid16_list[] = { ComputeUnitService::SERVICE_UUID };

    gap.accumulateAdvertisingPayload(
      GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS,
      (uint8_t*)service_uuid16_list,
      sizeof(service_uuid16_list)
    );

    /* Add the device name for the advertisment data. */
    const char BCUDeviceName[] = "DaninjaComputeUnit";
    gap.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME,
                                     (uint8_t*)BCUDeviceName,
                                     sizeof(BCUDeviceName));

    /* Specify flags in the payload. */
    gap.accumulateAdvertisingPayload(
      GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);

    /* Configure the advertisment. */
    gap.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    gap.setAdvertisingInterval(1000);

    gap.startAdvertising();
    printf("Started advertising\r\n");
}