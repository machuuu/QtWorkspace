#pragma once

#define TEST_DEVICE_API __declspec(dllexport)

// Eventually add a device
namespace TestDevice{

    TEST_DEVICE_API void Increment();
    TEST_DEVICE_API void PrintCount();

}

  


