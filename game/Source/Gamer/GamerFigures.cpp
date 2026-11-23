#include "GamerFigures.h"

#include "Gamer.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Dom/JsonObject.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/Actor.h"
#include "GroomAsset.h"
#include "GroomBindingAsset.h"
#include "GroomComponent.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace GamerFigures
{
	namespace
	{
		struct FEntry
		{
			FString BodyPath;
			FString FacePath;
			TArray<TPair<FString, FString>> GroomPaths; // groom, binding
			TArray<TArray<FString>> GroomMaterialPaths;  // per groom: material overrides
			FFigure Loaded;
			bool bTried = false;
		};

		/** Binding asset name -> kind: Hair_M_TwistedBraids / Beard_L_Full_Binding / Eyebrows_... */
		FName GroomKind(const FString& BindingPath)
		{
			const FString Name = FPaths::GetBaseFilename(BindingPath);
			if (Name.StartsWith(TEXT("Eyebrows")))
			{
				return TEXT("Eyebrows");
			}
			if (Name.StartsWith(TEXT("Beard")) || Name.StartsWith(TEXT("Goatee")) || Name.StartsWith(TEXT("Mustache")))
			{
				return TEXT("Beard");
			}
			return Name.StartsWith(TEXT("Hair")) ? FName(TEXT("Hair")) : NAME_None;
		}

		TArray<FString> ReadStrings(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Field)
		{
			TArray<FString> Out;
			Obj->TryGetStringArrayField(Field, Out);
			return Out;
		}

		struct FKit
		{
			TArray<FString> MaterialPaths;
			FString GroomPath;
			FString BindingPath;
			FString FacePath; // stubble: baked into this kit's face textures (no groom in the Medium build)
			USkeletalMesh* Face = nullptr;
			FGroom Loaded;
			bool bTried = false;
		};

		/** "strong|hair_long" -> kit (figures.json "hair_kits"). Filled together with Entries(). */
		TMap<FName, FKit>& Kits()
		{
			static TMap<FName, FKit> Map;
			return Map;
		}

		TMap<FName, FEntry>& Entries()
		{
			static TMap<FName, FEntry> Map;
			static bool bLoaded = false;
			if (!bLoaded)
			{
				bLoaded = true;
				FString Text;
				const FString File = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Data"), TEXT("figures.json"));
				TSharedPtr<FJsonObject> Root;
				if (FFileHelper::LoadFileToString(Text, *File)
					&& FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Root) && Root.IsValid())
				{
					const TSharedPtr<FJsonObject>* Figures = nullptr;
					if (Root->TryGetObjectField(TEXT("figures"), Figures))
					{
						for (const auto& Pair : (*Figures)->Values)
						{
							const TSharedPtr<FJsonObject>* Obj = nullptr;
							if (Pair.Value->TryGetObject(Obj))
							{
								FEntry& Entry = Map.Add(FName(*Pair.Key));
								(*Obj)->TryGetStringField(TEXT("body"), Entry.BodyPath);
								(*Obj)->TryGetStringField(TEXT("face"), Entry.FacePath);
								const TArray<TSharedPtr<FJsonValue>>* Grooms = nullptr;
								if ((*Obj)->TryGetArrayField(TEXT("grooms"), Grooms))
								{
									for (const TSharedPtr<FJsonValue>& Value : *Grooms)
									{
										const TSharedPtr<FJsonObject>* G = nullptr;
										FString Groom, Binding;
										if (Value->TryGetObject(G) && (*G)->TryGetStringField(TEXT("groom"), Groom))
										{
											(*G)->TryGetStringField(TEXT("binding"), Binding);
											Entry.GroomPaths.Emplace(Groom, Binding);
											Entry.GroomMaterialPaths.Add(ReadStrings(*G, TEXT("materials")));
										}
									}
								}
							}
						}
					}
					const TSharedPtr<FJsonObject>* HairKits = nullptr;
					if (Root->TryGetObjectField(TEXT("hair_kits"), HairKits))
					{
						for (const auto& Body : (*HairKits)->Values)
						{
							const TSharedPtr<FJsonObject>* Options = nullptr;
							if (!Body.Value->TryGetObject(Options))
							{
								continue;
							}
							for (const auto& Option : (*Options)->Values)
							{
								const TSharedPtr<FJsonObject>* G = nullptr;
								FKit Kit;
								if (Option.Value->TryGetObject(G))
								{
									(*G)->TryGetStringField(TEXT("groom"), Kit.GroomPath);
									(*G)->TryGetStringField(TEXT("binding"), Kit.BindingPath);
									(*G)->TryGetStringField(TEXT("face"), Kit.FacePath);
									Kit.MaterialPaths = ReadStrings(*G, TEXT("materials"));
									Kits().Add(FName(*FString::Printf(TEXT("%s|%s"), *Body.Key, *Option.Key)), Kit);
								}
							}
						}
					}
					UE_LOG(LogGamer, Log, TEXT("Figures: %d MetaHuman figure(s), %d hair kit(s) in %s"), Map.Num(), Kits().Num(), *File);
				}
			}
			return Map;
		}

		template <typename T>
		T* LoadKept(const FString& Path)
		{
			// Kept alive for the whole session (a handful of assets; avoids reloading after garbage collection).
			T* Object = Path.IsEmpty() ? nullptr : LoadObject<T>(nullptr, *Path);
			if (Object)
			{
				Object->AddToRoot();
			}
			return Object;
		}
	}

	bool Find(FName Key, FFigure& Out)
	{
		FEntry* Entry = Entries().Find(Key);
		if (!Entry || Entry->BodyPath.IsEmpty())
		{
			return false;
		}
		if (!Entry->bTried)
		{
			Entry->bTried = true;
			Entry->Loaded.Body = LoadKept<USkeletalMesh>(Entry->BodyPath);
			Entry->Loaded.Face = LoadKept<USkeletalMesh>(Entry->FacePath);
			for (int32 g = 0; g < Entry->GroomPaths.Num(); ++g)
			{
				const TPair<FString, FString>& Paths = Entry->GroomPaths[g];
				FGroom Groom;
				Groom.Groom = LoadKept<UGroomAsset>(Paths.Key);
				Groom.Binding = LoadKept<UGroomBindingAsset>(Paths.Value);
				Groom.Kind = GroomKind(Paths.Value);
				for (const FString& MaterialPath : Entry->GroomMaterialPaths[g])
				{
					Groom.Materials.Add(LoadKept<UMaterialInterface>(MaterialPath));
				}
				if (Groom.Groom)
				{
					Entry->Loaded.Grooms.Add(Groom);
				}
			}
			UE_LOG(LogGamer, Log, TEXT("Figure %s: body %s, face %s, grooms %d/%d"), *Key.ToString(),
				Entry->Loaded.Body ? TEXT("ok") : TEXT("MISSING"), Entry->Loaded.Face ? TEXT("ok") : TEXT("-"),
				Entry->Loaded.Grooms.Num(), Entry->GroomPaths.Num());
		}
		Out = Entry->Loaded;
		const FString KeyText = Key.ToString();
		Out.GroomLOD = (KeyText.EndsWith(TEXT("_low")) || KeyText.StartsWith(TEXT("villager_")) || KeyText.StartsWith(TEXT("npc_"))) ? 5 : -1;
		// *_mid (Medium assembly): LODSync of its blueprint maps the grooms to 3 near, 5/7 far; LOD 0-2 render nothing
		// in that build. LOD 3 cards simulate after tools/unreal/metahuman_hairsim.py (assembly step).
		Out.bSimulateHair = Out.GroomLOD < 0;
		if (KeyText.EndsWith(TEXT("_mid")))
		{
			Out.GroomLOD = 3;
		}
		return Out.Body != nullptr;
	}

	bool FindVariant(const FString& Prefix, uint32 Hash, FFigure& Out)
	{
		TArray<FName> Keys;
		for (const auto& Pair : Entries())
		{
			if (Pair.Key.ToString().StartsWith(Prefix))
			{
				Keys.Add(Pair.Key);
			}
		}
		Keys.Sort(FNameLexicalLess());
		// Try from the hashed pick onwards, so a broken entry falls back to the next one.
		for (int32 i = 0; i < Keys.Num(); ++i)
		{
			if (Find(Keys[(Hash + i) % Keys.Num()], Out))
			{
				UE_LOG(LogGamer, Verbose, TEXT("Figure variant %s for hash %u"), *Keys[(Hash + i) % Keys.Num()].ToString(), Hash);
				return true;
			}
		}
		return false;
	}

	FLinearColor ClothPalette(int32 Index)
	{
		static const FColor Colors[] = {
			FColor(0x56, 0x65, 0x4F), FColor(0x4E, 0x64, 0x75), FColor(0x71, 0x69, 0x57), FColor(0xA9, 0x84, 0x4A),
			FColor(0x90, 0x4C, 0x3E), FColor(0x8A, 0x8C, 0x86), FColor(0xA9, 0x9B, 0x80)};
		return FLinearColor(Colors[((Index % 7) + 7) % 7]);
	}

	void ApplyClothing(USkeletalMeshComponent* Body, TObjectPtr<USkeletalMeshComponent>& InOutCloth, bool bShow,
		bool bSlimBody, const TCHAR* Outfit, const FLinearColor& Main, const FLinearColor& Accent)
	{
		AActor* Owner = Body ? Body->GetOwner() : nullptr;
		if (!Owner)
		{
			return;
		}
		USkeletalMesh* Mesh = nullptr;
		if (bShow && Outfit && *Outfit)
		{
			const TCHAR* BodyName = bSlimBody ? TEXT("Schmal") : TEXT("Kraeftig");
			const FString Path = FString::Printf(TEXT("/Game/Gamer/Characters/Clothing/%s/SKM_Cloth_%s_%s.SKM_Cloth_%s_%s"),
				BodyName, BodyName, Outfit, BodyName, Outfit);
			static TMap<FString, TWeakObjectPtr<USkeletalMesh>> Cache;
			TWeakObjectPtr<USkeletalMesh>& Cached = Cache.FindOrAdd(Path);
			if (!Cached.IsValid())
			{
				if (USkeletalMesh* Loaded = LoadObject<USkeletalMesh>(nullptr, *Path))
				{
					Loaded->AddToRoot(); // four small assets, kept for the session
					Cached = Loaded;
				}
				UE_LOG(LogGamer, Log, TEXT("Clothing %s: %s"), *Path, Cached.IsValid() ? TEXT("ok") : TEXT("MISSING"));
			}
			Mesh = Cached.Get();
		}
		if (!InOutCloth)
		{
			if (!Mesh)
			{
				return;
			}
			InOutCloth = NewObject<USkeletalMeshComponent>(Owner);
			InOutCloth->SetupAttachment(Body);
			InOutCloth->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			InOutCloth->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
			InOutCloth->RegisterComponent();
		}
		if (Mesh && InOutCloth->GetSkeletalMeshAsset() != Mesh)
		{
			InOutCloth->SetSkeletalMesh(Mesh);
		}
		InOutCloth->SetLeaderPoseComponent(Body);
		InOutCloth->SetVisibility(Mesh != nullptr);
		if (!Mesh)
		{
			return;
		}
		// Colours per slot (slot names from tools/blender/build_clothing.py): Wool/Cloak = main, Trim/Apron = accent.
		const TArray<FName> Slots = InOutCloth->GetMaterialSlotNames();
		for (int32 i = 0; i < Slots.Num(); ++i)
		{
			const FString Slot = Slots[i].ToString();
			const bool bWool = Slot.StartsWith(TEXT("Wool")) || Slot.StartsWith(TEXT("Cloak"));
			const bool bTrim = Slot.StartsWith(TEXT("Trim")) || Slot.StartsWith(TEXT("Apron"));
			if (!bWool && !bTrim)
			{
				continue;
			}
			UMaterialInstanceDynamic* Material = Cast<UMaterialInstanceDynamic>(InOutCloth->GetMaterial(i));
			if (!Material)
			{
				Material = InOutCloth->CreateAndSetMaterialInstanceDynamic(i);
			}
			if (Material)
			{
				Material->SetVectorParameterValue(TEXT("Tint"), bWool ? Main : Accent);
			}
		}
	}

	namespace
	{
		/** A light sea breeze until the weather system sets real wind (SetHairWind). */
		FVector DefaultBreeze()
		{
			return FVector(120.f, 40.f, 0.f);
		}
	}

	void SetHairWind(const TArray<TObjectPtr<USceneComponent>>& Components, const FVector& AirVelocity, float AirDrag)
	{
		for (USceneComponent* Component : Components)
		{
			if (UGroomComponent* Groom = Cast<UGroomComponent>(Component))
			{
				Groom->SimulationSettings.ExternalForces.AirVelocity = AirVelocity;
				Groom->SimulationSettings.ExternalForces.AirDrag = AirDrag;
			}
		}
	}

	void SetHairColour(const TArray<TObjectPtr<USceneComponent>>& Components, FName ColourOption)
	{
		struct FHairColour { const TCHAR* Option; float Melanin; float Redness; float White; };
		// Values like tools/unreal/metahuman_looks.py (melanin 0 blond .. 1 black, redness, white = grey share).
		static const FHairColour Colours[] = {
			{ TEXT("hair_color_blond"), 0.33f, 0.35f, 0.f },
			{ TEXT("hair_color_red"),   0.50f, 1.00f, 0.f },
			{ TEXT("hair_color_brown"), 0.55f, 0.35f, 0.f },
			{ TEXT("hair_color_black"), 0.92f, 0.10f, 0.f },
			{ TEXT("hair_color_grey"),  0.60f, 0.05f, 0.60f },
			{ TEXT("hair_color_ash"),   0.45f, 0.05f, 0.30f },
		};
		const FHairColour* Colour = nullptr;
		for (const FHairColour& C : Colours)
		{
			Colour = ColourOption == C.Option ? &C : Colour;
		}
		if (!Colour)
		{
			return;
		}
		for (USceneComponent* Component : Components)
		{
			UGroomComponent* Groom = Cast<UGroomComponent>(Component);
			for (int32 i = 0; Groom && i < Groom->GetNumMaterials(); ++i)
			{
				UMaterialInstanceDynamic* Material = Cast<UMaterialInstanceDynamic>(Groom->GetMaterial(i));
				if (!Material)
				{
					Material = Groom->CreateDynamicMaterialInstance(i);
				}
				if (Material)
				{
					float Before = -1.f;
					const bool bHas = Material->GetScalarParameterValue(FMaterialParameterInfo(TEXT("hairMelanin")), Before);
					UE_LOG(LogGamer, Verbose, TEXT("Hair colour %s on %s slot %d (%s): hairMelanin %s %.2f"), *ColourOption.ToString(),
						*GetNameSafe(Groom->GroomAsset), i, *GetNameSafe(Material->Parent), bHas ? TEXT("found") : TEXT("MISSING"), Before);
					Material->SetScalarParameterValue(TEXT("hairMelanin"), Colour->Melanin);
					Material->SetScalarParameterValue(TEXT("hairRedness"), Colour->Redness);
					Material->SetScalarParameterValue(TEXT("WhiteAmount"), Colour->White);

				}
			}
		}
	}

	void ApplyHairChoice(FFigure& InOut, bool bSlimBody, FName HairOption, FName BeardOption)
	{
		Entries(); // reads figures.json (and the kits) once
		auto Choose = [&InOut, bSlimBody](FName Kind, FName Option)
		{
			if (Option.IsNone())
			{
				return; // nothing chosen (old profile): keep the figure's own groom
			}
			FKit* Kit = Kits().Find(FName(*FString::Printf(TEXT("%s|%s"), bSlimBody ? TEXT("slim") : TEXT("strong"), *Option.ToString())));
			const bool bRemove = Option.ToString().EndsWith(TEXT("_none"));
			if (!Kit && !bRemove)
			{
				return; // no kit assembled for this option yet
			}
			InOut.Grooms.RemoveAll([Kind](const FGroom& G) { return G.Kind == Kind; });
			if (Kit)
			{
				if (!Kit->bTried)
				{
					Kit->bTried = true;
					Kit->Loaded.Groom = LoadKept<UGroomAsset>(Kit->GroomPath);
					Kit->Loaded.Binding = LoadKept<UGroomBindingAsset>(Kit->BindingPath);
					Kit->Loaded.Kind = Kind;
					Kit->Face = LoadKept<USkeletalMesh>(Kit->FacePath);
					for (const FString& MaterialPath : Kit->MaterialPaths)
					{
						Kit->Loaded.Materials.Add(LoadKept<UMaterialInterface>(MaterialPath));
					}
					UE_LOG(LogGamer, Log, TEXT("Hair kit %s: groom %s, face %s"), *Option.ToString(),
						Kit->Loaded.Groom ? *Kit->GroomPath : TEXT("-"), Kit->Face ? *Kit->FacePath : TEXT("-"));
				}
				if (Kit->Loaded.Groom)
				{
					InOut.Grooms.Add(Kit->Loaded);
				}
				if (Kit->Face)
				{
					InOut.Face = Kit->Face;
				}
			}
		};
		Choose(TEXT("Hair"), HairOption);
		Choose(TEXT("Beard"), BeardOption);
	}

	void ApplyGrooms(USkeletalMeshComponent* FaceMesh, TArray<TObjectPtr<USceneComponent>>& InOutComponents, const FFigure& Figure)
	{
		AActor* Owner = FaceMesh ? FaceMesh->GetOwner() : nullptr;
		if (!Owner)
		{
			return;
		}
		USkeletalMesh* Face = FaceMesh->GetSkeletalMeshAsset();
		// A groom binding belongs to one face mesh. When the face changes (e.g. a villager's name, and so its
		// figure, replicates late), rebinding a live component left some figures bald: start with fresh ones.
		const FName FaceTag = Face ? Face->GetFName() : NAME_None;
		bool bFaceChanged = InOutComponents.Num() > 0 && InOutComponents[0] && !InOutComponents[0]->ComponentTags.Contains(FaceTag);
		// Another hair or beard (character menu): also fresh components, so the simulation is set up for the new groom.
		for (int32 i = 0; i < InOutComponents.Num() && !bFaceChanged; ++i)
		{
			const UGroomComponent* Existing = Cast<UGroomComponent>(InOutComponents[i]);
			bFaceChanged = Existing && Figure.Grooms.IsValidIndex(i) && Existing->GroomAsset != Figure.Grooms[i].Groom;
		}
		if (bFaceChanged)
		{
			for (USceneComponent* Old : InOutComponents)
			{
				if (Old)
				{
					Old->DestroyComponent();
				}
			}
			InOutComponents.Reset();
		}
		for (int32 i = 0; i < FMath::Max(InOutComponents.Num(), Figure.Grooms.Num()); ++i)
		{
			const bool bWanted = Face && Figure.Grooms.IsValidIndex(i);
			if (!InOutComponents.IsValidIndex(i))
			{
				if (!bWanted)
				{
					break;
				}
				UGroomComponent* New = NewObject<UGroomComponent>(Owner);
				// Like the assembled MetaHuman blueprint (BP_<Name>): attached to the face mesh itself (no socket, else
				// the hair takes over the facial root bone's rotation and turns away); AttachmentName is the groom's
				// own binding bone. Asset and binding are set before registering, so the groom is built once.
				New->AttachmentName = TEXT("FACIAL_C_FacialRoot");
				New->SetupAttachment(FaceMesh);
				// Hair movement (gravity, running, head turns, wind) via the guide simulation; only where the groom
				// renders cards/strands (Mid/High assemblies). The helmet LODs of Low figures are static anyway.
				const bool bSimulate = Figure.bSimulateHair;
				New->SimulationSettings.bOverrideSettings = bSimulate;
				New->SimulationSettings.SolverSettings.bEnableSimulation = bSimulate;
				New->SimulationSettings.ExternalForces.AirVelocity = DefaultBreeze();
				New->SimulationSettings.ExternalForces.AirDrag = 0.1f;
				New->SetGroomAsset(Figure.Grooms[i].Groom, Figure.Grooms[i].Binding, /*bUpdateSimulation*/ bSimulate);
				New->ComponentTags.Add(FaceTag);
				// The assembled blueprint's materials (own card atlas, colour), not the asset's generic MI_Hair*.
				for (int32 m = 0; m < Figure.Grooms[i].Materials.Num(); ++m)
				{
					if (Figure.Grooms[i].Materials[m])
					{
						New->SetMaterial(m, Figure.Grooms[i].Materials[m]);
					}
				}
				New->RegisterComponent();
				InOutComponents.Add(New);
				// Check that the asset itself allows simulation at the LOD we force (cards LODs may not).
				for (int32 Group = 0; Group < Figure.Grooms[i].Groom->GetNumHairGroups(); ++Group)
				{
					UE_LOG(LogGamer, Log, TEXT("Hair %s group %d: wanted %d, asset at LOD %d: %d, component: %d"),
						*GetNameSafe(Figure.Grooms[i].Groom), Group, bSimulate ? 1 : 0, Figure.GroomLOD,
						Figure.Grooms[i].Groom->IsSimulationEnable(Group, Figure.GroomLOD) ? 1 : 0,
						New->IsSimulationEnable(Group, Figure.GroomLOD) ? 1 : 0);
				}
			}
			UGroomComponent* Groom = Cast<UGroomComponent>(InOutComponents[i]);
			if (!Groom)
			{
				continue;
			}
			if (bWanted && Groom->GroomAsset != Figure.Grooms[i].Groom)
			{
				Groom->SetGroomAsset(Figure.Grooms[i].Groom, Figure.Grooms[i].Binding, /*bUpdateSimulation*/ false);
			}
			// Like the blueprint's LODSync: Low assemblies only have visible hair in the helmet LODs.
			Groom->SetForcedLOD(Figure.GroomLOD);
			Groom->SetVisibility(bWanted);
		}
	}
}
