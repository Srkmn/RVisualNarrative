#pragma once

#include "CoreMinimal.h"
#include "RVNBlackboardKeyType.h"
#include "RVNBlackboardKeyType_Object.generated.h"

UCLASS(EditInlineNew, meta=(DisplayName="Object"))
class RVISUALNARRATIVE_API URVNBlackboardKeyType_Object : public URVNBlackboardKeyType
{
	GENERATED_BODY()

public:
	URVNBlackboardKeyType_Object();

	typedef UObject* FDataType;

	static const FDataType InvalidValue;

	UPROPERTY(Category=Blackboard, EditDefaultsOnly, meta=(AllowAbstract="1"))
	TObjectPtr<UClass> BaseClass;

	static UObject* GetValue(const URVNBlackboardKeyType_Object* KeyOb, const uint8* RawData);

	static bool SetValue(URVNBlackboardKeyType_Object* KeyOb, uint8* RawData, UObject* Value);

	virtual ERVNBlackboardCompare::Type CompareValues(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                                  const URVNBlackboardKeyType* OtherKeyOb,
	                                                  const uint8* OtherMemoryBlock) const override;

	virtual FString DescribeSelf() const override;

	virtual bool IsAllowedByFilter(URVNBlackboardKeyType* FilterOb) const override;

protected:
	virtual FString DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const override;

	virtual bool GetLocation(const URVNDialogueManager& OwnerComp, const uint8* RawData,
	                         FVector& Location) const override;

	virtual bool GetRotation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                         FRotator& Rotation) const override;

	virtual bool TestBasicOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                ERVNBasicKeyOperation::Type Op) const override;
};
