#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RVNBlackboardData.generated.h"

class URVNBlackboardKeyType;

USTRUCT()
struct FRVNBlackboardEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FName EntryName;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category=Blackboard,
		Meta=(ToolTip="Optional description to explain what this blackboard entry does."))
	FString EntryDescription;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FName EntryCategory;
#endif

	UPROPERTY(EditAnywhere, Instanced, Category=Blackboard)
	TObjectPtr<URVNBlackboardKeyType> KeyType;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	uint32 bInstanceSynced : 1;

	FRVNBlackboardEntry()
		: KeyType(nullptr), bInstanceSynced(0)
	{
	}

	bool operator==(const FRVNBlackboardEntry& Other) const;
};

namespace FRVNBlackboard
{
	typedef uint8 FKey;

	constexpr FKey InvalidKey = static_cast<FKey>(-1);
}

UCLASS(BlueprintType, AutoExpandCategories=(Blackboard))
class RVISUALNARRATIVE_API URVNBlackboardData : public UDataAsset
{
	GENERATED_BODY()
	DECLARE_MULTICAST_DELEGATE_OneParam(FRVNKeyUpdate, URVNBlackboardData* /*asset*/);

public:
	UPROPERTY(EditAnywhere, Category=Parent)
	TObjectPtr<URVNBlackboardData> Parent;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleDefaultsOnly, Transient, Category=Parent)
	TArray<FRVNBlackboardEntry> ParentKeys;
#endif

	UPROPERTY(EditAnywhere, Category=Blackboard)
	TArray<FRVNBlackboardEntry> Keys;

private:
	UPROPERTY()
	uint32 bHasSynchronizedKeys : 1;

public:
	bool HasSynchronizedKeys() const { return bHasSynchronizedKeys; }

	bool IsKeyInstanceSynced(FRVNBlackboard::FKey KeyID) const;

	FRVNBlackboard::FKey GetKeyID(const FName& KeyName) const;

	FName GetKeyName(FRVNBlackboard::FKey KeyID) const;

	TSubclassOf<URVNBlackboardKeyType> GetKeyType(FRVNBlackboard::FKey KeyID) const;

	int32 GetNumKeys() const;

	FORCEINLINE FRVNBlackboard::FKey GetFirstKeyID() const { return FirstKeyID; }

	const FRVNBlackboardEntry* GetKey(FRVNBlackboard::FKey KeyID) const;

	const TArray<FRVNBlackboardEntry>& GetKeys() const;

	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	void PropagateKeyChangesToDerivedBlackboardAssets();

	bool IsValid() const;

	template <class T>
	T* UpdatePersistentKey(const FName& KeyName)
	{
		T* CreatedKeyType = nullptr;

		const FRVNBlackboard::FKey KeyID = InternalGetKeyID(KeyName, DontCheckParentKeys);
		if (KeyID == FRVNBlackboard::InvalidKey && Parent == nullptr)
		{
			FRVNBlackboardEntry Entry;
			Entry.EntryName = KeyName;

			CreatedKeyType = NewObject<T>(this);
			Entry.KeyType = CreatedKeyType;

			Keys.Add(Entry);
			MarkPackageDirty();
			PropagateKeyChangesToDerivedBlackboardAssets();
		}
		else if (KeyID != FRVNBlackboard::InvalidKey && Parent != nullptr)
		{
			const FRVNBlackboard::FKey KeyIndex = KeyID - FirstKeyID;
			Keys.RemoveAt(KeyIndex);
			MarkPackageDirty();
			PropagateKeyChangesToDerivedBlackboardAssets();
		}

		return CreatedKeyType;
	}

#if WITH_EDITOR
	DECLARE_MULTICAST_DELEGATE_OneParam(FRVNBlackboardDataChanged, URVNBlackboardData* /*Asset*/);
	static FRVNBlackboardDataChanged OnBlackboardDataChanged;
#endif

	static FRVNKeyUpdate OnUpdateKeys;

	void UpdateParentKeys();

	void UpdateKeyIDs();

	void UpdateIfHasSynchronizedKeys();

	void UpdateDeprecatedKeys();

	bool IsChildOf(const URVNBlackboardData& OtherAsset) const;

	bool IsRelatedTo(const URVNBlackboardData& OtherAsset) const;

protected:
	enum EKeyLookupMode
	{
		CheckParentKeys,
		DontCheckParentKeys,
	};

	FRVNBlackboard::FKey FirstKeyID;

	FRVNBlackboard::FKey InternalGetKeyID(const FName& KeyName, EKeyLookupMode LookupMode) const;

	static bool ContainsKeyName(FName KeyName, const TArray<FRVNBlackboardEntry>& Keys,
	                            const TArray<FRVNBlackboardEntry>& ParentKeys);
};
