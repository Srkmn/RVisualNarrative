#pragma once

#include "CoreMinimal.h"
#include "RVNBlackboardKeyType.h"
#include "RVNBlackboardKeyType_Rotator.generated.h"

UCLASS(EditInlineNew, meta=(DisplayName="Rotator"))
class RVISUALNARRATIVE_API URVNBlackboardKeyType_Rotator : public URVNBlackboardKeyType
{
	GENERATED_BODY()

public:
	URVNBlackboardKeyType_Rotator();

	typedef FRotator FDataType;

	static const FDataType InvalidValue;

	static FRotator GetValue(const URVNBlackboardKeyType_Rotator* KeyOb, const uint8* RawData);

	static bool SetValue(URVNBlackboardKeyType_Rotator* KeyOb, uint8* RawData, const FRotator& Value);

	virtual ERVNBlackboardCompare::Type CompareValues(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                                  const URVNBlackboardKeyType* OtherKeyOb,
	                                                  const uint8* OtherMemoryBlock) const override;

protected:
	virtual void InitializeMemory(URVNDialogueManager& OwnerComp, uint8* RawData) override;

	virtual FString DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const override;

	virtual bool GetRotation(const URVNDialogueManager& OwnerComp, const uint8* RawData,
	                         FRotator& Rotation) const override;

	virtual bool IsEmpty(const URVNDialogueManager& OwnerComp, const uint8* RawData) const override;

	virtual void Clear(URVNDialogueManager& OwnerComp, uint8* RawData) override;

	virtual bool TestBasicOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                ERVNBasicKeyOperation::Type Op) const override;
};
