#include "AssetTypeActions_RVNBlackboard.h"
#include "RVNBlackboardData.h"

FAssetTypeActions_RVNBlackboard::FAssetTypeActions_RVNBlackboard(EAssetTypeCategories::Type AsstCategory)
{
	RVNAssetCategory = AsstCategory;
}

FText FAssetTypeActions_RVNBlackboard::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_Blackboard", "RVisual Narrative Blackboard");
}

FColor FAssetTypeActions_RVNBlackboard::GetTypeColor() const
{
	return FColor(201, 29, 85);
}

UClass* FAssetTypeActions_RVNBlackboard::GetSupportedClass() const
{
	return URVNBlackboardData::StaticClass();
}

void FAssetTypeActions_RVNBlackboard::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	// @todo
}

uint32 FAssetTypeActions_RVNBlackboard::GetCategories()
{
	return RVNAssetCategory;
}
