// CloudHu:604746493@qq.com All Rights Reserved


#include "UI/Messaging/Widget_SaveFileAction.h"
#include "CommonRichTextBlock.h"
#include "CommonTextBlock.h"
#include "ICommonInputModule.h"
#include "UI/Foundation/LyraButtonBase.h"

#define LOCTEXT_NAMESPACE "Messaging"

void UWidget_SaveFileAction::SetTitle(const FText& InTitle) const
{
	Text_Title->SetText(InTitle);
}

void UWidget_SaveFileAction::SetDescription(const FText& InDescription) const
{
	RichText_Description->SetText(InDescription);
}


void UWidget_SaveFileAction::CloseConfirmationWindow_Implementation(int Result)
{
	DeactivateWidget();
}

void UWidget_SaveFileAction::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetTitle(Header);

	SetDescription(Body);

	const FDataTableRowHandle ActionRow = ICommonInputModule::GetSettings().GetDefaultClickAction();

	Button_Continue->SetTriggeringInputAction(ActionRow);
	Button_Continue->OnClicked().AddUObject(this, &ThisClass::CloseConfirmationWindow, 1);
	Button_Continue->SetButtonText(LOCTEXT("Continue", "Continue"));

	Button_Continue->SetTriggeringInputAction(CancelAction);
	Button_Continue->OnClicked().AddUObject(this, &ThisClass::CloseConfirmationWindow, 0);
	Button_Continue->SetButtonText(LOCTEXT("Cancel", "Cancel"));
}
#undef LOCTEXT_NAMESPACE
