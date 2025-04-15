#include "BlackboardKeysType/RVNBlackboardKeyType_Class.h"

const URVNBlackboardKeyType_Class::FDataType URVNBlackboardKeyType_Class::InvalidValue = nullptr;

URVNBlackboardKeyType_Class::URVNBlackboardKeyType_Class()
{
	ValueSize = sizeof(TWeakObjectPtr<UClass>);

	BaseClass = UObject::StaticClass();

	SupportedOp = ERVNBlackboardKeyOperation::Basic;
}

UClass* URVNBlackboardKeyType_Class::GetValue(const URVNBlackboardKeyType_Class* KeyOb, const uint8* RawData)
{
	const auto WeakObjPtr = GetValueFromMemory<TWeakObjectPtr<UClass>>(RawData);

	return WeakObjPtr.Get();
}

bool URVNBlackboardKeyType_Class::SetValue(URVNBlackboardKeyType_Class* KeyOb, uint8* RawData, UClass* Value)
{
	const auto WeakObjPtr(Value);

	return SetWeakObjectInMemory<UClass>(RawData, WeakObjPtr);
}

ERVNBlackboardCompare::Type URVNBlackboardKeyType_Class::CompareValues(const URVNDialogueManager& OwnerComp,
                                                                       const uint8* MemoryBlock,
                                                                       const URVNBlackboardKeyType* OtherKeyOb,
                                                                       const uint8* OtherMemoryBlock) const
{
	const UClass* MyValue = GetValue(this, MemoryBlock);

	const UClass* OtherValue = GetValue(static_cast<const URVNBlackboardKeyType_Class*>(OtherKeyOb), OtherMemoryBlock);

	return MyValue == OtherValue ? ERVNBlackboardCompare::Equal : ERVNBlackboardCompare::NotEqual;
}

FString URVNBlackboardKeyType_Class::DescribeSelf() const
{
	return *GetNameSafe(BaseClass);
}

bool URVNBlackboardKeyType_Class::IsAllowedByFilter(URVNBlackboardKeyType* FilterOb) const
{
	const auto FilterClass = Cast<URVNBlackboardKeyType_Class>(FilterOb);

	return FilterClass && (FilterClass->BaseClass == BaseClass || BaseClass->IsChildOf(FilterClass->BaseClass));
}

FString URVNBlackboardKeyType_Class::DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const
{
	return *GetNameSafe(GetValue(this, RawData));
}

bool URVNBlackboardKeyType_Class::TestBasicOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
                                                     ERVNBasicKeyOperation::Type Op) const
{
	const auto WeakObjPtr = GetValueFromMemory<TWeakObjectPtr<UClass>>(MemoryBlock);

	return Op == ERVNBasicKeyOperation::Set ? WeakObjPtr.IsValid() : !WeakObjPtr.IsValid();
}
