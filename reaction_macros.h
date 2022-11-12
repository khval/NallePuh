


#define MakeSlider(n,a,b) obj[n] = (Object*) SliderObject, \
	GA_ID, n,				\
	GA_RelVerify, TRUE,		\
	SLIDER_Min, a,			\
	SLIDER_Max, b,		\
	SLIDER_Ticks, b+1,		\
	SLIDER_Orientation,SLIDER_HORIZONTAL, \
	SliderEnd

#define MakeRect(n)  obj[n] = (Object*) ButtonObject, \
	GA_ID, n,				\
	GA_Text, _L(n),		\
	GA_RelVerify, TRUE,		\
	ButtonEnd

#define MakeButton(n)  obj[n] = (Object*) ButtonObject, \
	GA_ID, n,				\
	GA_Text, _L(n),		\
	GA_RelVerify, TRUE,		\
	ButtonEnd

#define MakeImageButton(n,nn)  obj[n] = (Object*) ButtonObject, \
	GA_ID, n,				\
	BUTTON_AutoButton, nn , \
	GA_RelVerify, TRUE,		\
	ButtonEnd
//	CHILD_Label, _L(n)


#define MakeCheck(n,v)	obj[n] = (Object*) CheckBoxObject, \
	GA_ID, n,				\
	GA_Selected, v,		\
	CheckBoxEnd

// LEFT location of checkbox text label
#define MakeCheckL(n, v) obj[n] = (Object*) CheckBoxObject, \
 GA_ID,              n,              \
 GA_Selected,        v,              \
 GA_Text,            _L(n),          \
 CHECKBOX_TextPlace, PLACETEXT_LEFT, \
 TAG_END)

// RIGHT location of checkbox text label
#define MakeCheckR(n, v) obj[n] = (Object*) CheckBoxObject, \
 GA_ID,       n,     \
 GA_Selected, v,     \
 GA_Text,     _L(n), \
 TAG_END)

#define MakeString(n) obj[n] =  (Object*) StringObject, \
	GA_ID, n,				\
	GA_Text, _L(n),		\
	GA_RelVerify, TRUE,		\
	StringEnd

#define MakeHexString(n) obj[n] =  (Object*) StringObject, \
	GA_ID, n,				\
	GA_Text, _L(n),		\
	GA_RelVerify, TRUE,		\
	STRINGA_HookType, SHK_HEXIDECIMAL, \
	StringEnd
// 	STRINGA_TextVal, _L(n)	

#define MakeLabel(n)  LabelObject, \
	GA_ID, n,				\
	LABEL_Text, _L(n),		\
	LabelEnd

#define MakeLabelEmpty()  LabelObject, \
	LABEL_Text, "",		\
	LabelEnd

#define MakePopFile(n,l,v)  obj[n] = (Object*) StringObject, \
	GA_ID, n, 				\
	GA_Text, _L(n),		\
	GA_RelVerify, TRUE,		\
	STRINGA_TextVal, v,	\
	StringEnd

#define MakeInteger(n,v)  obj[n] = (Object*) IntegerObject, \
	GA_ID, n,				\
	GA_Text, _L(n),		\
	GA_RelVerify, TRUE,		\
	INTEGER_Number, v,	\
	IntegerEnd

#define MakeInteger_min_max(n,nr,min,max)  obj[n] = (Object*) IntegerObject,	\
	GA_ID, n, 					\
	GA_TabCycle, TRUE,		\
	GA_RelVerify, TRUE, 		\
	INTEGER_Number, nr, 		\
	INTEGER_Minimum, min,		\
	INTEGER_Maximum, max,	\
	StringEnd

#define MakeCycle(n,v)  obj[n] = (Object*) ChooserObject, \
	GA_ID, n,				\
	GA_Text, _L(n),		\
	GA_RelVerify, TRUE,		\
	CHOOSER_LabelArray, v, \
	ChooserEnd 

#define MakeList(n,ci,list) StartMember, obj[n] =(Object*) ListBrowserObject, \
	GA_ID, n,							\
	GA_RelVerify, TRUE,					\
	LISTBROWSER_Labels, list,			\
	LISTBROWSER_ColumnInfo, ci,			\
	LISTBROWSER_ColumnTitles, TRUE,		\
	LISTBROWSER_ShowSelected, TRUE,	\
	EndMember,  

#define GFrame(txt) \
	LAYOUT_SpaceOuter, TRUE,		\
	LAYOUT_DeferLayout, TRUE,		\
	LAYOUT_BevelStyle, GroupFrame,	\
	LAYOUT_Label, txt

#define RSetAttrO( win_nr, index , opt, value ) \
	if ( win[win_nr])	 SetGadgetAttrs( (struct Gadget *) obj[index], \
	win[win_nr],  NULL,  opt, value, TAG_DONE); 

#define RSetAttr( win_nr, obj , opt, value ) \
	if ( win[win_nr])	 SetGadgetAttrs( (struct Gadget *) obj, \
	win[win_nr],  NULL,  opt, value, TAG_DONE); 

#define RDetach(win_nr, index) \
	if (win[win_nr]) SetGadgetAttrs( (struct Gadget *) obj[index], win[win_nr] , NULL, \
		LISTBROWSER_MultiSelect, FALSE, \
		LISTBROWSER_Labels, ~0, \
		TAG_DONE); 

#define RAttach(win_nr,index,list) \
	if (win[win_nr]) SetGadgetAttrs( (struct Gadget *) obj[index], win[win_nr], NULL, \
	LISTBROWSER_Labels, list, TAG_DONE); 

extern ULONG getv( Object *obj, ULONG arg ) ;
