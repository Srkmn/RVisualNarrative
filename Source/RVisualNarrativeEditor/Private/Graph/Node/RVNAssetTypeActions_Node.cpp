#include "Graph/Node/RVNAssetTypeActions_Node.h"

#include "RVisualNarrativeEditor.h"
#include "Decorator/Condition/RVNCondition.h"
#include "Decorator/DefaultNode/RVNAsyncTask_Delay.h"
#include "Decorator/Task/RVNSyncTask.h"
#include "Graph/Node/RVNNodeEditor.h"
#include "Graph/Node/Factory/RVNConditionFactory.h"
#include "Graph/Node/Factory/RVNTaskFactory.h"
#include "Blueprint/RVNConditionNodeBlueprint.h"
#include "Blueprint/RVNTaskNodeBlueprint.h"

#define LOCTEXT_NAMESPACE "RVNAssetTypeActions_Node"

uint32 FRVNAssetTypeActions_Base::GetCategories()
{
	return AssetCategory;
}

FColor FRVNAssetTypeActions_Base::GetTypeColor() const
{
	return FColor(129, 196, 115);
}

FText FAssetTypeActions_RVNTaskBlueprint::GetName() const
{
	return NSLOCTEXT("TaskNodeTypeActions", "AssetTypeActions_RVNTaskBlueprint", "RVisual Narrative Task Node");
}

FColor FAssetTypeActions_RVNTaskBlueprint::GetTypeColor() const
{
	return FColor(175, 0, 255);
}

UClass* FAssetTypeActions_RVNTaskBlueprint::GetSupportedClass() const
{
	return URVNTaskNodeBlueprint::StaticClass();
}

void FAssetTypeActions_RVNTaskBlueprint::OpenAssetEditor(const TArray<UObject*>& InObjects,
                                                         TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (const auto Blueprint = Cast<UBlueprint>(*ObjIt))
		{
			bool bLetOpen = true;
			if (!Blueprint->ParentClass)
			{
				bLetOpen = EAppReturnType::Yes == FMessageDialog::Open(
					EAppMsgType::YesNo,
					LOCTEXT("FailedToLoadRVNNodeBPWithContinue",
					        "RVN Node Blueprint could not be loaded because it derives from an invalid class.  "
					        "Check to make sure the parent class for this blueprint hasn't been removed! "
					        "Do you want to continue (it can crash the editor)?")
				);
			}

			if (bLetOpen)
			{
				TSharedRef<FRVNNodeEditor> NewEditor(new FRVNNodeEditor());

				TArray<UBlueprint*> Blueprints;
				Blueprints.Add(Blueprint);

				NewEditor->InitNodeEditor(Mode, EditWithinLevelEditor, Blueprints, ShouldUseDataOnlyEditor(Blueprint));
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok,
			                     LOCTEXT("FailedToLoadNodeBlueprint",
			                             "RVN Node Blueprint could not be loaded because it derives from an invalid class.  "
			                             "Check to make sure the parent class for this blueprint hasn't been removed!"));
		}
	}
}

uint32 FAssetTypeActions_RVNTaskBlueprint::GetCategories()
{
	return FRVisualNarrativeEditorModule::GetRVNAssetCategoryType();
}

UFactory* FAssetTypeActions_RVNTaskBlueprint::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	URVNTaskFactory* GameplayAbilitiesBlueprintFactory = NewObject<URVNTaskFactory>();

	GameplayAbilitiesBlueprintFactory->ParentClass = TSubclassOf<URVNTaskBase>(*InBlueprint->GeneratedClass);

	return GameplayAbilitiesBlueprintFactory;
}

bool FAssetTypeActions_RVNTaskBlueprint::ShouldUseDataOnlyEditor(const UBlueprint* Blueprint) const
{
	if (Blueprint->GeneratedClass->IsChildOf(URVNAsyncTask_Delay::StaticClass()))
	{
		return true;
	}

	return false;
}

FText FAssetTypeActions_RVNConditionBlueprint::GetName() const
{
	return NSLOCTEXT("ConditionNodeTypeActions", "AssetTypeActions_RVNConditionBlueprint",
	                 "RVisual Narrative Condition Node");
}

FColor FAssetTypeActions_RVNConditionBlueprint::GetTypeColor() const
{
	return FColor(175, 0, 255);
}

UClass* FAssetTypeActions_RVNConditionBlueprint::GetSupportedClass() const
{
	return URVNConditionNodeBlueprint::StaticClass();
}

void FAssetTypeActions_RVNConditionBlueprint::OpenAssetEditor(const TArray<UObject*>& InObjects,
                                                              TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (const auto Blueprint = Cast<UBlueprint>(*ObjIt))
		{
			bool bLetOpen = true;
			if (!Blueprint->ParentClass)
			{
				bLetOpen = EAppReturnType::Yes == FMessageDialog::Open(
					EAppMsgType::YesNo,
					LOCTEXT("FailedToLoadRVNNodeBPWithContinue",
					        "RVN Node Blueprint could not be loaded because it derives from an invalid class. "
					        "Check to make sure the parent class for this blueprint hasn't been removed! "
					        "Do you want to continue (it can crash the editor)?")
				);
			}

			if (bLetOpen)
			{
				TSharedRef<FRVNNodeEditor> NewEditor(new FRVNNodeEditor());

				TArray<UBlueprint*> Blueprints;

				Blueprints.Add(Blueprint);

				NewEditor->InitNodeEditor(Mode, EditWithinLevelEditor, Blueprints, false);
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok,
			                     LOCTEXT("FailedToLoadNodeBlueprint",
			                             "RVN Node Blueprint could not be loaded because it derives from an invalid class. "
			                             "Check to make sure the parent class for this blueprint hasn't been removed!"));
		}
	}
}

uint32 FAssetTypeActions_RVNConditionBlueprint::GetCategories()
{
	return FRVisualNarrativeEditorModule::GetRVNAssetCategoryType();
}

UFactory* FAssetTypeActions_RVNConditionBlueprint::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	URVNConditionFactory* ConditionBlueprintFactory = NewObject<URVNConditionFactory>();

	ConditionBlueprintFactory->ParentClass = TSubclassOf<URVNConditionBase>(*InBlueprint->GeneratedClass);

	return ConditionBlueprintFactory;
}


#undef LOCTEXT_NAMESPACE
