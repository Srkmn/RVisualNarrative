#pragma once

#include "CoreMinimal.h"
#include "RVNBlackboardKeyType.h"
#include "RVNBlackboardKeyType_Float.generated.h"

UCLASS(EditInlineNew, meta=(DisplayName="Float"))
class RVISUALNARRATIVE_API URVNBlackboardKeyType_Float : public URVNBlackboardKeyType
{
	GENERATED_BODY()

public:
	URVNBlackboardKeyType_Float();

	typedef float FDataType;

	static const FDataType InvalidValue;

	static float GetValue(const URVNBlackboardKeyType_Float* KeyOb, const uint8* RawData);

	static bool SetValue(URVNBlackboardKeyType_Float* KeyOb, uint8* RawData, float Value);

	virtual ERVNBlackboardCompare::Type CompareValues(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                                  const URVNBlackboardKeyType* OtherKeyOb,
	                                                  const uint8* OtherMemoryBlock) const override;

	virtual FString DescribeArithmeticParam(int32 IntValue, float FloatValue) const override;

protected:
	virtual FString DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const override;

	virtual bool TestArithmeticOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                     ERVNArithmeticKeyOperation::Type Op, int32 OtherIntValue,
	                                     float OtherFloatValue) const override;
};
