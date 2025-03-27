﻿#include "RVisualNarrativeEditor.h"
#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "RVNAssetActions.h"
#include "EdGraphUtilities.h"
#include "RVNEditorCommands.h"
#include "RVNEditorStyle.h"
#include "ToolMenus.h"
#include "ClassCollector/RVNClassCollector.h"
#include "Graph/RVNGraphPanelFactories.h"
#include "Graph/Node/RVNAssetTypeActions_Node.h"
#include "Blueprint/RVNConditionNodeBlueprint.h"
#include "Blueprint/RVNTaskNodeBlueprint.h"
#include "Settings/RVisualNarrativeEditorSetting.h"
#include "Toolbar/RVNBlueprintToolBar.h"
#include "Toolbar/RVNPlayToolBar.h"

#define LOCTEXT_NAMESPACE "FRVisualNarrativeEditorModule"

EAssetTypeCategories::Type FRVisualNarrativeEditorModule::RVNAssetCategoryType = EAssetTypeCategories::None;

void FRVisualNarrativeEditorModule::StartupModule()
{
	TArray<TSubclassOf<UObject>> DefaultSupportedAssetClasses = {
		URVNTaskNodeBlueprint::StaticClass(),
		URVNConditionNodeBlueprint::StaticClass()
	};

	URVisualNarrativeEditorSetting::RegisterSettings(DefaultSupportedAssetClasses);

	FRVNEditorStyle::Initialize();

	FRVNEditorStyle::ReloadTextures();

	FRVNEditorCommands::Register();

	RVNPlayToolBar = MakeShared<FRVNPlayToolBar>();

	RVNBlueprintToolBar = MakeShared<FRVNBlueprintToolBar>();

	OnPostEngineInitDelegateHandle = FCoreDelegates::OnPostEngineInit.AddRaw(
		this, &FRVisualNarrativeEditorModule::OnPostEngineInit);

	RegisterRVNAsset();

	FEdGraphUtilities::RegisterVisualNodeFactory(MakeShared<FRVNGraphNodeFactory>());
}

void FRVisualNarrativeEditorModule::ShutdownModule()
{
	FEdGraphUtilities::UnregisterVisualNodeFactory(MakeShared<FRVNGraphNodeFactory>());

	FCoreDelegates::OnPostEngineInit.Remove(OnPostEngineInitDelegateHandle);

	URVisualNarrativeEditorSetting::UnregisterSettings();
}

FRVisualNarrativeEditorModule& FRVisualNarrativeEditorModule::Get()
{
	return FModuleManager::LoadModuleChecked<FRVisualNarrativeEditorModule>("RVisualNarrativeEditor");
}

void FRVisualNarrativeEditorModule::GetClasses(UClass* KeyClass, TArray<UClass*>& OutClasses)
{
	if (!ClassCollector.IsValid())
	{
		ClassCollector = MakeShared<FRVNClassCollector>();
	}

	ClassCollector->GetClasses(KeyClass, OutClasses);
}

void FRVisualNarrativeEditorModule::OnPostEngineInit()
{
	RegisterMenus();
}

void FRVisualNarrativeEditorModule::RegisterRVNAsset()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	RVNAssetCategoryType = AssetTools.RegisterAdvancedAssetCategory(
		TEXT("RVNCategoryType"), LOCTEXT("RVNCategoryType", "RVN"));

	{
		const auto AssetActionsPtr = MakeShared<FRVNAssetActions>(RVNAssetCategoryType);

		AssetTools.RegisterAssetTypeActions(AssetActionsPtr);
	}

	{
		const auto TaskBlueprintAction = MakeShared<FAssetTypeActions_RVNTaskBlueprint>();

		AssetTools.RegisterAssetTypeActions(TaskBlueprintAction);
	}

	{
		const auto ConditionBlueprintAction = MakeShared<FAssetTypeActions_RVNConditionBlueprint>();

		AssetTools.RegisterAssetTypeActions(ConditionBlueprintAction);
	}
}

void FRVisualNarrativeEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	if (RVNBlueprintToolBar.IsValid())
	{
		RVNBlueprintToolBar->Initialize();
	}

	if (RVNPlayToolBar.IsValid())
	{
		RVNPlayToolBar->Initialize();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRVisualNarrativeEditorModule, RVisualNarrativeEditor)
