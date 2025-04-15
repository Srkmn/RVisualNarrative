#include "RVNBlackboardData.h"
#include "RVNBlackboardKeyType.h"

URVNBlackboardData::FRVNKeyUpdate URVNBlackboardData::OnUpdateKeys;

#if WITH_EDITOR
URVNBlackboardData::FRVNBlackboardDataChanged URVNBlackboardData::OnBlackboardDataChanged;
#endif


bool FRVNBlackboardEntry::operator==(const FRVNBlackboardEntry& Other) const
{
	return
		EntryName == Other.EntryName &&
		((KeyType && Other.KeyType && KeyType->GetClass() == Other.KeyType->GetClass())
			|| (KeyType == nullptr && Other.KeyType == nullptr));
}

bool URVNBlackboardData::IsKeyInstanceSynced(FRVNBlackboard::FKey KeyID) const
{
	const auto KeyEntry = GetKey(KeyID);

	return KeyEntry ? KeyEntry->bInstanceSynced : false;
}

FRVNBlackboard::FKey URVNBlackboardData::GetKeyID(const FName& KeyName) const
{
	return InternalGetKeyID(KeyName, CheckParentKeys);
}

FName URVNBlackboardData::GetKeyName(FRVNBlackboard::FKey KeyID) const
{
	const auto KeyEntry = GetKey(KeyID);

	return KeyEntry ? KeyEntry->EntryName : NAME_None;
}

TSubclassOf<URVNBlackboardKeyType> URVNBlackboardData::GetKeyType(FRVNBlackboard::FKey KeyID) const
{
	const auto KeyEntry = GetKey(KeyID);

	return KeyEntry && KeyEntry->KeyType ? KeyEntry->KeyType->GetClass() : nullptr;
}

int32 URVNBlackboardData::GetNumKeys() const
{
	return FirstKeyID + Keys.Num();
}

const FRVNBlackboardEntry* URVNBlackboardData::GetKey(FRVNBlackboard::FKey KeyID) const
{
	if (KeyID != FRVNBlackboard::InvalidKey)
	{
		if (KeyID >= FirstKeyID)
		{
			return &Keys[KeyID - FirstKeyID];
		}

		if (Parent)
		{
			return Parent->GetKey(KeyID);
		}
	}

	return nullptr;
}

const TArray<FRVNBlackboardEntry>& URVNBlackboardData::GetKeys() const
{
	return Keys;
}

void URVNBlackboardData::PostInitProperties()
{
	Super::PostInitProperties();

	if (HasAnyFlags(RF_NeedPostLoad | RF_ClassDefaultObject) == false)
	{
		//UpdatePersistentKeys(*this);
	}
}

void URVNBlackboardData::PostLoad()
{
	Super::PostLoad();

	if (Parent)
	{
		Parent->ConditionalPostLoad();
	}

	UpdateParentKeys();
	UpdateIfHasSynchronizedKeys();
}

#if WITH_EDITOR
void URVNBlackboardData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	static const FName NAME_Parent = GET_MEMBER_NAME_CHECKED(URVNBlackboardData, Parent);
	static const FName NAME_InstanceSynced = GET_MEMBER_NAME_CHECKED(FRVNBlackboardEntry, bInstanceSynced);
	static const FName NAME_Keys = GET_MEMBER_NAME_CHECKED(URVNBlackboardData, Keys);

	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.Property->GetFName() == NAME_Parent)
		{
			if (Parent && Parent->IsChildOf(*this))
			{
				Parent = nullptr;
			}

			UpdateParentKeys();
		}

		if (PropertyChangedEvent.Property->GetFName() == NAME_InstanceSynced || PropertyChangedEvent.Property->
			GetFName() == NAME_Parent)
		{
			UpdateIfHasSynchronizedKeys();
		}
	}
	if (PropertyChangedEvent.MemberProperty)
	{
		if (PropertyChangedEvent.MemberProperty->GetFName() == NAME_Keys)
		{
			PropagateKeyChangesToDerivedBlackboardAssets();
		}
	}

	OnBlackboardDataChanged.Broadcast(this);
}
#endif

void URVNBlackboardData::PropagateKeyChangesToDerivedBlackboardAssets()
{
	for (TObjectIterator<URVNBlackboardData> It; It; ++It)
	{
		if (It->Parent == this)
		{
			It->UpdateParentKeys();
			It->UpdateIfHasSynchronizedKeys();
			It->PropagateKeyChangesToDerivedBlackboardAssets();
		}
	}
}

bool URVNBlackboardData::IsValid() const
{
	if (Parent)
	{
		for (int32 KeyIndex = 0; KeyIndex < Keys.Num(); KeyIndex++)
		{
			const FRVNBlackboard::FKey KeyID = Parent->InternalGetKeyID(Keys[KeyIndex].EntryName, CheckParentKeys);
			if (KeyID != FRVNBlackboard::InvalidKey)
			{
				return false;
			}
		}
	}

	return true;
}

void URVNBlackboardData::UpdateParentKeys()
{
	if (Parent == this)
	{
		Parent = nullptr;
	}

#if WITH_EDITORONLY_DATA
	ParentKeys.Reset();

	for (URVNBlackboardData* It = Parent; It; It = It->Parent)
	{
		for (int32 KeyIndex = 0; KeyIndex < It->Keys.Num(); KeyIndex++)
		{
			const bool bAlreadyExist = ContainsKeyName(It->Keys[KeyIndex].EntryName, Keys, ParentKeys);
			if (!bAlreadyExist)
			{
				ParentKeys.Add(It->Keys[KeyIndex]);
			}
		}
	}
#endif

	UpdateKeyIDs();
	//UpdatePersistentKeys(*this);
	OnUpdateKeys.Broadcast(this);
}

void URVNBlackboardData::UpdateKeyIDs()
{
	FirstKeyID = Parent ? Parent->GetNumKeys() : 0;
}

void URVNBlackboardData::UpdateIfHasSynchronizedKeys()
{
	bHasSynchronizedKeys = Parent != nullptr && Parent->bHasSynchronizedKeys;

	for (int32 KeyIndex = 0; KeyIndex < Keys.Num() && bHasSynchronizedKeys == false; ++KeyIndex)
	{
		bHasSynchronizedKeys |= Keys[KeyIndex].bInstanceSynced;
	}
}

void URVNBlackboardData::UpdateDeprecatedKeys()
{
	for (int32 KeyIndex = 0; KeyIndex < Keys.Num(); KeyIndex++)
	{
		FRVNBlackboardEntry& Entry = Keys[KeyIndex];
		if (Entry.KeyType)
		{
			URVNBlackboardKeyType* UpdatedKey = Entry.KeyType->UpdateDeprecatedKey();
			if (UpdatedKey)
			{
				Entry.KeyType = UpdatedKey;
			}
		}
	}
}

bool URVNBlackboardData::IsChildOf(const URVNBlackboardData& OtherAsset) const
{
	const URVNBlackboardData* TmpParent = Parent;

	while (TmpParent != nullptr && TmpParent != &OtherAsset)
	{
		TmpParent = TmpParent->Parent;
	}

	return (TmpParent == &OtherAsset);
}

bool URVNBlackboardData::IsRelatedTo(const URVNBlackboardData& OtherAsset) const
{
	return this == &OtherAsset ||
		IsChildOf(OtherAsset) ||
		OtherAsset.IsChildOf(*this) ||
		(Parent && OtherAsset.Parent && Parent->IsRelatedTo(*OtherAsset.Parent));
}

FRVNBlackboard::FKey URVNBlackboardData::InternalGetKeyID(const FName& KeyName, EKeyLookupMode LookupMode) const
{
	for (int32 KeyIndex = 0; KeyIndex < Keys.Num(); KeyIndex++)
	{
		if (Keys[KeyIndex].EntryName == KeyName)
		{
			return KeyIndex + FirstKeyID;
		}
	}

	return Parent && (LookupMode == CheckParentKeys)
		       ? Parent->InternalGetKeyID(KeyName, LookupMode)
		       : FRVNBlackboard::InvalidKey;
}

bool URVNBlackboardData::ContainsKeyName(FName KeyName, const TArray<FRVNBlackboardEntry>& Keys,
                                         const TArray<FRVNBlackboardEntry>& ParentKeys)
{
	for (int32 KeyIndex = 0; KeyIndex < Keys.Num(); KeyIndex++)
	{
		if (Keys[KeyIndex].EntryName == KeyName)
		{
			return true;
		}
	}

	for (int32 KeyIndex = 0; KeyIndex < ParentKeys.Num(); KeyIndex++)
	{
		if (ParentKeys[KeyIndex].EntryName == KeyName)
		{
			return true;
		}
	}

	return false;
}
