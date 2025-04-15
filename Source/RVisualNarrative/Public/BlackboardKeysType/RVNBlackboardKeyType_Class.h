#pragma once

#include "CoreMinimal.h"
#include "RVNBlackboardKeyType.h"
#include "RVNBlackboardKeyType_Class.generated.h"

UCLASS(EditInlineNew, meta=(DisplayName="Class"))
class RVISUALNARRATIVE_API URVNBlackboardKeyType_Class : public URVNBlackboardKeyType
{
	GENERATED_BODY()

public:
	URVNBlackboardKeyType_Class();

	typedef UClass* FDataType;

	static const FDataType InvalidValue;

	UPROPERTY(Category=Blackboard, EditDefaultsOnly, meta=(AllowAbstract="1"))
	TObjectPtr<UClass> BaseClass;

	static UClass* GetValue(const URVNBlackboardKeyType_Class* KeyOb, const uint8* RawData);

	static bool SetValue(URVNBlackboardKeyType_Class* KeyOb, uint8* RawData, UClass* Value);

	virtual ERVNBlackboardCompare::Type CompareValues(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                                  const URVNBlackboardKeyType* OtherKeyOb,
	                                                  const uint8* OtherMemoryBlock) const override;

	virtual FString DescribeSelf() const override;

	virtual bool IsAllowedByFilter(URVNBlackboardKeyType* FilterOb) const override;

protected:
	virtual FString DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const override;

	virtual bool TestBasicOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
	                                ERVNBasicKeyOperation::Type Op) const override;
};
