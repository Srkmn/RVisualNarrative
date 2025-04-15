#pragma once

#include "CoreMinimal.h"
#include "RVNBlackboardKeyType.h"
#include "RVNBlackboardKeyType_Enum.generated.h"

UCLASS(EditInlineNew, meta=(DisplayName="Enum"))
class RVISUALNARRATIVE_API URVNBlackboardKeyType_Enum : public URVNBlackboardKeyType
{
	GENERATED_BODY()

public:
	URVNBlackboardKeyType_Enum();

	typedef uint8 FDataType;

	static const FDataType InvalidValue;

	UPROPERTY(Category=Blackboard, EditDefaultsOnly)
	TObjectPtr<UEnum> EnumType;

	UPROPERTY(Category=Blackboard, EditDefaultsOnly)
	FString EnumName;

	UPROPERTY(Category = Blackboard, VisibleDefaultsOnly)
	uint32 bIsEnumNameValid : 1;

	static uint8 GetValue(const URVNBlackboardKeyType_Enum* KeyOb, const uint8* RawData);

	static bool SetValue(URVNBlackboardKeyType_Enum* KeyOb, uint8* RawData, uint8 Value);

	virtual ERVNBlackboardCompare::Type CompareValues(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                                  const URVNBlackboardKeyType* OtherKeyOb,
	                                                  const uint8* OtherMemoryBlock) const override;

	virtual FString DescribeSelf() const override;

	virtual FString DescribeArithmeticParam(int32 IntValue, float FloatValue) const override;

	virtual bool IsAllowedByFilter(URVNBlackboardKeyType* FilterOb) const override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	virtual FString DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const override;

	virtual bool TestArithmeticOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                     ERVNArithmeticKeyOperation::Type Op, int32 OtherIntValue,
	                                     float OtherFloatValue) const override;
};
