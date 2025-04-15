#include "BlackboardKeysType/RVNBlackboardKeyType_Object.h"

const URVNBlackboardKeyType_Object::FDataType URVNBlackboardKeyType_Object::InvalidValue = nullptr;

URVNBlackboardKeyType_Object::URVNBlackboardKeyType_Object()
{
	ValueSize = sizeof(FWeakObjectPtr);

	BaseClass = UObject::StaticClass();

	SupportedOp = ERVNBlackboardKeyOperation::Basic;
}

UObject* URVNBlackboardKeyType_Object::GetValue(const URVNBlackboardKeyType_Object* KeyOb, const uint8* RawData)
{
	if (RawData == nullptr)
	{
		return nullptr;
	}

	FWeakObjectPtr WeakObjPtr = GetValueFromMemory<FWeakObjectPtr>(RawData);

	return WeakObjPtr.Get();
}

bool URVNBlackboardKeyType_Object::SetValue(URVNBlackboardKeyType_Object* KeyOb, uint8* RawData, UObject* Value)
{
	TWeakObjectPtr<UObject> WeakObjPtr(Value);

	return SetWeakObjectInMemory<UObject>(RawData, WeakObjPtr);
}

ERVNBlackboardCompare::Type URVNBlackboardKeyType_Object::CompareValues(const URVNDialogueManager& OwnerComp,
                                                                        const uint8* MemoryBlock,
                                                                        const URVNBlackboardKeyType* OtherKeyOb,
                                                                        const uint8* OtherMemoryBlock) const
{
	const UObject* MyValue = GetValue(this, MemoryBlock);

	const UObject* OtherValue = GetValue((URVNBlackboardKeyType_Object*)OtherKeyOb, OtherMemoryBlock);

	return (MyValue == OtherValue) ? ERVNBlackboardCompare::Equal : ERVNBlackboardCompare::NotEqual;
}

FString URVNBlackboardKeyType_Object::DescribeSelf() const
{
	return *GetNameSafe(BaseClass);
}

bool URVNBlackboardKeyType_Object::IsAllowedByFilter(URVNBlackboardKeyType* FilterOb) const
{
	URVNBlackboardKeyType_Object* FilterObject = Cast<URVNBlackboardKeyType_Object>(FilterOb);

	return (FilterObject && (FilterObject->BaseClass == BaseClass || BaseClass->IsChildOf(FilterObject->BaseClass)));
}

FString URVNBlackboardKeyType_Object::DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const
{
	return *GetNameSafe(GetValue(this, RawData));
}

bool URVNBlackboardKeyType_Object::GetLocation(const URVNDialogueManager& OwnerComp, const uint8* RawData,
                                               FVector& Location) const
{
	AActor* MyActor = Cast<AActor>(GetValue(this, RawData));

	if (MyActor)
	{
		Location = MyActor->GetActorLocation();

		return true;
	}

	return false;
}

bool URVNBlackboardKeyType_Object::GetRotation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
                                               FRotator& Rotation) const
{
	AActor* MyActor = Cast<AActor>(GetValue(this, MemoryBlock));

	if (MyActor)
	{
		Rotation = MyActor->GetActorRotation();

		return true;
	}

	return false;
}

bool URVNBlackboardKeyType_Object::TestBasicOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
                                                      ERVNBasicKeyOperation::Type Op) const
{
	if (MemoryBlock == nullptr)
	{
		return false;
	}

	FWeakObjectPtr WeakObjPtr = GetValueFromMemory<FWeakObjectPtr>(MemoryBlock);

	return (Op == ERVNBasicKeyOperation::Set) ? WeakObjPtr.IsValid() : !WeakObjPtr.IsValid();
}
