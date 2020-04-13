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

struct HidReportSharedMem
{
    u8 unk[0x3000];
};

int main()
{
    Event register_hid_report_event;
    Event hid_report_event;
    void* shmem;
    nn::bluetooth::CircularBuffer circbuf;
    circbuf.GetWriteableSize();
    circbuf.Read();
    static HidReportSharedMem shmem_copy;
    Event hid_event;
    Event bt_event;
    consoleInit(nullptr);
    printf("nn::bluetooth::InitializeBluetoothDriver: 0x%x\n", nn::bluetooth::InitializeBluetoothDriver());
    //printf("nn::bluetooth::InitializeBluetooth: 0x%x\n", nn::bluetooth::InitializeBluetooth(&bt_event));
    printf("nn::bluetooth::InitializeHid: 0x%x\n", nn::bluetooth::InitializeHid(&hid_event, 0));
    printf("nn::bluetooth::RegisterHidReportEvent: 0x%x\n", nn::bluetooth::RegisterHidReportEvent(&register_hid_report_event));
    printf("nn::bluetooth::HidGetReportEventInfo: 0x%x\n", nn::bluetooth::HidGetReportEventInfo(&shmem));

    memcpy(&shmem_copy, shmem, sizeof(HidReportSharedMem));

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

        if (kDown & KEY_Y)
        {
            printf("nn::bluetooth::CancelBond: 0x%x\n", nn::bluetooth::CancelBond(&currMac));
        }

        if (kDown & KEY_DRIGHT)
        {
            printf("nn::bluetooth::HidConnect: 0x%x\n", nn::bluetooth::HidConnect(&currMac));
        }

        if (kDown & KEY_DLEFT)
        {
            printf("nn::bluetooth::HidDisconnect: 0x%x\n", nn::bluetooth::HidDisconnect(&currMac));
        }

        if (kDown & KEY_MINUS)
        {
            constexpr bool rumble_motors_enabled = true;
            constexpr u8 weak_magnitude = 0xFF;
            constexpr u8 strong_magnitude = 0xFF;
            constexpr u8 led_R = 0;
            constexpr u8 led_G = 0;
            constexpr u8 led_B = 0;

            u8 report[79] = {0xA2,       // transaction type | report type
                             0x11,       // report ID
                             0xc0, 0x20, // unknown
                             rumble_motors_enabled ? 0xf3 : 0xf0,
                             0x04, 0x00, // unknown
                             weak_magnitude, strong_magnitude,
                             led_R, led_G, led_B};
            // the last 4 bytes are the crc32 of the entire packet before it, so we do a little hack to write it as a u32 (stored in little endian)
            *reinterpret_cast<u32*>(&report[75]) = crc32Calculate(report, 75);

            nn::bluetooth::HidData data{};

            // we want to substract the report type byte from the packet
            data.size = sizeof(report) - 1;
            memcpy(data.buffer, report + 1, data.size);

            // This one doesn't care about first byte or CRC32 as long as the packet is correct and report type is OUTPUT
            printf("nn::bluetooth::HidSetReport: 0x%x\n", nn::bluetooth::HidSetReport(&currMac, nn::bluetooth::BluetoothHhReportType::OUTPUT, &data));

            // These ones don't work unless the first byte is 0xA2 and CRC32 matches the packet
            //printf("nn::bluetooth::HidSendData2: 0x%x\n", nn::bluetooth::HidSendData2(&currMac, &data));
            //printf("nn::bluetooth::HidSendData: 0x%x\n", nn::bluetooth::HidSendData(&currMac, &data));
        }

        if (kDown & KEY_ZL)
        {
            nn::settings::system::BluetoothDevicesSettings buffer{};
            printf("nn::bluetooth::HidGetPairedDevice: 0x%x\n", nn::bluetooth::HidGetPairedDevice(&currMac, &buffer));
            printf("word_x26: 0x%x\n", buffer.word_x26);
            printf("byte_x28: 0x%x\n", buffer.byte_x28);
            printf("byte_x39: 0x%x\n", buffer.byte_x39);
            printf("word_x3A: 0x%x\n", buffer.word_x3A);
            printf("flags_maybe: 0x%x\n", buffer.flags_maybe);
            printf("word_x40: 0x%x\n", buffer.vendor_ID);
            printf("product_ID: 0x%x\n", buffer.product_ID);
            printf("byte_x44: 0x%x\n", buffer.byte_x44);
            printf("byte_x45: 0x%x\n", buffer.byte_x45);
            printf("callbacks_size: 0x%x\n", buffer.callbacks_size);
        }

        if (kDown & KEY_L)
        {
            printf("nn::bluetooth::HidGetReport: 0x%x\n", nn::bluetooth::HidGetReport(&currMac, nn::bluetooth::BluetoothHhReportType::INPUT, 0x01));
        }

        if (true)
        {
            HidReportSharedMem* hidShmem = static_cast<HidReportSharedMem*>(shmem);
            //constexpr u64 curr_index = 0x00;
            for (u64 i = 0; i != sizeof(HidReportSharedMem); ++i)
            {
                if (hidShmem->unk[i] != shmem_copy.unk[i])
                    printf("i%02lX: %02X\n", i, hidShmem->unk[i]);
            }
            memcpy(&shmem_copy, shmem, sizeof(HidReportSharedMem));
        }

        if (kDown & KEY_DUP)
            printf("nn::bluetooth::StartDiscovery: 0x%x\n", nn::bluetooth::StartDiscovery());

        if (kDown & KEY_DDOWN)
            printf("nn::bluetooth::CancelDiscovery: 0x%x\n", nn::bluetooth::CancelDiscovery());

        if (R_SUCCEEDED(eventWait(&register_hid_report_event, 0)))
        {
            //printf("Register HID Report Event went off!\n");
            eventClear(&register_hid_report_event);
        }

        if (R_SUCCEEDED(eventWait(&hid_report_event, 0)))
        {
            printf("HID Report Event went off!\n");
            eventClear(&hid_report_event);
        }

        if (R_SUCCEEDED(eventWait(&hid_event, 0)))
        {
            printf("HID Event went off!\n");
            eventClear(&hid_event);
        }

        if (R_SUCCEEDED(eventWait(&bt_event, 0)))
        {
            printf("Bluetooth Event went off!\n");
            eventClear(&bt_event);
        }

        consoleUpdate(NULL);
    }
    consoleExit(nullptr);

    eventClose(&register_hid_report_event);
    eventClose(&hid_report_event);
    eventClose(&hid_event);
    eventClose(&bt_event);

    nn::bluetooth::FinalizeBluetoothDriver();
}