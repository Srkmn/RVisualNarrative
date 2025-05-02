#pragma once

#include "CoreMinimal.h"
#include "Decorator/RVNDecorator.h"
#include "RVNCondition.generated.h"

struct FRVNConditionParams
{
	bool bIsPass = false;
	URVNComponent* InRVNComponent = nullptr;
};

UCLASS(abstract)
class RVISUALNARRATIVE_API URVNConditionBase : public URVNDecorator
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual FString GetNodeIconName() const override;

	virtual URVNDecorator* PasteDecorator() override;
#endif

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Condition")
	bool CheckCondition(URVNComponent* OwnerComponent);
	virtual bool CheckCondition_Implementation(URVNComponent* OwnerComponent);
};
