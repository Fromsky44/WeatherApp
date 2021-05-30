// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Modules/ModuleManager.h"
#include "SQLiteDatabaseConnection.h"
#include "MainGameModeBase.generated.h"

/**
 * 
 */

UCLASS()
class WEATHERAPP_API AMainGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	AMainGameModeBase();

private:
	UPROPERTY()
	FString MainLink;

public:
	FHttpModule* Http;

	FSQLiteDatabaseConnection Database;

	float TemperatureEstimated;
	float TemperatureFeelsLike;
	FString WeatherDiscription;
	float WindSpeed;

	void ParseCitiesJson();

	TArray<FString> Cities;

	/* The actual HTTP call */
	UFUNCTION(BlueprintCallable)
	void WeatherHttpCall();

	/*Assign this function to call when the GET request processes sucessfully*/
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void BeginPlay() override;

	virtual void BeginDestroy() override;
};
