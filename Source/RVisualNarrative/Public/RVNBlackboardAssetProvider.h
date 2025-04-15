#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"
#include "RVNBlackboardAssetProvider.generated.h"

class URVNBlackboardData;

UINTERFACE(BlueprintType, MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class URVNBlackboardAssetProvider : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class RVISUALNARRATIVE_API IRVNBlackboardAssetProvider
{
	GENERATED_IINTERFACE_BODY()
#if WITH_EDITOR
	DECLARE_MULTICAST_DELEGATE_TwoParams(FBlackboardOwnerChanged, UObject*, URVNBlackboardData*);

	static FBlackboardOwnerChanged OnBlackboardOwnerChanged;
#endif

	UFUNCTION(BlueprintCallable, Category = GameplayTags)
	virtual URVNBlackboardData* GetBlackboardAsset() const PURE_VIRTUAL(IRVNBlackboardAssetProvider::GetBlackboardAsset,
	                                                                    return nullptr;);
};
