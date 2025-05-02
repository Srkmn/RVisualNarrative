#pragma once

#include "CoreMinimal.h"
#include "RVNTask.h"
#include "RVNAsyncTask.generated.h"

UCLASS(abstract)
class RVISUALNARRATIVE_API URVNAsyncTask : public URVNTaskBase
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE(FAsyncTaskCompleted);

public:
	virtual void OnStart_Implementation(URVNComponent* OwnerComponent) override;

	virtual void ExecuteTask_Implementation(URVNComponent* OwnerComponent) override;

	virtual void OnEnd_Implementation(URVNComponent* OwnerComponent) override;

	FORCEINLINE FAsyncTaskCompleted& GetCompletedCallback() { return CompletedCallback; }

	UFUNCTION(BlueprintNativeEvent, Category = "Task")
	void BreakAsyncTask(URVNComponent* OwnerComponent);

	virtual void BreakAsyncTask_Implementation(URVNComponent* OwnerComponent)
	{
		CompletedCallback.Unbind();
	}

protected:
	// 当你结束异步任务时，调用此函数来通知任务完成。
	UFUNCTION(BlueprintCallable, Category = "Task")
	virtual void FinishAsyncTask();

	FAsyncTaskCompleted CompletedCallback;
};
