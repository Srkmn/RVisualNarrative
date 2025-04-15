#include "BlackboardKeysType/RVNBlackboardKeyType_Enum.h"
#include "UEVersion.h"

const URVNBlackboardKeyType_Enum::FDataType URVNBlackboardKeyType_Enum::InvalidValue = 0;

URVNBlackboardKeyType_Enum::URVNBlackboardKeyType_Enum()
{
	ValueSize = sizeof(uint8);

	SupportedOp = ERVNBlackboardKeyOperation::Arithmetic;
}

uint8 URVNBlackboardKeyType_Enum::GetValue(const URVNBlackboardKeyType_Enum* KeyOb, const uint8* RawData)
{
	return GetValueFromMemory<uint8>(RawData);
}

bool URVNBlackboardKeyType_Enum::SetValue(URVNBlackboardKeyType_Enum* KeyOb, uint8* RawData, uint8 Value)
{
	return SetValueInMemory<uint8>(RawData, Value);
}

ERVNBlackboardCompare::Type URVNBlackboardKeyType_Enum::CompareValues(const URVNDialogueManager& OwnerComp,
                                                                      const uint8* MemoryBlock,
                                                                      const URVNBlackboardKeyType* OtherKeyOb,
                                                                      const uint8* OtherMemoryBlock) const
{
	const uint8 MyValue = GetValue(this, MemoryBlock);

	const uint8 OtherValue = GetValue(static_cast<const URVNBlackboardKeyType_Enum*>(OtherKeyOb), OtherMemoryBlock);

	return MyValue > OtherValue
		       ? ERVNBlackboardCompare::Greater
		       : MyValue < OtherValue
		       ? ERVNBlackboardCompare::Less
		       : ERVNBlackboardCompare::Equal;
}

FString URVNBlackboardKeyType_Enum::DescribeSelf() const
{
	return *GetNameSafe(EnumType);
}

FString URVNBlackboardKeyType_Enum::DescribeArithmeticParam(int32 IntValue, float FloatValue) const
{
	return EnumType ? EnumType->GetDisplayNameTextByValue(IntValue).ToString() : FString("UNKNOWN!");
}

bool URVNBlackboardKeyType_Enum::IsAllowedByFilter(URVNBlackboardKeyType* FilterOb) const
{
	const auto FilterEnum = Cast<URVNBlackboardKeyType_Enum>(FilterOb);

	return FilterEnum && FilterEnum->EnumType == EnumType;
}

#if WITH_EDITOR
void URVNBlackboardKeyType_Enum::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property &&
		PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(URVNBlackboardKeyType_Enum, EnumName))
	{
#if UE_ANY_PACKAGE
		EnumType = UClass::TryFindTypeSlow<UEnum>(EnumName, EFindFirstObjectOptions::ExactClass);
#else
		EnumType = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
#endif
	}

	bIsEnumNameValid = EnumType && !EnumName.IsEmpty();
}
#endif

FString URVNBlackboardKeyType_Enum::DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const
{
	return EnumType ? EnumType->GetDisplayNameTextByValue(GetValue(this, RawData)).ToString() : FString("UNKNOWN!");
}

bool URVNBlackboardKeyType_Enum::TestArithmeticOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
                                                         ERVNArithmeticKeyOperation::Type Op, int32 OtherIntValue,
                                                         float OtherFloatValue) const
{
	const uint8 Value = GetValue(this, MemoryBlock);
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
