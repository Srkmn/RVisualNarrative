#pragma once

#include "CoreMinimal.h"
#include "Decorator/RVNDecorator.h"
#include "Decorator/Condition/RVNCondition.h"
#include "RVNTask.generated.h"

struct FRVNTaskParams
{
	URVNComponent* InRVNComponent = nullptr;
};

UCLASS(Blueprintable)
class RVISUALNARRATIVE_API URVNTaskBase : public URVNDecorator
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual FString GetNodeIconName() const override;
	//Please note that the legality of the original object cannot be guaranteed after this deep copy
	virtual URVNDecorator* PasteDecorator() override;
	URVNTaskBase* PasteTaskNoChildren();
#endif

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Task")
	void OnStart(URVNComponent* OwnerComponent);

	virtual void OnStart_Implementation(URVNComponent* OwnerComponent)
	{
	}

	UFUNCTION(BlueprintNativeEvent, Category = "Task")
	void OnEnd(URVNComponent* OwnerComponent);

	virtual void OnEnd_Implementation(URVNComponent* OwnerComponent)
	{
	}

	UFUNCTION(BlueprintNativeEvent, Category = "Task")
	void ExecuteTask(URVNComponent* OwnerComponent);

	virtual void ExecuteTask_Implementation(URVNComponent* OwnerComponent)
	{
	}

	const TArray<TObjectPtr<URVNConditionBase>>& GetConditions() const;
	TArray<TObjectPtr<URVNConditionBase>>& GetConditions();

	const TArray<TObjectPtr<URVNTaskBase>>& GetChildren() const;
	TArray<TObjectPtr<URVNTaskBase>>& GetChildren();

#if WITH_EDITOR

public:
	void AddCondition(URVNConditionBase* Condition);
	void RemoveCondition(URVNConditionBase* Condition);
	void SetChildren(const TArray<TObjectPtr<URVNTaskBase>>& NewChildren);
#endif

#if WITH_EDITOR

private:
	URVNTaskBase* PasteDecorator_Internal(TMap<URVNTaskBase*, URVNTaskBase*>& CloneMap);
#endif

private:
	UPROPERTY()
	TArray<TObjectPtr<URVNConditionBase>> Conditions;

	UPROPERTY()
	TArray<TObjectPtr<URVNTaskBase>> Children;
};
