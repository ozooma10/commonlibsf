#pragma once

namespace RE
{
	enum class USER_EVENT_SENDER_ID : std::uint32_t
	{
		None = 0,
		Gameplay = 1,
		Menu = 2,
		Script = 3
	};

	enum class USER_EVENT_FLAG : std::uint32_t
	{
		Walking = (1 << 0),
		Looking = (1 << 1),       // mouse-look / camera (runtime-proven 1.16.244, not CamSwitch)
		Activation = (1 << 2),    // activate / "use" (runtime-proven 1.16.244; user-side, distinct from OTHER::Activate)
		TabMenuMaybe = (1 << 3),  //Unconfirmed (live test inconclusive)
		Console = (1 << 4),       //Unconfirmed — clearing this did NOT gate the console on 1.16.244; name is a guess
		POVSwitch = (1 << 5),     // first/third-person view toggle (runtime-proven 1.16.244, not OTHER::CamSwitch)
		Fighting = (1 << 6),
		Sneaking = (1 << 7),
		Menu = (1 << 8),
		WheelZoom = (1 << 9),     // zoom (runtime-proven 1.16.244)
		Jumping = (1 << 10),
		Movement = Walking | Jumping,
	};
	REX_DEFINE_ENUM_CLASS_FLAGS(USER_EVENT_FLAG);

	enum class OTHER_EVENT_FLAG : std::uint32_t
	{
		Journal = (1 << 0),
		Activate = (1 << 1),
		FastTravel = (1 << 2),
		CamSwitch = (1 << 3),
		VATS = (1 << 4),
		Favorites = (1 << 5),
		PipboyLight = (1 << 6),  //Unconfirmed
		ZKey = (1 << 7),
		Running = (1 << 8),
		Unk = (1 << 9),  //??
		Sprinting = (1 << 10),
		HandScanner = (1 << 11),
		Takeoff = (1 << 12),
		Inventory = (1 << 13),
		GravJump = (1 << 14),
		FarTravel = (1 << 15),
		LocationDiscovery = (1 << 16),
		Others = HandScanner | Takeoff | Inventory | GravJump | FarTravel,
	};
	REX_DEFINE_ENUM_CLASS_FLAGS(OTHER_EVENT_FLAG);
}
