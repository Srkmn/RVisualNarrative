#include "BlackboardKeysType/RVNBlackboardKeyType_Vector.h"

#include "AITypes.h"

const URVNBlackboardKeyType_Vector::FDataType URVNBlackboardKeyType_Vector::InvalidValue = FAISystem::InvalidLocation;

URVNBlackboardKeyType_Vector::URVNBlackboardKeyType_Vector()
{
	ValueSize = sizeof(FVector);

	SupportedOp = ERVNBlackboardKeyOperation::Basic;
}

FVector URVNBlackboardKeyType_Vector::GetValue(const URVNBlackboardKeyType_Vector* KeyOb, const uint8* RawData)
{
	return GetValueFromMemory<FVector>(RawData);
}

bool URVNBlackboardKeyType_Vector::SetValue(URVNBlackboardKeyType_Vector* KeyOb, uint8* RawData, const FVector& Value)
{
	return SetValueInMemory<FVector>(RawData, Value);
}

ERVNBlackboardCompare::Type URVNBlackboardKeyType_Vector::CompareValues(const URVNDialogueManager& OwnerComp,
                                                                        const uint8* RawData,
                                                                        const URVNBlackboardKeyType* OtherKeyOb,
                                                                        const uint8* OtherRawData) const
{
	const FVector MyValue = GetValue(this, RawData);

	const FVector OtherValue = GetValue((URVNBlackboardKeyType_Vector*)OtherKeyOb, OtherRawData);

	return MyValue.Equals(OtherValue) ? ERVNBlackboardCompare::Equal : ERVNBlackboardCompare::NotEqual;
}

void URVNBlackboardKeyType_Vector::InitializeMemory(URVNDialogueManager& OwnerComp, uint8* RawData)
{
	SetValue(this, RawData, FAISystem::InvalidLocation);
}

FString URVNBlackboardKeyType_Vector::DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* RawData) const
{
	const FVector Location = GetValue(this, RawData);

	return FAISystem::IsValidLocation(Location) ? Location.ToString() : TEXT("(invalid)");
}

bool URVNBlackboardKeyType_Vector::GetLocation(const URVNDialogueManager& OwnerComp, const uint8* RawData,
                                               FVector& Location) const
{
	Location = GetValue(this, RawData);

	return FAISystem::IsValidLocation(Location);
}

bool URVNBlackboardKeyType_Vector::IsEmpty(const URVNDialogueManager& OwnerComp, const uint8* RawData) const
{
	const FVector Location = GetValue(this, RawData);

	return !FAISystem::IsValidLocation(Location);
}

void URVNBlackboardKeyType_Vector::Clear(URVNDialogueManager& OwnerComp, uint8* RawData)
{
	SetValueInMemory<FVector>(RawData, FAISystem::InvalidLocation);
}

bool URVNBlackboardKeyType_Vector::TestBasicOperation(const URVNDialogueManager& OwnerComp, const uint8* RawData,
                                                      ERVNBasicKeyOperation::Type Op) const
{
	const FVector Location = GetValue(this, RawData);

	return (Op == ERVNBasicKeyOperation::Set)
		       ? FAISystem::IsValidLocation(Location)
		       : !FAISystem::IsValidLocation(Location);
}
