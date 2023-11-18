#ifndef UI_WIDGETS_H
#define UI_WIDGETS_H

#define UI_ScrollableContainer(s) UI_PushScrollableContainer(s); UI_DeferLoop((void)0, UI_PopScrollableContainer()) 

#define UI_Row() UI_DeferLoop(UI_BeginRow(), UI_EndRow())
#define UI_Column() UI_DeferLoop(UI_BeginColumn(), UI_EndColumn())

#define UI_NamedRow(s) UI_BeginNamedRow(s); UI_DeferLoop((void)0, UI_EndNamedRow())
#define UI_NamedColumn(s) UI_BeginNamedColumn(s); UI_DeferLoop((void)0, UI_EndNamedColumn())

#define UI_Tree(str) if (UI_BeginTree(str)) \
for(int _i_ = 0; !_i_; _i_ += 1, UI_EndTree())

#define UI_TreeF(str, ...) if (UI_BeginTreeF(str, __VA_ARGS__)) \
for(int _i_ = 0; !_i_; _i_ += 1, UI_EndTree())

#define UI_Popup(b) if (UI_BeginPopup(b)) \
for(int _i_ = 0; !_i_; _i_ += 1, UI_EndPopup())

#endif