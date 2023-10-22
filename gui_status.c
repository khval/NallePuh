
PAGE_Add,  VGroupObject,

	LAYOUT_AddChild, VGroupObject,

		LAYOUT_AddChild, VGroupObject, 
			GFrame("CIAA"),
			LAYOUT_AddChild, MakeString(GAD_CIAA_CR),
				CHILD_Label, MakeLabel(GAD_CIAA_CR),
			LAYOUT_AddChild, MakeString(GAD_CIAA_TA),
				CHILD_Label, MakeLabel(GAD_CIAA_TA),
			LAYOUT_AddChild, MakeString(GAD_CIAA_TB),
				CHILD_Label, MakeLabel(GAD_CIAA_TB),
		LayoutEnd,

		LAYOUT_AddChild, VGroupObject, 
			GFrame("CIAB"),
			LAYOUT_AddChild, MakeString(GAD_CIAB_CR),
				CHILD_Label, MakeLabel(GAD_CIAB_CR),
			LAYOUT_AddChild, MakeString(GAD_CIAB_TA),
				CHILD_Label, MakeLabel(GAD_CIAB_TA),
			LAYOUT_AddChild, MakeString(GAD_CIAB_TB),
				CHILD_Label, MakeLabel(GAD_CIAB_TB),
		LayoutEnd,

	LayoutEnd,
	CHILD_WeightedHeight, 0,

LayoutEnd,
