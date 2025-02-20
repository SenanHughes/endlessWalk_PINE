// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "endlessWalk/Public/TerrainGenerator.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTerrainGenerator() {}

// Begin Cross Module References
ENDLESSWALK_API UClass* Z_Construct_UClass_ATerrainGenerator();
ENDLESSWALK_API UClass* Z_Construct_UClass_ATerrainGenerator_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_AActor();
ENGINE_API UClass* Z_Construct_UClass_UMaterialInterface_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_USplineComponent_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UStaticMesh_NoRegister();
PROCEDURALMESHCOMPONENT_API UClass* Z_Construct_UClass_UProceduralMeshComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_endlessWalk();
// End Cross Module References

// Begin Class ATerrainGenerator
void ATerrainGenerator::StaticRegisterNativesATerrainGenerator()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ATerrainGenerator);
UClass* Z_Construct_UClass_ATerrainGenerator_NoRegister()
{
	return ATerrainGenerator::StaticClass();
}
struct Z_Construct_UClass_ATerrainGenerator_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "TerrainGenerator.h" },
		{ "ModuleRelativePath", "Public/TerrainGenerator.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Spline_MetaData[] = {
		{ "Category", "Terrain" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// The Spline to generate terrain along\n" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/TerrainGenerator.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "The Spline to generate terrain along" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TerrainMesh_MetaData[] = {
		{ "Category", "Terrain" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// The mesh to use for the terrain\n" },
#endif
		{ "ModuleRelativePath", "Public/TerrainGenerator.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "The mesh to use for the terrain" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TerrainMaterial_MetaData[] = {
		{ "Category", "Terrain" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Material to apply to terrain\n" },
#endif
		{ "ModuleRelativePath", "Public/TerrainGenerator.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Material to apply to terrain" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaxSplineLength_MetaData[] = {
		{ "Category", "Terrain" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Maximum length of the spline (10,000 units)\n" },
#endif
		{ "ModuleRelativePath", "Public/TerrainGenerator.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Maximum length of the spline (10,000 units)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_NoiseFrequency_MetaData[] = {
		{ "Category", "Noise" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Frequency of terrain noise\n" },
#endif
		{ "ModuleRelativePath", "Public/TerrainGenerator.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Frequency of terrain noise" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_NoiseAmplitude_MetaData[] = {
		{ "Category", "Noise" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Amplitude of terrain noise\n" },
#endif
		{ "ModuleRelativePath", "Public/TerrainGenerator.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Amplitude of terrain noise" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PlaneDistance_MetaData[] = {
		{ "Category", "Terrain" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// The distance at which to start adding new terrain planes\n" },
#endif
		{ "ModuleRelativePath", "Public/TerrainGenerator.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "The distance at which to start adding new terrain planes" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CheckPoints_MetaData[] = {
		{ "Category", "Terrain" },
		{ "ModuleRelativePath", "Public/TerrainGenerator.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TerrainMeshProc_MetaData[] = {
		{ "Category", "TerrainGenerator" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/TerrainGenerator.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Spline;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_TerrainMesh;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_TerrainMaterial;
	static const UECodeGen_Private::FIntPropertyParams NewProp_MaxSplineLength;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_NoiseFrequency;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_NoiseAmplitude;
	static const UECodeGen_Private::FIntPropertyParams NewProp_PlaneDistance;
	static const UECodeGen_Private::FIntPropertyParams NewProp_CheckPoints;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_TerrainMeshProc;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ATerrainGenerator>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_Spline = { "Spline", nullptr, (EPropertyFlags)0x00100000000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ATerrainGenerator, Spline), Z_Construct_UClass_USplineComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Spline_MetaData), NewProp_Spline_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_TerrainMesh = { "TerrainMesh", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ATerrainGenerator, TerrainMesh), Z_Construct_UClass_UStaticMesh_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TerrainMesh_MetaData), NewProp_TerrainMesh_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_TerrainMaterial = { "TerrainMaterial", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ATerrainGenerator, TerrainMaterial), Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TerrainMaterial_MetaData), NewProp_TerrainMaterial_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_MaxSplineLength = { "MaxSplineLength", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ATerrainGenerator, MaxSplineLength), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaxSplineLength_MetaData), NewProp_MaxSplineLength_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_NoiseFrequency = { "NoiseFrequency", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ATerrainGenerator, NoiseFrequency), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_NoiseFrequency_MetaData), NewProp_NoiseFrequency_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_NoiseAmplitude = { "NoiseAmplitude", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ATerrainGenerator, NoiseAmplitude), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_NoiseAmplitude_MetaData), NewProp_NoiseAmplitude_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_PlaneDistance = { "PlaneDistance", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ATerrainGenerator, PlaneDistance), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PlaneDistance_MetaData), NewProp_PlaneDistance_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_CheckPoints = { "CheckPoints", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ATerrainGenerator, CheckPoints), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CheckPoints_MetaData), NewProp_CheckPoints_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_TerrainMeshProc = { "TerrainMeshProc", nullptr, (EPropertyFlags)0x00100000000a0009, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ATerrainGenerator, TerrainMeshProc), Z_Construct_UClass_UProceduralMeshComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TerrainMeshProc_MetaData), NewProp_TerrainMeshProc_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_ATerrainGenerator_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_Spline,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_TerrainMesh,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_TerrainMaterial,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_MaxSplineLength,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_NoiseFrequency,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_NoiseAmplitude,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_PlaneDistance,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_CheckPoints,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ATerrainGenerator_Statics::NewProp_TerrainMeshProc,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ATerrainGenerator_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_ATerrainGenerator_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_endlessWalk,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ATerrainGenerator_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ATerrainGenerator_Statics::ClassParams = {
	&ATerrainGenerator::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_ATerrainGenerator_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_ATerrainGenerator_Statics::PropPointers),
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ATerrainGenerator_Statics::Class_MetaDataParams), Z_Construct_UClass_ATerrainGenerator_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ATerrainGenerator()
{
	if (!Z_Registration_Info_UClass_ATerrainGenerator.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ATerrainGenerator.OuterSingleton, Z_Construct_UClass_ATerrainGenerator_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ATerrainGenerator.OuterSingleton;
}
template<> ENDLESSWALK_API UClass* StaticClass<ATerrainGenerator>()
{
	return ATerrainGenerator::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ATerrainGenerator);
ATerrainGenerator::~ATerrainGenerator() {}
// End Class ATerrainGenerator

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_TerrainGenerator_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ATerrainGenerator, ATerrainGenerator::StaticClass, TEXT("ATerrainGenerator"), &Z_Registration_Info_UClass_ATerrainGenerator, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ATerrainGenerator), 740687414U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_TerrainGenerator_h_1522158038(TEXT("/Script/endlessWalk"),
	Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_TerrainGenerator_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_TerrainGenerator_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
