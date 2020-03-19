#pragma once
#include <switch.h>

namespace nn::bluetooth
{
    struct Address
    {
        u8 mac[6];
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
        u32 dword_x3C;
        u16 word_x40;
        u16 word_x42;
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
        u8 unk_byte_two;
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

    struct PlrStatistics
    {
    };

    struct ChannelMap
    {
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

    struct BleEventType
    {
    };

    //Not necessarily a struct
    struct GattId
    {
    };

    typedef u32 BluetoothTransport;

    typedef u8 BluetoothSspVariant;

    typedef Result BluetoothFatalReason;

    typedef u32 BluetoothHhReportType;

    void InitializeBluetoothDriverByDfc();
    Result InitializeBluetoothDriver();
    void FinalizeBluetoothDriver();

    bool GetIsManufacturingMode();

    Result EmulateBluetoothCrash(BluetoothFatalReason reason);

    Result InitializeBluetooth(Event* outEvent);

    Result EnableBluetooth();
    Result DisableBluetooth();
    Result CleanupBluetooth();

    Result GetAdapterProperties(AdapterProperty* out);

    // buffer and size get contatenated into a struct on the stack
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

    Result InitializeHid(Event* out, u16 unk);
    Result HidConnect(Address const* address);
    Result HidDisconnect(Address const* address);
    Result HidSendData(Address const* address, HidData const* data);
    Result HidSendData(Address const* address, u8 const* buffer, u16 size);
    Result HidSetReport(Address const* address, BluetoothHhReportType reportType, HidData const* buffer);
    Result HidGetReport(Address const* address, BluetoothHhReportType reportType, u8 unk);
    Result HidWakeController(Address const* address, u16 propSetting);
    Result HidAddPairedDevice(nn::settings::system::BluetoothDevicesSettings const* settings);
    Result HidGetPairedDevice(Address const* address, nn::settings::system::BluetoothDevicesSettings* settings);
    Result CleanupHid();
    Result HidGetEventInfo(HidEventType* out, u8* outBuffer, u16 bufferSize);
    Result RegisterHidReportEvent(Event* outEvent);
    Result HidGetReportEventInfo(Event* outEvent);

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
    Result SetLeScanParameter(u16, u16);
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
    Result LeClientDisconnect(u32);
    Result LeClientGetAttributes(u32, Address const* address);
    Result LeClientDiscoverService(u32, GattAttributeUuid);
    Result LeClientConfigureMtu(u32, u16);
    Result RegisterLeServer(GattAttributeUuid);
    Result UnregisterLeServer(u8);
    Result LeServerConnect(u8, Address const* address, bool);
    Result LeServerDisconnect(u8, Address const* address);
    Result CreateLeService(u8, GattAttributeUuid, u8, bool);
    Result StartLeService(u8, GattAttributeUuid);
    Result AddLeCharacteristic(u8, GattAttributeUuid, GattAttributeUuid, u16, u8);
    Result AddLeDescriptor(u8, GattAttributeUuid, GattAttributeUuid, u16);
    Result GetLeCoreEventInfo(BleEventType*, u8* buffer, u16 unk);
    Result LeGetFirstCharacteristic(GattId*, u8* buffer, u32, GattId, bool, GattAttributeUuid);
    Result LeGetNextCharacteristic(GattId*, u8* buffer, u32, GattId, bool, GattAttributeUuid);
    Result LeGetFirstDescriptor(GattId*, u32, GattId, bool, GattId, GattAttributeUuid);
    Result LeGetNextDescriptor(GattId*, u32, GattId, bool, GattId, GattId, GattAttributeUuid);
    Result RegisterLeCoreDataPath(GattAttributeUuid const&);
    Result UnregisterLeCoreDataPath(GattAttributeUuid const&);
    Result RegisterLeHidDataPath(GattAttributeUuid const&);
    Result UnregisterLeHidDataPath(GattAttributeUuid const&);
    Result RegisterLeDataPath(GattAttributeUuid const&);
    Result UnregisterLeDataPath(GattAttributeUuid const&);
    Result RegisterBleHidEvent(nn::os::SystemEventType*);
    Result GetLeHidEventInfo(BleEventType*, u8*, u16);
    Result LeClientReadCharacteristic(u32, GattId, bool, GattId, u8);
    Result LeClientReadDescriptor(u32, GattId, bool, GattId, GattId, u8);
    Result LeClientWriteCharacteristic(u32, GattId, bool, GattId, u8 const* buffer, u16, u8, bool);
    Result LeClientWriteDescriptor(u32, GattId, bool, GattId, GattId, u8 const* buffer, u16, u8);
    Result LeClientRegisterNotification(u32, GattId, bool, GattId);
    Result LeClientDeregisterNotification(u32, GattId, bool, GattId);

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

    class CircularBuffer
    {
    private:
    public:
        CircularBuffer() {}
        bool IsInitialized();
        u64 GetWriteableSize();
        u32 _getWriteOffset();
        u32 _getReadOffset();
        u32 Write(u8, const void*, u64);
        u32 _write(u8, const void*, u64);
        CircularBuffer _updateUtilization();
    };

    Result InitializeBle();

} // namespace nn::bluetooth