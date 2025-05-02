#include "ClassCollector/RVNClassCollector.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Decorator/Condition/RVNCondition.h"
#include "Decorator/Task/RVNTask.h"
#include "Settings/RVisualNarrativeEditorSetting.h"
#include "Blueprint/RVNDecoratorBlueprint.h"
#include "UEVersion.h"

TMultiMap<UClass*, FRVNClassInfo> FRVNClassCollector::Classes;

FDelegateHandle FRVNClassCollector::OnAssetAddedDelegateHandle;

FDelegateHandle FRVNClassCollector::OnAssetRemovedDelegateHandle;

FDelegateHandle FRVNClassCollector::OnAssetRenamedDelegateHandle;

FRVNClassCollector::FRVNClassCollector()
{
	PopulateClasses();

	if (const auto AssetRegistryModule = FModuleManager::GetModulePtr<FAssetRegistryModule>(TEXT("AssetRegistry")))
	{
		OnAssetAddedDelegateHandle =
			AssetRegistryModule->Get().OnAssetAdded().AddStatic(
				&FRVNClassCollector::AssetAdded);

		OnAssetRemovedDelegateHandle =
			AssetRegistryModule->Get().OnAssetRemoved().AddStatic(
				&FRVNClassCollector::RemoveAsset);

		OnAssetRenamedDelegateHandle =
			AssetRegistryModule->Get().OnAssetRenamed().AddStatic(
				&FRVNClassCollector::OnAssetRenamed);
	}
}

FRVNClassCollector::~FRVNClassCollector()
{
	if (const auto AssetRegistryModule = FModuleManager::GetModulePtr<FAssetRegistryModule>(TEXT("AssetRegistry")))
	{
		AssetRegistryModule->Get().OnAssetRenamed().Remove(OnAssetRenamedDelegateHandle);

		AssetRegistryModule->Get().OnAssetRemoved().Remove(OnAssetRemovedDelegateHandle);

		AssetRegistryModule->Get().OnAssetAdded().Remove(OnAssetAddedDelegateHandle);
	}
}

void FRVNClassCollector::GetClasses(const UClass* KeyClass, TArray<UClass*>& OutClasses)
{
	if (!Classes.Contains(KeyClass))
	{
		return;
	}

	for (auto It = Classes.CreateKeyIterator(KeyClass); It; ++It)
	{
		if (const auto ClassToAdd = It.Value().GetClass())
		{
			OutClasses.Add(ClassToAdd);
		}
	}
}

void FRVNClassCollector::PopulateClasses()
{
	PopulateClassInMemory();

	PopulateClassByAsset();
}

void FRVNClassCollector::PopulateClassInMemory()
{
	PopulateClassInMemoryByClass(URVNTaskBase::StaticClass());

	PopulateClassInMemoryByClass(URVNConditionBase::StaticClass());
}

void FRVNClassCollector::PopulateClassByAsset()
{
	if (const auto RVNEditorSetting = GetMutableDefault<URVisualNarrativeEditorSetting>())
	{
		const auto& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
			TEXT("AssetRegistry"));

		TArray<FName> AssetPaths;

		for (auto AssetPath : RVNEditorSetting->GetSupportedAssetPath())
		{
			AssetPath = AssetPath == FApp::GetProjectName() ? TEXT("Game") : AssetPath;

			AssetPaths.Add(*FString::Printf(TEXT(
				"/%s"
			),
			                                *AssetPath));
		}

		TArray<FAssetData> AllAssetData;

		AssetRegistryModule.Get().GetAssetsByPaths(AssetPaths, AllAssetData, true);

		ProcessSupportedAsset([&AllAssetData](const TArray<UClass*>& InSupportedClasses,
		                                      const TArray<FName>& InSupportedAssetClassNames)
		{
			for (const auto& AssetData : AllAssetData)
			{
				if (const auto& AssetDataClassName = AssetData.
#if UE_ASSET_DATA_GET_ASSET_CLASS_STRING
				                                     AssetClassPath.GetAssetName();
#else
				                                     AssetClass;
#endif

					IsSupportedAssetData(InSupportedAssetClassNames, AssetData))
				{
					const auto DecoratorBlueprint = Cast<URVNDecoratorBlueprint>(
						InSupportedClasses[InSupportedAssetClassNames.Find(AssetDataClassName)]->ClassDefaultObject);

					if (const auto DecoratorParentClass = DecoratorBlueprint->GetParentClass())
					{
						Classes.Add(DecoratorParentClass, FRVNClassInfo(AssetData));
					}
				}
			}
		});
	}
}

void FRVNClassCollector::PopulateClassInMemoryByClass(UClass* BaseClass)
{
	TSet<UClass*> CurClasses;

	for (TObjectIterator<UClass> ObjectIt; ObjectIt; ++ObjectIt)
	{
		if (ObjectIt->IsChildOf(BaseClass))
		{
			CurClasses.Add(*ObjectIt);
		}
	}

	CurClasses.Remove(BaseClass);

	for (const auto CurClass : CurClasses)
	{
		if (!CurClass)
		{
			continue;
		}

		if (CurClass->HasAnyClassFlags(CLASS_Native) && !CurClass->HasAnyClassFlags(CLASS_Abstract))
		{
			Classes.Add(BaseClass, FRVNClassInfo(CurClass->GetName(), CurClass->GetPathName()));
		}
	}
}

void FRVNClassCollector::AssetAdded(const FAssetData& InAssetData)
{
	ProcessSupportedAsset([&InAssetData](const TArray<UClass*>& InSupportedClasses,
	                                     const TArray<FName>& InSupportedAssetClassNames)
	{
		if (const auto& AssetDataClassName = InAssetData.
#if UE_ASSET_DATA_GET_ASSET_CLASS_STRING
		                                     AssetClassPath.GetAssetName();
#else
											 AssetClass;
#endif

			IsSupportedAssetData(InSupportedAssetClassNames, InAssetData))
		{
			const auto DecoratorBlueprint = Cast<URVNDecoratorBlueprint>(
				InSupportedClasses[InSupportedAssetClassNames.Find(AssetDataClassName)]->ClassDefaultObject);

			if (const auto DecoratorParentClass = DecoratorBlueprint->GetParentClass())
			{
				Classes.Add(DecoratorParentClass, FRVNClassInfo(InAssetData));
			}
		}
	});
}

void FRVNClassCollector::RemoveAsset(const FAssetData& InAssetData)
{
	ProcessSupportedAsset([&InAssetData](const TArray<UClass*>& InSupportedClasses,
	                                     const TArray<FName>& InSupportedAssetClassNames)
	{
		if (const auto& AssetDataClassName = InAssetData.
#if UE_ASSET_DATA_GET_ASSET_CLASS_STRING
		                                     AssetClassPath.GetAssetName();
#else
											 AssetClass;
#endif

			IsSupportedAssetData(InSupportedAssetClassNames, InAssetData))
		{
			const auto DecoratorBlueprint = Cast<URVNDecoratorBlueprint>(
				InSupportedClasses[InSupportedAssetClassNames.Find(AssetDataClassName)]->ClassDefaultObject);

			if (const auto DecoratorParentClass = DecoratorBlueprint->GetParentClass())
			{
				Classes.Remove(DecoratorParentClass, FRVNClassInfo(InAssetData));
			}
		}
	});
}

void FRVNClassCollector::OnAssetRenamed(const FAssetData& InAssetData, const FString& InOldObjectPath)
{
	ProcessSupportedAsset([&InAssetData, &InOldObjectPath](const TArray<UClass*>& InSupportedClasses,
	                                                       const TArray<FName>& InSupportedAssetClassNames)
	{
		if (const auto& AssetDataClassName = InAssetData.
#if UE_ASSET_DATA_GET_ASSET_CLASS_STRING
		                                     AssetClassPath.GetAssetName();
#else
											 AssetClass;
#endif

			IsSupportedAssetData(InSupportedAssetClassNames, InAssetData))
		{
			const auto DecoratorBlueprint = Cast<URVNDecoratorBlueprint>(
				InSupportedClasses[InSupportedAssetClassNames.Find(AssetDataClassName)]->ClassDefaultObject);

			const auto DecoratorParentClass = DecoratorBlueprint->GetParentClass();

			check(DecoratorParentClass)

			for (auto It = Classes.CreateKeyIterator(DecoratorParentClass); It; ++It)
			{
				if (auto& OldClassInfo = It.Value(); OldClassInfo == InOldObjectPath)
				{
					OldClassInfo.ClassName = InAssetData.
#if UE_ASSET_DATA_GET_ASSET_CLASS_STRING
					                         AssetClassPath.GetAssetName()
#else
											 AssetClass
#endif
						.ToString();

					OldClassInfo.ObjectPath = InAssetData.
#if UE_ASSET_DATA_GET_SOFT_OBJECT_PATH_STRING
						GetSoftObjectPath()
#else
						ObjectPath
#endif
						.ToString();
				}
			}
		}
	});
}

void FRVNClassCollector::ProcessSupportedAsset(
	const TFunction<void(const TArray<UClass*>&, const TArray<FName>&)>& InActuallyCheckAssetClass)
{
	if (const auto RVNEditorSetting = GetMutableDefault<URVisualNarrativeEditorSetting>())
	{
		TArray<UClass*> SupportedClasses;

		TArray<FName> SupportedAssetClassNames;

		for (const auto& SupportedAssetClass : RVNEditorSetting->GetSupportedAssetClass())
		{
			SupportedClasses.Add(SupportedAssetClass.Get());

			SupportedAssetClassNames.Add(SupportedAssetClass->GetFName());
		}

		InActuallyCheckAssetClass(SupportedClasses, SupportedAssetClassNames);
	}
}

bool FRVNClassCollector::IsSupportedAssetData(const TArray<FName>& InSupportedAssetClassNames,
                                              const FAssetData& InAssetData)
{
	if (InSupportedAssetClassNames.Find(InAssetData.
#if UE_ASSET_DATA_GET_ASSET_CLASS_STRING
	                                    AssetClassPath.GetAssetName()
#else
										AssetClass
#endif
	) != INDEX_NONE)
	{
		return true;
	}

	return false;
}
