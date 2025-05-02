#pragma once

#include "CoreMinimal.h"
#include "RVNBlackboardData.h"
#include "RVNBlackboardKeyType.h"
#include "RVNComponent.h"
#include "RVNDialogueManager.generated.h"

class URVNTaskBase;
class URVNComponent;

namespace ERVNBlackboardDescription
{
	enum Type
	{
		OnlyValue,
		KeyWithValue,
		DetailedKeyWithValue,
		Full,
	};
}

UCLASS(BlueprintType, Blueprintable)
class RVISUALNARRATIVE_API URVNDialogueManager : public UActorComponent
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueCompleted);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTaskCompleted);

	URVNDialogueManager();

	/** BEGIN UActorComponent overrides */
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	/** END UActorComponent overrides */

	void Initialize(URVNComponent* InComponent);

	void InitializeBlackboard();

	FName GetKeyName(FRVNBlackboard::FKey KeyID) const;

	FRVNBlackboard::FKey GetKeyID(const FName& KeyName) const;

	TSubclassOf<URVNBlackboardKeyType> GetKeyType(FRVNBlackboard::FKey KeyID) const;

	bool IsKeyInstanceSynced(FRVNBlackboard::FKey KeyID) const;

	int32 GetNumKeys() const;

	FORCEINLINE bool HasValidAsset() const;

	FDelegateHandle RegisterObserver(FRVNBlackboard::FKey KeyID, UObject* NotifyOwner,
	                                 const FOnRVNBlackboardChangeNotification& ObserverDelegate);

	void UnregisterObserver(FRVNBlackboard::FKey KeyID, FDelegateHandle ObserverHandle);

	void UnregisterObserversFrom(UObject* NotifyOwner);

	void NotifyObservers(FRVNBlackboard::FKey KeyID) const;

	URVNBlackboardData* GetBlackboardAsset() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	FRVNNodeData StartDialogue();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	FRVNNodeData StartDialogueFromId(int32 InNodeId);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	TArray<FRVNNodeData> GetPassedNextNodeData(TArray<int32> NextNodesId);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	FRVNNodeData GetNodeDataFromId(int32 InNodeId);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool CheckAllConditions(int32 InNodeId) const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void ProcessAllTasks(int32 InNodeId);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void BreakCurrentDialogue();

	void ProcessTask(URVNTaskBase* InTask);

	bool CheckAllConditions(URVNTaskBase* InTask) const;

	bool CheckCondition(URVNConditionBase* InCondition) const;

	// Start Process Blackboard
	bool IsCompatibleWith(URVNBlackboardData* TestAsset) const;

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	UObject* GetValueAsObject(const FName& KeyName) const;

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	UClass* GetValueAsClass(const FName& KeyName) const;

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	uint8 GetValueAsEnum(const FName& KeyName) const;

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	int32 GetValueAsInt(const FName& KeyName) const;

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	float GetValueAsFloat(const FName& KeyName) const;

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	bool GetValueAsBool(const FName& KeyName) const;

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	FString GetValueAsString(const FName& KeyName) const;

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	FName GetValueAsName(const FName& KeyName) const;

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	FVector GetValueAsVector(const FName& KeyName) const;

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	FRotator GetValueAsRotator(const FName& KeyName) const;

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	void SetValueAsObject(const FName& KeyName, UObject* ObjectValue);

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	void SetValueAsClass(const FName& KeyName, UClass* ClassValue);

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	void SetValueAsEnum(const FName& KeyName, uint8 EnumValue);

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	void SetValueAsInt(const FName& KeyName, int32 IntValue);

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	void SetValueAsFloat(const FName& KeyName, float FloatValue);

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	void SetValueAsBool(const FName& KeyName, bool BoolValue);

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	void SetValueAsString(const FName& KeyName, FString StringValue);

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	void SetValueAsName(const FName& KeyName, FName NameValue);

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	void SetValueAsVector(const FName& KeyName, FVector VectorValue);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetValueAsRotator(const FName& KeyName, FRotator VectorValue);

	UFUNCTION(BlueprintCallable, Category="Dialogue",
		Meta=(Tooltip=
			"If the vector value has been set (and not cleared), this function returns true (indicating that the value should be valid).  If it's not set, the vector value is invalid and this function will return false.  (Also returns false if the key specified does not hold a vector.)"
		))
	bool IsVectorValueSet(const FName& KeyName) const;
	bool IsVectorValueSet(FRVNBlackboard::FKey KeyID) const;

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	bool GetLocationFromEntry(const FName& KeyName, FVector& ResultLocation) const;
	bool GetLocationFromEntry(FRVNBlackboard::FKey KeyID, FVector& ResultLocation) const;

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	bool GetRotationFromEntry(const FName& KeyName, FRotator& ResultRotation) const;
	bool GetRotationFromEntry(FRVNBlackboard::FKey KeyID, FRotator& ResultRotation) const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void ClearValue(const FName& KeyName);
	void ClearValue(FRVNBlackboard::FKey KeyID);

	bool CopyKeyValue(FRVNBlackboard::FKey SourceKeyID, FRVNBlackboard::FKey DestinationKeyID);

	template <class TDataClass>
	bool IsKeyOfType(FRVNBlackboard::FKey KeyID) const;

	template <class TDataClass>
	bool SetValue(const FName& KeyName, typename TDataClass::FDataType Value);

	template <class TDataClass>
	bool SetValue(FRVNBlackboard::FKey KeyID, typename TDataClass::FDataType Value);

	template <class TDataClass>
	typename TDataClass::FDataType GetValue(const FName& KeyName) const;

	template <class TDataClass>
	typename TDataClass::FDataType GetValue(FRVNBlackboard::FKey KeyID) const;

public:
	/** get pointer to raw data for given key */
	FORCEINLINE uint8* GetKeyRawData(const FName& KeyName) { return GetKeyRawData(GetKeyID(KeyName)); }
	FORCEINLINE uint8* GetKeyRawData(FRVNBlackboard::FKey KeyID)
	{
		return ValueMemory.Num() && ValueOffsets.IsValidIndex(KeyID)
			       ? ValueMemory.GetData() + ValueOffsets[KeyID]
			       : nullptr;
	}

	FORCEINLINE const uint8* GetKeyRawData(const FName& KeyName) const { return GetKeyRawData(GetKeyID(KeyName)); }
	FORCEINLINE const uint8* GetKeyRawData(FRVNBlackboard::FKey KeyID) const
	{
		return ValueMemory.Num() && ValueOffsets.IsValidIndex(KeyID)
			       ? ValueMemory.GetData() + ValueOffsets[KeyID]
			       : nullptr;
	}

public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnDialogueCompleted OnDialogueCompleted;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnTaskCompleted OnTaskCompletedCallback;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	int32 CurrentNodeId;

protected:
	void InitializeParentChain(URVNBlackboardData* NewAsset);

	void DestroyValues();

	void PopulateSynchronizedKeys();

	bool ShouldSyncWithBlackboard(URVNDialogueManager& OtherManager) const;

protected:
	UPROPERTY(transient)
	TObjectPtr<URVNBlackboardData> BlackboardAsset;

	TArray<uint8> ValueMemory;

	TArray<uint16> ValueOffsets;

	UPROPERTY(transient)
	TArray<TObjectPtr<URVNBlackboardKeyType>> KeyInstances;

protected:
	struct FOnRVNBlackboardChangeNotificationInfo
	{
		FOnRVNBlackboardChangeNotificationInfo(const FOnRVNBlackboardChangeNotification& InDelegateHandle)
			: DelegateHandle(InDelegateHandle)
		{
		}

		FDelegateHandle GetHandle() const
		{
			return DelegateHandle.GetHandle();
		}

		FOnRVNBlackboardChangeNotification DelegateHandle;
		bool bToBeRemoved = false;
	};

	// The number of decorators being notified
	mutable int32 NotifyObserversRecursionCount = 0;

	mutable int32 ObserversToRemoveCount = 0;

	mutable TMultiMap<uint8, FOnRVNBlackboardChangeNotificationInfo> Observers;

	mutable TMultiMap<UObject*, FDelegateHandle> ObserverHandles;

	uint32 bSynchronizedKeyPopulated : 1;

private:
	TWeakObjectPtr<URVNComponent> RVNComponent;

	UPROPERTY()
	TObjectPtr<URVNTaskBase> PendingProcessTask;

	static TMultiMap<TWeakObjectPtr<URVNBlackboardData>, TWeakObjectPtr<URVNDialogueManager>>
	BlackboardDataToManagersMap;

	void OnTaskCompleted(URVNTaskBase* CompletedTask);

	void TryProcessNextTask();

	friend URVNBlackboardKeyType;
};

template <class TDataClass>
bool URVNDialogueManager::IsKeyOfType(FRVNBlackboard::FKey KeyID) const
{
	const FRVNBlackboardEntry* EntryInfo = BlackboardAsset ? BlackboardAsset->GetKey(KeyID) : nullptr;

	return (EntryInfo != nullptr) && (EntryInfo->KeyType != nullptr) && (EntryInfo->KeyType->GetClass() ==
		TDataClass::StaticClass());
}

template <class TDataClass>
bool URVNDialogueManager::SetValue(const FName& KeyName, typename TDataClass::FDataType Value)
{
	const FRVNBlackboard::FKey KeyID = GetKeyID(KeyName);

	return SetValue<TDataClass>(KeyID, Value);
}

template <class TDataClass>
bool URVNDialogueManager::SetValue(FRVNBlackboard::FKey KeyID, typename TDataClass::FDataType Value)
{
	const FRVNBlackboardEntry* EntryInfo = BlackboardAsset ? BlackboardAsset->GetKey(KeyID) : nullptr;
	if ((EntryInfo == nullptr) || (EntryInfo->KeyType == nullptr) || (EntryInfo->KeyType->GetClass() !=
		TDataClass::StaticClass()))
	{
		return false;
	}

	const uint16 DataOffset = EntryInfo->KeyType->HasInstance() ? sizeof(FRVNBlackboardInstancedKeyMemory) : 0;

	if (uint8* RawData = GetKeyRawData(KeyID) + DataOffset)
	{
		if (URVNBlackboardKeyType* KeyOb = EntryInfo->KeyType->HasInstance() ? KeyInstances[KeyID] : EntryInfo->KeyType;
			TDataClass::SetValue((TDataClass*)KeyOb, RawData, Value))
		{
			NotifyObservers(KeyID);
			if (BlackboardAsset->HasSynchronizedKeys() && IsKeyInstanceSynced(KeyID))
			{
				for (auto Iter = BlackboardDataToManagersMap.CreateIterator(); Iter; ++Iter)
				{
					URVNBlackboardData* OtherBlackboardAsset = Iter.Key().Get();
					if (OtherBlackboardAsset == nullptr)
					{
						continue;
					}

					bool bCanSync = OtherBlackboardAsset == BlackboardAsset;
					if (!bCanSync)
					{
						URVNBlackboardData const* CurParent = BlackboardAsset->Parent;
						while (CurParent)
						{
							if (CurParent == OtherBlackboardAsset)
							{
								bCanSync = true;
								break;
							}
							CurParent = CurParent->Parent;
						}
					}

					if (!bCanSync)
					{
						continue;
					}

					URVNDialogueManager* OtherManager = Iter.Value().Get();
					if (OtherManager && ShouldSyncWithBlackboard(*OtherManager))
					{
						const FRVNBlackboard::FKey OtherKeyID = OtherBlackboardAsset
							                                        ? OtherBlackboardAsset->GetKeyID(
								                                        EntryInfo->EntryName)
							                                        : FRVNBlackboard::InvalidKey;

						if (OtherKeyID != FRVNBlackboard::InvalidKey)
						{
							URVNBlackboardKeyType* OtherKeyOb = EntryInfo->KeyType->HasInstance()
								                                    ? OtherManager->KeyInstances[OtherKeyID]
								                                    : EntryInfo->KeyType;

							uint8* OtherRawData = OtherManager->GetKeyRawData(OtherKeyID) + DataOffset;

							TDataClass::SetValue((TDataClass*)OtherKeyOb, OtherRawData, Value);
							OtherManager->NotifyObservers(OtherKeyID);
						}
					}
				}
			}
		}

		return true;
	}

	return false;
}

template <class TDataClass>
typename TDataClass::FDataType URVNDialogueManager::GetValue(const FName& KeyName) const
{
	const FRVNBlackboard::FKey KeyID = GetKeyID(KeyName);

	return GetValue<TDataClass>(KeyID);
}

template <class TDataClass>
typename TDataClass::FDataType URVNDialogueManager::GetValue(FRVNBlackboard::FKey KeyID) const
{
	const FRVNBlackboardEntry* EntryInfo = BlackboardAsset ? BlackboardAsset->GetKey(KeyID) : nullptr;
	if ((EntryInfo == nullptr) || (EntryInfo->KeyType == nullptr) || (EntryInfo->KeyType->GetClass() !=
		TDataClass::StaticClass()))
	{
		return TDataClass::InvalidValue;
	}

	URVNBlackboardKeyType* KeyOb = EntryInfo->KeyType->HasInstance() ? KeyInstances[KeyID] : EntryInfo->KeyType;
	const uint16 DataOffset = EntryInfo->KeyType->HasInstance() ? sizeof(FRVNBlackboardInstancedKeyMemory) : 0;

	const uint8* RawData = GetKeyRawData(KeyID) + DataOffset;
	return RawData ? TDataClass::GetValue((TDataClass*)KeyOb, RawData) : TDataClass::InvalidValue;
}
