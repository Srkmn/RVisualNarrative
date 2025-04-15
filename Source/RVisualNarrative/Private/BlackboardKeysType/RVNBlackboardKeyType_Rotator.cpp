#include "BlackboardKeysType/RVNBlackboardKeyType_Rotator.h"
#include "AITypes.h"

const URVNBlackboardKeyType_Rotator::FDataType URVNBlackboardKeyType_Rotator::InvalidValue = FAISystem::InvalidRotation;

URVNBlackboardKeyType_Rotator::URVNBlackboardKeyType_Rotator()
{
	ValueSize = sizeof(FRotator);

	SupportedOp = ERVNBlackboardKeyOperation::Basic;
}

FRotator URVNBlackboardKeyType_Rotator::GetValue(const URVNBlackboardKeyType_Rotator* KeyOb, const uint8* RawData)
{
	return GetValueFromMemory<FRotator>(RawData);
}

bool URVNBlackboardKeyType_Rotator::SetValue(URVNBlackboardKeyType_Rotator* KeyOb, uint8* RawData,
                                             const FRotator& Value)
{
	return SetValueInMemory<FRotator>(RawData, Value);
}

ERVNBlackboardCompare::Type URVNBlackboardKeyType_Rotator::CompareValues(const URVNDialogueManager& OwnerComp,
                                                                         const uint8* MemoryBlock,
                                                                         const URVNBlackboardKeyType* OtherKeyOb,
                                                                         const uint8* OtherMemoryBlock) const
{
	const FRotator MyValue = GetValue(this, MemoryBlock);

	const FRotator OtherValue = GetValue((URVNBlackboardKeyType_Rotator*)OtherKeyOb, OtherMemoryBlock);

	return MyValue.Equals(OtherValue) ? ERVNBlackboardCompare::Equal : ERVNBlackboardCompare::NotEqual;
}

void URVNBlackboardKeyType_Rotator::InitializeMemory(URVNDialogueManager& OwnerComp, uint8* RawData)
{
	SetValue(this, RawData, FAISystem::InvalidRotation);
}

FString URVNBlackboardKeyType_Rotator::DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const
{
	const FRotator Rotation = GetValue(this, RawData);

	return FAISystem::IsValidRotation(Rotation) ? Rotation.ToString() : TEXT("(invalid)");
}

bool URVNBlackboardKeyType_Rotator::GetRotation(const URVNDialogueManager& OwnerComp, const uint8* RawData,
                                                FRotator& Rotation) const
{
	Rotation = GetValue(this, RawData);

	return FAISystem::IsValidRotation(Rotation);
}

bool URVNBlackboardKeyType_Rotator::IsEmpty(const URVNDialogueManager& OwnerComp, const uint8* RawData) const
{
	const FRotator Rotation = GetValue(this, RawData);

	return !FAISystem::IsValidRotation(Rotation);
}

void URVNBlackboardKeyType_Rotator::Clear(URVNDialogueManager& OwnerComp, uint8* RawData)
{
	SetValueInMemory<FRotator>(RawData, FAISystem::InvalidRotation);
}

bool URVNBlackboardKeyType_Rotator::TestBasicOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
                                                       ERVNBasicKeyOperation::Type Op) const
{
	const FRotator Rotation = GetValue(this, MemoryBlock);

	return (Op == ERVNBasicKeyOperation::Set)
		       ? FAISystem::IsValidRotation(Rotation)
		       : !FAISystem::IsValidRotation(Rotation);
}
