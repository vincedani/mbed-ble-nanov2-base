#ifndef COMPUTE_UNIT
#define COMPUTE_UNIT

#include <string>
#include <math.h>
#include <stdlib.h>
#include <ble/BLE.h>

class ComputeUnitService {
public:
  const static uint16_t SERVICE_UUID = 0xAA00;
  const static uint16_t INPUT_A_CHARACTERISTIC_UUID = 0xAA0F;
  const static uint16_t INPUT_B_CHARACTERISTIC_UUID = 0xAA1F;
  const static uint16_t OPERATION_CHARACTERISTIC_UUID = 0xAA2F;
  const static uint16_t RESULT_CHARACTERISTIC_UUID = 0xAA3F;

  ComputeUnitService(BLE& ble) :
    ble(ble),
    a_value(0.0),
    b_value(0.0),
    operation('+'),
    result(0.0),
    inputAChar(INPUT_A_CHARACTERISTIC_UUID, &a_value),
    inputBChar(INPUT_B_CHARACTERISTIC_UUID, &b_value),
    operationChar(OPERATION_CHARACTERISTIC_UUID, &operation),
    resultChar(RESULT_CHARACTERISTIC_UUID, &result,
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY)
  {
    /* Build the service characteristic table: */
    GattCharacteristic* characteristics[] = {
      &inputAChar, &inputBChar, &operationChar, &resultChar
    };

    /* Build the service. */
    GattService service(
      SERVICE_UUID,
      characteristics,
      sizeof(characteristics) / sizeof(characteristics[0]));

    /* Register the service in the BLE interface. */
    ble.gattServer().addService(service);
    ble.gattServer().onDataWritten(this, &ComputeUnitService::onDataWritten);
  }

private:
  void updateResult() {
    result = 0;

    if (operation == '+') {
      result = a_value + b_value;

    } else if (operation == '-') {
      result = a_value - b_value;

    } else if (operation == '*') {
      result = a_value * b_value;

    } else if (operation == '/') {
      result = a_value / ((b_value != 0) ? b_value : 1);

    } else if (operation == '^') {
      result = pow(a_value, b_value);

    } else if (operation == '#') {
      result = pow(b_value, 1.0/a_value);

    } else {
      fprintf(stderr, "[BCU] Error: Invalid operant\r\n");
    }

    ble.gattServer().write(resultChar.getValueAttribute().getHandle(),
                           reinterpret_cast<uint8_t*>(&result), sizeof(result));
  }

  void onDataWritten(const GattWriteCallbackParams* params) {
    /* Check if the data received is for these characterisics. */
    GattAttribute::Handle_t handle = params->handle;

    if (handle == inputAChar.getValueAttribute().getHandle()) {
      std::string value(reinterpret_cast<const char *>(params->data), params->len);
      a_value = atof(value.c_str());

      ble.gattServer().write(inputAChar.getValueAttribute().getHandle(),
                             reinterpret_cast<uint8_t*>(&a_value), sizeof(a_value));

      fprintf(stderr, "A = %f\r\n", a_value);
    }

    if (handle == inputBChar.getValueAttribute().getHandle()) {
      std::string value(reinterpret_cast<const char *>(params->data), params->len);

      b_value = atof(value.c_str());
      ble.gattServer().write(inputBChar.getValueAttribute().getHandle(),
                             reinterpret_cast<uint8_t*>(&b_value), sizeof(b_value));

      fprintf(stderr, "B = %f\r\n", b_value);
    }

    if (handle == operationChar.getValueAttribute().getHandle()) {
      operation = params->data[0];
      ble.gattServer().write(operationChar.getValueAttribute().getHandle(),
                             &operation, sizeof(operation));

      fprintf(stderr, "OP = %c\r\n", operation);
    }

    updateResult();
  }

  BLE& ble;

  float32_t a_value;
  float32_t b_value;
  uint8_t operation;
  float32_t result;

  ReadWriteGattCharacteristic<float32_t> inputAChar;
  ReadWriteGattCharacteristic<float32_t> inputBChar;
  WriteOnlyGattCharacteristic<uint8_t> operationChar;
  ReadOnlyGattCharacteristic<float32_t> resultChar;
};

#endif // COMPUTE_UNIT
