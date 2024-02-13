#include "TestDeviceAPI.h"

int main()
{
    TestDevice::Increment();
    TestDevice::PrintCount();
    TestDevice::Increment();
    TestDevice::PrintCount();
    TestDevice::Increment();
    TestDevice::PrintCount();

    return 0;
}