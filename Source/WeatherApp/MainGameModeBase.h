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

USTRUCT(BlueprintType)
struct FDataFromDB
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
	FString City;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
	float TemperatureEstimated;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
	float TemperatureFeel;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
	FString WeatherDesription;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
	float WindSpeed;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
	FString DateTime;
};

UCLASS()
class WEATHERAPP_API AMainGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	AMainGameModeBase();

private:
	FString MainLink;

	FHttpModule* Http;

	FSQLiteDatabaseConnection Database;

	/*Assign this function to call when the GET request processes sucessfully*/
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TArray<FString> InitialCities;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Default")
	TArray<FString> Cities;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
	TArray<FDataFromDB> CityInfoFromDB;

	UFUNCTION(BlueprintCallable)
	void ParseCitiesJson();

	UFUNCTION(BlueprintCallable)
	void OpenDatabase();

	/* The actual HTTP call */
	UFUNCTION(BlueprintCallable)
	void WeatherHttpCall(FString City);

	UFUNCTION(BlueprintCallable)
	void GetDataFromDatabase();

	UFUNCTION(BlueprintCallable)
	void GetCityDataFromDatabase(FString City);

	void BeginPlay() override;

	virtual void BeginDestroy() override;
};
