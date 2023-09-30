
PAGE_Add,  VGroupObject,

	LAYOUT_AddChild, HGroupObject,

		LAYOUT_AddChild, MakeCycle(LIST_Frequency, frequency_names),
			CHILD_MinWidth, 200,
			CHILD_Label, MakeLabel(LIST_Frequency),

	LayoutEnd,
	CHILD_WeightedHeight, 0,

LayoutEnd,
