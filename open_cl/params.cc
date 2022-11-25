#include <iostream>
#include <sstream>
#include <locale>

#define CL_HPP_ENABLE_EXCEPTIONS
#include <CL/opencl.hpp>

using systemCL = std::pair<cl::Platform, cl::Device>;

std::vector<systemCL> getDevices() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    std::vector<systemCL> ret;

    for(auto&& platform : platforms) {
        std::vector<cl::Device> platform_devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &platform_devices);

        for(auto&& device : platform_devices) {
            ret.push_back({platform, device});
        }
    }

    return ret;
}

int main()
{
    auto cl = getDevices();
    std::cout << "Number of available devices: " << cl.size() << std::endl;
    cl::Device dev = cl[0].second;
    std::cout << "Device info " << std::endl;
    std::cout << "Name: " << dev.getInfo<CL_DEVICE_NAME>() << std::endl;
    std::cout << "Little endian: " << std::boolalpha << static_cast<bool>(dev.getInfo<CL_DEVICE_ENDIAN_LITTLE>()) << std::endl;
    std::cout << "Global mem size: " << dev.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() / 1024 / 1024 << " MB" << std::endl;
    std::cout << "Global mem-cache size: " << dev.getInfo<CL_DEVICE_GLOBAL_MEM_CACHE_SIZE>() / 1024 / 1024 << " MB"<< std::endl;
    std::cout << "Max CLK frequency: " << dev.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << " MHZ"<< std::endl;
    std::cout << "Max compute units: " << dev.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
    std::cout << "Device vendor " << dev.getInfo<CL_DEVICE_VENDOR>() << std::endl;
    std::cout << "Driver version " << dev.getInfo<CL_DRIVER_VERSION>() << std::endl;
    std::cout << "Device version " << dev.getInfo<CL_DEVICE_VERSION>() << std::endl;
    std::cout << "Device vendor ID " << dev.getInfo<CL_DEVICE_VENDOR_ID>() << std::endl;


    return 0;
}
