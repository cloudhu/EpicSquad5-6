// CloudHu:604746493@qq.com All Rights Reserved


#include "UI/Controller/FrontendPlayerController.h"

#include "Camera/CameraActor.h"

#include "Kismet/GameplayStatics.h"

void AFrontendPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	TArray<AActor*> FoundCameras;
	UGameplayStatics::GetAllActorsOfClassWithTag(this,ACameraActor::StaticClass(),FName("DefaultCamera"),FoundCameras);

	if (!FoundCameras.IsEmpty())
	{
		SetViewTarget(FoundCameras[0]);
	}

	// UFrontendGameUserSettings* GameUserSettings = UFrontendGameUserSettings::Get();
	//
	// if (GameUserSettings->GetLastCPUBenchmarkResult() == -1.f || GameUserSettings->GetLastGPUBenchmarkResult() == -1.f)
	// {
	// 	GameUserSettings->RunHardwareBenchmark();
	// 	GameUserSettings->ApplyHardwareBenchmarkResults();
	// }
}
