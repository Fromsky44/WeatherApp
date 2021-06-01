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
	FString MainLink;

	FHttpModule* Http;

	FSQLiteDatabaseConnection Database;

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Default")
	TArray<FString> Cities;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
	TArray<FString> CitiesFromDB;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
	TArray<float> TemperatureEstimatedFromDB;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
	TArray<float> TemperatureFeelFromDB;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
	TArray<FString> WeatherDescriptionFromDB;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
	TArray<float> WindSpeedFromDB;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
	TArray<FString> DateTimeFromDB;

	UFUNCTION(BlueprintCallable)
	void ParseCitiesJson();

	/* The actual HTTP call */
	UFUNCTION(BlueprintCallable)
	void WeatherHttpCall(FString City);

	/*Assign this function to call when the GET request processes sucessfully*/
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void OpenDatabase();

	UFUNCTION(BlueprintCallable)
	void GetDataFromDatabase();

	void BeginPlay() override;

	virtual void BeginDestroy() override;
};
