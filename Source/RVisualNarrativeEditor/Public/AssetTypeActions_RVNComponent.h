#pragma once

#include "AssetTypeActions_Base.h"

class FAssetTypeActions_RVNComponent : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_RVNComponent(EAssetTypeCategories::Type AsstCategory);

	virtual FText GetName() const override;

	virtual FColor GetTypeColor() const override;

	virtual UClass* GetSupportedClass() const override;

	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects,
	                             const TSharedPtr<IToolkitHost> EditWithinLevelEditor = TSharedPtr<
		                             IToolkitHost>()) override;

	virtual uint32 GetCategories() override;

private:
	EAssetTypeCategories::Type RVNAssetCategory;
};
