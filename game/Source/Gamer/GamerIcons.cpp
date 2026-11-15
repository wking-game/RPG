#include "GamerIcons.h"

#include "Engine/Texture2D.h"
#include "Misc/PackageName.h"
#include "UObject/StrongObjectPtr.h"

namespace GamerIcons
{
	UTexture2D* Find(FName Id)
	{
		static TMap<FName, TStrongObjectPtr<UTexture2D>> Cache;
		if (Id.IsNone())
		{
			return nullptr;
		}
		if (const TStrongObjectPtr<UTexture2D>* Found = Cache.Find(Id))
		{
			return Found->Get();
		}
		const FString PackagePath = FString::Printf(TEXT("/Game/Gamer/UI/Icons/T_Icon_%s"), *Id.ToString());
		UTexture2D* Texture = nullptr;
		if (FPackageName::DoesPackageExist(PackagePath))
		{
			Texture = LoadObject<UTexture2D>(nullptr, *FString::Printf(TEXT("%s.T_Icon_%s"), *PackagePath, *Id.ToString()));
		}
		Cache.Add(Id, TStrongObjectPtr<UTexture2D>(Texture));
		return Texture;
	}
}
