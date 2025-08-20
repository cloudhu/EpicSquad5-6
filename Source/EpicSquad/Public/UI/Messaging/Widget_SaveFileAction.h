// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UI/CommonGame/Widget_ActivatableBase.h"
#include "Widget_SaveFileAction.generated.h"

class ULyraButtonBase;
class UCommonRichTextBlock;
class UDynamicEntryBox;
class UCommonTextBlock;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class EPICSQUAD_API UWidget_SaveFileAction : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	/** The header of the message to display */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Header;

	/** The body of the message to display */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Body;

	UFUNCTION(BlueprintNativeEvent)
	void CloseConfirmationWindow(int Result);

protected:
	virtual void NativeOnInitialized() override;

private:
	void SetTitle(const FText& InTitle) const;

	void SetDescription(const FText& InDescription) const;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Text_Title;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCommonRichTextBlock> RichText_Description;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<ULyraButtonBase> Button_Cancel;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<ULyraButtonBase> Button_Continue;

	UPROPERTY(EditDefaultsOnly, meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle CancelAction;
};
