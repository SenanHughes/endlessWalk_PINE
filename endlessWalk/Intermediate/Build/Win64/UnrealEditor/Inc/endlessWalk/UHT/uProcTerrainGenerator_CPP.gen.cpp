// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "endlessWalk/Public/uProcTerrainGenerator_CPP.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeuProcTerrainGenerator_CPP() {}

// Begin Cross Module References
ENDLESSWALK_API UClass* Z_Construct_UClass_AuProcTerrainGenerator_CPP();
ENDLESSWALK_API UClass* Z_Construct_UClass_AuProcTerrainGenerator_CPP_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_AActor();
ENGINE_API UClass* Z_Construct_UClass_UMaterialInterface_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_USplineComponent_NoRegister();
PROCEDURALMESHCOMPONENT_API UClass* Z_Construct_UClass_UProceduralMeshComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_endlessWalk();
// End Cross Module References

// Begin Class AuProcTerrainGenerator_CPP
void AuProcTerrainGenerator_CPP::StaticRegisterNativesAuProcTerrainGenerator_CPP()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AuProcTerrainGenerator_CPP);
UClass* Z_Construct_UClass_AuProcTerrainGenerator_CPP_NoRegister()
{
	return AuProcTerrainGenerator_CPP::StaticClass();
}
struct Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "uProcTerrainGenerator_CPP.h" },
		{ "ModuleRelativePath", "Public/uProcTerrainGenerator_CPP.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Spline_MetaData[] = {
		{ "Category", "Terrain" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// The Spline to generate terrain along\n" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/uProcTerrainGenerator_CPP.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "The Spline to generate terrain along" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaxSplineLength_MetaData[] = {
		{ "Category", "Terrain" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Maximum length of the spline (10,000 units)\n" },
#endif
		{ "ModuleRelativePath", "Public/uProcTerrainGenerator_CPP.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Maximum length of the spline (10,000 units)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SplinePoints_MetaData[] = {
		{ "Category", "Terrain" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Spline Steps\n" },
#endif
		{ "ModuleRelativePath", "Public/uProcTerrainGenerator_CPP.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Spline Steps" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_NoiseFrequency_MetaData[] = {
		{ "Category", "Noise" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Frequency of terrain noise\n" },
#endif
		{ "ModuleRelativePath", "Public/uProcTerrainGenerator_CPP.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Frequency of terrain noise" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_NoiseAmplitude_MetaData[] = {
		{ "Category", "Noise" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Amplitude of terrain noise\n" },
#endif
		{ "ModuleRelativePath", "Public/uProcTerrainGenerator_CPP.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Amplitude of terrain noise" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PlaneDistance_MetaData[] = {
		{ "Category", "Terrain" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// The distance at which to start adding new terrain planes\n" },
#endif
		{ "ModuleRelativePath", "Public/uProcTerrainGenerator_CPP.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "The distance at which to start adding new terrain planes" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CheckPoints_MetaData[] = {
		{ "Category", "Terrain" },
		{ "ModuleRelativePath", "Public/uProcTerrainGenerator_CPP.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PathWidth_MetaData[] = {
		{ "Category", "Terrain" },
		{ "ModuleRelativePath", "Public/uProcTerrainGenerator_CPP.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_UVScale_MetaData[] = {
		{ "Category", "Terrain" },
		{ "ModuleRelativePath", "Public/uProcTerrainGenerator_CPP.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AssignedMaterial_MetaData[] = {
		{ "Category", "Materials" },
		{ "ModuleRelativePath", "Public/uProcTerrainGenerator_CPP.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PathMesh_MetaData[] = {
		{ "Category", "ProceduralMesh" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/uProcTerrainGenerator_CPP.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Spline;
	static const UECodeGen_Private::FIntPropertyParams NewProp_MaxSplineLength;
	static const UECodeGen_Private::FIntPropertyParams NewProp_SplinePoints;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_NoiseFrequency;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_NoiseAmplitude;
	static const UECodeGen_Private::FIntPropertyParams NewProp_PlaneDistance;
	static const UECodeGen_Private::FIntPropertyParams NewProp_CheckPoints;
	static const UECodeGen_Private::FIntPropertyParams NewProp_PathWidth;
	static const UECodeGen_Private::FIntPropertyParams NewProp_UVScale;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_AssignedMaterial;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PathMesh;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AuProcTerrainGenerator_CPP>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_Spline = { "Spline", nullptr, (EPropertyFlags)0x00100000000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcTerrainGenerator_CPP, Spline), Z_Construct_UClass_USplineComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Spline_MetaData), NewProp_Spline_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_MaxSplineLength = { "MaxSplineLength", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcTerrainGenerator_CPP, MaxSplineLength), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaxSplineLength_MetaData), NewProp_MaxSplineLength_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_SplinePoints = { "SplinePoints", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcTerrainGenerator_CPP, SplinePoints), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SplinePoints_MetaData), NewProp_SplinePoints_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_NoiseFrequency = { "NoiseFrequency", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcTerrainGenerator_CPP, NoiseFrequency), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_NoiseFrequency_MetaData), NewProp_NoiseFrequency_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_NoiseAmplitude = { "NoiseAmplitude", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcTerrainGenerator_CPP, NoiseAmplitude), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_NoiseAmplitude_MetaData), NewProp_NoiseAmplitude_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_PlaneDistance = { "PlaneDistance", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcTerrainGenerator_CPP, PlaneDistance), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PlaneDistance_MetaData), NewProp_PlaneDistance_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_CheckPoints = { "CheckPoints", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcTerrainGenerator_CPP, CheckPoints), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CheckPoints_MetaData), NewProp_CheckPoints_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_PathWidth = { "PathWidth", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcTerrainGenerator_CPP, PathWidth), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PathWidth_MetaData), NewProp_PathWidth_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_UVScale = { "UVScale", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcTerrainGenerator_CPP, UVScale), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_UVScale_MetaData), NewProp_UVScale_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_AssignedMaterial = { "AssignedMaterial", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcTerrainGenerator_CPP, AssignedMaterial), Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AssignedMaterial_MetaData), NewProp_AssignedMaterial_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_PathMesh = { "PathMesh", nullptr, (EPropertyFlags)0x00100000000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcTerrainGenerator_CPP, PathMesh), Z_Construct_UClass_UProceduralMeshComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PathMesh_MetaData), NewProp_PathMesh_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_Spline,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_MaxSplineLength,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_SplinePoints,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_NoiseFrequency,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_NoiseAmplitude,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_PlaneDistance,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_CheckPoints,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_PathWidth,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_UVScale,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_AssignedMaterial,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::NewProp_PathMesh,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_endlessWalk,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::ClassParams = {
	&AuProcTerrainGenerator_CPP::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::PropPointers),
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::Class_MetaDataParams), Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AuProcTerrainGenerator_CPP()
{
	if (!Z_Registration_Info_UClass_AuProcTerrainGenerator_CPP.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AuProcTerrainGenerator_CPP.OuterSingleton, Z_Construct_UClass_AuProcTerrainGenerator_CPP_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AuProcTerrainGenerator_CPP.OuterSingleton;
}
template<> ENDLESSWALK_API UClass* StaticClass<AuProcTerrainGenerator_CPP>()
{
	return AuProcTerrainGenerator_CPP::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AuProcTerrainGenerator_CPP);
AuProcTerrainGenerator_CPP::~AuProcTerrainGenerator_CPP() {}
// End Class AuProcTerrainGenerator_CPP

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_uProcTerrainGenerator_CPP_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AuProcTerrainGenerator_CPP, AuProcTerrainGenerator_CPP::StaticClass, TEXT("AuProcTerrainGenerator_CPP"), &Z_Registration_Info_UClass_AuProcTerrainGenerator_CPP, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AuProcTerrainGenerator_CPP), 129320190U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_uProcTerrainGenerator_CPP_h_1183479919(TEXT("/Script/endlessWalk"),
	Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_uProcTerrainGenerator_CPP_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_uProcTerrainGenerator_CPP_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
