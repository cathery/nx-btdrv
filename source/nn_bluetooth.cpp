#include "nn_bluetooth.hpp"
#include <string.h>
#include <switch.h>

static Service btdrv;

#define btdrvDispatch(...) serviceDispatch(&btdrv, __VA_ARGS__)
#define btdrvDispatchIn(...) serviceDispatchIn(&btdrv, __VA_ARGS__)
#define btdrvDispatchInOut(...) serviceDispatchInOut(&btdrv, __VA_ARGS__)
#define btdrvDispatchOut(...) serviceDispatchOut(&btdrv, __VA_ARGS__)

static Result _btdrvGetHandle(Handle* handle_out, u32 cmd_id)
{
    return btdrvDispatch(
        cmd_id,
        .out_handle_attrs = {SfOutHandleAttr_HipcCopy},
        .out_handles = handle_out);
}

static Result _btdrvGetEvent(Event* out_event, bool autoclear, u32 cmd_id)
{
    Handle tmp_handle = INVALID_HANDLE;
    Result rc = 0;

    rc = _btdrvGetHandle(&tmp_handle, cmd_id);
    if (R_SUCCEEDED(rc))
        eventLoadRemote(out_event, tmp_handle, autoclear);
    return rc;
}

namespace nn::bluetooth
{
    void InitializeBluetoothDriverByDfc()
    {
    }

    Result InitializeBluetoothDriver()
    {
        Result rc = smGetService(&btdrv, "btdrv");
        if (R_FAILED(rc))
            return rc;
        return btdrvDispatch(0);
    }

    void FinalizeBluetoothDriver()
    {
        serviceClose(&btdrv);
    }

    Result InitializeBluetooth(Event* outEvent)
    {
        return _btdrvGetEvent(outEvent, false, 1);
    }

    Result EnableBluetooth()
    {
        return btdrvDispatch(2);
    }

    Result DisableBluetooth()
    {
        return btdrvDispatch(3);
    }

    Result CleanupBluetooth()
    {
        return btdrvDispatch(4);
    }

    Result GetAdapterProperties(AdapterProperty* out)
    {
        return btdrvDispatch(
            5,
            .buffer_attrs = {SfBufferAttr_Out | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize},
            .buffers = {{out, sizeof(AdapterProperty)}});
    }

    Result GetAdapterProperty(BluetoothProperty type, u8* buffer, u16 size)
    {
        static_assert(sizeof(type) == 0x4, "GetAdapterProperty: Bad Input");

        return btdrvDispatchIn(
            6, type,
            .buffer_attrs = {SfBufferAttr_Out | SfBufferAttr_HipcPointer},
            .buffers = {{buffer, size}});
    }

    Result SetAdapterProperty(BluetoothProperty type, const u8* buffer, u16 size)
    {
        static_assert(sizeof(type) == 0x4, "SetAdapterProperty: Bad Input");

        return btdrvDispatchIn(
            7, type,
            .buffer_attrs = {SfBufferAttr_In | SfBufferAttr_HipcPointer},
            .buffers = {{buffer, size}});
    }

    Result StartDiscovery()
    {
        return btdrvDispatch(8);
    }

    Result CancelDiscovery()
    {
        return btdrvDispatch(9);
    }

    Result CreateBond(Address const* address, BluetoothTransport transport)
    {
        struct
        {
            Address address;
            u32 transport; // unused?
        } in = {*address, transport};

        static_assert(sizeof(in) == 0xC, "CreateBond: Bad Input");

        return btdrvDispatchIn(10, in);
    }

    Result RemoveBond(Address const* address)
    {
        return btdrvDispatchIn(11, *address);
    }

    Result CancelBond(Address const* address)
    {
        return btdrvDispatchIn(12, *address);
    }

    Result PinReply(Address const* address, bool unk1, BluetoothPinCode const* pin, u8 pinlength)
    {
        struct
        {
            Address addr;
            bool unk1;            // unused?
            u8 pinlength;         // unused?
            BluetoothPinCode pin; // unused?
        } in = {*address, unk1, pinlength, *pin};

        static_assert(sizeof(in) == 0x18, "PinReply: Bad Input");

        return btdrvDispatchIn(13, in);
    }

    Result SspReply(Address const* address, BluetoothSspVariant variant, bool unk1, u32 unk2)
    {
        struct
        {
            Address addr;
            BluetoothSspVariant variant; // unused?
            bool unk1;                   // unused?
            u32 unk2;                    // unused?
        } in = {*address, variant, unk1, unk2};

        static_assert(sizeof(in) == 0xC, "SspReply: Bad Input");

        return btdrvDispatchIn(14, in);
    }

    Result GetEventInfo(EventType* outEvent, u8* outBuffer, u16 bufferSize)
    {
        return btdrvDispatchOut(
            15, *outEvent,
            .buffer_attrs = {SfBufferAttr_Out | SfBufferAttr_HipcPointer},
            .buffers = {{outBuffer, bufferSize}});
    }

    Result InitializeHid(Event* out, u16 unk)
    {
        Handle tmp_handle = INVALID_HANDLE;
        Result rc;

        rc = btdrvDispatchIn(
            16, unk,
            .out_handle_attrs = {SfOutHandleAttr_HipcCopy},
            .out_handles = &tmp_handle);

        if (R_SUCCEEDED(rc))
            eventLoadRemote(out, tmp_handle, false);
        return rc;
    }

    Result HidConnect(Address const* address)
    {
        return btdrvDispatchIn(17, *address);
    }

    Result HidDisconnect(Address const* address)
    {
        return btdrvDispatchIn(18, *address);
    }

    Result HidSendData(Address const* address, HidData const* out)
    {
        return btdrvDispatchIn(
            19, *address,
            .buffer_attrs = {SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize},
            .buffers = {{out, sizeof(HidData)}});
    }

    Result HidSendData2(Address const* address, HidData const* out)
    {
        return btdrvDispatchIn(
            20, *address,
            .buffer_attrs = {SfBufferAttr_In | SfBufferAttr_HipcPointer},
            .buffers = {{out, sizeof(HidData)}});
    }

    Result HidSetReport(Address const* address, BluetoothHhReportType reportType, HidData const* buffer)
    {
        struct
        {
            Address address;
            BluetoothHhReportType reportType;
        } in = {*address, reportType};

        static_assert(sizeof(in) == 12, "HidSetReport: Bad Input");

        return btdrvDispatchIn(
            21, in,
            .buffer_attrs = {SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize},
            .buffers = {{buffer, sizeof(HidData)}});
    }

    Result HidGetReport(Address const* address, BluetoothHhReportType reportType, u8 unk)
    {
        //TODO: test
        struct
        {
            Address address;
            u8 unk;
            BluetoothHhReportType reportType;
        } in = {*address, unk, reportType};

        static_assert(sizeof(in) == 12, "HidGetReport: Bad Input");

        return btdrvDispatchIn(22, in);
    }

    Result HidWakeController(Address const* address, u16 propSetting)
    {
        //TODO: test
        struct
        {
            Address address;
            u16 propSetting;
        } in = {*address, propSetting};

        static_assert(sizeof(in) == 8, "HidWakeController: Bad Input");

        return btdrvDispatchIn(23, in);
    }

    Result HidAddPairedDevice(nn::settings::system::BluetoothDevicesSettings const* settings)
    {
        //TODO: test

        static_assert(sizeof(nn::settings::system::BluetoothDevicesSettings) == 0x200, "HidAddPairedDevice: Bad Input");

        return btdrvDispatch(
            24,
            .buffer_attrs = {SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize},
            .buffers = {{settings, sizeof(nn::settings::system::BluetoothDevicesSettings)}});
    }

    Result HidGetPairedDevice(Address const* address, nn::settings::system::BluetoothDevicesSettings* settings)
    {
        //TODO: test

        static_assert(sizeof(nn::settings::system::BluetoothDevicesSettings) == 0x200, "HidAddPairedDevice: Bad Input");

        return btdrvDispatchIn(
            25, *address,
            .buffer_attrs = {SfBufferAttr_Out | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize},
            .buffers = {{settings, sizeof(nn::settings::system::BluetoothDevicesSettings)}});
    }

    Result CleanupHid()
    {
        //TODO: test
        return btdrvDispatch(26);
    }

    Result HidGetEventInfo(HidEventType* out, u8* outBuffer, u16 bufferSize)
    {
        //TODO: test
        return btdrvDispatchOut(
            27, *out,
            .buffer_attrs = {SfBufferAttr_Out | SfBufferAttr_HipcPointer},
            .buffers = {{outBuffer, bufferSize}});
    }

    Result ExtSetTsi(Address const* address, u8 tsi)
    {
        //TODO: test

        struct
        {
            Address address;
            u8 tsi;
        } in = {*address, tsi};

        static_assert(sizeof(in) == 7, "ExtSetTsi: Bad Input");

        return btdrvDispatchIn(28, in);
    }

    Result ExtSetBurstMode(Address const* address, bool burstMode)
    {
        //TODO: test

        struct
        {
            Address address;
            bool burstMode;
        } in = {*address, burstMode};

        static_assert(sizeof(in) == 7, "ExtSetBurstMode: Bad Input");

        return btdrvDispatchIn(29, in);
    }

    Result ExtSetZeroRetran(Address const* address, u8* buffer, u8 bufferSize)
    {
        //TODO: test
        return btdrvDispatchIn(
            30, *address,
            .buffer_attrs = {SfBufferAttr_In | SfBufferAttr_HipcPointer},
            .buffers = {{buffer, bufferSize}});
    }

    Result ExtSetMcMode(bool mcMode)
    {
        //TODO: test
        return btdrvDispatchIn(31, mcMode);
    }

    Result ExtStartLlrMode()
    {
        //TODO: test
        return btdrvDispatch(32);
    }

    Result ExtExitLlrMode()
    {
        //TODO: test
        return btdrvDispatch(33);
    }

    Result ExtSetRadio(bool radio)
    {
        //TODO: test
        return btdrvDispatchIn(34, radio);
    }

    Result ExtSetVisibility(bool discoverable, bool connectable)
    {
        struct
        {
            bool discoverable;
            bool connectable;
        } in = {discoverable, connectable};

        static_assert(sizeof(in) == 0x2, "ExtSetVisibility: Bad Input");

        return btdrvDispatchIn(35, in);
    }

    Result ExtSetTbfcScan(bool tbfcScan)
    {
        //TODO: test
        return btdrvDispatchIn(36, tbfcScan);
    }

    Result RegisterHidReportEvent(Event* outEvent)
    {
        //TODO: test
        return _btdrvGetEvent(outEvent, false, 37);
    }

    Result HidGetReportEventInfo(Event* outEvent)
    {
        //TODO: test
        return _btdrvGetEvent(outEvent, false, 38);
    }

    Result GetLatestPlr(PlrStatistics* out)
    {
        //TODO: test
        static_assert(sizeof(PlrStatistics) == 0xA4, "GetLatestPlr: PlrStatistics has incorrect size");
        return btdrvDispatch(
            39,
            .buffer_attrs = {SfBufferAttr_Out | SfBufferAttr_HipcMapAlias | SfBufferAttr_FixedSize},
            .buffers = {{out, sizeof(PlrStatistics)}});
    }

    Result ExtGetPendingConnections()
    {
        //TODO: test
        return btdrvDispatch(40);
    }

    Result GetChannelMap(ChannelMap* out)
    {

        //TODO: test
        static_assert(sizeof(ChannelMap) == 0x77, "GetChannelMap: ChannelMap has incorrect size");
        return btdrvDispatch(
            41,
            .buffer_attrs = {SfBufferAttr_Out | SfBufferAttr_HipcMapAlias | SfBufferAttr_FixedSize},
            .buffers = {{out, sizeof(ChannelMap)}});
    }

    Result EnableBluetoothBoostSetting(bool boostSetting)
    {
        //TODO: test
        return btdrvDispatchIn(42, boostSetting);
    }

    Result IsBluetoothBoostSettingEnabled(bool* outBoostSetting)
    {
        //TODO: test
        bool tmp_setting;
        Result rc = btdrvDispatchOut(43, tmp_setting);
        if (outBoostSetting && R_SUCCEEDED(rc))
            *outBoostSetting = tmp_setting;
        return rc;
    }

    Result EnableBluetoothAfhSetting(bool afhSetting)
    {
        //TODO: test
        return btdrvDispatchIn(44, afhSetting);
    }

    Result IsBluetoothAfhSettingEnabled(bool* outAfhSetting)
    {
        //TODO: test
        bool tmp_setting;
        Result rc = btdrvDispatchOut(45, tmp_setting);
        if (outAfhSetting && R_SUCCEEDED(rc))
            *outAfhSetting = tmp_setting;
        return rc;
    }

    Result InitializeBluetoothLe(Event* outEvent)
    {
        //TODO: test
        return _btdrvGetEvent(outEvent, false, 46);
    }

    Result EnableBluetoothLe()
    {
        //TODO: test
        return btdrvDispatch(47);
    }

    Result DisableBluetoothLe()
    {
        //TODO: test
        return btdrvDispatch(48);
    }

    Result CleanupBluetoothLe()
    {
        //TODO: test
        return btdrvDispatch(49);
    }

    Result SetLeVisibility(bool discoverable, bool connectable)
    {
        //TODO: test
        struct
        {
            bool discoverable;
            bool connectable;
        } in = {discoverable, connectable};

        static_assert(sizeof(in) == 0x2, "SetLeVisibility: Bad Input");

        return btdrvDispatchIn(50, in);
    }

    Result SetLeConnectionParameter(Address const* address, LeConnectionParams const* param)
    {
        //TODO: test
        struct
        {
            Address address;
            LeConnectionParams param;
        } in = {*address, *param};

        static_assert(sizeof(in) == 20, "SetLeConnectionParameter: Bad Input");

        return btdrvDispatchIn(51, in);
    }

    Result SetLeDefaultConnectionParameter(LeConnectionParams const* param)
    {
        //TODO: test
        static_assert(sizeof(LeConnectionParams) == 12, "SetLeDefaultConnectionParameter: Bad Input");

        return btdrvDispatchIn(52, *param);
    }

    Result SetLeAdvertiseData(LeAdvertiseData const* advertise)
    {
        //TODO: test
        return btdrvDispatch(
            53,
            .buffer_attrs = {SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize},
            .buffers = {{advertise, sizeof(LeAdvertiseData)}});
    }

    Result SetLeAdvertiseParameter(Address const* address, u16 unk, u16 unk2)
    {
        //TODO: test
        struct
        {
            Address address;
            u16 unk;
            u16 unk2; // unused?
        } in = {*address, unk, unk2};

        static_assert(sizeof(in) == 10, "SetLeAdvertiseParameter: Bad Input");

        return btdrvDispatchIn(54, in);
    }

    Result StartLeScan()
    {
        //TODO: test
        return btdrvDispatch(55);
    }

    Result StopLeScan()
    {
        //TODO: test
        return btdrvDispatch(56);
    }

    Result AddLeScanFilterCondition(BleAdvertiseFilter const* filter)
    {
        //TODO: test
        return btdrvDispatch(
            57,
            .buffer_attrs = {SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize},
            .buffers = {{filter, sizeof(BleAdvertiseFilter)}});
    }

    Result DeleteLeScanFilterCondition(BleAdvertiseFilter const* filter)
    {
        //TODO: test
        return btdrvDispatch(
            58,
            .buffer_attrs = {SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize},
            .buffers = {{filter, sizeof(BleAdvertiseFilter)}});
    }

    Result DeleteLeScanFilter(u8 index)
    {
        //TODO: test
        return btdrvDispatchIn(59, index);
    }

    Result ClearLeScanFilters()
    {
        //TODO: test
        return btdrvDispatch(60);
    }

    Result EnableLeScanFilter(bool unk)
    {
        //TODO: test
        return btdrvDispatchIn(61, unk);
    }

    Result RegisterLeClient(GattAttributeUuid const* uuid)
    {
        //TODO: test
        static_assert(sizeof(GattAttributeUuid) == 0x14, "RegisterLeClient: Bad Input");
        return btdrvDispatchIn(62, *uuid);
    }

    Result UnregisterLeClient(u8 id)
    {
        //TODO: test
        return btdrvDispatchIn(63, id);
    }

    Result UnregisterLeClientAll()
    {
        //TODO: test
        return btdrvDispatch(64);
    }

    Result LeClientConnect(nn::applet::AppletResourceUserId const& uid, u8 id, Address const* address, bool unk)
    {
        //TODO: test
        struct
        {
            u8 id;
            Address address;
            bool unk;
            nn::applet::AppletResourceUserId uid;
        } in = {id, *address, unk, uid};

        static_assert(sizeof(in) == 0x10, "LeClientConnect: Bad Input");

        return btdrvDispatchIn(65, in);
    }

    Result LeClientCancelConnection(u8 id, Address const* address, bool unk)
    {
        //TODO: test
        struct
        {
            u8 id;
            Address address;
            bool unk;
        } in = {id, *address, unk};

        static_assert(sizeof(in) == 8, "LeClientCancelConnection: Bad Input");

        return btdrvDispatchIn(66, in);
    }

    Result LeClientDisconnect(s32 clientState)
    {
        //TODO: test
        return btdrvDispatchIn(67, clientState);
    }

    Result LeClientGetAttributes(s32 clientState, Address const* address)
    {
        //TODO: test
        return btdrvDispatchIn(68, clientState);
    }

    Result LeClientDiscoverService(s32 clientState, GattAttributeUuid const& uuid)
    {
        //TODO: test
        struct
        {
            s32 clientState;
            GattAttributeUuid uuid;
        } in = {clientState, uuid};

        static_assert(sizeof(in) == 0x18, "LeClientDiscoverService: Bad Input");

        return btdrvDispatchIn(69, in);
    }

    Result LeClientConfigureMtu(s32 clientState, u16 mtu)
    {
        //TODO: test
        struct
        {
            u16 mtu;
            s32 clientState;
        } in = {mtu, clientState};

        static_assert(sizeof(in) == 8, "LeClientConfigureMtu: Bad Input");

        return btdrvDispatchIn(70, in);
    }

    Result RegisterLeServer(GattAttributeUuid const& uuid)
    {
        //TODO: test
        static_assert(sizeof(uuid) == 0x14, "RegisterLeServer: Bad Input");

        return btdrvDispatchIn(71, uuid);
    }

    Result UnregisterLeServer(u8 serverId)
    {
        //TODO: test
        return btdrvDispatchIn(72, serverId);
    }

    Result LeServerConnect(u8 serverId, Address const* address, bool connectBool)
    {
        //TODO: test
        struct
        {
            u8 serverId;
            Address address;
            bool connectBool;
        } in = {serverId, *address, connectBool};

        static_assert(sizeof(in) == 8, "LeServerConnect: Bad Input");

        return btdrvDispatchIn(73, in);
    }

    Result LeServerDisconnect(u8 serverId, Address const* address)
    {
        //TODO: test
        return btdrvDispatchIn(74, serverId);
    }

    Result CreateLeService(u8 serverId, GattAttributeUuid const& uuid, u8 unk, bool connectBool)
    {
        //TODO: test
        struct
        {
            u8 serverId;
            u8 unk;
            bool connectBool;
            GattAttributeUuid uuid;
        } in = {serverId, unk, connectBool, uuid};

        static_assert(sizeof(in) == 0x18, "CreateLeService: Bad Input");

        return btdrvDispatchIn(75, in);
    }

    Result StartLeService(u8 serverId, GattAttributeUuid const& uuid)
    {
        //TODO: test
        struct
        {
            u8 serverId;
            GattAttributeUuid uuid;
        } in = {serverId, uuid};

        static_assert(sizeof(in) == 0x18, "StartLeService: Bad Input");

        return btdrvDispatchIn(76, in);
    }

    Result AddLeCharacteristic(u8 serverId, GattAttributeUuid const& uuid, GattAttributeUuid const& uuid2, u16 unk, u8 unk2)
    {
        //TODO: test
        struct
        {
            u8 serverId;
            u8 unk2;
            u16 unk;
            GattAttributeUuid uuid;
            GattAttributeUuid uuid2;
        } in = {serverId, unk2, unk, uuid, uuid2};

        static_assert(sizeof(in) == 0x2C, "AddLeCharacteristic: Bad Input");

        return btdrvDispatchIn(77, in);
    }

    Result AddLeDescriptor(u8 serverId, GattAttributeUuid const& uuid, GattAttributeUuid const& uuid2, u16 unk)
    {
        //TODO: test
        struct
        {
            u8 serverId;
            u16 unk;
            GattAttributeUuid uuid;
            GattAttributeUuid uuid2;
        } in = {serverId, unk, uuid, uuid2};

        static_assert(sizeof(in) == 0x2C, "AddLeDescriptor: Bad Input");
        return btdrvDispatchIn(78, in);
    }

    Result GetLeCoreEventInfo(BleEventType* outEvent, LeCoreEventInfo* outInfo)
    {
        //TODO: test
        static_assert(sizeof(LeCoreEventInfo) == 0x400, "GetLeCoreEventInfo: Bad Input");
        return btdrvDispatchOut(
            79, *outEvent,
            .buffer_attrs = {SfBufferAttr_Out | SfBufferAttr_HipcPointer},
            .buffers = {{outInfo, sizeof(LeCoreEventInfo)}});
    }

    Result LeGetFirstCharacteristic(GattId* outId, u8* outByte, u32 unk, GattId const& gattId, bool unk2, GattAttributeUuid const& uuid)
    {
        //TODO: test
        struct
        {
            bool unk2;
            u32 unk;
            GattId gattId;
            GattAttributeUuid uuid;
        } in = {unk2, unk, gattId, uuid};

        struct
        {
            u8 byte;
            GattId id;
        } out;

        static_assert(sizeof(in) == 0x34, "LeGetFirstCharacteristic: Bad Input");
        static_assert(sizeof(out) == 0x1C, "LeGetFirstCharacteristic: Bad Output");

        Result rc = btdrvDispatchInOut(80, in, out);

        if (R_SUCCEEDED(rc))
        {
            *outId = out.id;
            *outByte = out.byte;
        }

        return rc;
    }

    Result LeGetNextCharacteristic(GattId* outId, u8* outByte, u32 unk, GattId const& gattId, bool unk2, GattId const& gattId2, GattAttributeUuid const& uuid)
    {
        //TODO: test
        struct
        {
            bool unk2;
            u32 unk;
            GattId id;
            GattId id2;
            GattAttributeUuid uuid;
        } in = {unk2, unk, gattId, gattId2, uuid};

        struct
        {
            u8 byte;
            GattId id;
        } out;
        static_assert(sizeof(in) == 0x4C, "LeGetNextCharacteristic: Bad Input");
        static_assert(sizeof(out) == 0x1C, "LeGetNextCharacteristic: Bad Output");

        Result rc = btdrvDispatchInOut(81, in, out);

        if (R_SUCCEEDED(rc))
        {
            *outId = out.id;
            *outByte = out.byte;
        }

        return rc;
    }

    Result LeGetFirstDescriptor(GattId* out, u32 unk, GattId const& gattId, bool unk2, GattId const& gattId2, GattAttributeUuid const& uuid)
    {
        //TODO: test
        struct
        {
            bool unk2;
            u32 unk;
            GattId id;
            GattId id2;
            GattAttributeUuid uuid;
        } in = {unk2, unk, gattId, gattId2, uuid};

        static_assert(sizeof(in) == 0x4C, "LeGetFirstDescriptor: Bad Input");
        static_assert(sizeof(GattId) == 0x18, "LeGetFirstDescriptor: Bad Output");

        return btdrvDispatchInOut(82, in, *out);
    }

    Result LeGetNextDescriptor(GattId* out, u32 unk, GattId const& gattId, bool unk2, GattId const& gattId2, GattId const& gattId3, GattAttributeUuid const& uuid)
    {
        //TODO: test
        struct
        {
            bool unk2;
            u32 unk;
            GattId id;
            GattId id2;
            GattId id3;
            GattAttributeUuid uuid;
        } in = {unk2, unk, gattId, gattId2, gattId3, uuid};

        static_assert(sizeof(in) == 0x64, "LeGetNextDescriptor: Bad Input");
        static_assert(sizeof(GattId) == 0x18, "LeGetNextDescriptor: Bad Output");

        return btdrvDispatchInOut(83, in, *out);
    }

    Result RegisterLeCoreDataPath(GattAttributeUuid const& uuid)
    {
        //TODO: test
        static_assert(sizeof(uuid) == 0x14, "RegisterLeCoreDataPath: Bad Input");
        return btdrvDispatchIn(84, uuid);
    }

    Result UnregisterLeCoreDataPath(GattAttributeUuid const& uuid)
    {
        //TODO: test
        static_assert(sizeof(uuid) == 0x14, "UnregisterLeCoreDataPath: Bad Input");
        return btdrvDispatchIn(85, uuid);
    }

    Result RegisterLeHidDataPath(GattAttributeUuid const& uuid)
    {
        //TODO: test
        static_assert(sizeof(uuid) == 0x14, "RegisterLeHidDataPath: Bad Input");
        return btdrvDispatchIn(86, uuid);
    }

    Result UnregisterLeHidDataPath(GattAttributeUuid const& uuid)
    {
        //TODO: test
        static_assert(sizeof(uuid) == 0x14, "UnregisterLeHidDataPath: Bad Input");
        return btdrvDispatchIn(87, uuid);
    }

    Result RegisterLeDataPath(GattAttributeUuid const& uuid)
    {
        //TODO: test
        static_assert(sizeof(uuid) == 0x14, "RegisterLeDataPath: Bad Input");
        return btdrvDispatchIn(88, uuid);
    }

    Result UnregisterLeDataPath(GattAttributeUuid const& uuid)
    {
        //TODO: test
        static_assert(sizeof(uuid) == 0x14, "UnregisterLeDataPath: Bad Input");
        return btdrvDispatchIn(89, uuid);
    }

    Result LeClientReadCharacteristic(u32 connectedState, GattId const& gattId, bool unk, GattId const& gattId2, u8 unk2)
    {
        //TODO: test
        struct
        {
            bool unk;
            u8 unk2;
            u32 connectedState;
            GattId gattId;
            GattId gattId2;
        } in = {unk, unk2, connectedState, gattId, gattId2};

        static_assert(sizeof(in) == 0x38, "LeClientReadCharacteristic: Bad Input");
        return btdrvDispatchIn(90, in);
    }

    Result LeClientReadDescriptor(u32 connectedState, GattId const& gattId, bool unk, GattId const& gattId2, GattId const& gattId3, u8 unk2)
    {
        //TODO: test
        struct
        {
            bool unk;
            u8 unk2;
            u32 connectedState;
            GattId gattId;
            GattId gattId2;
            GattId gattId3;
        } in = {unk, unk2, connectedState, gattId, gattId2, gattId3};

        static_assert(sizeof(in) == 0x50, "LeClientReadDescriptor: Bad Input");
        return btdrvDispatchIn(91, in);
    }

    Result LeClientWriteCharacteristic(u32 connectedState, GattId const& gattId, bool unk, GattId const& gattId2, u8 const* buffer, u16 size, u8 unk2, bool unk3)
    {
        //TODO: test
        struct
        {
            bool unk;
            u8 unk2;
            bool unk3;
            u32 connectedState;
            GattId gattId;
            GattId gattId2;
        } in = {unk, unk2, unk3, connectedState, gattId, gattId2};

        static_assert(sizeof(in) == 0x38, "LeClientWriteCharacteristic: Bad Input");
        return btdrvDispatchIn(
            92, in,
            .buffer_attrs = {SfBufferAttr_In | SfBufferAttr_HipcPointer},
            .buffers = {{buffer, size}});
    }

    Result LeClientWriteDescriptor(u32 connectedState, GattId const& gattId, bool unk, GattId const& gattId2, GattId const& gattId3, u8 const* buffer, u16 size, u8 unk2)
    {
        //TODO: test
        struct
        {
            bool unk;
            u8 unk2;
            u32 connectedState;
            GattId gattId;
            GattId gattId2;
            GattId gattId3;
        } in = {unk, unk2, connectedState, gattId, gattId2, gattId3};

        static_assert(sizeof(in) == 0x50, "LeClientWriteDescriptor: Bad Input");
        return btdrvDispatchIn(
            93, in,
            .buffer_attrs = {SfBufferAttr_In | SfBufferAttr_HipcPointer},
            .buffers = {{buffer, size}});
    }

    Result LeClientRegisterNotification(u32 connectedState, GattId const& gattId, bool unk, GattId const& gattId2)
    {
        //TODO: test
        struct
        {
            bool unk;
            u32 connectedState;
            GattId gattId;
            GattId gattId2;
        } in = {unk, connectedState, gattId, gattId2};

        static_assert(sizeof(in) == 0x38, "LeClientRegisterNotification: Bad Input");
        return btdrvDispatchIn(94, in);
    }

    Result LeClientDeregisterNotification(u32 connectedState, GattId const& gattId, bool unk, GattId const& gattId2)
    {
        //TODO: test
        struct
        {
            bool unk;
            u32 connectedState;
            GattId gattId;
            GattId gattId2;
        } in = {unk, connectedState, gattId, gattId2};

        static_assert(sizeof(in) == 0x38, "LeClientDeregisterNotification: Bad Input");
        return btdrvDispatchIn(95, in);
    }

    Result GetLeHidEventInfo(BleEventType* outEvent, u8* buffer, u16 size)
    {
        //TODO: test
        // copies a struct of exactly 0x400 bytes
        return btdrvDispatchOut(
            96, *outEvent,
            .buffer_attrs = {SfBufferAttr_Out | SfBufferAttr_HipcPointer},
            .buffers = {{buffer, size}});
    }

    Result RegisterBleHidEvent(Event* outEvent)
    {
        //TODO: test
        return _btdrvGetEvent(outEvent, false, 97);
    }

    Result SetLeScanParameter(u16 param1, u16 param2)
    {
        //TODO: test
        struct
        {
            u16 param1;
            u16 param2;
        } in = {param1, param2};

        static_assert(sizeof(in) == 4, "SetLeScanParameter: Bad Input");
        return btdrvDispatchIn(98, in);
    }

    Result GetIsManufacturingMode(bool* out)
    {
        //TODO: test
        return btdrvDispatchIn(256, *out);
    }

    Result EmulateBluetoothCrash(BluetoothFatalReason reason)
    {
        //TODO: test
        static_assert(sizeof(reason) == 0x4, "EmulateBluetoothCrash: Bad Input");
        return btdrvDispatchIn(257, reason);
    }

    Result GetBleChannelMap(u8* outBuffer, u16 size)
    {
        //TODO: test
        return btdrvDispatch(
            258,
            .buffer_attrs = {SfBufferAttr_Out | SfBufferAttr_HipcMapAlias | SfBufferAttr_FixedSize},
            .buffers = {{outBuffer, size}});
    }

} // namespace nn::bluetooth