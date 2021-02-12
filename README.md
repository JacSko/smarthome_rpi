# SmartHome_RPi

## Description
Application is GUI client for [SmartHome_CoreApplication](https://github.com/JacSko/SmartHome_CoreApplication).
It is parsing the received data from CoreApplication, than presents it on GUI.

![image](https://user-images.githubusercontent.com/47041583/107795495-0054b380-6d59-11eb-81cc-b71e6e31b8a3.png)

![github_core_diagram](https://user-images.githubusercontent.com/47041583/107790916-a00f4300-6d53-11eb-9905-4ffcf12df80a.png)

## Details
Connection is established via TCP sockets.
Socket driver is working as a client an keeps trying to connect until success.
![rpi_sw_design](https://user-images.githubusercontent.com/47041583/107795332-cbe0f780-6d58-11eb-8a8b-9f46e1a9e492.png)

## Building
There are some dependencies to take care before building.
Project use QT5 library for GUI, therefore such libraries have to be compiled regarding the target architecture (Host(ubuntu), RaspberryPi).
The libraries has to be compiled or cross-compiled and be ready to use on target system.

### Building for Raspberry
```
cd <project_dir>
mkdir build_rpi && cd build_rpi
cmake .. -DBUILD_RPI
make
```
After the binary ./smarthome_rpi can be moved to Raspberry and run

### Building on host
```
cd <project_dir>
mkdir build && cd build
cmake ..
make
```
### Building Unit tests
To build unit test, You have to clone googletest repo:
```
cd <project_dir>
cd ext_libs/googletest
git clone https://github.com/google/googletest.git
```
Than run script:
```
./build_and_run_ut.sh
```
### Coverage calculation
Run script:
```
./build_and_run_coverage.sh
```
HTML report will be opened automatically in Firefox, but it can be also found:
```
build_ut/html/coverage.html
```
## TODO
- [ ] Requesting data update from Main Board after fresh connection
