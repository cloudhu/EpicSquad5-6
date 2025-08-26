// CloudHu:604746493@qq.com All Rights Reserved


#include "UI/CommonGame/Widget_KeyRemapButtonBase.h"

void UWidget_KeyRemapButtonBase::SetKeyIndex(const int32 Index)
{
	KeyIndex = Index;
}

void UWidget_KeyRemapButtonBase::NativeOnClicked()
{
	Super::NativeOnClicked();
	if (OnKeyRemapClicked.IsBound())
	{
		OnKeyRemapClicked.Broadcast(KeyIndex);
	}
}

void UWidget_KeyRemapButtonBase::NativeDestruct()
{
	Super::NativeDestruct();
	if (OnKeyRemapClicked.IsBound())
	{
		OnKeyRemapClicked.Clear();
	}
}
