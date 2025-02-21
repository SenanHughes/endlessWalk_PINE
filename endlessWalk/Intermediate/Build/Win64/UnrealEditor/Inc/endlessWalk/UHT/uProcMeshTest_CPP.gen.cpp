// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "endlessWalk/Public/uProcMeshTest_CPP.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeuProcMeshTest_CPP() {}

// Begin Cross Module References
ENDLESSWALK_API UClass* Z_Construct_UClass_AuProcMeshTest_CPP();
ENDLESSWALK_API UClass* Z_Construct_UClass_AuProcMeshTest_CPP_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_AActor();
ENGINE_API UClass* Z_Construct_UClass_USplineComponent_NoRegister();
PROCEDURALMESHCOMPONENT_API UClass* Z_Construct_UClass_UProceduralMeshComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_endlessWalk();
// End Cross Module References

// Begin Class AuProcMeshTest_CPP
void AuProcMeshTest_CPP::StaticRegisterNativesAuProcMeshTest_CPP()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AuProcMeshTest_CPP);
UClass* Z_Construct_UClass_AuProcMeshTest_CPP_NoRegister()
{
	return AuProcMeshTest_CPP::StaticClass();
}
struct Z_Construct_UClass_AuProcMeshTest_CPP_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "uProcMeshTest_CPP.h" },
		{ "ModuleRelativePath", "Public/uProcMeshTest_CPP.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MeshComponent_MetaData[] = {
		{ "Category", "ProceduralMesh" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/uProcMeshTest_CPP.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Spline_MetaData[] = {
		{ "Category", "Terrain" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// The Spline to generate terrain along\n" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/uProcMeshTest_CPP.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "The Spline to generate terrain along" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_MeshComponent;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Spline;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AuProcMeshTest_CPP>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AuProcMeshTest_CPP_Statics::NewProp_MeshComponent = { "MeshComponent", nullptr, (EPropertyFlags)0x00100000000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcMeshTest_CPP, MeshComponent), Z_Construct_UClass_UProceduralMeshComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MeshComponent_MetaData), NewProp_MeshComponent_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AuProcMeshTest_CPP_Statics::NewProp_Spline = { "Spline", nullptr, (EPropertyFlags)0x00100000000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AuProcMeshTest_CPP, Spline), Z_Construct_UClass_USplineComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Spline_MetaData), NewProp_Spline_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AuProcMeshTest_CPP_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcMeshTest_CPP_Statics::NewProp_MeshComponent,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AuProcMeshTest_CPP_Statics::NewProp_Spline,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AuProcMeshTest_CPP_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_AuProcMeshTest_CPP_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_endlessWalk,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AuProcMeshTest_CPP_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AuProcMeshTest_CPP_Statics::ClassParams = {
	&AuProcMeshTest_CPP::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_AuProcMeshTest_CPP_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_AuProcMeshTest_CPP_Statics::PropPointers),
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AuProcMeshTest_CPP_Statics::Class_MetaDataParams), Z_Construct_UClass_AuProcMeshTest_CPP_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AuProcMeshTest_CPP()
{
	if (!Z_Registration_Info_UClass_AuProcMeshTest_CPP.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AuProcMeshTest_CPP.OuterSingleton, Z_Construct_UClass_AuProcMeshTest_CPP_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AuProcMeshTest_CPP.OuterSingleton;
}
template<> ENDLESSWALK_API UClass* StaticClass<AuProcMeshTest_CPP>()
{
	return AuProcMeshTest_CPP::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AuProcMeshTest_CPP);
AuProcMeshTest_CPP::~AuProcMeshTest_CPP() {}
// End Class AuProcMeshTest_CPP

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_uProcMeshTest_CPP_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AuProcMeshTest_CPP, AuProcMeshTest_CPP::StaticClass, TEXT("AuProcMeshTest_CPP"), &Z_Registration_Info_UClass_AuProcMeshTest_CPP, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AuProcMeshTest_CPP), 300119068U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_uProcMeshTest_CPP_h_1275891895(TEXT("/Script/endlessWalk"),
	Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_uProcMeshTest_CPP_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_senan_OneDrive_Documents_Masters_in_Computing_in_Comp_Anim_finalProject_projectFiles_endlessWalk_Source_endlessWalk_Public_uProcMeshTest_CPP_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
