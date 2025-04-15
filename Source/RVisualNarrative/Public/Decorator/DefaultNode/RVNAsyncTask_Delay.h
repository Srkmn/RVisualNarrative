#pragma once

#include "CoreMinimal.h"
#include "Decorator/Task/RVNAsyncTask.h"
#include "RVNAsyncTask_Delay.generated.h"

UCLASS(Blueprintable)
class RVISUALNARRATIVE_API URVNAsyncTask_Delay : public URVNAsyncTask
{
	GENERATED_BODY()

public:
	virtual void OnStart_Implementation(URVNComponent* OwnerComponent) override;

	virtual void ExecuteTask_Implementation(URVNComponent* OwnerComponent) override;

	virtual void OnEnd_Implementation(URVNComponent* OwnerComponent) override;

	virtual void BreakAsyncTask_Implementation(URVNComponent* OwnerComponent) override;

public:
	UPROPERTY(EditAnywhere, Category = "Delay")
	float DelayTime = 2.f;

	FTimerHandle TimerHandle;
};
