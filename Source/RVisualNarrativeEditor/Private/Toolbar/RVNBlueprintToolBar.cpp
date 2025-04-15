#include "Toolbar/RVNBlueprintToolBar.h"
#include "BlueprintEditorModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Blueprint/RVNAssetBlueprint.h"
#include "RVNComponent.h"
#include "RVNEditorCommands.h"

#define LOCTEXT_NAMESPACE "RVNBlueprintToolBar"

FRVNBlueprintToolBar::FRVNBlueprintToolBar()
	: CommandList(MakeShared<FUICommandList>())
{
	BuildAction();
}

void FRVNBlueprintToolBar::Initialize()
{
	auto& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");

	BlueprintEditorModule.GetMenuExtensibilityManager()->GetExtenderDelegates().Add(
		FAssetEditorExtender::CreateLambda(
			[&](const TSharedRef<FUICommandList>& InCommandList, const TArray<UObject*>& InContextSensitiveObjects)
			{
				return GenerateBlueprintExtender(InContextSensitiveObjects.IsEmpty()
					                                 ? nullptr
					                                 : Cast<UBlueprint>(InContextSensitiveObjects[0]));
			}));
}

void FRVNBlueprintToolBar::Deinitialize()
{
}

void FRVNBlueprintToolBar::BuildAction()
{
	CommandList->MapAction(
		FRVNEditorCommands::Get().CompactNodesData,
		FExecuteAction::CreateLambda([this]
		{
			if (!Blueprint.IsValid())
			{
				return;
			}

			if (Blueprint->GeneratedClass == nullptr)
			{
				return;
			}

			if (Blueprint->GeneratedClass->ClassDefaultObject == nullptr)
			{
				return;
			}

			if (const auto RVNComponent = Cast<URVNComponent>(Blueprint->GeneratedClass->ClassDefaultObject))
			{
				FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint.Get());

				RVNComponent->CompactNodesData();
			}
		}), FCanExecuteAction());
}

TSharedRef<FExtender> FRVNBlueprintToolBar::GenerateBlueprintExtender(UBlueprint* InBlueprint)
{
	TSharedRef<FExtender> Extender(new FExtender());

	if (!InBlueprint->IsA(URVNAssetBlueprint::StaticClass()))
	{
		return Extender;
	}

	const auto ExtensionDelegate = FToolBarExtensionDelegate::CreateLambda(
		[this, InBlueprint](FToolBarBuilder& ToolbarBuilder)
		{
			ToolbarBuilder.BeginSection(NAME_None);

			ToolbarBuilder.AddComboButton(
				FUIAction(),
				FOnGetContent::CreateLambda([&]()
				{
					Blueprint = InBlueprint;

					const FRVNEditorCommands& Commands = FRVNEditorCommands::Get();

					FMenuBuilder MenuBuilder(true, CommandList);

					MenuBuilder.AddMenuEntry(Commands.CompactNodesData, NAME_None,
					                         LOCTEXT("CompactNodesData", "Compact Nodes Data"));

					return MenuBuilder.MakeWidget();
				}),
				LOCTEXT("RVisualNarrativeEditor_Label", "RVisualNarrative"),
				LOCTEXT("RVisualNarrativeEditor_ToolTip", "RVisualNarrative"),
				FSlateIcon(FRVNEditorStyle::GetStyleSetName(), "RVNEditor.PluginAction")
			);

			ToolbarBuilder.EndSection();
		});

	Extender->AddToolBarExtension("Debugging", EExtensionHook::After, CommandList, ExtensionDelegate);

	return Extender;
}

#undef LOCTEXT_NAMESPACE
