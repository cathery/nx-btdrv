#include "nn_bluetooth.hpp"
#include <cstring>
#include <malloc.h>
#include <stdio.h>
#include <switch.h>

// info for my xbox one controller
//MAC address: 5CBA37F3E0FF
//GUID: {e0cbf06c-cd8b-4647-bb8a-263b43f0f974}
constexpr nn::bluetooth::Address xboxMac = {{0x5C, 0xBA, 0x37, 0xF3, 0xE0, 0xFF}};

// info for my ds4 controller
//MAC address: 90895F97AFE2
constexpr nn::bluetooth::Address ds4Mac = {{0x90, 0x89, 0x5F, 0x97, 0xAF, 0xE2}};

constexpr auto currMac = ds4Mac;

struct XboxOneRumbleData
{
    uint8_t command;
    uint8_t dummy1;
    uint8_t counter;
    uint8_t size;
    uint8_t mode;
    uint8_t rumble_mask;
    uint8_t trigger_left;
    uint8_t trigger_right;
    uint8_t strong_magnitude;
    uint8_t weak_magnitude;
    uint8_t duration;
    uint8_t period;
    uint8_t extra;
};

int main()
{
    consoleInit(nullptr);
    printf("Initialize bluetooth driver: 0x%x\n", nn::bluetooth::InitializeBluetoothDriver());

    //printf("nn::bluetooth::ExtSetVisibility: 0x%x\n", nn::bluetooth::ExtSetVisibility(false, false));

    while (appletMainLoop())
    {
        hidScanInput();
        u64 kDown = 0;
        for (u8 controller = 0; controller < 10; controller++)
            kDown |= hidKeysDown(static_cast<HidControllerID>(controller));

        if (kDown & KEY_PLUS)
            break;

        if (kDown & KEY_A)
        {
            printf("nn::bluetooth::CreateBond: 0x%x\n", nn::bluetooth::CreateBond(&currMac, static_cast<nn::bluetooth::BluetoothTransport>(0)));
        }

        if (kDown & KEY_B)
        {
            printf("nn::bluetooth::SspReply: 0x%x\n", nn::bluetooth::SspReply(&currMac, static_cast<nn::bluetooth::BluetoothSspVariant>(1), true, 0));
        }

        if (kDown & KEY_X)
        {
            printf("nn::bluetooth::RemoveBond: 0x%x\n", nn::bluetooth::RemoveBond(&currMac));
        }

        if (kDown & KEY_DRIGHT)
        {
            printf("nn::bluetooth::HidConnect: 0x%x\n", nn::bluetooth::HidConnect(&currMac));
        }

        if (kDown & KEY_DUP)
        {
            printf("nn::bluetooth::HidDisconnect: 0x%x\n", nn::bluetooth::HidDisconnect(&currMac));
        }

        if (kDown & KEY_DDOWN)
        {
            /*
            constexpr u8 strong_magnitude = 0xFF;
            constexpr u8 weak_magnitude = 0xFF;

            constexpr u8 rumble_data[]{
                0x09, 0x00, 0x00,
                0x09, 0x00, 0x0f, 0x00, 0x00,
                strong_magnitude,
                weak_magnitude,
                0xff, 0x00, 0x00};
            */

            constexpr u8 ledValueR = 0x00;
            constexpr u8 ledValueG = 0x00;
            constexpr u8 ledValueB = 0x00;

            constexpr u8 initds4[32] = {
                0x05, 0x07, 0x00, 0x00,
                0x00, 0x00,                      //initial strong and weak rumble
                ledValueR, ledValueG, ledValueB, //LED color
                0x00, 0x00};

            nn::bluetooth::HidData data{};

            data.size = sizeof(initds4);
            memcpy(data.buffer, initds4, data.size);

            printf("nn::bluetooth::HidSendData2: 0x%x\n", nn::bluetooth::HidSendData2(&currMac, &data));
        }

        if (kDown & KEY_L)
        {
            printf("nn::bluetooth::HidGetReport: 0x%x\n", nn::bluetooth::HidGetReport(&currMac, 0, 0));
        }

        consoleUpdate(NULL);
    }
    consoleExit(nullptr);

    nn::bluetooth::FinalizeBluetoothDriver();
}