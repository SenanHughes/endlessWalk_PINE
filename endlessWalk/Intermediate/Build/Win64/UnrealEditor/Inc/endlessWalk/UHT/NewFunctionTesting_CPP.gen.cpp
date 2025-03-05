// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "endlessWalk/Public/NewFunctionTesting_CPP.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeNewFunctionTesting_CPP() {}

// Begin Cross Module References
ENDLESSWALK_API UClass* Z_Construct_UClass_ANewFunctionTesting_CPP();
ENDLESSWALK_API UClass* Z_Construct_UClass_ANewFunctionTesting_CPP_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_AActor();
ENGINE_API UClass* Z_Construct_UClass_USplineComponent_NoRegister();
PROCEDURALMESHCOMPONENT_API UClass* Z_Construct_UClass_UProceduralMeshComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_endlessWalk();
// End Cross Module References

// Begin Class ANewFunctionTesting_CPP
void ANewFunctionTesting_CPP::StaticRegisterNativesANewFunctionTesting_CPP()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ANewFunctionTesting_CPP);
UClass* Z_Construct_UClass_ANewFunctionTesting_CPP_NoRegister()
{
	return ANewFunctionTesting_CPP::StaticClass();
}
struct Z_Construct_UClass_ANewFunctionTesting_CPP_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "NewFunctionTesting_CPP.h" },
		{ "ModuleRelativePath", "Public/NewFunctionTesting_CPP.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PathSpline_MetaData[] = {
		{ "Category", "Terrain" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/NewFunctionTesting_CPP.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PathMesh_MetaData[] = {
		{ "Category", "ProceduralMesh" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/NewFunctionTesting_CPP.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PathSpline;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PathMesh;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ANewFunctionTesting_CPP>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ANewFunctionTesting_CPP_Statics::NewProp_PathSpline = { "PathSpline", nullptr, (EPropertyFlags)0x00100000000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ANewFunctionTesting_CPP, PathSpline), Z_Construct_UClass_USplineComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PathSpline_MetaData), NewProp_PathSpline_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ANewFunctionTesting_CPP_Statics::NewProp_PathMesh = { "PathMesh", nullptr, (EPropertyFlags)0x00100000000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ANewFunctionTesting_CPP, PathMesh), Z_Construct_UClass_UProceduralMeshComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PathMesh_MetaData), NewProp_PathMesh_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_ANewFunctionTesting_CPP_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ANewFunctionTesting_CPP_Statics::NewProp_PathSpline,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ANewFunctionTesting_CPP_Statics::NewProp_PathMesh,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ANewFunctionTesting_CPP_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_ANewFunctionTesting_CPP_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_endlessWalk,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ANewFunctionTesting_CPP_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ANewFunctionTesting_CPP_Statics::ClassParams = {
	&ANewFunctionTesting_CPP::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_ANewFunctionTesting_CPP_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_ANewFunctionTesting_CPP_Statics::PropPointers),
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ANewFunctionTesting_CPP_Statics::Class_MetaDataParams), Z_Construct_UClass_ANewFunctionTesting_CPP_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ANewFunctionTesting_CPP()
{
	if (!Z_Registration_Info_UClass_ANewFunctionTesting_CPP.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ANewFunctionTesting_CPP.OuterSingleton, Z_Construct_UClass_ANewFunctionTesting_CPP_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ANewFunctionTesting_CPP.OuterSingleton;
}
template<> ENDLESSWALK_API UClass* StaticClass<ANewFunctionTesting_CPP>()
{
	return ANewFunctionTesting_CPP::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ANewFunctionTesting_CPP);
ANewFunctionTesting_CPP::~ANewFunctionTesting_CPP() {}
// End Class ANewFunctionTesting_CPP

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_NewFunctionTesting_CPP_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ANewFunctionTesting_CPP, ANewFunctionTesting_CPP::StaticClass, TEXT("ANewFunctionTesting_CPP"), &Z_Registration_Info_UClass_ANewFunctionTesting_CPP, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ANewFunctionTesting_CPP), 3790083086U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_NewFunctionTesting_CPP_h_4005379561(TEXT("/Script/endlessWalk"),
	Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_NewFunctionTesting_CPP_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_NewFunctionTesting_CPP_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
