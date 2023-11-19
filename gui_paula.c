
PAGE_Add,  VGroupObject,

	LAYOUT_AddChild, VGroupObject,
		GFrame(_L(frame_audio)),

		LAYOUT_AddChild, MakeString(GAD_MODE_ID),
		CHILD_Label, MakeLabel(GAD_MODE_ID),

		LAYOUT_AddChild, MakeString(GAD_MODE_INFO),
		CHILD_Label, MakeLabel(GAD_MODE_INFO),

		LAYOUT_AddChild, MakeButton(GAD_SELECT_MODE),

	LayoutEnd,
	CHILD_WeightedHeight, 0,

	LAYOUT_AddChild, VGroupObject,
		GFrame(_L(frame_blitter)),

		LAYOUT_AddChild, MakeCycle(LIST_BLITTER, blitter_names),
			CHILD_MinWidth, 200,
			CHILD_Label, MakeLabel(LIST_BLITTER),

	LayoutEnd,
	CHILD_WeightedHeight, 0,

LayoutEnd,
