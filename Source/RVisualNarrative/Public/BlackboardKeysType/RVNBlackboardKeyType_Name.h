#pragma once

#include "CoreMinimal.h"
#include "RVNBlackboardKeyType.h"
#include "RVNBlackboardKeyType_Name.generated.h"

UCLASS(EditInlineNew, meta=(DisplayName="Name"))
class RVISUALNARRATIVE_API URVNBlackboardKeyType_Name : public URVNBlackboardKeyType
{
	GENERATED_BODY()

public:
	URVNBlackboardKeyType_Name();

	typedef FName FDataType;

	static const FDataType InvalidValue;

	static FName GetValue(const URVNBlackboardKeyType_Name* KeyOb, const uint8* RawData);

	static bool SetValue(URVNBlackboardKeyType_Name* KeyOb, uint8* RawData, const FName& Value);

	virtual ERVNBlackboardCompare::Type CompareValues(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                                  const URVNBlackboardKeyType* OtherKeyOb,
	                                                  const uint8* OtherMemoryBlock) const override;

protected:
	virtual FString DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const override;

	virtual bool TestTextOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                               ERVNTextKeyOperation::Type Op, const FString& OtherString) const override;
};
