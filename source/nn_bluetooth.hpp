#pragma once
#include <atomic>
#include <switch.h>

namespace nn::bluetooth
{
    struct Address
    {
        u8 mac[6];

        bool operator==(const nn::bluetooth::Address& a2)
        {
            return mac[0] == a2.mac[0] &&
                   mac[1] == a2.mac[1] &&
                   mac[2] == a2.mac[2] &&
                   mac[3] == a2.mac[3] &&
                   mac[4] == a2.mac[4] &&
                   mac[5] == a2.mac[5];
        }
    };
}; // namespace nn::bluetooth

namespace nn::settings::system
{
    struct BluetoothDevicesSettings
    {
        nn::bluetooth::Address addr;
        char name[32];
        u16 word_x26;
        u8 byte_x28;
        Uuid uuid;
        u8 byte_x39;
        u16 word_x3A;
        u32 flags_maybe;
        u16 vendor_ID;
        u16 product_ID;
        u8 byte_x44;
        u8 byte_x45;
        u16 callbacks_size;
        u64 callbacks[16];
        u8 byte_xC8;
        u8 byte_xC9;
        u16 word_xCA;
        u8 qword_xCC[8];
        u8 byte_xD4;
        u8 unk_xD5[299];
    };
} // namespace nn::settings::system

namespace nn::applet
{
    typedef u64 AppletResourceUserId;
} // namespace nn::applet

namespace nn::os
{
    struct SystemEventType
    {
        nn::os::SystemEventType* event_0;
        nn::os::SystemEventType* event_8;
        bool is_signaled;
        bool is_initially_signaled;
        bool autoclear;
        bool is_finalized;
        uint32_t readable_handle;
        uint32_t writable_handle;
        Mutex sectionImpl;
        CondVar condvar;
        u32 unk;
        u8 unku8_two;
    };

} // namespace nn::os

namespace nn::bluetooth
{

    // not officially defined
    struct PACKED WordByte
    {
        u16 word0;
        u8 byte2;
    };

    struct AdapterProperty
    {
        Address address;
        WordByte unk1;
        u8 name[249];
        u8 unk2;
    };

    enum class BluetoothProperty : u32
    {
        Name = 1,    // The name of the device.
        Address = 2, // The MAC address of the bluetooth adapter
        Unk3 = 3,    //Does nothing on GetAdapterProperty
        Unk4 = 4,    //Does nothing on GetAdapterProperty
        UnkWordByte = 5,
        UnkByte = 6, // Always initally set to 0x68. Only ever changed through SetAdapterProperty
        Unk7 = 7,
    };

    struct BluetoothPinCode
    {
        char pin[16];
    };

    typedef uint32_t EventType;

    struct HidData
    {
        u16 size;
        u8 buffer[700];
    };

    typedef u32 HidEventType;

    struct Plr
    {
        u32 dword0;
        u16 word4;
        u32 dword6;
        u32 dwordA;
        u32 dwordE;
    };

    struct PlrStatistics
    {
        u32 dword0;
        Plr plrs[8];
    };

    struct PACKED ChannelMapSub
    {
        u32 dword0;
        u16 word4;
        //could be another struct below

        u64 qword6;
        u16 wordE;
        u8 byteF;
    };

    struct ChannelMap
    {
        ChannelMapSub sub[7];
    };

    struct LeConnectionParamsImpl
    {
        u32 test;
        u16 test2;
        u16 test3;
    };

    struct LeConnectionParams
    {
        LeConnectionParamsImpl impl;
        u32 dword;
    };

    struct LeAdvertiseData
    {
        u32 dword0;
        u8 byte4;
        u8 buffer1Size;
        u8 buffer1[31];
        u8 gap25;
        u16 word26;
        u8 wordCount;
        u8 gap29[7];
        u16 wordBuffer[58];
        u8 byteA4;
        u8 byteA5;
        u16 wordA6;
        u8 qwordA8[31];
        u8 byteC7;
        u8 byteC8;
        u8 gap1[3];
    };

    struct BleAdvertiseFilter
    {
        u8 index;
        u8 size;
        u8 buffer1Size;
        u8 buffer1[29];
        u8 buffer2[29];
        u8 maybeSize;
    };

    //Not necessarily a struct
    class GattAttributeUuid
    {
    private:
        u32 length;
        u8 byte[16];

    public:
        bool operator==(GattAttributeUuid const& other) const
        {
            if (length != other.length)
                return false;
            if (length == 2)
                return *reinterpret_cast<const u16*>(byte) == *reinterpret_cast<const u16*>(other.byte);

            if (length != 4)
            {
                if (length == 16)
                {
                    for (int i = 0; i != 16; ++i)
                    {
                        if (byte[i] != other.byte[i])
                            return false;
                    }
                    return true;
                }
                return false;
            }
            return *reinterpret_cast<const u32*>(byte) == *reinterpret_cast<const u32*>(other.byte);
        }
    };

    typedef u32 BleEventType;

    struct PACKED LeCoreEventInfo
    {
        u32 dword0;
        u32 dword4;
        u64 qword8;
        u64 qword10;
        u32 dword14;
        u32 dword18;
        u64 qword20;
        u64 qword28;
        u32 dword30;
        u64 qword34;
        u64 qword3C;
        u32 dword44;
        u16 word48;
        u8 gap0[249];
        u8 byte143;
        u32 dword144;
        u8 gap1[202];
        u16 word212;
        u16 word214;
        u8 gap2[490];
    };

    //Not necessarily a struct
    struct GattId
    {
        u8 byte0;
        u8 gap[3];
        u32 length;
        u8 id[16];
    };

    typedef u32 BluetoothTransport;

    typedef u8 BluetoothSspVariant;

    typedef Result BluetoothFatalReason;

    // Basic report types, may not be accurate
    enum class BluetoothHhReportType : u32
    {
        INPUT = 0x01,
        OUTPUT = 0x02,
        FEATURE = 0x03,
    };

    void InitializeBluetoothDriverByDfc();
    Result InitializeBluetoothDriver();
    void FinalizeBluetoothDriver();

    Result GetIsManufacturingMode(bool* out);

    Result EmulateBluetoothCrash(BluetoothFatalReason reason);

    Result GetBleChannelMap(u8* outBuffer, u16 size);

    Result InitializeBluetooth(Event* outEvent);

    Result EnableBluetooth();
    Result DisableBluetooth();
    Result CleanupBluetooth();

    Result GetAdapterProperties(AdapterProperty* out);

    // Depending on each BluetoothProperty, it will return a different buffer. See comments in BluetoothProperty
    Result GetAdapterProperty(BluetoothProperty type, u8* buffer, u16 size);
    Result SetAdapterProperty(BluetoothProperty type, const u8* buffer, u16 size);

    Result StartDiscovery();
    Result CancelDiscovery();

    // BluetoothTransport doesn't matter here
    Result CreateBond(Address const* address, BluetoothTransport transport);
    Result RemoveBond(Address const* address);
    Result CancelBond(Address const* address);

    // Only the mac address matters here, other values are discarded.
    Result PinReply(Address const* address, bool unk1, BluetoothPinCode const* pin, u8 unk2);

    // Only the mac address matters here, other values are discarded. Pair the bond
    Result SspReply(Address const* address, BluetoothSspVariant variant, bool unk1, u32 unk2);

    // Get the event info and put it into outBuffer. Max buffer size is 0x400
    Result GetEventInfo(EventType* outEvent, u8* outBuffer, u16 bufferSize);

    // Bluetooth hidhost functions

    //Seems to go off when an HID device is connected?
    Result InitializeHid(Event* out, u16 unk);
    Result HidConnect(Address const* address);
    Result HidDisconnect(Address const* address);
    Result HidSendData(Address const* address, HidData const* data);
    Result HidSendData2(Address const* address, HidData const* data);
    Result HidSetReport(Address const* address, BluetoothHhReportType reportType, HidData const* buffer);
    Result HidGetReport(Address const* address, BluetoothHhReportType reportType, u8 unk);
    Result HidWakeController(Address const* address, u16 propSetting);
    Result HidAddPairedDevice(nn::settings::system::BluetoothDevicesSettings const* settings);
    Result HidGetPairedDevice(Address const* address, nn::settings::system::BluetoothDevicesSettings* settings);
    Result CleanupHid();
    Result HidGetEventInfo(HidEventType* out, u8* outBuffer, u16 bufferSize);
    Result RegisterHidReportEvent(Event* outEvent);
    //this returns a shared mem pointer to a circular buffer
    Result HidGetReportEventInfo(void** shmemAddr);

    // Bluetooth extension functions

    Result ExtSetTsi(Address const* address, u8 tsi);
    Result ExtSetBurstMode(Address const* address, bool burstMode);
    Result ExtSetZeroRetran(Address const* address, u8* buffer, u8 bufferSize);
    Result ExtSetMcMode(bool mcMode);
    Result ExtStartLlrMode();
    Result ExtExitLlrMode();
    Result ExtSetRadio(bool radio);
    Result ExtSetVisibility(bool discoverable, bool connectable);
    Result ExtSetTbfcScan(bool tbfcScan);
    Result ExtGetPendingConnections();

    Result GetLatestPlr(PlrStatistics* out);
    Result GetChannelMap(ChannelMap* out);

    Result EnableBluetoothBoostSetting(bool boostSetting);
    Result IsBluetoothBoostSettingEnabled(bool* outBoostSetting);

    Result EnableBluetoothAfhSetting(bool afhSetting);
    Result IsBluetoothAfhSettingEnabled(bool* outAfhSetting);

    // Bluetooth low-energy functions

    Result InitializeBluetoothLe(Event* outEvent);
    Result EnableBluetoothLe();
    Result DisableBluetoothLe();
    Result CleanupBluetoothLe();
    Result SetLeVisibility(bool discoverable, bool connectable);
    Result SetLeConnectionParameter(Address const* address, LeConnectionParams const* param);
    Result SetLeDefaultConnectionParameter(LeConnectionParams const* param);
    Result SetLeAdvertiseData(LeAdvertiseData advertise);
    Result SetLeAdvertiseParameter(Address const* address, u16 unk, u16 unk2);
    Result StartLeScan();
    Result StopLeScan();
    Result SetLeScanParameter(u16 param1, u16 param2);
    Result AddLeScanFilterCondition(BleAdvertiseFilter const* filter);
    Result DeleteLeScanFilterCondition(BleAdvertiseFilter const* filter);
    // there are 19 LE scan filters total, from 0 to 18
    Result DeleteLeScanFilter(u8 index);
    Result ClearLeScanFilters();
    Result EnableLeScanFilter(bool unk);
    Result RegisterLeClient(GattAttributeUuid const* uuid);
    Result UnregisterLeClient(u8 id);
    Result UnregisterLeClientAll();
    // AppletResourceUserId seems unused
    Result LeClientConnect(nn::applet::AppletResourceUserId const& uid, u8 id, Address const* address, bool unk);
    Result LeClientCancelConnection(u8 id, Address const* address, bool unk);
    // I don't actually know what the s32 means. It just searches for the client that has the same variable
    Result LeClientDisconnect(s32 clientState);
    Result LeClientGetAttributes(s32 clientState, Address const* address);
    Result LeClientDiscoverService(s32 clientState, GattAttributeUuid const& uuid);
    Result LeClientConfigureMtu(s32 clientState, u16 mtu);
    Result RegisterLeServer(GattAttributeUuid const& uuid);
    Result UnregisterLeServer(u8 serverId);
    Result LeServerConnect(u8 serverId, Address const* address, bool connectBool);
    // address is unused on [9.0.0+]
    Result LeServerDisconnect(u8 serverId, Address const* address);
    Result CreateLeService(u8 serverId, GattAttributeUuid const& uuid, u8 unk, bool connectBool);
    Result StartLeService(u8 serverId, GattAttributeUuid const& uuid);
    Result AddLeCharacteristic(u8 serverId, GattAttributeUuid const& uuid, GattAttributeUuid const& uuid2, u16 unk, u8 unk2);
    Result AddLeDescriptor(u8 serverId, GattAttributeUuid const& uuid, GattAttributeUuid const& uuid2, u16 unk);
    Result GetLeCoreEventInfo(BleEventType* outEvent, LeCoreEventInfo* outInfo);
    Result LeGetFirstCharacteristic(GattId* outId, u8* outByte, u32 unk, GattId const& gattId, bool unk2, GattAttributeUuid const& uuid);
    Result LeGetNextCharacteristic(GattId* outId, u8* outByte, u32 unk, GattId const& gattId, bool unk2, GattId const& gattId2, GattAttributeUuid const& uuid);
    Result LeGetFirstDescriptor(GattId* out, u32 unk, GattId const& gattId, bool unk2, GattId const& gattId2, GattAttributeUuid const& uuid);
    Result LeGetNextDescriptor(GattId* out, u32 unk, GattId const& gattId, bool unk2, GattId const& gattId2, GattId const& gattId3, GattAttributeUuid const& uuid);
    Result RegisterLeCoreDataPath(GattAttributeUuid const& uuid);
    Result UnregisterLeCoreDataPath(GattAttributeUuid const& uuid);
    Result RegisterLeHidDataPath(GattAttributeUuid const& uuid);
    Result UnregisterLeHidDataPath(GattAttributeUuid const& uuid);
    Result RegisterLeDataPath(GattAttributeUuid const& uuid);
    Result UnregisterLeDataPath(GattAttributeUuid const& uuid);
    Result RegisterBleHidEvent(Event* outEvent);
    Result GetLeHidEventInfo(BleEventType* outEvent, u8* buffer, u16 size);
    Result LeClientReadCharacteristic(u32 connectedState, GattId const& gattId, bool unk, GattId const& gattId2, u8 unk2);
    Result LeClientReadDescriptor(u32 connectedState, GattId const& gattId, bool unk, GattId const& gattId2, GattId const& gattId3, u8 unk2);
    Result LeClientWriteCharacteristic(u32 connectedState, GattId const& gattId, bool unk, GattId const& gattId2, u8 const* buffer, u16 size, u8 unk2, bool unk3);
    Result LeClientWriteDescriptor(u32 connectedState, GattId const& gattId, bool unk, GattId const& gattId2, GattId const& gattId3, u8 const* buffer, u16 size, u8 unk2);
    Result LeClientRegisterNotification(u32 connectedState, GattId const& gattId, bool unk, GattId const& gattId2);
    Result LeClientDeregisterNotification(u32 connectedState, GattId const& gattId, bool unk, GattId const& gattId2);

    namespace user
    {
        Result FinalizeBluetoothUserInterface();
        Result RegisterBleEvent(nn::os::SystemEventType*);
        Result LeClientReadCharacteristic(u32, GattId, bool, GattId, u8);
        Result LeClientReadDescriptor(u32, GattId, bool, GattId, GattId, u8);
        Result LeClientWriteCharacteristic(u32, GattId, bool, GattId, u8 const* buffer, u16, u8, bool);
        Result LeClientWriteDescriptor(u32, GattId, bool, GattId, GattId, u8 const* buffer, u16, u8);
        Result LeClientRegisterNotification(u32, GattId, bool, GattId);
        Result LeClientDeregisterNotification(u32, GattId, bool, GattId);
        Result SetLeResponse(u8, GattAttributeUuid, GattAttributeUuid, u8 const* buffer, u16);
        Result LeSendIndication(u8, GattAttributeUuid, GattAttributeUuid, u8 const* buffer, u8, bool);
        Result GetLeEventInfo(BleEventType*, u8* buffer, u16);
    } // namespace user

#define CIRCBUF_SIZE 10000

    class CircularBuffer
    {
    public:
        enum CircularBufferType : u32
        {
            CB_HID_REPORT = 0x1,
            CB_BLUETOOTH = 0x2,
            CB_BLE = 0x3,
            CB_BLE_CORE = 0x4,
            CB_BLE_HID = 0x5,
        };

        // Only use this to cast pointers. You will never need a buffer this large
        struct Packet
        {
            u8 packetType;
            u8 padding[7];
            u64 packetTick;
            u64 bufferSize;
            u8 buffer[CIRCBUF_SIZE];
        };

    private:
        Mutex section;
        u8 gap4[4];
        Event* eventPointer;
        u8 buffer[CIRCBUF_SIZE];
        std::atomic<s32> writeOffset; //tail
        std::atomic<s32> readOffset;  //head
        u64 bufferSize;
        char name[16];
        bool some_bool_maybe;
        bool initialized;
        u32 dword2742;
        u32 cb_bufferType;
        CircularBufferType bufferType;
        bool readyFlag;

    public:
        CircularBuffer();
        void Initialize(char* name, Event* event);
        bool IsInitialized();
        u64 GetWriteableSize();
        s32 _getWriteOffset();
        s32 _getReadOffset();
        u32 Write(u8, const void*, u64);
        int _write(u8, const void*, u64);
        Packet* Read();
        Packet* _read();
        u32 Free();
        void DiscardOldPackets(u8, u32);

        void _updateUtilization();
    };

    Result InitializeBle();

} // namespace nn::bluetooth