# CMakeLists.txt
#
# Top-level CMake file for the iec 61850 client.
#
#*******************************************************************************
# This is part of the Paho MQTT C++ client library.
#
# Copyright (c) 2021-2022, chao.xia
#
# 
# Contributors:
#   chao.xia
#*******************************************************************************/
# --- load library ---
find_package(PahoMqttCpp REQUIRED)
find_package(log4cpp REQUIRED)
find_package(iec61850 REQUIRED)

set(iec61850_objs
    gwmanager/gwdata.cpp
    mqtt/mqtt_client.cpp
    node.cpp
    client.cpp
    dataset.cpp
    file.cpp
    init.cpp
    model.cpp
    reports.cpp
    sgcb.cpp
    main.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${iec61850_objs})

target_link_libraries(${PROJECT_NAME} jsoncpp iec61850 paho-mqtt3a paho-mqtt3as paho-mqttpp3)