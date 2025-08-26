// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/LyraButtonBase.h"
#include "Widget_KeyRemapButtonBase.generated.h"

/**
 * Button for key remap
 */
UCLASS()
class EPICSQUAD_API UWidget_KeyRemapButtonBase : public ULyraButtonBase
{
	GENERATED_BODY()

public:
	DECLARE_EVENT_OneParam(UWidget_KeyRemapButtonBase, FOnKeyRemapClicked, int32);

	FOnKeyRemapClicked OnKeyRemapClicked;

	void SetKeyIndex(int32 Index);

protected:

	virtual void NativeOnClicked() override;
	
	virtual void NativeDestruct() override;

private:
	int32 KeyIndex = 0;
};
