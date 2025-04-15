#include "RVisualNarrativeEditor/Public/AssetTypeActions_RVNComponent.h"
#include "Blueprint/RVNAssetBlueprint.h"
#include "RVNEditor.h"

FAssetTypeActions_RVNComponent::FAssetTypeActions_RVNComponent(EAssetTypeCategories::Type AsstCategory)
{
	RVNAssetCategory = AsstCategory;
}

FText FAssetTypeActions_RVNComponent::GetName() const
{
	return NSLOCTEXT("RVNActions", "RVNActionsName", "RVisual Narrative Component");
}

FColor FAssetTypeActions_RVNComponent::GetTypeColor() const
{
	return FColor(255, 175, 0);
}

UClass* FAssetTypeActions_RVNComponent::GetSupportedClass() const
{
	return URVNAssetBlueprint::StaticClass();
}

void FAssetTypeActions_RVNComponent::OpenAssetEditor(const TArray<UObject*>& InObjects,
                                       const TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	const auto Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (const auto Object : InObjects)
	{
		if (const auto NewBlueprint = Cast<URVNAssetBlueprint>(Object))
		{
			const auto NewEditor = MakeShared<FRVNEditor>();

			NewEditor->InitRVNEditor(Mode, EditWithinLevelEditor, NewBlueprint);
		}
	}
}

uint32 FAssetTypeActions_RVNComponent::GetCategories()
{
	return RVNAssetCategory;
}
