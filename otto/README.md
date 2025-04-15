# Sample - NOS3 Component
This repository contains the NOS3 Sample Component.
This includes flight software (FSW), ground software (GSW), simulation, and support directories.

## Overview
This sample component is a UART device that accepts multiple commands, including requests for telemetry and data.
The available FSW is for use in the core Flight System (cFS) while the GSW supports COSMOS.
A NOS3 simulation is available which includes both sample and 42 data providers.


# Device Communications
The protocol, commands, and responses of the component are captured below.

## Protocol
The protocol in use is UART 115200 8N1.
The device is speak when spoken too.
All communications with the device require / contain a header of 0xDEAD and a trailer of 0xBEEF.

## Commands
All commands received by the device are echoed back to the sender to confirm receipt.
Should commmands involve a reply, the device immediately sends the reply after the command echo.
Device commands are all formatted in the same manner and are fixed in size:
* uint16, 0xDEAD
* uint8, command identifier
  - (0) Get Housekeeping
  - (1) Get Sample
  - (2) Set Configuration
* uint32, command payload
  - Unused for all but set configuration command
* uint16, 0xBEEF

## Response
Response formats are as follows:
* Housekeeping
  - uint16, 0xDEAD
  - uint32, Command Counter
    * Increments for each command received
  - uint32, Configuration
    * Internal configuration number in use by the device
  - uint32, Status
    * Self reported status of the component where zero is completely healthy and each bit represents different errors
    * No means to clear / set status except for a power cycle to the device
  - uint16, 0xBEEF
* Sample
  - uint16, 0xDEAD
  - uint32, Command Counter
    * Increments for each command received
  - uint16, Data X
    * X component of sample data
  - uint16, Data Y
    * X component of sample data
  - uint16, Data Z
    * X component of sample data
  - uint16, 0xBEEF


# Configuration
The various configuration parameters available for each portion of the component are captured below.

## FSW
Refer to the file [fsw/platform_inc/sample_platform_cfg.h](fsw/platform_inc/sample_platform_cfg.h) for the default
configuration settings, as well as a summary on overriding parameters in mission-specific repositories.

## Simulation
The default configuration returns data that is X * 0.001, Y * 0.002, and Z * 0.003 the request count after conversions:
```
<simulator>
    <name>sample_sim</name>
    <active>true</active>
    <library>libsample_sim.so</library>
    <hardware-model>
        <type>SAMPLE</type>
        <connections>
            <connection><type>command</type>
                <bus-name>command</bus-name>
                <node-name>sample-sim-command-node</node-name>
            </connection>
            <connection><type>usart</type>
                <bus-name>usart_29</bus-name>
                <node-port>29</node-port>
            </connection>
        </connections>
        <data-provider>
            <type>SAMPLE_PROVIDER</type>
        </data-provider>
    </hardware-model>
</simulator>
```

## 42
Optionally the 42 data provider can be configured in the `nos3-simulator.xml`:
```
        <data-provider>
            <type>SAMPLE_42_PROVIDER</type>
            <hostname>localhost</hostname>
            <port>4242</port>
            <max-connection-attempts>5</max-connection-attempts>
            <retry-wait-seconds>5</retry-wait-seconds>
            <spacecraft>0</spacecraft>
        </data-provider>
```


# Documentation
If this sample application had an ICD and/or test procedure, they would be linked here.

## Releases
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.
* v1.0.0 - X/Y/Z 
  - Updated to be a component repository including FSW, GSW, Sim, and Standalone checkout
* v0.1.0 - 10/9/2021 
  - Initial release with version tagging
