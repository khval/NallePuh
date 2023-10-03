
PAGE_Add,  VGroupObject,

	LAYOUT_AddChild, VGroupObject,

		LAYOUT_AddChild, HGroupObject, 
			GFrame("Port1: Joystick 2 / Mouse"),
			LAYOUT_AddChild, MakeButton(GAD_JOY1_BUTTON1),
			LAYOUT_AddChild, MakeButton(GAD_JOY1_BUTTON2),

		LayoutEnd,

		LAYOUT_AddChild, HGroupObject, 
			GFrame("Port2: Joystick 1"),
			LAYOUT_AddChild, MakeButton(GAD_JOY2_BUTTON1),
			LAYOUT_AddChild, MakeButton(GAD_JOY2_BUTTON2),

		LayoutEnd,

		LAYOUT_AddChild, MakeCycle(LIST_Frequency, frequency_names),
			CHILD_MinWidth, 200,
			CHILD_Label, MakeLabel(LIST_Frequency),

	LayoutEnd,
	CHILD_WeightedHeight, 0,

LayoutEnd,
