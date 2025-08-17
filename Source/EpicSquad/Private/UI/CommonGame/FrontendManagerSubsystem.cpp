// CloudHu:604746493@qq.com All Rights Reserved


#include "UI/CommonGame/FrontendManagerSubsystem.h"


UFrontendManagerSubsystem* UFrontendManagerSubsystem::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::Assert);

		return UGameInstance::GetSubsystem<UFrontendManagerSubsystem>(World->GetGameInstance());
	}

	return nullptr;
}

bool UFrontendManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		// Only create an instance if there is no override implementation defined elsewhere
		return ChildClasses.Num() == 0;
	}

	return false;
}

void UFrontendManagerSubsystem::RegisterCreatedPrimaryLayoutWidget(UPrimaryGameLayout* InCreatedWidget)
{
	check(InCreatedWidget);
	CreatedPrimaryLayout=InCreatedWidget;
}
