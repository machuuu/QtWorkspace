#include <iostream>
#include <TestDeviceAPI.h>

static int count = 0;

TEST_DEVICE_API void TestDevice::Increment()
{
    std::cout << "Incrementing Count" << std::endl;
    count++;
}

TEST_DEVICE_API void TestDevice::PrintCount()
{
    std::cout << "Count: " << count << std::endl;
} 