// src/Launcher/Headers/Efi/SystemTable.hpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on UEFI Specification 2.10. Independent implementation.

#pragma once

#include "Protocol/Protocols.hpp"
#include "Macros.hpp"

namespace efi
{
    struct TableHeader
    {
        uint64 Signature;
        uint32 Revision;
        uint32 HeaderSize;
        uint32 CRC32;
        uint32 Reserved;
    };

    enum class MemoryType : uint32
    {
        ReservedMemoryType,
        LoaderCode,
        LoaderData,
        BootServicesCode,
        BootServicesData,
        RuntimeServicesCode,
        RuntimeServicesData,
        ConventionalMemory,
        UnusableMemory,
        ACPIReclaimMemory,
        ACPIMemoryNVS,
        MemoryMappedIO,
        MemoryMappedIOPortSpace,
        PalCode,
        PersistentMemory,
        UnacceptedMemoryType,
        MaxMemoryType
    };

    struct MemoryDescriptor
    {
        MemoryType Type;
        uintn PhysicalStart;
        uintn VirtualStart;
        uint64 NumberOfPages;
        uint64 Attribute;
    };

    enum class ResetType : uint32
    {
        Cold = 0,
        Warm = 1,
        Shutdown = 2,
        PlatformSpecific = 3
    };

    // Runtime Services
    enum class ResetType : uint32;

    struct Time
    {
        uint16 Year;
        uint8 Month;
        uint8 Day;
        uint8 Hour;
        uint8 Minute;
        uint8 Second;
        uint8 Pad1;
        uint32 Nanosecond;
        int16 TimeZone;
        uint8 Daylight;
        uint8 Pad2;
    };

    struct TimeCapabilities
    {
        uint32 Resolution;
        uint32 Accuracy;
        bool SetsToZero;
    };

    namespace guid
    {
        inline constexpr Guid GlobalVariableGuid = {0x8BE4DF61, 0x93CA, 0x11d2, {0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C}};
    }

    enum class VariableAttribute : uint32
    {
        None = 0x00000000,
        NonVolatile = 0x00000001,
        BootServiceAccess = 0x00000002,
        RuntimeAccess = 0x00000004,
        HardwareErrorRecord = 0x00000008,
        AuthenticatedWriteAccess = 0x00000010,
        TimeBasedAuthenticatedWriteAccess = 0x00000020,
        AppendWrite = 0x00000040,
        EnhancedAuthenticatedAccess = 0x00000080
    };

    constexpr VariableAttribute operator|(VariableAttribute a, VariableAttribute b) noexcept
    {
        return static_cast<VariableAttribute>(
            static_cast<uint32>(a) | static_cast<uint32>(b));
    }

    constexpr VariableAttribute operator&(VariableAttribute a, VariableAttribute b) noexcept
    {
        return static_cast<VariableAttribute>(
            static_cast<uint32>(a) & static_cast<uint32>(b));
    }

    constexpr VariableAttribute &operator|=(VariableAttribute &a, VariableAttribute b) noexcept
    {
        a = a | b;
        return a;
    }

    struct RuntimeServices
    {
        TableHeader Header;

        Status(efiapi *GetTime)(Time *time, TimeCapabilities *capabilities);
        Status(efiapi *SetTime)(Time *time);
        Status(efiapi *GetWakeupTime)(bool *enabled, bool *pending, Time *time);
        Status(efiapi *SetWakeupTime)(bool enable, Time *time);

        Status(efiapi *SetVirtualAddressMap)(uintn memoryMapSize, uintn descriptorSize, uint32 descriptorVersion, MemoryDescriptor *virtualMap);

        Status(efiapi *ConvertPointer)(uintn debugDisposition, void **address);

        Status(efiapi *GetVariable)(char16 *variableName, const Guid *vendorGuid, uint32 *attributes, uintn *dataSize, void *data);
        Status(efiapi *GetNextVariableName)(uintn *variableNameSize, char16 *variableName, const Guid *vendorGuid);
        Status(efiapi *SetVariable)(char16 *variableName, const Guid *vendorGuid, VariableAttribute attributes, uintn dataSize, void *data);

        Status(efiapi *GetNextHighMonotonicCount)(uint32 *highCount);

        noreturn void(efiapi *ResetSystem)(ResetType resetType, Status resetStatus, uintn dataSize, void *resetData);

        Status(efiapi *UpdateCapsule)(void **capsuleHeaderArray, uintn capsuleCount, uint64 scatterGatherList);
        Status(efiapi *QueryCapsuleCapabilities)(void **capsuleHeaderArray, uintn capsuleCount, uint64 *maximumCapsuleSize, ResetType *resetType);

        Status(efiapi *QueryVariableInfo)(uint32 attributes, uint64 *maximumVariableStorageSize, uint64 *remainingVariableStorageSize, uint64 *maximumVariableSize);
    };

    // Boot Services
    struct BootServices
    {
        TableHeader Header;

        Status(efiapi *RaiseTPL)(TPL newTpl);
        Status(efiapi *RestoreTPL)(TPL oldTpl);

        Status(efiapi *AllocatePages)(int allocateType, MemoryType memoryType, uintn pages, uint64 *memory);
        Status(efiapi *FreePages)(uint64 memory, uintn pages);
        Status(efiapi *GetMemoryMap)(uintn *memoryMapSize, MemoryDescriptor *memoryMap, uintn *mapKey, uintn *descriptorSize, uint32 *descriptorVersion);
        Status(efiapi *AllocatePool)(MemoryType poolType, uintn size, void **buffer);
        Status(efiapi *FreePool)(void *buffer);

        Status(efiapi *CreateEvent)(uint32 type, TPL notifyTpl, void *notifyFunction, void *notifyContext, Event *event);
        Status(efiapi *SetTimer)(Event event, int delayType, uint64 triggerTime);
        Status(efiapi *WaitForEvent)(uintn numberOfEvents, Event *event, uintn *index);
        Status(efiapi *SignalEvent)(Event event);
        Status(efiapi *CloseEvent)(Event event);
        Status(efiapi *CheckEvent)(Event event);

        Status(efiapi *InstallProtocolInterface)(Handle *current, const Guid *protocol, int interfaceType, void *interface);
        Status(efiapi *ReinstallProtocolInterface)(Handle current, const Guid *protocol, void *oldInterface, void *newInterface);
        Status(efiapi *UninstallProtocolInterface)(Handle current, const Guid *protocol, void *interface);
        Status(efiapi *HandleProtocol)(Handle current, const Guid *protocol, void **interface);

        void *Reserved;

        Status(efiapi *RegisterProtocolNotify)(const Guid *protocol, Event event, void **registration);
        Status(efiapi *LocateHandle)(int searchType, const Guid *protocol, void *searchKey, uintn *bufferSize, Handle *buffer);
        Status(efiapi *LocateDevicePath)(const Guid *protocol, void **devicePath, Handle *device);
        Status(efiapi *InstallConfigurationTable)(const Guid *guid, void *table);

        Status(efiapi *LoadImage)(bool bootPolicy, Handle parentImageHandle, void *devicePath, void *sourceBuffer, uintn sourceSize, Handle *imageHandle);
        Status(efiapi *StartImage)(Handle imageHandle, uintn *exitDataSize, char16 **exitData);
        Status(efiapi *Exit)(Handle imageHandle, Status exitStatus, uintn exitDataSize, char16 *exitData);
        Status(efiapi *UnloadImage)(Handle imageHandle);
        Status(efiapi *ExitBootServices)(Handle imageHandle, uintn mapKey);

        Status(efiapi *GetNextMonotonicCount)(uint64 *count);
        Status(efiapi *Stall)(uintn microseconds);
        Status(efiapi *SetWatchdogTimer)(uintn timeout, uint64 watchdogCode, uintn dataSize, char16 *watchdogData);

        Status(efiapi *ConnectController)(Handle controllerHandle, Handle *driverImageHandle, void *remainingDevicePath, bool recursive);
        Status(efiapi *DisconnectController)(Handle controllerHandle, Handle driverImageHandle, Handle childHandle);

        Status(efiapi *OpenProtocol)(Handle current, const Guid *protocol, void **interface, Handle agentHandle, Handle controllerHandle, uint32 attributes);
        Status(efiapi *CloseProtocol)(Handle current, const Guid *protocol, Handle agentHandle, Handle controllerHandle);
        Status(efiapi *OpenProtocolInformation)(Handle current, const Guid *protocol, void **entryBuffer, uintn *entryCount);

        Status(efiapi *ProtocolsPerHandle)(Handle current, Guid ***protocolBuffer, uintn *protocolBufferCount);
        Status(efiapi *LocateHandleBuffer)(int searchType, const Guid *protocol, void *searchKey, uintn *noHandles, Handle **buffer);
        Status(efiapi *LocateProtocol)(const Guid *protocol, void *registration, void **interface);
        Status(efiapi *InstallMultipleProtocolInterfaces)(Handle *current, ...);
        Status(efiapi *UninstallMultipleProtocolInterfaces)(Handle current, ...);

        Status(efiapi *CalculateCrc32)(void *data, uintn dataSize, uint32 *crc32);

        void(efiapi *CopyMem)(void *destination, void *source, uintn length);
        void(efiapi *SetMem)(void *buffer, uintn size, uint8 value);

        Status(efiapi *CreateEventEx)(uint32 type, TPL notifyTpl, void *notifyFunction, void *notifyContext, Guid *eventGroup, Event *event);
    };

    struct SystemTable
    {
        TableHeader Header;
        char16 *FirmwareVendor;
        uint32 FirmwareRevision;
        Handle ConsoleInputHandle;
        protocol::SimpleTextInput *ConsoleInput;
        Handle ConsoleOutputHandle;
        protocol::SimpleTextOutput *ConsoleOutput;
        Handle StandardErrorHandle;
        protocol::SimpleTextOutput *StandardError;
        RuntimeServices *RuntimeServices;
        BootServices *BootServices;
        uintn NumberOfTableEntries;
        void *ConfigurationTable;
    };
}