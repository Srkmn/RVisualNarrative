#pragma once

#include "CoreMinimal.h"
#include "RVNBlackboardKeyType.h"
#include "RVNBlackboardKeyType_Vector.generated.h"

UCLASS(EditInlineNew, meta=(DisplayName="Vector"))
class RVISUALNARRATIVE_API URVNBlackboardKeyType_Vector : public URVNBlackboardKeyType
{
	GENERATED_BODY()

public:
	URVNBlackboardKeyType_Vector();

	typedef FVector FDataType;

	static const FDataType InvalidValue;

	static FVector GetValue(const URVNBlackboardKeyType_Vector* KeyOb, const uint8* RawData);

	static bool SetValue(URVNBlackboardKeyType_Vector* KeyOb, uint8* RawData, const FVector& Value);

	virtual ERVNBlackboardCompare::Type CompareValues(const URVNDialogueManager& OwnerComp, const uint8* RawData,
	                                                  const URVNBlackboardKeyType* OtherKeyOb,
	                                                  const uint8* OtherRawData) const override;

protected:
	virtual void InitializeMemory(URVNDialogueManager& OwnerComp, uint8* RawData) override;

	virtual FString DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const override;

	virtual bool GetLocation(const URVNDialogueManager& OwnerComp, const uint8* RawData,
	                         FVector& Location) const override;

	virtual bool IsEmpty(const URVNDialogueManager& OwnerComp, const uint8* RawData) const override;

	virtual void Clear(URVNDialogueManager& OwnerComp, uint8* RawData) override;

	virtual bool TestBasicOperation(const URVNDialogueManager& OwnerComp, const uint8* RawData,
	                                ERVNBasicKeyOperation::Type Op) const override;
};
