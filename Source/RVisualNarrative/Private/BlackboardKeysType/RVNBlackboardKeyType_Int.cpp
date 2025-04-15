#include "BlackboardKeysType/RVNBlackboardKeyType_Int.h"

const URVNBlackboardKeyType_Int::FDataType URVNBlackboardKeyType_Int::InvalidValue = 0;

URVNBlackboardKeyType_Int::URVNBlackboardKeyType_Int()
{
	ValueSize = sizeof(int32);

	SupportedOp = ERVNBlackboardKeyOperation::Arithmetic;
}

int32 URVNBlackboardKeyType_Int::GetValue(const URVNBlackboardKeyType_Int* KeyOb, const uint8* RawData)
{
	return GetValueFromMemory<int32>(RawData);
}

bool URVNBlackboardKeyType_Int::SetValue(URVNBlackboardKeyType_Int* KeyOb, uint8* RawData, int32 Value)
{
	return SetValueInMemory<int32>(RawData, Value);
}

FString URVNBlackboardKeyType_Int::DescribeArithmeticParam(int32 IntValue, float FloatValue) const
{
	return FString::Printf(TEXT("%d"), IntValue);
}

ERVNBlackboardCompare::Type URVNBlackboardKeyType_Int::CompareValues(const URVNDialogueManager& OwnerComp,
                                                                     const uint8* MemoryBlock,
                                                                     const URVNBlackboardKeyType* OtherKeyOb,
                                                                     const uint8* OtherMemoryBlock) const
{
	const int32 MyValue = GetValue(this, MemoryBlock);

	const int32 OtherValue = GetValue(static_cast<const URVNBlackboardKeyType_Int*>(OtherKeyOb), OtherMemoryBlock);

	return MyValue > OtherValue
		       ? ERVNBlackboardCompare::Greater
		       : MyValue < OtherValue
		       ? ERVNBlackboardCompare::Less
		       : ERVNBlackboardCompare::Equal;
}

FString URVNBlackboardKeyType_Int::DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const
{
	return FString::Printf(TEXT("%d"), GetValue(this, RawData));
}

bool URVNBlackboardKeyType_Int::TestArithmeticOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
                                                        ERVNArithmeticKeyOperation::Type Op, int32 OtherIntValue,
                                                        float OtherFloatValue) const
{
	const int32 Value = GetValue(this, MemoryBlock);
	switch (Op)
	{
	case ERVNArithmeticKeyOperation::Equal:
		{
			return (Value == OtherIntValue);
		}

	case ERVNArithmeticKeyOperation::NotEqual:
		{
			return (Value != OtherIntValue);
		}

	case ERVNArithmeticKeyOperation::Less:
		{
			return (Value < OtherIntValue);
		}

	case ERVNArithmeticKeyOperation::LessOrEqual:
		{
			return (Value <= OtherIntValue);
		}

	case ERVNArithmeticKeyOperation::Greater:
		{
			return (Value > OtherIntValue);
		}

	case ERVNArithmeticKeyOperation::GreaterOrEqual:
		{
			return (Value >= OtherIntValue);
		}

	default: break;
	}

	return false;
}
