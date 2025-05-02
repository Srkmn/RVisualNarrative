#pragma once

#include "CoreMinimal.h"
#include "Graph/Node/Slate/InternalWidget/SRVNDecoratorItemBase.h"
#include "Decorator/RVNDecorator.h"
#include "RVNTaskNode.h"

class FRVNDecoratorItemFactory
{
public:
	static TSharedRef<SRVNDecoratorItemBase> CreateDecoratorItemWidget(
		URVNTaskNode* OwnerNode,
		URVNDecorator* Decorator,
		int32 FirstItemIndex,
		bool IsSelected,
		FOnDecoratorSelected OnDecoratorSelected,
		FOnDecoratorDeleted OnDecoratorDeleted);

	template <typename DecoratorClass, typename SlateWidgetClass>
	static void RegisterDecoratorItemClass();

private:
	typedef TSharedRef<SRVNDecoratorItemBase> (*FCreateDecoratorItemFunc)(
		URVNTaskNode*, URVNDecorator*, int32, bool, FOnDecoratorSelected, FOnDecoratorDeleted);

	static TMap<UClass*, FCreateDecoratorItemFunc> DecoratorItemCreators;

	static TSharedRef<SRVNDecoratorItemBase> CreateDefaultDecoratorItem(
		URVNTaskNode* OwnerNode,
		URVNDecorator* Decorator,
		int32 FirstItemIndex,
		bool IsSelected,
		FOnDecoratorSelected OnDecoratorSelected,
		FOnDecoratorDeleted OnDecoratorDeleted);
};

template <typename DecoratorClass, typename SlateWidgetClass>
void FRVNDecoratorItemFactory::RegisterDecoratorItemClass()
{
	DecoratorItemCreators.Add(DecoratorClass::StaticClass(),
	                          [](URVNTaskNode* OwnerNode,
	                             URVNDecorator* Decorator,
	                             int32 FirstItemIndex,
	                             bool IsSelected,
	                             FOnDecoratorSelected OnDecoratorSelected,
	                             FOnDecoratorDeleted OnDecoratorDeleted) -> TSharedRef<SRVNDecoratorItemBase>
	                          {
		                          return SNew(SlateWidgetClass)
			                          .OwnerTaskNode(OwnerNode)
			                          .Decorator(Decorator)
			                          .FirstItemIndex(FirstItemIndex)
			                          .IsSelected(IsSelected)
			                          .OnDecoratorSelected(OnDecoratorSelected)
			                          .OnDecoratorDeleted(OnDecoratorDeleted);
	                          }
	);
}
