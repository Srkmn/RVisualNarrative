#pragma once

#include "CoreMinimal.h"
#include "RVNBlackboardKeyType.h"
#include "RVNBlackboardKeyType_String.generated.h"

UCLASS(EditInlineNew, meta=(DisplayName="String"))
class RVISUALNARRATIVE_API URVNBlackboardKeyType_String : public URVNBlackboardKeyType
{
	GENERATED_BODY()

public:
	URVNBlackboardKeyType_String();

	typedef FString FDataType;

	static const FDataType InvalidValue;

	static FString GetValue(const URVNBlackboardKeyType_String* KeyOb, const uint8* RawData);

	static bool SetValue(URVNBlackboardKeyType_String* KeyOb, uint8* RawData, const FString& Value);

	virtual ERVNBlackboardCompare::Type CompareValues(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                                  const URVNBlackboardKeyType* OtherKeyOb,
	                                                  const uint8* OtherMemoryBlock) const override;

	UPROPERTY()
	FString StringValue;

protected:
	virtual FString DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const override;

	virtual bool TestTextOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                               ERVNTextKeyOperation::Type Op, const FString& OtherString) const override;

	virtual void Clear(URVNDialogueManager& OwnerComp, uint8* MemoryBlock) override;

	virtual bool IsEmpty(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock) const override;

	virtual void CopyValues(URVNDialogueManager& OwnerComp, uint8* MemoryBlock,
	                        const URVNBlackboardKeyType* SourceKeyOb, const uint8* SourceBlock) override;
};
