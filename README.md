# ModBusMultiMode Firmware

This firmware allows you to take advantage of the features of the ESP32 and A9/A9G SOCs as peripherals on a MODBUS-RTU bus. MODBUS-RTU allows the use of these modules in both embedded system projects and industrial network applications. Moreover, as a standardized protocol, its implementation does not involve excessive use of hardware resources and has extensive documentation and free implementations, being able to be adapted to different hardware architectures (from the most modest to the most powerful). To make the most of the hardware resources of these SOCs, it was decided to implement a configurable firmware through an external interface. This interface enables the module to be used in three different operating modes:

Master Mode: The device behaves as a master of a MODBUS-RTU network and executes read and write commands on slaves connected to the bus according to the orders it receives through an API by MQTT from an internet server.
Slave Mode: The device behaves as a slave of a MODBUS-RTU network and publishes all coil and register addresses of the MODBUS-RTU protocol, so that a master can write or read information from these addresses. These addresses are also accessible through an API by MQTT from a server.
Listen Mode: The device behaves as an analyzer of the MODBUS-RTU bus, publishing detailed information to the server about which register, discrete input or coil has been modified or read in each slave on the bus.

___

## ModBusMultiMode Firmware A9G (project tasks)

* [ ] Slave Mode.
  * [ ] Perform port to FreeModbus on A9G (if possible).
    * [ ] If not possible, reimplement ModbusSlave on A9G.
  * [ ] Tests of reading/writing Modbus variables on A9G (simulation).
  * [ ] Connection to internet service via MQTT (test with geolocation).
  * [ ] MODBUS variables shared with internet service via MQTT.
  * [ ] Tests of the port with coils (simulation).
  * [ ] Tests of the port with registers (simulation).
  * [ ] Tests in real application (PLC in model).
  * [ ] Implementation of an API for the server (cross-platform).

* [ ] Spy Mode.
  * [ ] Modify behavior of FreeModbus on A9G for slave mode (if possible).
    * [ ] If not possible, implement library in C99 for bus listening (ModbusSpy).
  * [ ] MODBUS variables shared (read-only) with internet service via MQTT.
  * [ ] Tests in real application (PLC in model).
  * [ ] Complement the API for the server (cross-platform).

* [ ] Master Mode.
  * [ ] Perform port to FreeModbus on A9G for master mode (if possible).
    * [ ] Port to ESP_MODBUS (if possible).
    * [ ] If not possible, reimplement ModbusMaster on A9G.
  * [ ] MODBUS variables shared (read-only) with internet service via MQTT.
  * [ ] Tests in real application (PLC in model).
  * [ ] Complement the API for the server (cross-platform).
<<<<<<< HEAD
* [ ] OTA Support

## ModBusMultiMode Firmware ESP32

___

## Licensing

We are delighted to announce that ModbusMultiMode is open-source software. You can find the details of the license in the COPYING file for your reference. If the license poses any issues for you, please do not hesitate to contact us. This software is licensed under the GNU General Public License (GPL). Your satisfaction and convenience are our priority
