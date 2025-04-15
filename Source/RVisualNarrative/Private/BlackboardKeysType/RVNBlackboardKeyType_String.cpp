#include "BlackboardKeysType/RVNBlackboardKeyType_String.h"

const URVNBlackboardKeyType_String::FDataType URVNBlackboardKeyType_String::InvalidValue = FString();

URVNBlackboardKeyType_String::URVNBlackboardKeyType_String()
{
	ValueSize = 0;

	bCreateKeyInstance = true;

	SupportedOp = ERVNBlackboardKeyOperation::Text;
}

FString URVNBlackboardKeyType_String::GetValue(const URVNBlackboardKeyType_String* KeyOb, const uint8* RawData)
{
	return KeyOb->StringValue;
}

bool URVNBlackboardKeyType_String::SetValue(URVNBlackboardKeyType_String* KeyOb, uint8* RawData, const FString& Value)
{
	const bool bChanged = !KeyOb->StringValue.Equals(Value);

	KeyOb->StringValue = Value;

	return bChanged;
}

ERVNBlackboardCompare::Type URVNBlackboardKeyType_String::CompareValues(const URVNDialogueManager& OwnerComp,
                                                                        const uint8* MemoryBlock,
                                                                        const URVNBlackboardKeyType* OtherKeyOb,
                                                                        const uint8* OtherMemoryBlock) const
{
	const FString MyValue = GetValue(this, MemoryBlock);

	const FString OtherValue = GetValue((URVNBlackboardKeyType_String*)OtherKeyOb, OtherMemoryBlock);

	return MyValue.Equals(OtherValue) ? ERVNBlackboardCompare::Equal : ERVNBlackboardCompare::NotEqual;
}

FString URVNBlackboardKeyType_String::DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const
{
	return StringValue;
}

bool URVNBlackboardKeyType_String::TestTextOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
                                                     ERVNTextKeyOperation::Type Op, const FString& OtherString) const
{
	switch (Op)
	{
	case ERVNTextKeyOperation::Equal:
		{
			return (StringValue == OtherString);
		}

	case ERVNTextKeyOperation::NotEqual:
		{
			return (StringValue != OtherString);
		}

	case ERVNTextKeyOperation::Contain:
		{
			return (StringValue.Contains(OtherString) == true);
		}

	case ERVNTextKeyOperation::NotContain:
		{
			return (StringValue.Contains(OtherString) == false);
		}

	default: break;
	}

	return false;
}

void URVNBlackboardKeyType_String::Clear(URVNDialogueManager& OwnerComp, uint8* MemoryBlock)
{
	StringValue.Empty();
}

bool URVNBlackboardKeyType_String::IsEmpty(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock) const
{
	return StringValue.IsEmpty();
}

void URVNBlackboardKeyType_String::CopyValues(URVNDialogueManager& OwnerComp, uint8* MemoryBlock,
                                              const URVNBlackboardKeyType* SourceKeyOb, const uint8* SourceBlock)
{
	StringValue = ((URVNBlackboardKeyType_String*)SourceKeyOb)->StringValue;
}
