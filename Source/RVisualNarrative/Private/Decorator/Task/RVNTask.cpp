#include "Decorator/Task/RVNTask.h"

#if	WITH_EDITOR
FString URVNTaskBase::GetNodeIconName() const
{
	return TEXT("BTEditor.Graph.NewTask");
}

URVNDecorator* URVNTaskBase::PasteDecorator()
{
	TMap<URVNTaskBase*, URVNTaskBase*> CloneMap;

	return Cast<URVNDecorator>(PasteDecorator_Internal(CloneMap));
}

URVNTaskBase* URVNTaskBase::PasteDecorator_Internal(TMap<URVNTaskBase*, URVNTaskBase*>& CloneMap)
{
	if (const auto ExistingClone = CloneMap.Find(this))
	{
		return *ExistingClone;
	}

	URVNTaskBase* Clone = DuplicateObject<URVNTaskBase>(this, GetOuter());

	CloneMap.Add(this, Clone);

	Clone->Conditions.Empty();
	Clone->Children.Empty();

	for (const auto& Condition : Conditions)
	{
		if (IsValid(Condition))
		{
			if (const auto ClonedCondition = Cast<URVNConditionBase>(Condition->PasteDecorator()))
			{
				Clone->AddCondition(ClonedCondition);
			}
		}
	}

	for (const auto& Child : Children)
	{
		if (IsValid(Child))
		{
			if (const auto ClonedChild = Child->PasteDecorator_Internal(CloneMap))
			{
				Clone->Children.Add(ClonedChild);
			}
		}
	}

	return Clone;
}

URVNTaskBase* URVNTaskBase::PasteTaskNoChildren()
{
	URVNTaskBase* Clone = DuplicateObject<URVNTaskBase>(this, GetOuter());

	Clone->Conditions.Empty();
	Clone->Children.Empty();

	for (const auto& Condition : Conditions)
	{
		if (IsValid(Condition))
		{
			URVNConditionBase* ClonedCondition = Cast<URVNConditionBase>(Condition->PasteDecorator());

			Clone->AddCondition(ClonedCondition);
		}
	}

	return Clone;
}
#endif

const TArray<TObjectPtr<URVNConditionBase>>& URVNTaskBase::GetConditions() const
{
	return Conditions;
}

TArray<TObjectPtr<URVNConditionBase>>& URVNTaskBase::GetConditions()
{
	return Conditions;
}

const TArray<TObjectPtr<URVNTaskBase>>& URVNTaskBase::GetChildren() const
{
	return Children;
}

TArray<TObjectPtr<URVNTaskBase>>& URVNTaskBase::GetChildren()
{
	return Children;
}

#if WITH_EDITOR
void URVNTaskBase::AddCondition(URVNConditionBase* Condition)
{
	if (IsValid(Condition))
	{
		SetFlags(RF_Transactional);
		Modify();

		Conditions.Add(Condition);
	}
}

void URVNTaskBase::RemoveCondition(URVNConditionBase* Condition)
{
	if (IsValid(Condition))
	{
		SetFlags(RF_Transactional);
		Modify();

		Conditions.Remove(Condition);
	}
}

void URVNTaskBase::SetChildren(const TArray<TObjectPtr<URVNTaskBase>>& NewChildren)
{
	Children = NewChildren;
}
#endif
