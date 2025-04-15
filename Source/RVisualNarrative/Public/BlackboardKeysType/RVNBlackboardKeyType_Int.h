#pragma once

#include "CoreMinimal.h"
#include "RVNBlackboardKeyType.h"
#include "RVNBlackboardKeyType_Int.generated.h"

UCLASS(EditInlineNew, meta=(DisplayName="Int"))
class RVISUALNARRATIVE_API URVNBlackboardKeyType_Int : public URVNBlackboardKeyType
{
	GENERATED_BODY()

public:
	URVNBlackboardKeyType_Int();

	typedef int32 FDataType;

	static const FDataType InvalidValue;

	static int32 GetValue(const URVNBlackboardKeyType_Int* KeyOb, const uint8* RawData);

	static bool SetValue(URVNBlackboardKeyType_Int* KeyOb, uint8* RawData, int32 Value);

	virtual FString DescribeArithmeticParam(int32 IntValue, float FloatValue) const override;

	virtual ERVNBlackboardCompare::Type CompareValues(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                                  const URVNBlackboardKeyType* OtherKeyOb,
	                                                  const uint8* OtherMemoryBlock) const override;

protected:
	virtual FString DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const override;

	virtual bool TestArithmeticOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                     ERVNArithmeticKeyOperation::Type Op, int32 OtherIntValue,
	                                     float OtherFloatValue) const override;
};
