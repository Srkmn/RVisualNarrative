#include "Graph/Node/RVNDecoratorItemFactory.h"
#include "Graph/Node/Slate/InternalWidget/SRVNDecoratorItemBase.h"

TMap<UClass*, FRVNDecoratorItemFactory::FCreateDecoratorItemFunc> FRVNDecoratorItemFactory::DecoratorItemCreators;

TSharedRef<SRVNDecoratorItemBase> FRVNDecoratorItemFactory::CreateDecoratorItemWidget(
	URVNTaskNode* OwnerNode,
	URVNDecorator* Decorator,
	int32 FirstItemIndex,
	bool IsSelected,
	FOnDecoratorSelected OnDecoratorSelected,
	FOnDecoratorDeleted OnDecoratorDeleted)
{
	if (!Decorator)
	{
		return CreateDefaultDecoratorItem(OwnerNode, Decorator, FirstItemIndex, IsSelected, OnDecoratorSelected,
		                                  OnDecoratorDeleted);
	}

	const UClass* DecoratorClass = Decorator->GetClass();
	while (DecoratorClass)
	{
		if (const FCreateDecoratorItemFunc* CreatorFunc = DecoratorItemCreators.Find(DecoratorClass))
		{
			return (*CreatorFunc)(OwnerNode, Decorator, FirstItemIndex, IsSelected, OnDecoratorSelected,
			                      OnDecoratorDeleted);
		}
		DecoratorClass = DecoratorClass->GetSuperClass();
	}

	return CreateDefaultDecoratorItem(OwnerNode, Decorator, FirstItemIndex, IsSelected, OnDecoratorSelected,
	                                  OnDecoratorDeleted);
}

TSharedRef<SRVNDecoratorItemBase> FRVNDecoratorItemFactory::CreateDefaultDecoratorItem(
	URVNTaskNode* OwnerNode,
	URVNDecorator* Decorator,
	int32 FirstItemIndex,
	bool IsSelected,
	FOnDecoratorSelected OnDecoratorSelected,
	FOnDecoratorDeleted OnDecoratorDeleted)
{
	return SNew(SRVNDecoratorItemBase)
		.OwnerTaskNode(OwnerNode)
		.Decorator(Decorator)
		.FirstItemIndex(FirstItemIndex)
		.IsSelected(IsSelected)
		.OnDecoratorSelected(OnDecoratorSelected)
		.OnDecoratorDeleted(OnDecoratorDeleted);
}
