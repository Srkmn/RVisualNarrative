#pragma once

#include "CoreMinimal.h"
#include "RVNBlackboardData.h"
#include "UObject/Object.h"
#include "RVNBlackboardKeyType.generated.h"

class URVNDialogueManager;

namespace ERVNBlackboardCompare
{
	enum Type
	{
		Less = -1,
		Equal = 0,
		Greater = 1,

		NotEqual = 1,
	};
}

namespace ERVNBlackboardKeyOperation
{
	enum Type
	{
		Basic,
		Arithmetic,
		Text,
	};
}

UENUM()
namespace ERVNBasicKeyOperation
{
	enum Type
	{
		Set UMETA(DisplayName="Is Set"),
		NotSet UMETA(DisplayName="Is Not Set"),
	};
}

UENUM()
namespace ERVNArithmeticKeyOperation
{
	enum Type
	{
		Equal UMETA(DisplayName="Is Equal To"),
		NotEqual UMETA(DisplayName="Is Not Equal To"),
		Less UMETA(DisplayName="Is Less Than"),
		LessOrEqual UMETA(DisplayName="Is Less Than Or Equal To"),
		Greater UMETA(DisplayName="Is Greater Than"),
		GreaterOrEqual UMETA(DisplayName="Is Greater Than Or Equal To"),
	};
}

UENUM()
namespace ERVNTextKeyOperation
{
	enum Type
	{
		Equal UMETA(DisplayName="Is Equal To"),
		NotEqual UMETA(DisplayName="Is Not Equal To"),
		Contain UMETA(DisplayName="Contains"),
		NotContain UMETA(DisplayName="Not Contains"),
	};
}

struct FRVNBlackboardInstancedKeyMemory
{
	int32 KeyIdx;
};

UCLASS(EditInlineNew, Abstract, CollapseCategories, AutoExpandCategories=(Blackboard))
class RVISUALNARRATIVE_API URVNBlackboardKeyType : public UObject
{
	GENERATED_BODY()

public:
	URVNBlackboardKeyType();

	virtual void PreInitialize(URVNDialogueManager& OwnerComp);

	void InitializeKey(URVNDialogueManager& OwnerComp, FRVNBlackboard::FKey KeyID);

	virtual bool IsAllowedByFilter(URVNBlackboardKeyType* FilterOb) const;

	bool WrappedGetLocation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock, FVector& Location) const;

	bool WrappedGetRotation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock, FRotator& Rotation) const;

	void WrappedFree(URVNDialogueManager& OwnerComp, uint8* MemoryBlock);

	void WrappedClear(const URVNDialogueManager& OwnerComp, uint8* MemoryBlock) const;

	bool WrappedIsEmpty(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock) const;

	virtual FString DescribeArithmeticParam(int32 IntValue, float FloatValue) const;

	virtual FString DescribeSelf() const;

	virtual URVNBlackboardKeyType* UpdateDeprecatedKey();

	const URVNBlackboardKeyType* GetKeyInstance(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock) const;
	URVNBlackboardKeyType* GetKeyInstance(URVNDialogueManager& OwnerComp, const uint8* MemoryBlock) const;

	virtual ERVNBlackboardCompare::Type CompareValues(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                                  const URVNBlackboardKeyType* OtherKeyOb,
	                                                  const uint8* OtherMemoryBlock) const;

	bool HasInstance() const;

	bool IsInstanced() const;

	uint16 GetValueSize() const;

	ERVNBlackboardKeyOperation::Type GetTestOperation() const;

protected:
	uint16 ValueSize;

	TEnumAsByte<ERVNBlackboardKeyOperation::Type> SupportedOp;

	uint8 bIsInstanced : 1;

	uint8 bCreateKeyInstance : 1;

	template <typename T>
	static T GetValueFromMemory(const uint8* MemoryBlock)
	{
		return *((T*)MemoryBlock);
	}

	template <typename T>
	static bool SetValueInMemory(uint8* MemoryBlock, const T& Value)
	{
		const bool bChanged = *((T*)MemoryBlock) != Value;
		*((T*)MemoryBlock) = Value;

		return bChanged;
	}

	template <typename T>
	static bool SetWeakObjectInMemory(uint8* MemoryBlock, const TWeakObjectPtr<T>& Value)
	{
		TWeakObjectPtr<T>* PrevValue = (TWeakObjectPtr<T>*)MemoryBlock;
		const bool bChanged =
			(Value.IsValid(false, true) != PrevValue->IsValid(false, true)) ||
			(Value.IsStale(false, true) != PrevValue->IsStale(false, true)) ||
			(*PrevValue) != Value;

		*((TWeakObjectPtr<T>*)MemoryBlock) = Value;

		return bChanged;
	}

	virtual void CopyValues(URVNDialogueManager& OwnerComp, uint8* MemoryBlock,
	                        const URVNBlackboardKeyType* SourceKeyOb, const uint8* SourceBlock);

	virtual void InitializeMemory(URVNDialogueManager& OwnerComp, uint8* MemoryBlock);

	virtual void FreeMemory(URVNDialogueManager& OwnerComp, uint8* MemoryBlock);

	virtual bool GetLocation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock, FVector& Location) const;

	virtual bool GetRotation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock, FRotator& Rotation) const;

	virtual void Clear(URVNDialogueManager& OwnerComp, uint8* MemoryBlock);

	virtual bool IsEmpty(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock) const;

	virtual bool TestBasicOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                ERVNBasicKeyOperation::Type Op) const;
	virtual bool TestArithmeticOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                     ERVNArithmeticKeyOperation::Type Op, int32 OtherIntValue,
	                                     float OtherFloatValue) const;
	virtual bool TestTextOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                               ERVNTextKeyOperation::Type Op, const FString& OtherString) const;

	virtual FString DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock) const;

	friend URVNDialogueManager;
};
