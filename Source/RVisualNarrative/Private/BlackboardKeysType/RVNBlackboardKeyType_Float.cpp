#include "BlackboardKeysType/RVNBlackboardKeyType_Float.h"

const URVNBlackboardKeyType_Float::FDataType URVNBlackboardKeyType_Float::InvalidValue = 0.f;

URVNBlackboardKeyType_Float::URVNBlackboardKeyType_Float()
{
	ValueSize = sizeof(float);

	SupportedOp = ERVNBlackboardKeyOperation::Arithmetic;
}

float URVNBlackboardKeyType_Float::GetValue(const URVNBlackboardKeyType_Float* KeyOb, const uint8* RawData)
{
	return GetValueFromMemory<float>(RawData);
}

bool URVNBlackboardKeyType_Float::SetValue(URVNBlackboardKeyType_Float* KeyOb, uint8* RawData, float Value)
{
	return SetValueInMemory<float>(RawData, Value);
}

ERVNBlackboardCompare::Type URVNBlackboardKeyType_Float::CompareValues(const URVNDialogueManager& OwnerComp,
                                                                       const uint8* MemoryBlock,
                                                                       const URVNBlackboardKeyType* OtherKeyOb,
                                                                       const uint8* OtherMemoryBlock) const
{
	const float MyValue = GetValue(this, MemoryBlock);

	const float OtherValue = GetValue(static_cast<const URVNBlackboardKeyType_Float*>(OtherKeyOb), OtherMemoryBlock);

	return FMath::Abs(MyValue - OtherValue) < KINDA_SMALL_NUMBER
		       ? ERVNBlackboardCompare::Equal
		       : MyValue > OtherValue
		       ? ERVNBlackboardCompare::Greater
		       : ERVNBlackboardCompare::Less;
}

FString URVNBlackboardKeyType_Float::DescribeArithmeticParam(int32 IntValue, float FloatValue) const
{
	return FString::Printf(TEXT("%f"), FloatValue);
}

FString URVNBlackboardKeyType_Float::DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const
{
	return FString::Printf(TEXT("%f"), GetValue(this, RawData));
}

bool URVNBlackboardKeyType_Float::TestArithmeticOperation(const URVNDialogueManager& OwnerComp,
                                                          const uint8* MemoryBlock, ERVNArithmeticKeyOperation::Type Op,
                                                          int32 OtherIntValue, float OtherFloatValue) const
{
	const float Value = GetValue(this, MemoryBlock);

	switch (Op)
	{
	case ERVNArithmeticKeyOperation::Equal:
		{
			return (Value == OtherFloatValue);
		}

	case ERVNArithmeticKeyOperation::NotEqual:
		{
			return (Value != OtherFloatValue);
		}

	case ERVNArithmeticKeyOperation::Less:
		{
			return (Value < OtherFloatValue);
		}

	case ERVNArithmeticKeyOperation::LessOrEqual:
		{
			return (Value <= OtherFloatValue);
		}

	case ERVNArithmeticKeyOperation::Greater:
		{
			return (Value > OtherFloatValue);
		}

	case ERVNArithmeticKeyOperation::GreaterOrEqual:
		{
			return (Value >= OtherFloatValue);
		}

	default: break;
	}

	return false;
}
