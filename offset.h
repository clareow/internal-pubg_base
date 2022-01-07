#pragma once
constexpr auto decrypt = 0x6E86928;
constexpr auto UWorld = 0x87C8600;
constexpr auto GNames = 0x8946CF8;
constexpr auto LocalPlayer = 0x898EC90;

constexpr auto PlayerController = 0x40;
constexpr auto AcknowledgedPawn = 0x488;
constexpr auto SpectatedCount = 0x12F0;

constexpr auto PlayerCameraManager = 0x4A8;
constexpr auto CameraLocation = 0x9C0;
constexpr auto CameraRotation = 0x9B0;
constexpr auto CameraFOV = 0x9A0;

constexpr auto ComponentToWorld = 0x350;
constexpr auto absoluteLocation = ComponentToWorld + 0x10;
constexpr auto BoneArray = 0xAF8;
constexpr auto RootComponent = 0x3E0;
constexpr auto Health = 0xF70;
constexpr auto GroggyHealth = 0xD50;
constexpr auto PersistentLevel = 0x910;
constexpr auto ActorsArray = 0x1E0;
constexpr auto ActorID = 0x30;
constexpr auto LastTeamNum = 0x18D0;
constexpr auto ActorMesh = 0x508; //0x510

constexpr auto WeaponProcessor = 0x1970;
constexpr auto EquippedWeapons = 0x2B8;
constexpr auto WeaponTrajectoryData = 0xFE0;
constexpr auto TrajectoryConfig = 0x118;
constexpr auto VDragCoefficient = 0x2C;

constexpr auto AnimScriptInstance = 0xCA0;
constexpr auto ControlRotation_CP = 0x6D0;
constexpr auto RecoilAdsRotation_CP = 0x8FC;
constexpr auto ChunkSize = 0x3FE0;

constexpr auto InputRotation = 0x610;

constexpr auto ActorVelocity = 0x234;
constexpr auto VehicleRiderComponent = 0x1A00;
constexpr auto LastVehiclePawn = 0x260;
constexpr auto ReplicatedMovement = 0x98;

constexpr auto DroppedItemGroup = 0x310;
constexpr auto DroppedItemGroup_Count = DroppedItemGroup + 8;
constexpr auto DroppedItemGroup_UItem = 0x650;
constexpr auto ItemID = 0x248;

constexpr auto LTSoffset = 0xdb040;