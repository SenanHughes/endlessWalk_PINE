// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "endlessWalk/Public/uProcAssetScatter_CPP.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeuProcAssetScatter_CPP() {}

// Begin Cross Module References
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector();
ENDLESSWALK_API UClass* Z_Construct_UClass_AuProcAssetScatter_CPP();
ENDLESSWALK_API UClass* Z_Construct_UClass_AuProcAssetScatter_CPP_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_AActor();
ENGINE_API UClass* Z_Construct_UClass_UMaterialInterface_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UStaticMesh_NoRegister();
PROCEDURALMESHCOMPONENT_API UClass* Z_Construct_UClass_UProceduralMeshComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_endlessWalk();
// End Cross Module References

// Begin Class AuProcAssetScatter_CPP
void AuProcAssetScatter_CPP::StaticRegisterNativesAuProcAssetScatter_CPP()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AuProcAssetScatter_CPP);
UClass* Z_Construct_UClass_AuProcAssetScatter_CPP_NoRegister()
{
	return AuProcAssetScatter_CPP::StaticClass();
}
struct Z_Construct_UClass_AuProcAssetScatter_CPP_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "uProcAssetScatter_CPP.h" },
		{ "ModuleRelativePath", "Public/uProcAssetScatter_CPP.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ProceduralMesh_MetaData[] = {
		{ "Category", "Procedural" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Procedural Mesh */" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/uProcAssetScatter_CPP.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Procedural Mesh" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Material_MetaData[] = {
		{ "Category", "Materials" },
		{ "ModuleRelativePath", "Public/uProcAssetScatter_CPP.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AssetToSpawn_MetaData[] = {
		{ "Category", "Procedural" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Static Mesh to Spawn */" },
#endif
		{ "ModuleRelativePath", "Public/uProcAssetScatter_CPP.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Static Mesh to Spawn" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_NumAssetsToSpawn_MetaData[] = {
		{ "Category", "Procedural" },
		{ "ClampMin", "1" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Number of assets to spawn */" },
#endif
		{ "ModuleRelativePath", "Public/uProcAssetScatter_CPP.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Number of assets to spawn" },
#endif
		{ "UIMin", "1" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SpawnAreaSize_MetaData[] = {
		{ "Category", "Procedural" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Spawning area size */" },
#endif
		{ "ModuleRelativePath", "Public/uProcAssetScatter_CPP.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Spawning area size" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ProceduralMesh;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Material;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_AssetToSpawn;
	static const UECodeGen_Private::FIntPropertyParams NewProp_NumAssetsToSpawn;
	static const UECodeGen_Private::FStructPropertyParams NewProp_SpawnAreaSize;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AuProcAssetScatter_CPP>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::NewProp_ProceduralMesh = { "ProceduralMesh", nullptr, (EPropertyFlags)0x00100000000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcAssetScatter_CPP, ProceduralMesh), Z_Construct_UClass_UProceduralMeshComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ProceduralMesh_MetaData), NewProp_ProceduralMesh_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::NewProp_Material = { "Material", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcAssetScatter_CPP, Material), Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Material_MetaData), NewProp_Material_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::NewProp_AssetToSpawn = { "AssetToSpawn", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcAssetScatter_CPP, AssetToSpawn), Z_Construct_UClass_UStaticMesh_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AssetToSpawn_MetaData), NewProp_AssetToSpawn_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::NewProp_NumAssetsToSpawn = { "NumAssetsToSpawn", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcAssetScatter_CPP, NumAssetsToSpawn), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_NumAssetsToSpawn_MetaData), NewProp_NumAssetsToSpawn_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::NewProp_SpawnAreaSize = { "SpawnAreaSize", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcAssetScatter_CPP, SpawnAreaSize), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SpawnAreaSize_MetaData), NewProp_SpawnAreaSize_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::NewProp_ProceduralMesh,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::NewProp_Material,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::NewProp_AssetToSpawn,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::NewProp_NumAssetsToSpawn,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::NewProp_SpawnAreaSize,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_endlessWalk,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::ClassParams = {
	&AuProcAssetScatter_CPP::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::PropPointers),
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::Class_MetaDataParams), Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AuProcAssetScatter_CPP()
{
	if (!Z_Registration_Info_UClass_AuProcAssetScatter_CPP.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AuProcAssetScatter_CPP.OuterSingleton, Z_Construct_UClass_AuProcAssetScatter_CPP_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AuProcAssetScatter_CPP.OuterSingleton;
}
template<> ENDLESSWALK_API UClass* StaticClass<AuProcAssetScatter_CPP>()
{
	return AuProcAssetScatter_CPP::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AuProcAssetScatter_CPP);
AuProcAssetScatter_CPP::~AuProcAssetScatter_CPP() {}
// End Class AuProcAssetScatter_CPP

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_uProcAssetScatter_CPP_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AuProcAssetScatter_CPP, AuProcAssetScatter_CPP::StaticClass, TEXT("AuProcAssetScatter_CPP"), &Z_Registration_Info_UClass_AuProcAssetScatter_CPP, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AuProcAssetScatter_CPP), 4277819088U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_uProcAssetScatter_CPP_h_3979949915(TEXT("/Script/endlessWalk"),
	Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_uProcAssetScatter_CPP_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_uProcAssetScatter_CPP_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
