#include "RVNBlackboardKeyType.h"
#include "RVNDialogueManager.h"

URVNBlackboardKeyType::URVNBlackboardKeyType()
{
	ValueSize = 0;

	SupportedOp = ERVNBlackboardKeyOperation::Basic;

	bCreateKeyInstance = false;

	bIsInstanced = false;
}

void URVNBlackboardKeyType::PreInitialize(URVNDialogueManager& OwnerComp)
{
}

void URVNBlackboardKeyType::InitializeKey(URVNDialogueManager& OwnerComp, FRVNBlackboard::FKey KeyID)
{
	uint8* RawData = OwnerComp.GetKeyRawData(KeyID);

	if (bCreateKeyInstance)
	{
		FRVNBlackboardInstancedKeyMemory* MyMemory = (FRVNBlackboardInstancedKeyMemory*)RawData;
		URVNBlackboardKeyType* KeyInstance = NewObject<URVNBlackboardKeyType>(&OwnerComp, GetClass());
		KeyInstance->bIsInstanced = true;
		MyMemory->KeyIdx = KeyID;
		OwnerComp.KeyInstances[KeyID] = KeyInstance;

		uint8* InstanceMemoryBlock = RawData + sizeof(FRVNBlackboardInstancedKeyMemory);
		KeyInstance->InitializeMemory(OwnerComp, InstanceMemoryBlock);
	}
	else
	{
		InitializeMemory(OwnerComp, RawData);
	}
}

bool URVNBlackboardKeyType::IsAllowedByFilter(URVNBlackboardKeyType* FilterOb) const
{
	return GetClass() == (FilterOb ? FilterOb->GetClass() : nullptr);
}

bool URVNBlackboardKeyType::WrappedGetLocation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
                                               FVector& Location) const
{
	if (HasInstance())
	{
		const URVNBlackboardKeyType* InstancedKey = GetKeyInstance(OwnerComp, MemoryBlock);
		const uint8* InstanceMemoryBlock = MemoryBlock + sizeof(FRVNBlackboardInstancedKeyMemory);
		return InstancedKey->GetLocation(OwnerComp, InstanceMemoryBlock, Location);
	}

	return GetLocation(OwnerComp, MemoryBlock, Location);
}

bool URVNBlackboardKeyType::WrappedGetRotation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
                                               FRotator& Rotation) const
{
	if (HasInstance())
	{
		const URVNBlackboardKeyType* InstancedKey = GetKeyInstance(OwnerComp, MemoryBlock);
		const uint8* InstanceMemoryBlock = MemoryBlock + sizeof(FRVNBlackboardInstancedKeyMemory);
		return InstancedKey->GetRotation(OwnerComp, InstanceMemoryBlock, Rotation);
	}

	return GetRotation(OwnerComp, MemoryBlock, Rotation);
}

void URVNBlackboardKeyType::WrappedFree(URVNDialogueManager& OwnerComp, uint8* MemoryBlock)
{
	if (HasInstance())
	{
		URVNBlackboardKeyType* InstancedKey = GetKeyInstance(OwnerComp, MemoryBlock);
		uint8* InstanceMemoryBlock = MemoryBlock + sizeof(FRVNBlackboardInstancedKeyMemory);
		InstancedKey->FreeMemory(OwnerComp, InstanceMemoryBlock);
	}

	return FreeMemory(OwnerComp, MemoryBlock);
}

void URVNBlackboardKeyType::WrappedClear(const URVNDialogueManager& OwnerComp, uint8* MemoryBlock) const
{
	URVNDialogueManager& MutableOwner = (URVNDialogueManager&)OwnerComp;
	if (HasInstance())
	{
		URVNBlackboardKeyType* InstancedKey = GetKeyInstance(MutableOwner, MemoryBlock);
		uint8* InstanceMemoryBlock = MemoryBlock + sizeof(FRVNBlackboardInstancedKeyMemory);
		InstancedKey->Clear(MutableOwner, InstanceMemoryBlock);
	}
	else
	{
		URVNBlackboardKeyType* MutableKey = (URVNBlackboardKeyType*)this;
		MutableKey->Clear(MutableOwner, MemoryBlock);
	}
}

bool URVNBlackboardKeyType::WrappedIsEmpty(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock) const
{
	if (HasInstance())
	{
		const URVNBlackboardKeyType* InstancedKey = GetKeyInstance(OwnerComp, MemoryBlock);
		const uint8* InstanceMemoryBlock = MemoryBlock + sizeof(FRVNBlackboardInstancedKeyMemory);
		return InstancedKey->IsEmpty(OwnerComp, InstanceMemoryBlock);
	}

	return IsEmpty(OwnerComp, MemoryBlock);
}

FString URVNBlackboardKeyType::DescribeArithmeticParam(int32 IntValue, float FloatValue) const
{
	return FString();
}

FString URVNBlackboardKeyType::DescribeSelf() const
{
	return FString();
}

URVNBlackboardKeyType* URVNBlackboardKeyType::UpdateDeprecatedKey()
{
	return nullptr;
}

const URVNBlackboardKeyType* URVNBlackboardKeyType::GetKeyInstance(const URVNDialogueManager& OwnerComp,
                                                                   const uint8* MemoryBlock) const
{
	FRVNBlackboardInstancedKeyMemory* MyMemory = (FRVNBlackboardInstancedKeyMemory*)MemoryBlock;

	return MyMemory && OwnerComp.KeyInstances.IsValidIndex(MyMemory->KeyIdx)
		       ? OwnerComp.KeyInstances[MyMemory->KeyIdx]
		       : nullptr;
}

URVNBlackboardKeyType* URVNBlackboardKeyType::GetKeyInstance(URVNDialogueManager& OwnerComp,
                                                             const uint8* MemoryBlock) const
{
	FRVNBlackboardInstancedKeyMemory* MyMemory = (FRVNBlackboardInstancedKeyMemory*)MemoryBlock;

	return MyMemory && OwnerComp.KeyInstances.IsValidIndex(MyMemory->KeyIdx)
		       ? OwnerComp.KeyInstances[MyMemory->KeyIdx]
		       : nullptr;
}

ERVNBlackboardCompare::Type URVNBlackboardKeyType::CompareValues(const URVNDialogueManager& OwnerComp,
                                                                 const uint8* MemoryBlock,
                                                                 const URVNBlackboardKeyType* OtherKeyOb,
                                                                 const uint8* OtherMemoryBlock) const
{
	const int32 MemCmp = FMemory::Memcmp(MemoryBlock, OtherMemoryBlock, GetValueSize());
	return MemCmp ? ERVNBlackboardCompare::NotEqual : ERVNBlackboardCompare::Equal;
}

bool URVNBlackboardKeyType::HasInstance() const
{
	return bCreateKeyInstance;
}

bool URVNBlackboardKeyType::IsInstanced() const
{
	return bIsInstanced;
}

uint16 URVNBlackboardKeyType::GetValueSize() const
{
	return ValueSize;
}

ERVNBlackboardKeyOperation::Type URVNBlackboardKeyType::GetTestOperation() const
{
	return SupportedOp;
}

void URVNBlackboardKeyType::CopyValues(URVNDialogueManager& OwnerComp, uint8* MemoryBlock,
                                       const URVNBlackboardKeyType* SourceKeyOb, const uint8* SourceBlock)
{
	FMemory::Memcpy(MemoryBlock, SourceBlock, GetValueSize());
}

void URVNBlackboardKeyType::InitializeMemory(URVNDialogueManager& OwnerComp, uint8* MemoryBlock)
{
}

void URVNBlackboardKeyType::FreeMemory(URVNDialogueManager& OwnerComp, uint8* MemoryBlock)
{
	// empty in base class
}

bool URVNBlackboardKeyType::GetLocation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
                                        FVector& Location) const
{
	return false;
}

bool URVNBlackboardKeyType::GetRotation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
                                        FRotator& Rotation) const
{
	return false;
}

void URVNBlackboardKeyType::Clear(URVNDialogueManager& OwnerComp, uint8* MemoryBlock)
{
	FMemory::Memzero(MemoryBlock, GetValueSize());
}

bool URVNBlackboardKeyType::IsEmpty(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock) const
{
	for (int32 ByteIndex = 0; ByteIndex < GetValueSize(); ++ByteIndex)
	{
		if (MemoryBlock[ByteIndex] != static_cast<uint8>(0))
		{
			return false;
		}
	}

	return true;
}

bool URVNBlackboardKeyType::TestBasicOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
                                               ERVNBasicKeyOperation::Type Op) const
{
	return false;
}

bool URVNBlackboardKeyType::TestArithmeticOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
                                                    ERVNArithmeticKeyOperation::Type Op, int32 OtherIntValue,
                                                    float OtherFloatValue) const
{
	return false;
}

bool URVNBlackboardKeyType::TestTextOperation(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock,
                                              ERVNTextKeyOperation::Type Op, const FString& OtherString) const
{
	return false;
}

FString URVNBlackboardKeyType::DescribeValue(const URVNDialogueManager& OwnerComp, const uint8* MemoryBlock) const
{
	FString DescBytes;
	for (int32 ValueIndex = 0; ValueIndex < ValueSize; ValueIndex++)
	{
		DescBytes += FString::Printf(TEXT("%X"), MemoryBlock[ValueIndex]);
	}

	return DescBytes.Len() ? (FString("0x") + DescBytes) : FString("empty");
}
