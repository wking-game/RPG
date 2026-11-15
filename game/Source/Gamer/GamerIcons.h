#pragma once

#include "CoreMinimal.h"

class UTexture2D;

/**
 * Icons of items and buildable pieces (decision 2026-11-14: rendered from the real 3D models by
 * tools/blender/render_icons.py, imported to /Game/Gamer/UI/Icons/T_Icon_<id>). Things without a model have no
 * icon: Find returns nullptr and the HUD draws a named placeholder tile. Looked up once per id and kept.
 */
namespace GamerIcons
{
	GAMER_API UTexture2D* Find(FName Id);
}
