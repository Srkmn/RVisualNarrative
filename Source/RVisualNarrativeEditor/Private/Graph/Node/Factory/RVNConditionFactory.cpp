#include "Graph/Node/Factory/RVNConditionFactory.h"

#include "Decorator/Condition/RVNCondition.h"
#include "Graph/Node/Slate/SRVNNodeBlueprintCreateDialog.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Blueprint/RVNConditionNodeBlueprint.h"

#define LOCTEXT_NAMESPACE "RVNConditionFactory"

URVNConditionFactory::URVNConditionFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = URVNConditionNodeBlueprint::StaticClass();
	ParentClass = URVNConditionBase::StaticClass();
}

bool URVNConditionFactory::ConfigureProperties()
{
	TSharedRef<SRVNBlueprintCreateDialog> Dialog =
		SNew(SRVNBlueprintCreateDialog)
		.BaseClass(URVNConditionBase::StaticClass())
		.Title(LOCTEXT("CreateConditionBlueprintOptions", "Create Condition Blueprint"));

	return Dialog->ConfigureProperties(this);
}

UObject* URVNConditionFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
                                                UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	if (!Class->IsChildOf(URVNConditionNodeBlueprint::StaticClass()))
	{
		return nullptr;
	}

	if (BlueprintType == BPTYPE_Interface)
	{
		ParentClass = UInterface::StaticClass();
	}

	if ((ParentClass == nullptr) || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass) || !ParentClass->
		IsChildOf(URVNConditionBase::StaticClass()))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ClassName"), (ParentClass != nullptr)
			                            ? FText::FromString(ParentClass->GetName())
			                            : LOCTEXT("Null", "(null)"));
		FMessageDialog::Open(EAppMsgType::Ok,
		                     FText::Format(LOCTEXT("CannotCreateGameplayAbilityBlueprint",
		                                           "Cannot create a Gameplay Ability Blueprint based on the class '{ClassName}'."), Args));
		return nullptr;
	}
	else
	{
		URVNConditionNodeBlueprint* NewBP = CastChecked<URVNConditionNodeBlueprint>(
			FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, Name, BlueprintType,
			                                        URVNConditionNodeBlueprint::StaticClass(),
			                                        UBlueprintGeneratedClass::StaticClass(), CallingContext));

		if (NewBP)
		{
			URVNConditionNodeBlueprint* AbilityBP = URVNConditionNodeBlueprint::FindRootRVNNodeBlueprint(NewBP);
			if (AbilityBP == nullptr)
			{
				const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

				UEdGraph* NewGraph = FBlueprintEditorUtils::CreateNewGraph(
					NewBP, TEXT("Gameplay Ability Graph"), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
#if WITH_EDITORONLY_DATA
				if (NewBP->UbergraphPages.Num())
				{
					FBlueprintEditorUtils::RemoveGraphs(NewBP, NewBP->UbergraphPages);
				}
#endif
				FBlueprintEditorUtils::AddUbergraphPage(NewBP, NewGraph);
				NewBP->LastEditedDocuments.Add(NewGraph);
				NewGraph->bAllowDeletion = false;
			}
		}

		return NewBP;
	}
}

UObject* URVNConditionFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
                                                UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}

#undef LOCTEXT_NAMESPACE
