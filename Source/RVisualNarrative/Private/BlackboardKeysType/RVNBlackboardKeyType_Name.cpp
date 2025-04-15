#include "BlackboardKeysType/RVNBlackboardKeyType_Name.h"

const URVNBlackboardKeyType_Name::FDataType URVNBlackboardKeyType_Name::InvalidValue = NAME_None;

URVNBlackboardKeyType_Name::URVNBlackboardKeyType_Name()
{
	ValueSize = sizeof(FName);

	SupportedOp = ERVNBlackboardKeyOperation::Text;
}

FName URVNBlackboardKeyType_Name::GetValue(const URVNBlackboardKeyType_Name* KeyOb, const uint8* RawData)
{
	return GetValueFromMemory<FName>(RawData);
}

bool URVNBlackboardKeyType_Name::SetValue(URVNBlackboardKeyType_Name* KeyOb, uint8* RawData, const FName& Value)
{
	return SetValueInMemory<FName>(RawData, Value);
}

ERVNBlackboardCompare::Type URVNBlackboardKeyType_Name::CompareValues(const URVNDialogueManager& OwnerComp,
                                                                      const uint8* MemoryBlock,
                                                                      const URVNBlackboardKeyType* OtherKeyOb,
                                                                      const uint8* OtherMemoryBlock) const
{
	const FName MyValue = GetValue(this, MemoryBlock);

	const FName OtherValue = GetValue((URVNBlackboardKeyType_Name*)OtherKeyOb, OtherMemoryBlock);

	return (MyValue == OtherValue) ? ERVNBlackboardCompare::Equal : ERVNBlackboardCompare::NotEqual;
}

FString URVNBlackboardKeyType_Name::DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const
{
	return GetValue(this, RawData).ToString();
}

bool URVNBlackboardKeyType_Name::TestTextOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
                                                   ERVNTextKeyOperation::Type Op, const FString& OtherString) const
{
	const FString Value = GetValue(this, MemoryBlock).ToString();
	switch (Op)
	{
	case ERVNTextKeyOperation::Equal:
		{
			return (Value == OtherString);
		}

	case ERVNTextKeyOperation::NotEqual:
		{
			return (Value != OtherString);
		}

	case ERVNTextKeyOperation::Contain:
		{
			return (Value.Contains(OtherString) == true);
		}

	case ERVNTextKeyOperation::NotContain:
		{
			return (Value.Contains(OtherString) == false);
		}

	default: break;
	}

	return false;
}
