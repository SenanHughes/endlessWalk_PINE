// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "endlessWalk/AWaterSplineManager.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAWaterSplineManager() {}

// Begin Cross Module References
COREUOBJECT_API UClass* Z_Construct_UClass_UClass();
ENDLESSWALK_API UClass* Z_Construct_UClass_AWaterSplineManager();
ENDLESSWALK_API UClass* Z_Construct_UClass_AWaterSplineManager_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_AActor();
ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_USplineComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_endlessWalk();
// End Cross Module References

// Begin Class AWaterSplineManager
void AWaterSplineManager::StaticRegisterNativesAWaterSplineManager()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AWaterSplineManager);
UClass* Z_Construct_UClass_AWaterSplineManager_NoRegister()
{
	return AWaterSplineManager::StaticClass();
}
struct Z_Construct_UClass_AWaterSplineManager_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "AWaterSplineManager.h" },
		{ "ModuleRelativePath", "AWaterSplineManager.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WaterSpline_MetaData[] = {
		{ "Category", "Water Spline" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Spline component for water body path\n" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "AWaterSplineManager.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Spline component for water body path" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WaterBodyBlueprint_MetaData[] = {
		{ "Category", "Water" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Blueprint reference for the WaterBodyRiver actor (optional, if you wish to spawn it)\n" },
#endif
		{ "ModuleRelativePath", "AWaterSplineManager.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Blueprint reference for the WaterBodyRiver actor (optional, if you wish to spawn it)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PlayerActor_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Reference to the Player Actor\n" },
#endif
		{ "ModuleRelativePath", "AWaterSplineManager.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Reference to the Player Actor" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SplineSpawnThreshold_MetaData[] = {
		{ "Category", "Water" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Distance threshold to add a new spline point\n" },
#endif
		{ "ModuleRelativePath", "AWaterSplineManager.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Distance threshold to add a new spline point" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WaterBodyRiver_MetaData[] = {
		{ "Category", "Water" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Reference to the WaterBodyRiver actor\n" },
#endif
		{ "ModuleRelativePath", "AWaterSplineManager.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Reference to the WaterBodyRiver actor" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_WaterSpline;
	static const UECodeGen_Private::FClassPropertyParams NewProp_WaterBodyBlueprint;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PlayerActor;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_SplineSpawnThreshold;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_WaterBodyRiver;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AWaterSplineManager>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AWaterSplineManager_Statics::NewProp_WaterSpline = { "WaterSpline", nullptr, (EPropertyFlags)0x00100000000a0009, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AWaterSplineManager, WaterSpline), Z_Construct_UClass_USplineComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WaterSpline_MetaData), NewProp_WaterSpline_MetaData) };
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_AWaterSplineManager_Statics::NewProp_WaterBodyBlueprint = { "WaterBodyBlueprint", nullptr, (EPropertyFlags)0x0014000000000001, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AWaterSplineManager, WaterBodyBlueprint), Z_Construct_UClass_UClass, Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WaterBodyBlueprint_MetaData), NewProp_WaterBodyBlueprint_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AWaterSplineManager_Statics::NewProp_PlayerActor = { "PlayerActor", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AWaterSplineManager, PlayerActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PlayerActor_MetaData), NewProp_PlayerActor_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AWaterSplineManager_Statics::NewProp_SplineSpawnThreshold = { "SplineSpawnThreshold", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AWaterSplineManager, SplineSpawnThreshold), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SplineSpawnThreshold_MetaData), NewProp_SplineSpawnThreshold_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AWaterSplineManager_Statics::NewProp_WaterBodyRiver = { "WaterBodyRiver", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AWaterSplineManager, WaterBodyRiver), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WaterBodyRiver_MetaData), NewProp_WaterBodyRiver_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AWaterSplineManager_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AWaterSplineManager_Statics::NewProp_WaterSpline,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AWaterSplineManager_Statics::NewProp_WaterBodyBlueprint,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AWaterSplineManager_Statics::NewProp_PlayerActor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AWaterSplineManager_Statics::NewProp_SplineSpawnThreshold,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AWaterSplineManager_Statics::NewProp_WaterBodyRiver,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AWaterSplineManager_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_AWaterSplineManager_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_endlessWalk,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AWaterSplineManager_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AWaterSplineManager_Statics::ClassParams = {
	&AWaterSplineManager::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_AWaterSplineManager_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_AWaterSplineManager_Statics::PropPointers),
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AWaterSplineManager_Statics::Class_MetaDataParams), Z_Construct_UClass_AWaterSplineManager_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AWaterSplineManager()
{
	if (!Z_Registration_Info_UClass_AWaterSplineManager.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AWaterSplineManager.OuterSingleton, Z_Construct_UClass_AWaterSplineManager_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AWaterSplineManager.OuterSingleton;
}
template<> ENDLESSWALK_API UClass* StaticClass<AWaterSplineManager>()
{
	return AWaterSplineManager::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AWaterSplineManager);
AWaterSplineManager::~AWaterSplineManager() {}
// End Class AWaterSplineManager

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Computer_Animation_finalProject_endlessWalk_PINE_endlessWalk_Source_endlessWalk_AWaterSplineManager_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AWaterSplineManager, AWaterSplineManager::StaticClass, TEXT("AWaterSplineManager"), &Z_Registration_Info_UClass_AWaterSplineManager, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AWaterSplineManager), 1077648484U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Computer_Animation_finalProject_endlessWalk_PINE_endlessWalk_Source_endlessWalk_AWaterSplineManager_h_2397462835(TEXT("/Script/endlessWalk"),
	Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Computer_Animation_finalProject_endlessWalk_PINE_endlessWalk_Source_endlessWalk_AWaterSplineManager_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Computer_Animation_finalProject_endlessWalk_PINE_endlessWalk_Source_endlessWalk_AWaterSplineManager_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
