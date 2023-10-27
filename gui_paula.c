
PAGE_Add,  VGroupObject,

	LAYOUT_AddChild, VGroupObject,
		GFrame("Audio options"),

		LAYOUT_AddChild, MakeString(GAD_MODE_ID),
		CHILD_Label, MakeLabel(GAD_MODE_ID),

		LAYOUT_AddChild, MakeString(GAD_MODE_INFO),
		CHILD_Label, MakeLabel(GAD_MODE_INFO),

		LAYOUT_AddChild, MakeButton(GAD_SELECT_MODE),

	LayoutEnd,
	CHILD_WeightedHeight, 0,

LayoutEnd,