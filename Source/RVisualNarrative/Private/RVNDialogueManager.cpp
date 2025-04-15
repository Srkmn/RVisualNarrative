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
	  , PendingProcessCount(0)
	  , CompletedTaskCount(0)
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

	CurrentTasks.Empty();

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
						                                                    ? sizeof(FRVNBlackboardInitializationData)
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
		if (CurrentCondition == nullptr)
		{
			continue;
		}

		const auto ConditionClass = CurrentCondition->GetClass();

		if (!ConditionClass->HasAnyClassFlags(CLASS_Native))
		{
			FRVNConditionParams ConditionParams;
			ConditionParams.InRVNComponent = RVNComponent.Get();

			if (const auto CheckConditionFunc = CurrentCondition->FindFunction(TEXT("CheckCondition")))
			{
				CurrentCondition->ProcessEvent(CheckConditionFunc, &ConditionParams);
			}

			if (!ConditionParams.bIsPass)
			{
				return false;
			}
		}
		else
		{
			if (!CurrentCondition->CheckCondition_Implementation(RVNComponent.Get()))
			{
				return false;
			}
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

	if (NodeData.Tasks.IsEmpty())
	{
		OnTaskCompletedCallback.Broadcast();
	}

	PendingProcessCount = NodeData.Tasks.Num();
	CompletedTaskCount = 0;
	CurrentTasks.Empty();
	CurrentTasks.Reserve(NodeData.Tasks.Num());

	for (auto CurrentTask : NodeData.Tasks)
	{
		if (CurrentTask == nullptr)
		{
			continue;
		}

		CurrentTasks.Add(CurrentTask);
	}

	// OnState
	for (const auto CurrentTask : CurrentTasks)
	{
		const auto TaskClass = CurrentTask->GetClass();

		if (!TaskClass->HasAnyClassFlags(CLASS_Native))
		{
			FRVNTaskParams ConditionParams;
			ConditionParams.InRVNComponent = RVNComponent.Get();

			if (const auto StartTaskFunc = CurrentTask->FindFunction(TEXT("OnStart")))
			{
				CurrentTask->ProcessEvent(StartTaskFunc, &ConditionParams);
			}
		}
		else
		{
			CurrentTask->OnStart_Implementation(RVNComponent.Get());
		}
	}

	// ExecuteTask
	for (const auto CurrentTask : CurrentTasks)
	{
		if (const auto TaskClass = CurrentTask->GetClass())
		{
			if (const auto AsyncTask = Cast<URVNAsyncTask>(CurrentTask))
			{
				AsyncTask->GetCompletedCallback().BindUObject(this, &URVNDialogueManager::OnTaskCompleted, CurrentTask);
			}

			if (!TaskClass->HasAnyClassFlags(CLASS_Native))
			{
				FRVNTaskParams ConditionParams;
				ConditionParams.InRVNComponent = RVNComponent.Get();

				if (const auto ExecuteTaskFunc = CurrentTask->FindFunction(TEXT("ExecuteTask")))
				{
					CurrentTask->ProcessEvent(ExecuteTaskFunc, &ConditionParams);
				}
			}
			else
			{
				CurrentTask->ExecuteTask_Implementation(RVNComponent.Get());
			}

			if (const auto SyncTask = Cast<URVNSyncTask>(CurrentTask))
			{
				OnTaskCompleted(SyncTask);
			}
		}
	}
}

void URVNDialogueManager::BreakCurrentDialogue()
{
	for (const auto CurrentTask : CurrentTasks)
	{
		if (const auto TaskClass = CurrentTask->GetClass())
		{
			if (const auto AsyncTask = Cast<URVNAsyncTask>(CurrentTask))
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

			if (BlackboardAsset->HasSynchronizedKeys() && IsKeyInstanceSynced(KeyID))
			{
				const bool bKeyHasInstance = EntryInfo->KeyType->HasInstance();

				URVNBlackboardKeyType* KeyOb = bKeyHasInstance ? KeyInstances[KeyID] : EntryInfo->KeyType;

				const uint16 DataOffset = bKeyHasInstance ? sizeof(FRVNBlackboardInstancedKeyMemory) : 0;

				uint8* InstancedRawData = RawData + DataOffset;

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
	if (BBAsset == nullptr)
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
	CompletedTaskCount++;

	if (const auto AsyncTask = Cast<URVNAsyncTask>(CompletedTask))
	{
		AsyncTask->GetCompletedCallback().Unbind();
	}

	if (CompletedTaskCount >= PendingProcessCount)
	{
		for (const auto CurrentTask : CurrentTasks)
		{
			if (const auto TaskClass = CurrentTask->GetClass())
			{
				if (!TaskClass->HasAnyClassFlags(CLASS_Native))
				{
					FRVNTaskParams ConditionParams;
					ConditionParams.InRVNComponent = RVNComponent.Get();

					if (const auto EndTaskFunc = CurrentTask->FindFunction(TEXT("OnEnd")))
					{
						CurrentTask->ProcessEvent(EndTaskFunc, &ConditionParams);
					}
				}
				else
				{
					CurrentTask->OnEnd_Implementation(RVNComponent.Get());
				}
			}
		}

		CurrentTasks.Empty();

		OnTaskCompletedCallback.Broadcast();
	}
}
