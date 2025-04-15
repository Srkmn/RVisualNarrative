#include "RVNBlackboardAssetProvider.h"

#if WITH_EDITOR
IRVNBlackboardAssetProvider::FBlackboardOwnerChanged IRVNBlackboardAssetProvider::OnBlackboardOwnerChanged;
#endif

URVNBlackboardAssetProvider::URVNBlackboardAssetProvider(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}
