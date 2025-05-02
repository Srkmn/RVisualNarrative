#include "RVNDialogueManager.h"
#include "AITypes.h"
#include "Misc/RuntimeErrors.h"
#include "Decorator/Task/RVNTask.h"
#include "Decorator/Task/RVNAsyncTask.h"
#include "Decorator/Condition/RVNCondition.h"
#include "Decorator/Task/RVNSyncTask.h"
#include "BlackboardKeysType/RVNBlackboardKeyType_Bool.h"
#include "BlackboardKeysType/RVNBlackboardKeyType_Class.h"
#include "BlackboardKeysType/RVNBlackboardKeyType_Enum.h"
#include "BlackboardKeysType/RVNBlackboardKeyType_Float.h"
#include "BlackboardKeysType/RVNBlackboardKeyType_Int.h"
#include "BlackboardKeysType/RVNBlackboardKeyType_Name.h"
#include "BlackboardKeysType/RVNBlackboardKeyType_Object.h"
#include "BlackboardKeysType/RVNBlackboardKeyType_Rotator.h"
#include "BlackboardKeysType/RVNBlackboardKeyType_String.h"
#include "BlackboardKeysType/RVNBlackboardKeyType_Vector.h"

TMultiMap<TWeakObjectPtr<URVNBlackboardData>, TWeakObjectPtr<URVNDialogueManager>>
URVNDialogueManager::BlackboardDataToManagersMap;

struct FRVNBlackboardInitializationData
{
	FRVNBlackboard::FKey KeyID;
	uint16 DataSize;

	FRVNBlackboardInitializationData()
	{
	}

	FRVNBlackboardInitializationData(FRVNBlackboard::FKey InKeyID, uint16 InDataSize) : KeyID(InKeyID)
	{
		DataSize = (InDataSize <= 2) ? InDataSize : ((InDataSize + 3) & ~3);
	}

	struct FMemorySort
	{
		FORCEINLINE bool operator()(const FRVNBlackboardInitializationData& A,
		                            const FRVNBlackboardInitializationData& B) const
		{
			return A.DataSize > B.DataSize;
		}
	};
};

URVNDialogueManager::URVNDialogueManager()
	: CurrentNodeId(INDEX_NONE)
{
	PrimaryComponentTick.bCanEverTick = false;

	bWantsInitializeComponent = true;

	bSynchronizedKeyPopulated = false;
}

void URVNDialogueManager::InitializeComponent()
{
	Super::InitializeComponent();
}

void URVNDialogueManager::UninitializeComponent()
{
	if (BlackboardAsset && BlackboardAsset->HasSynchronizedKeys())
	{
		BlackboardDataToManagersMap.RemoveSingle(BlackboardAsset, this);
	}

	DestroyValues();
}

void URVNDialogueManager::Initialize(URVNComponent* InComponent)
{
	RVNComponent = InComponent;

	BlackboardAsset = RVNComponent->BlackboardData;

	OnDialogueCompleted.AddDynamic(RVNComponent.Get(), &URVNComponent::OnDialogueComplete);

	if (BlackboardAsset)
	{
		InitializeBlackboard();
	}
}

void URVNDialogueManager::InitializeBlackboard()
{
	if (BlackboardAsset && BlackboardAsset->HasSynchronizedKeys())
	{
		BlackboardDataToManagersMap.RemoveSingle(BlackboardAsset, this);

		DestroyValues();
	}

	ValueMemory.Reset();

	ValueOffsets.Reset();

	bSynchronizedKeyPopulated = false;

	if (BlackboardAsset->IsValid())
	{
		InitializeParentChain(BlackboardAsset);

		TArray<FRVNBlackboardInitializationData> InitList;

		const int32 NumKeys = BlackboardAsset->GetNumKeys();

		InitList.Reserve(NumKeys);

		ValueOffsets.AddZeroed(NumKeys);

		for (URVNBlackboardData* It = BlackboardAsset; It; It = It->Parent)
		{
			for (int32 KeyIndex = 0; KeyIndex < It->Keys.Num(); KeyIndex++)
			{
				if (const auto KeyType = It->Keys[KeyIndex].KeyType)
				{
					KeyType->PreInitialize(*this);

					const uint16 KeyMemory = KeyType->GetValueSize() + (KeyType->HasInstance()
						                                                    ? sizeof(FRVNBlackboardInstancedKeyMemory)
						                                                    : 0);

					InitList.Add(FRVNBlackboardInitializationData(KeyIndex + It->GetFirstKeyID(), KeyMemory));
				}
			}
		}

		InitList.Sort(FRVNBlackboardInitializationData::FMemorySort());

		uint16 MemoryOffset = 0;
		for (int32 Index = 0; Index < InitList.Num(); Index++)
		{
			ValueOffsets[InitList[Index].KeyID] = MemoryOffset;

			MemoryOffset += InitList[Index].DataSize;
		}

		ValueMemory.AddZeroed(MemoryOffset);

		KeyInstances.AddZeroed(InitList.Num());

		for (int32 Index = 0; Index < InitList.Num(); Index++)
		{
			const auto KeyData = BlackboardAsset->GetKey(InitList[Index].KeyID);

			KeyData->KeyType->InitializeKey(*this, InitList[Index].KeyID);
		}

		if (BlackboardAsset->HasSynchronizedKeys())
		{
			PopulateSynchronizedKeys();
		}
	}
}

FName URVNDialogueManager::GetKeyName(FRVNBlackboard::FKey KeyID) const
{
	return BlackboardAsset ? BlackboardAsset->GetKeyName(KeyID) : NAME_None;
}

FRVNBlackboard::FKey URVNDialogueManager::GetKeyID(const FName& KeyName) const
{
	return BlackboardAsset ? BlackboardAsset->GetKeyID(KeyName) : FRVNBlackboard::InvalidKey;
}

TSubclassOf<URVNBlackboardKeyType> URVNDialogueManager::GetKeyType(FRVNBlackboard::FKey KeyID) const
{
	return BlackboardAsset ? BlackboardAsset->GetKeyType(KeyID) : nullptr;
}

bool URVNDialogueManager::IsKeyInstanceSynced(FRVNBlackboard::FKey KeyID) const
{
	return BlackboardAsset ? BlackboardAsset->IsKeyInstanceSynced(KeyID) : false;
}

int32 URVNDialogueManager::GetNumKeys() const
{
	return BlackboardAsset ? BlackboardAsset->GetNumKeys() : 0;
}

bool URVNDialogueManager::HasValidAsset() const
{
	return BlackboardAsset && BlackboardAsset->IsValid();
}

FDelegateHandle URVNDialogueManager::RegisterObserver(FRVNBlackboard::FKey KeyID, UObject* NotifyOwner,
                                                      const FOnRVNBlackboardChangeNotification& ObserverDelegate)
{
	for (auto It = Observers.CreateConstKeyIterator(KeyID); It; ++It)
	{
		if (It.Value().GetHandle() == ObserverDelegate.GetHandle())
		{
			return It.Value().GetHandle();
		}
	}

	const FDelegateHandle Handle = Observers.Add(KeyID, ObserverDelegate).GetHandle();
	ObserverHandles.Add(NotifyOwner, Handle);

	return Handle;
}

void URVNDialogueManager::UnregisterObserver(FRVNBlackboard::FKey KeyID, FDelegateHandle ObserverHandle)
{
	for (auto It = Observers.CreateKeyIterator(KeyID); It; ++It)
	{
		if (It.Value().GetHandle() == ObserverHandle)
		{
			for (auto HandleIt = ObserverHandles.CreateIterator(); HandleIt; ++HandleIt)
			{
				if (HandleIt.Value() == ObserverHandle)
				{
					HandleIt.RemoveCurrent();

					break;
				}
			}

			// If you are observing a notification, do not delete it, but enter a waiting state for deletion
			if (NotifyObserversRecursionCount == 0)
			{
				It.RemoveCurrent();
			}
			else if (!It.Value().bToBeRemoved)
			{
				It.Value().bToBeRemoved = true;

				++ObserversToRemoveCount;
			}
			break;
		}
	}
}

void URVNDialogueManager::UnregisterObserversFrom(UObject* NotifyOwner)
{
	for (auto It = ObserverHandles.CreateKeyIterator(NotifyOwner); It; ++It)
	{
		for (auto ObsIt = Observers.CreateIterator(); ObsIt; ++ObsIt)
		{
			if (ObsIt.Value().GetHandle() == It.Value())
			{
				// If you are observing a notification, do not delete it, but enter a waiting state for deletion
				if (NotifyObserversRecursionCount == 0)
				{
					ObsIt.RemoveCurrent();
				}
				else if (!ObsIt.Value().bToBeRemoved)
				{
					ObsIt.Value().bToBeRemoved = true;

					++ObserversToRemoveCount;
				}
				break;
			}
		}

		It.RemoveCurrent();
	}
}

void URVNDialogueManager::NotifyObservers(FRVNBlackboard::FKey KeyID) const
{
	TMultiMap<uint8, FOnRVNBlackboardChangeNotificationInfo>::TKeyIterator KeyIt(Observers, KeyID);

	if (KeyIt)
	{
		++NotifyObserversRecursionCount;
		for (; KeyIt; ++KeyIt)
		{
			FOnRVNBlackboardChangeNotificationInfo& ObserverDelegateInfo = KeyIt.Value();
			if (ObserverDelegateInfo.bToBeRemoved)
			{
				continue;
			}

			const FOnRVNBlackboardChangeNotification& ObserverDelegate = ObserverDelegateInfo.DelegateHandle;
			const bool bWantsToContinueObserving = ObserverDelegate.IsBound() &&
				(ObserverDelegate.Execute(*this, KeyID) == ERVNBlackboardNotificationResult::ContinueObserving);

			if (bWantsToContinueObserving == false)
			{
				// Remove from the ObserverHandle map, if not already removed
				if (!ObserverDelegateInfo.bToBeRemoved)
				{
					for (auto HandleIt = ObserverHandles.CreateIterator(); HandleIt; ++HandleIt)
					{
						if (HandleIt.Value() == ObserverDelegate.GetHandle())
						{
							HandleIt.RemoveCurrent();

							break;
						}
					}
				}

				// If you are observing a notification, do not delete it, but enter a waiting state for deletion
				if (NotifyObserversRecursionCount == 1)
				{
					KeyIt.RemoveCurrent();

					if (ObserverDelegateInfo.bToBeRemoved)
					{
						--ObserversToRemoveCount;
					}
				}
				else if (!ObserverDelegateInfo.bToBeRemoved)
				{
					ObserverDelegateInfo.bToBeRemoved = true;

					++ObserversToRemoveCount;
				}
			}
		}
		--NotifyObserversRecursionCount;

		if (NotifyObserversRecursionCount == 0 && ObserversToRemoveCount > 0)
		{
			for (auto ObsIt = Observers.CreateIterator(); ObsIt; ++ObsIt)
			{
				if (ObsIt.Value().bToBeRemoved)
				{
					ObsIt.RemoveCurrent();
					if (--ObserversToRemoveCount == 0)
					{
						break;
					}
				}
			}
			ObserversToRemoveCount = 0;
		}
	}
}

URVNBlackboardData* URVNDialogueManager::GetBlackboardAsset() const
{
	return BlackboardAsset;
}

FRVNNodeData URVNDialogueManager::StartDialogue()
{
	return StartDialogueFromId(0);
}

FRVNNodeData URVNDialogueManager::StartDialogueFromId(int32 InNodeId)
{
	FRVNNodeData CurrentNode;
	if (!RVNComponent.IsValid() || !RVNComponent->GetNodeData(InNodeId, CurrentNode))
	{
		return CurrentNode;
	}

	CurrentNodeId = InNodeId;

	return CurrentNode;
}

TArray<FRVNNodeData> URVNDialogueManager::GetPassedNextNodeData(TArray<int32> NextNodesId)
{
	TArray<FRVNNodeData> Results;

	for (const auto NextNodeId : NextNodesId)
	{
		if (CheckAllConditions(NextNodeId))
		{
			Results.Add(GetNodeDataFromId(NextNodeId));
		}
	}

	return Results;
}


FRVNNodeData URVNDialogueManager::GetNodeDataFromId(int32 InNodeId)
{
	FRVNNodeData NodeData;
	if (!RVNComponent->GetNodeData(InNodeId, NodeData))
	{
		return NodeData;
	}

	return NodeData;
}

bool URVNDialogueManager::CheckAllConditions(int32 InNodeId) const
{
	FRVNNodeData NodeData;
	if (!RVNComponent->GetNodeData(InNodeId, NodeData))
	{
		return false;
	}

	for (auto CurrentCondition : NodeData.Conditions)
	{
		if (!CheckCondition(CurrentCondition))
		{
			return false;
		}
	}

	return true;
}

void URVNDialogueManager::ProcessAllTasks(int32 InNodeId)
{
	FRVNNodeData NodeData;
	if (!RVNComponent->GetNodeData(InNodeId, NodeData))
	{
		return;
	}

	PendingProcessTask = nullptr;

	for (auto CurrentTask : NodeData.Tasks)
	{
		if (!IsValid(CurrentTask))
		{
			continue;
		}

		if (CheckAllConditions(CurrentTask))
		{
			PendingProcessTask = CurrentTask;

			break;
		}
	}

	if (!IsValid(PendingProcessTask))
	{
		OnTaskCompletedCallback.Broadcast();

		return;
	}

	ProcessTask(PendingProcessTask);
}

void URVNDialogueManager::BreakCurrentDialogue()
{
	if (!IsValid(PendingProcessTask))
	{
		return;
	}

	if (const auto TaskClass = PendingProcessTask->GetClass())
	{
		if (const auto AsyncTask = Cast<URVNAsyncTask>(PendingProcessTask))
		{
			if (!TaskClass->HasAnyClassFlags(CLASS_Native))
			{
				FRVNTaskParams ConditionParams;
				ConditionParams.InRVNComponent = RVNComponent.Get();

				if (const auto ExecuteTaskFunc = AsyncTask->FindFunction(TEXT("BreakAsyncTask")))
				{
					AsyncTask->ProcessEvent(ExecuteTaskFunc, &ConditionParams);
				}
			}
			else
			{
				AsyncTask->BreakAsyncTask_Implementation(RVNComponent.Get());
			}
		}
	}
}

void URVNDialogueManager::ProcessTask(URVNTaskBase* InTask)
{
	// OnStart
	if (const auto TaskClass = InTask->GetClass())
	{
		if (!TaskClass->HasAnyClassFlags(CLASS_Native))
		{
			FRVNTaskParams ConditionParams;
			ConditionParams.InRVNComponent = RVNComponent.Get();

			if (const auto StartTaskFunc = InTask->FindFunction(TEXT("OnStart")))
			{
				InTask->ProcessEvent(StartTaskFunc, &ConditionParams);
			}
		}
		else
		{
			InTask->OnStart_Implementation(RVNComponent.Get());
		}
	}

	// ExecuteTask
	if (const auto TaskClass = InTask->GetClass())
	{
		if (const auto AsyncTask = Cast<URVNAsyncTask>(InTask))
		{
			AsyncTask->GetCompletedCallback().BindUObject(this, &URVNDialogueManager::OnTaskCompleted, InTask);
		}

		if (!TaskClass->HasAnyClassFlags(CLASS_Native))
		{
			FRVNTaskParams ConditionParams;
			ConditionParams.InRVNComponent = RVNComponent.Get();

			if (const auto ExecuteTaskFunc = InTask->FindFunction(TEXT("ExecuteTask")))
			{
				InTask->ProcessEvent(ExecuteTaskFunc, &ConditionParams);
			}
		}
		else
		{
			InTask->ExecuteTask_Implementation(RVNComponent.Get());
		}

		if (const auto SyncTask = Cast<URVNSyncTask>(InTask))
		{
			OnTaskCompleted(SyncTask);
		}
	}
}

bool URVNDialogueManager::CheckAllConditions(URVNTaskBase* InTask) const
{
	if (!IsValid(InTask))
	{
		return false;
	}

	for (const auto Condition : InTask->GetConditions())
	{
		if (!CheckCondition(Condition))
		{
			return false;
		}
	}

	return true;
}

bool URVNDialogueManager::CheckCondition(URVNConditionBase* InCondition) const
{
	if (!IsValid(InCondition))
	{
		return true;
	}

	const auto ConditionClass = InCondition->GetClass();

	if (!ConditionClass->HasAnyClassFlags(CLASS_Native))
	{
		FRVNConditionParams ConditionParams;
		ConditionParams.InRVNComponent = RVNComponent.Get();

		if (const auto CheckConditionFunc = InCondition->FindFunction(TEXT("CheckCondition")))
		{
			InCondition->ProcessEvent(CheckConditionFunc, &ConditionParams);
		}

		if (!ConditionParams.bIsPass)
		{
			return false;
		}
	}
	else
	{
		if (!InCondition->CheckCondition_Implementation(RVNComponent.Get()))
		{
			return false;
		}
	}

	return true;
}

bool URVNDialogueManager::IsCompatibleWith(URVNBlackboardData* TestAsset) const
{
	for (auto It = BlackboardAsset; It; It = It->Parent)
	{
		if (It == TestAsset)
		{
			return true;
		}

		if (It->Keys == TestAsset->Keys)
		{
			return true;
		}
	}

	return false;
}

UObject* URVNDialogueManager::GetValueAsObject(const FName& KeyName) const
{
	return GetValue<URVNBlackboardKeyType_Object>(KeyName);
}

UClass* URVNDialogueManager::GetValueAsClass(const FName& KeyName) const
{
	return GetValue<URVNBlackboardKeyType_Class>(KeyName);
}

uint8 URVNDialogueManager::GetValueAsEnum(const FName& KeyName) const
{
	return GetValue<URVNBlackboardKeyType_Enum>(KeyName);
}

int32 URVNDialogueManager::GetValueAsInt(const FName& KeyName) const
{
	return GetValue<URVNBlackboardKeyType_Int>(KeyName);
}

float URVNDialogueManager::GetValueAsFloat(const FName& KeyName) const
{
	return GetValue<URVNBlackboardKeyType_Float>(KeyName);
}

bool URVNDialogueManager::GetValueAsBool(const FName& KeyName) const
{
	return GetValue<URVNBlackboardKeyType_Bool>(KeyName);
}

FString URVNDialogueManager::GetValueAsString(const FName& KeyName) const
{
	return GetValue<URVNBlackboardKeyType_String>(KeyName);
}

FName URVNDialogueManager::GetValueAsName(const FName& KeyName) const
{
	return GetValue<URVNBlackboardKeyType_Name>(KeyName);
}

FVector URVNDialogueManager::GetValueAsVector(const FName& KeyName) const
{
	return GetValue<URVNBlackboardKeyType_Vector>(KeyName);
}

FRotator URVNDialogueManager::GetValueAsRotator(const FName& KeyName) const
{
	return GetValue<URVNBlackboardKeyType_Rotator>(KeyName);
}

void URVNDialogueManager::SetValueAsObject(const FName& KeyName, UObject* ObjectValue)
{
	const FRVNBlackboard::FKey KeyID = GetKeyID(KeyName);

	SetValue<URVNBlackboardKeyType_Object>(KeyID, ObjectValue);
}

void URVNDialogueManager::SetValueAsClass(const FName& KeyName, UClass* ClassValue)
{
	const FRVNBlackboard::FKey KeyID = GetKeyID(KeyName);

	SetValue<URVNBlackboardKeyType_Class>(KeyID, ClassValue);
}

void URVNDialogueManager::SetValueAsEnum(const FName& KeyName, uint8 EnumValue)
{
	const FRVNBlackboard::FKey KeyID = GetKeyID(KeyName);

	SetValue<URVNBlackboardKeyType_Enum>(KeyID, EnumValue);
}

void URVNDialogueManager::SetValueAsInt(const FName& KeyName, int32 IntValue)
{
	const FRVNBlackboard::FKey KeyID = GetKeyID(KeyName);

	SetValue<URVNBlackboardKeyType_Int>(KeyID, IntValue);
}

void URVNDialogueManager::SetValueAsFloat(const FName& KeyName, float FloatValue)
{
	const FRVNBlackboard::FKey KeyID = GetKeyID(KeyName);

	SetValue<URVNBlackboardKeyType_Float>(KeyID, FloatValue);
}

void URVNDialogueManager::SetValueAsBool(const FName& KeyName, bool BoolValue)
{
	const FRVNBlackboard::FKey KeyID = GetKeyID(KeyName);

	SetValue<URVNBlackboardKeyType_Bool>(KeyID, BoolValue);
}

void URVNDialogueManager::SetValueAsString(const FName& KeyName, FString StringValue)
{
	const FRVNBlackboard::FKey KeyID = GetKeyID(KeyName);

	SetValue<URVNBlackboardKeyType_String>(KeyID, StringValue);
}

void URVNDialogueManager::SetValueAsName(const FName& KeyName, FName NameValue)
{
	const FRVNBlackboard::FKey KeyID = GetKeyID(KeyName);

	SetValue<URVNBlackboardKeyType_Name>(KeyID, NameValue);
}

void URVNDialogueManager::SetValueAsVector(const FName& KeyName, FVector VectorValue)
{
	const FRVNBlackboard::FKey KeyID = GetKeyID(KeyName);

	SetValue<URVNBlackboardKeyType_Vector>(KeyID, VectorValue);
}

void URVNDialogueManager::SetValueAsRotator(const FName& KeyName, FRotator RotatorValue)
{
	const FRVNBlackboard::FKey KeyID = GetKeyID(KeyName);

	SetValue<URVNBlackboardKeyType_Rotator>(KeyID, RotatorValue);
}

bool URVNDialogueManager::IsVectorValueSet(const FName& KeyName) const
{
	const FRVNBlackboard::FKey KeyID = GetKeyID(KeyName);

	return IsVectorValueSet(KeyID);
}

bool URVNDialogueManager::IsVectorValueSet(FRVNBlackboard::FKey KeyID) const
{
	FVector VectorValue = GetValue<URVNBlackboardKeyType_Vector>(KeyID);

	return (VectorValue != FAISystem::InvalidLocation);
}

bool URVNDialogueManager::GetLocationFromEntry(const FName& KeyName, FVector& ResultLocation) const
{
	const FRVNBlackboard::FKey KeyID = GetKeyID(KeyName);

	return GetLocationFromEntry(KeyID, ResultLocation);
}

bool URVNDialogueManager::GetLocationFromEntry(FRVNBlackboard::FKey KeyID, FVector& ResultLocation) const
{
	if (BlackboardAsset && ValueOffsets.IsValidIndex(KeyID))
	{
		if (const auto EntryInfo = BlackboardAsset->GetKey(KeyID); EntryInfo->KeyType)
		{
			const uint8* ValueData = ValueMemory.GetData() + ValueOffsets[KeyID];

			return EntryInfo->KeyType->WrappedGetLocation(*this, ValueData, ResultLocation);
		}
	}

	return false;
}

bool URVNDialogueManager::GetRotationFromEntry(const FName& KeyName, FRotator& ResultRotation) const
{
	const FRVNBlackboard::FKey KeyID = GetKeyID(KeyName);

	return GetRotationFromEntry(KeyID, ResultRotation);
}

bool URVNDialogueManager::GetRotationFromEntry(FRVNBlackboard::FKey KeyID, FRotator& ResultRotation) const
{
	if (BlackboardAsset && ValueOffsets.IsValidIndex(KeyID))
	{
		if (const FRVNBlackboardEntry* EntryInfo = BlackboardAsset->GetKey(KeyID); EntryInfo->KeyType)
		{
			const uint8* ValueData = ValueMemory.GetData() + ValueOffsets[KeyID];

			return EntryInfo->KeyType->WrappedGetRotation(*this, ValueData, ResultRotation);
		}
	}

	return false;
}


void URVNDialogueManager::ClearValue(const FName& KeyName)
{
	const FRVNBlackboard::FKey KeyID = GetKeyID(KeyName);

	ClearValue(KeyID);
}

void URVNDialogueManager::ClearValue(FRVNBlackboard::FKey KeyID)
{
	if (!ensureAsRuntimeWarning(BlackboardAsset != nullptr))
	{
		return;
	}

	const FRVNBlackboardEntry* EntryInfo = BlackboardAsset->GetKey(KeyID);

	if (uint8* RawData = GetKeyRawData(KeyID))
	{
		if (EntryInfo->KeyType->WrappedIsEmpty(*this, RawData) == false)
		{
			EntryInfo->KeyType->WrappedClear(*this, RawData);
			NotifyObservers(KeyID);

			if (BlackboardAsset->HasSynchronizedKeys() && IsKeyInstanceSynced(KeyID))
			{
				const bool bKeyHasInstance = EntryInfo->KeyType->HasInstance();

				URVNBlackboardKeyType* KeyOb = bKeyHasInstance ? KeyInstances[KeyID] : EntryInfo->KeyType;

				const uint16 DataOffset = bKeyHasInstance ? sizeof(FRVNBlackboardInstancedKeyMemory) : 0;

				uint8* InstancedRawData = RawData + DataOffset;

				for (auto Iter = BlackboardDataToManagersMap.CreateIterator(); Iter; ++Iter)
				{
					URVNBlackboardData* OtherBlackboardAsset = Iter.Key().Get();
					if (!IsValid(OtherBlackboardAsset))
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

					if (URVNDialogueManager* OtherManager = Iter.Value().Get();
						OtherManager != nullptr && ShouldSyncWithBlackboard(*OtherManager))
					{
						const int32 OtherKeyID = OtherBlackboardAsset
							                         ? OtherBlackboardAsset->GetKeyID(EntryInfo->EntryName)
							                         : FRVNBlackboard::InvalidKey;

						if (OtherKeyID != FRVNBlackboard::InvalidKey)
						{
							URVNBlackboardKeyType* OtherKeyOb = bKeyHasInstance
								                                    ? OtherManager->KeyInstances[OtherKeyID]
								                                    : EntryInfo->KeyType;

							uint8* OtherRawData = OtherManager->GetKeyRawData(OtherKeyID) + DataOffset;

							OtherKeyOb->CopyValues(*OtherManager, OtherRawData, KeyOb, InstancedRawData);
							OtherManager->NotifyObservers(OtherKeyID);
						}
					}
				}
			}
		}
	}
}

bool URVNDialogueManager::CopyKeyValue(FRVNBlackboard::FKey SourceKeyID, FRVNBlackboard::FKey DestinationKeyID)
{
	URVNBlackboardData* BBAsset = GetBlackboardAsset();
	if (!IsValid(BBAsset))
	{
		return false;
	}

	// copy only when values are initialized
	if (ValueMemory.Num() == 0)
	{
		return false;
	}

	const FRVNBlackboardEntry* SourceValueEntryInfo = BBAsset->GetKey(SourceKeyID);
	const FRVNBlackboardEntry* DestinationValueEntryInfo = BBAsset->GetKey(DestinationKeyID);

	if (SourceValueEntryInfo == nullptr || DestinationValueEntryInfo == nullptr || SourceValueEntryInfo->KeyType ==
		nullptr || DestinationValueEntryInfo->KeyType == nullptr)
	{
		return false;
	}

	if (SourceValueEntryInfo->KeyType->GetClass() != DestinationValueEntryInfo->KeyType->GetClass())
	{
		return false;
	}

	const bool bKeyHasInstance = SourceValueEntryInfo->KeyType->HasInstance();

	const uint16 MemDataOffset = bKeyHasInstance ? sizeof(FRVNBlackboardInstancedKeyMemory) : 0;

	const uint8* SourceValueMem = GetKeyRawData(SourceKeyID) + MemDataOffset;
	uint8* DestinationValueMem = GetKeyRawData(DestinationKeyID) + MemDataOffset;

	const URVNBlackboardKeyType* SourceKeyOb = bKeyHasInstance
		                                           ? KeyInstances[SourceKeyID]
		                                           : SourceValueEntryInfo->KeyType;
	URVNBlackboardKeyType* DestKeyOb = bKeyHasInstance
		                                   ? KeyInstances[DestinationKeyID]
		                                   : DestinationValueEntryInfo->KeyType;

	DestKeyOb->CopyValues(*this, DestinationValueMem, SourceKeyOb, SourceValueMem);

	return true;
}

void URVNDialogueManager::InitializeParentChain(URVNBlackboardData* NewAsset)
{
	if (NewAsset)
	{
		InitializeParentChain(NewAsset->Parent);
		NewAsset->UpdateKeyIDs();
	}
}

void URVNDialogueManager::DestroyValues()
{
	for (URVNBlackboardData* It = BlackboardAsset; It; It = It->Parent)
	{
		for (int32 KeyIndex = 0; KeyIndex < It->Keys.Num(); KeyIndex++)
		{
			URVNBlackboardKeyType* KeyType = It->Keys[KeyIndex].KeyType;
			if (KeyType)
			{
				const int32 UseIdx = KeyIndex + It->GetFirstKeyID();
				uint8* KeyMemory = GetKeyRawData(UseIdx);
				KeyType->WrappedFree(*this, KeyMemory);
			}
		}
	}

	ValueOffsets.Reset();
	ValueMemory.Reset();
}

void URVNDialogueManager::PopulateSynchronizedKeys()
{
	ensure(bSynchronizedKeyPopulated == false);

	BlackboardDataToManagersMap.Add(BlackboardAsset, this);

	for (auto Iter = BlackboardDataToManagersMap.CreateIterator(); Iter; ++Iter)
	{
		URVNBlackboardData* OtherBlackboardAsset = Iter.Key().Get();
		if (!IsValid(OtherBlackboardAsset))
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
			for (const auto& Key : BlackboardAsset->Keys)
			{
				if (Key.bInstanceSynced)
				{
					const int32 OtherKeyID = OtherBlackboardAsset
						                         ? OtherBlackboardAsset->GetKeyID(Key.EntryName)
						                         : FRVNBlackboard::InvalidKey;

					if (OtherKeyID != FRVNBlackboard::InvalidKey)
					{
						const FRVNBlackboardEntry* const OtherKey = OtherManager->GetBlackboardAsset()->GetKey(
							OtherKeyID);
						check(Key.EntryName == OtherKey->EntryName);
						check(Key.KeyType && OtherKey->KeyType && Key.KeyType->GetClass() == OtherKey->KeyType->GetClass
							());

						const bool bKeyHasInstance = Key.KeyType->HasInstance();
						const uint16 DataOffset = bKeyHasInstance ? sizeof(FRVNBlackboardInstancedKeyMemory) : 0;

						const int32 KeyID = BlackboardAsset->GetKeyID(Key.EntryName);
						uint8* RawData = GetKeyRawData(KeyID) + DataOffset;
						uint8* RawSource = OtherManager->GetKeyRawData(OtherKeyID) + DataOffset;

						URVNBlackboardKeyType* KeyOb = bKeyHasInstance
							                               ? ToRawPtr(KeyInstances[KeyID])
							                               : ToRawPtr(Key.KeyType);

						const URVNBlackboardKeyType* SourceKeyOb = bKeyHasInstance
							                                           ? ToRawPtr(
								                                           OtherManager->KeyInstances[OtherKeyID])
							                                           : ToRawPtr(Key.KeyType);

						KeyOb->CopyValues(*this, RawData, SourceKeyOb, RawSource);
					}
				}
			}
		}
	}

	bSynchronizedKeyPopulated = true;
}

bool URVNDialogueManager::ShouldSyncWithBlackboard(URVNDialogueManager& OtherManager) const
{
	return &OtherManager != this && OtherManager.BlackboardAsset->IsRelatedTo(*BlackboardAsset);
}

void URVNDialogueManager::OnTaskCompleted(URVNTaskBase* CompletedTask)
{
	if (const auto AsyncTask = Cast<URVNAsyncTask>(CompletedTask);
		AsyncTask && AsyncTask->GetCompletedCallback().IsBoundToObject(this))
	{
		AsyncTask->GetCompletedCallback().Unbind();
	}

	if (const auto TaskClass = CompletedTask->GetClass())
	{
		if (!TaskClass->HasAnyClassFlags(CLASS_Native))
		{
			FRVNTaskParams ConditionParams;
			ConditionParams.InRVNComponent = RVNComponent.Get();

			if (const auto EndTaskFunc = CompletedTask->FindFunction(TEXT("OnEnd")))
			{
				CompletedTask->ProcessEvent(EndTaskFunc, &ConditionParams);
			}
		}
		else
		{
			CompletedTask->OnEnd_Implementation(RVNComponent.Get());
		}
	}

	TryProcessNextTask();
}

void URVNDialogueManager::TryProcessNextTask()
{
	if (IsValid(PendingProcessTask) && !PendingProcessTask->GetChildren().IsEmpty())
	{
		for (URVNTaskBase* ChildTask : PendingProcessTask->GetChildren())
		{
			if (CheckAllConditions(ChildTask))
			{
				PendingProcessTask = ChildTask;

				ProcessTask(PendingProcessTask);

				return;
			}
		}
	}

	OnTaskCompletedCallback.Broadcast();
}
