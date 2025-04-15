#pragma once

#include "CoreMinimal.h"
#include "RVNBlackboardKeyType.h"
#include "RVNBlackboardKeyType_Bool.generated.h"

UCLASS(EditInlineNew, meta=(DisplayName="Bool"))
class RVISUALNARRATIVE_API URVNBlackboardKeyType_Bool : public URVNBlackboardKeyType
{
	GENERATED_BODY()

public:
	URVNBlackboardKeyType_Bool();

	typedef bool FDataType;

	static const FDataType InvalidValue;

	static bool GetValue(const URVNBlackboardKeyType_Bool* KeyOb, const uint8* RawData);

	static bool SetValue(URVNBlackboardKeyType_Bool* KeyOb, uint8* RawData, bool bValue);

	virtual ERVNBlackboardCompare::Type CompareValues(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                                  const URVNBlackboardKeyType* OtherKeyOb,
	                                                  const uint8* OtherMemoryBlock) const override;

protected:
	virtual FString DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const override;

	virtual bool TestBasicOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                ERVNBasicKeyOperation::Type Op) const override;
};
