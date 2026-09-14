#pragma once

#include "RE/S/ScaleformGFxStateBag.h"
#include "RE/S/ScaleformPtr.h"

namespace RE::Scaleform::GFx
{
	class ASMovieRootBase;

	class Movie :
		public RefCountBase<Movie, 327>,  // 00
		public StateBag                   // 10
	{
	public:
		SF_RTTI_VTABLE(Scaleform__GFx__Movie);

		enum class SetVarType : std::uint32_t
		{
			kNormal,
			kSticky,
			kPermanent
		};

		enum class SetArrayType : std::uint32_t
		{
			kInt,
			kDouble,
			kFloat,
			kString,
			kStringW,
			kValue
		};

		// Scaleform stage scaling (ActionScript Stage.scaleMode).
		enum class ScaleModeType : std::int32_t
		{
			kNoScale = 0,   // Keep the authored stage size.
			kShowAll = 1,   // Fit the entire stage, preserving its aspect ratio.
			kExactFit = 2,  // Stretch to fill the viewport.
			kNoBorder = 3   // Fill the viewport, preserving aspect ratio and cropping as needed.
		};

		// Scaleform stage placement (ActionScript Stage.align).
		enum class AlignType : std::int32_t
		{
			kCenter = 0,
			kTopCenter = 1,
			kBottomCenter = 2,
			kCenterLeft = 3,
			kCenterRight = 4,
			kTopLeft = 5,
			kTopRight = 6,
			kBottomLeft = 7,
			kBottomRight = 8
		};

		virtual ~Movie() = default;  // 00

		// members
		Ptr<ASMovieRootBase> asMovieRoot;
	};

	class MovieImpl :
		public Movie  // 00
	{
	public:
		SF_RTTI_VTABLE(Scaleform__GFx__MovieImpl);

		virtual ~MovieImpl() = default;  // 00
	};
}
