#include "RVisualNarrative/Public/BlackboardKeysType/RVNBlackboardKeyType_Bool.h"

const URVNBlackboardKeyType_Bool::FDataType URVNBlackboardKeyType_Bool::InvalidValue = false;

URVNBlackboardKeyType_Bool::URVNBlackboardKeyType_Bool()
{
	ValueSize = sizeof(uint8);

	SupportedOp = ERVNBlackboardKeyOperation::Basic;
}

bool URVNBlackboardKeyType_Bool::GetValue(const URVNBlackboardKeyType_Bool* KeyOb, const uint8* RawData)
{
	return GetValueFromMemory<uint8>(RawData) != 0;
}

bool URVNBlackboardKeyType_Bool::SetValue(URVNBlackboardKeyType_Bool* KeyOb, uint8* RawData, bool bValue)
{
	return SetValueInMemory<uint8>(RawData, bValue);
}

ERVNBlackboardCompare::Type URVNBlackboardKeyType_Bool::CompareValues(const URVNDialogueManager& OwnerComp,
                                                                      const uint8* MemoryBlock,
                                                                      const URVNBlackboardKeyType* OtherKeyOb,
                                                                      const uint8* OtherMemoryBlock) const
{
	const bool MyValue = GetValue(this, MemoryBlock);

	const bool OtherValue = GetValue(static_cast<const URVNBlackboardKeyType_Bool*>(OtherKeyOb), OtherMemoryBlock);

	return MyValue == OtherValue ? ERVNBlackboardCompare::Equal : ERVNBlackboardCompare::NotEqual;
}

FString URVNBlackboardKeyType_Bool::DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const
{
	return GetValue(this, RawData) ? TEXT("true") : TEXT("false");
}

bool URVNBlackboardKeyType_Bool::TestBasicOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
                                                    ERVNBasicKeyOperation::Type Op) const
{
	const bool Value = GetValue(this, MemoryBlock);

	return Op == ERVNBasicKeyOperation::Set ? Value : !Value;
}
