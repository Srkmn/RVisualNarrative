#include "RVisualNarrativeEditor.h"
#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "EdGraphUtilities.h"
#include "ToolMenus.h"
#include "RVNEditorCommands.h"
#include "RVNEditorStyle.h"
#include "ClassCollector/RVNClassCollector.h"
#include "Blueprint/RVNConditionNodeBlueprint.h"
#include "Blueprint/RVNTaskNodeBlueprint.h"
#include "Settings/RVisualNarrativeEditorSetting.h"
#include "Toolbar/RVNBlueprintToolBar.h"
#include "Toolbar/RVNPlayToolBar.h"
#include "AssetTypeActions_RVNBlackboard.h"
#include "AssetTypeActions_RVNComponent.h"
#include "Decorator/Condition/RVNCondition.h"
#include "Decorator/Task/RVNTask.h"
#include "Graph/RVNGraphPanelFactories.h"
#include "Graph/Node/RVNAssetTypeActions_Node.h"
#include "Graph/Node/RVNDecoratorItemFactory.h"
#include "Graph/Node/Slate/InternalWidget/SRVNConditionItemWidget.h"
#include "Graph/Node/Slate/InternalWidget/SRVNTaskItemWidget.h"

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

	FRVNBlackboardCommands::Register();

	RVNPlayToolBar = MakeShared<FRVNPlayToolBar>();

	RVNBlueprintToolBar = MakeShared<FRVNBlueprintToolBar>();

	OnPostEngineInitDelegateHandle = FCoreDelegates::OnPostEngineInit.AddRaw(
		this, &FRVisualNarrativeEditorModule::OnPostEngineInit);

	RegisterRVNAsset();

	FEdGraphUtilities::RegisterVisualNodeFactory(MakeShared<FRVNGraphNodeFactory>());

	FRVNDecoratorItemFactory::RegisterDecoratorItemClass<URVNTaskBase, SRVNTaskItemWidget>();
	FRVNDecoratorItemFactory::RegisterDecoratorItemClass<URVNConditionBase, SRVNConditionItemWidget>();
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
		TEXT("RVNCategoryType"), LOCTEXT("RVNCategoryType", "RVisual Narrative"));

	{
		const auto ComponentActionsPtr = MakeShared<FAssetTypeActions_RVNComponent>(RVNAssetCategoryType);

		AssetTools.RegisterAssetTypeActions(ComponentActionsPtr);
	}

	{
		const auto BlackboardActionsPtr = MakeShared<FAssetTypeActions_RVNBlackboard>(RVNAssetCategoryType);

		AssetTools.RegisterAssetTypeActions(BlackboardActionsPtr);
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
