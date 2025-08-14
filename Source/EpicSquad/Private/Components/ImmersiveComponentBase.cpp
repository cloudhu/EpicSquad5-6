// CloudHu:604746493@qq.com All Rights Reserved


#include "Components/ImmersiveComponentBase.h"

// Sets default values for this component's properties
UImmersiveComponentBase::UImmersiveComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UImmersiveComponentBase::SetAxisForward(const float InAxisForward)
{
	AxisRight = InAxisForward;
	if (OnInputValueChangeAxisForward.IsBound())
	{
		OnInputValueChangeAxisForward.Broadcast(InAxisForward);
	}
}

void UImmersiveComponentBase::SetAxisRight(const float InAxisRight)
{
	AxisRight = InAxisRight;
	OnInputValueChangeAxisRight.Broadcast(InAxisRight);
}
