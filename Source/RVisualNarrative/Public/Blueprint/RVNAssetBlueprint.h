#pragma once

#include "CoreMinimal.h"
#include "Engine/Blueprint.h"
#include "RVNAssetBlueprint.generated.h"

UCLASS()
class RVISUALNARRATIVE_API URVNAssetBlueprint : public UBlueprint
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UEdGraph> DialogueGraphPtr;

	UPROPERTY()
	TObjectPtr<UEdGraph> EventGraphPtr;
};
