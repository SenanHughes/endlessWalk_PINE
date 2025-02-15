// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "endlessWalk/ATileManager.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeATileManager() {}

// Begin Cross Module References
COREUOBJECT_API UClass* Z_Construct_UClass_UClass();
ENDLESSWALK_API UClass* Z_Construct_UClass_ATileManager();
ENDLESSWALK_API UClass* Z_Construct_UClass_ATileManager_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_AActor();
ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
UPackage* Z_Construct_UPackage__Script_endlessWalk();
WATER_API UClass* Z_Construct_UClass_AWaterBodyRiver_NoRegister();
// End Cross Module References

// Begin Class ATileManager
void ATileManager::StaticRegisterNativesATileManager()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ATileManager);
UClass* Z_Construct_UClass_ATileManager_NoRegister()
{
	return ATileManager::StaticClass();
}
struct Z_Construct_UClass_ATileManager_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "ATileManager.h" },
		{ "ModuleRelativePath", "ATileManager.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TileBlueprint_MetaData[] = {
		{ "Category", "Tile Spawning" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Blueprint tile to spawn (greybox tile)\n" },
#endif
		{ "ModuleRelativePath", "ATileManager.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Blueprint tile to spawn (greybox tile)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SpawnThreshold_MetaData[] = {
		{ "Category", "Tile Spawning" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Distance threshold for spawning a new tile\n" },
#endif
		{ "ModuleRelativePath", "ATileManager.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Distance threshold for spawning a new tile" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WaterBodyRiver_MetaData[] = {
		{ "Category", "River" },
		{ "ModuleRelativePath", "ATileManager.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PlayerActor_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Reference to the player\n" },
#endif
		{ "ModuleRelativePath", "ATileManager.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Reference to the player" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FClassPropertyParams NewProp_TileBlueprint;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_SpawnThreshold;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_WaterBodyRiver;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PlayerActor;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ATileManager>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_ATileManager_Statics::NewProp_TileBlueprint = { "TileBlueprint", nullptr, (EPropertyFlags)0x0014000000000001, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ATileManager, TileBlueprint), Z_Construct_UClass_UClass, Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TileBlueprint_MetaData), NewProp_TileBlueprint_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_ATileManager_Statics::NewProp_SpawnThreshold = { "SpawnThreshold", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ATileManager, SpawnThreshold), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SpawnThreshold_MetaData), NewProp_SpawnThreshold_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ATileManager_Statics::NewProp_WaterBodyRiver = { "WaterBodyRiver", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ATileManager, WaterBodyRiver), Z_Construct_UClass_AWaterBodyRiver_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WaterBodyRiver_MetaData), NewProp_WaterBodyRiver_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ATileManager_Statics::NewProp_PlayerActor = { "PlayerActor", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ATileManager, PlayerActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PlayerActor_MetaData), NewProp_PlayerActor_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_ATileManager_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ATileManager_Statics::NewProp_TileBlueprint,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ATileManager_Statics::NewProp_SpawnThreshold,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ATileManager_Statics::NewProp_WaterBodyRiver,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ATileManager_Statics::NewProp_PlayerActor,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ATileManager_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_ATileManager_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_endlessWalk,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ATileManager_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ATileManager_Statics::ClassParams = {
	&ATileManager::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_ATileManager_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_ATileManager_Statics::PropPointers),
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ATileManager_Statics::Class_MetaDataParams), Z_Construct_UClass_ATileManager_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ATileManager()
{
	if (!Z_Registration_Info_UClass_ATileManager.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ATileManager.OuterSingleton, Z_Construct_UClass_ATileManager_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ATileManager.OuterSingleton;
}
template<> ENDLESSWALK_API UClass* StaticClass<ATileManager>()
{
	return ATileManager::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ATileManager);
ATileManager::~ATileManager() {}
// End Class ATileManager

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Computer_Animation_finalProject_endlessWalk_PINE_endlessWalk_Source_endlessWalk_ATileManager_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ATileManager, ATileManager::StaticClass, TEXT("ATileManager"), &Z_Registration_Info_UClass_ATileManager, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ATileManager), 2155174558U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Computer_Animation_finalProject_endlessWalk_PINE_endlessWalk_Source_endlessWalk_ATileManager_h_4011890685(TEXT("/Script/endlessWalk"),
	Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Computer_Animation_finalProject_endlessWalk_PINE_endlessWalk_Source_endlessWalk_ATileManager_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Computer_Animation_finalProject_endlessWalk_PINE_endlessWalk_Source_endlessWalk_ATileManager_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
