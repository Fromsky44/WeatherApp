// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameModeBase.h"
#include "Interfaces/IHttpRequest.h"
#include "SQLiteDatabaseConnection.h"
#include "Runtime/Online/HTTP/Public/HttpModule.h"

//FSQLiteDatabaseConnection AMainGameModeBase::Database;

AMainGameModeBase::AMainGameModeBase()
{
	MainLink = "http://api.openweathermap.org/data/2.5/weather?APPID=1db23e366b79c4270b94472ec6ed1890&q=perm";
	//When the object is constructed, Get the HTTP module
	Http = &FHttpModule::Get();
	TemperatureEstimated = 0;
	TemperatureFeelsLike = 0;
	WeatherDiscription = "Description";
	WindSpeed = 0;
}

/*Http call*/
void AMainGameModeBase::WeatherHttpCall()
{
	// Create request object
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	// Bind function with response
	Request->OnProcessRequestComplete().BindUObject(this, &AMainGameModeBase::OnResponseReceived);
	Request->SetURL(MainLink);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	// Send Request
	Request->ProcessRequest();
}

/*Assigned function on successfull http call*/
void AMainGameModeBase::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject;

	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		//Get the value of the json object by field name
		TemperatureEstimated = JsonObject->GetObjectField("main")->GetNumberField("temp");
		TemperatureFeelsLike = JsonObject->GetObjectField("main")->GetNumberField("feels_like");
		TArray<TSharedPtr<FJsonValue>> JsonWeatherArray = JsonObject->GetArrayField("weather");
		TSharedPtr<FJsonObject> JsonArrayObject = JsonWeatherArray[0]->AsObject();
		WeatherDiscription = JsonArrayObject->GetStringField("description");
		WindSpeed = JsonObject->GetObjectField("wind")->GetNumberField("speed");

		UE_LOG(LogTemp, Warning, TEXT("%f"), TemperatureEstimated);
		UE_LOG(LogTemp, Warning, TEXT("%f"), TemperatureFeelsLike);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *WeatherDiscription);
		UE_LOG(LogTemp, Warning, TEXT("%f"), WindSpeed);

		// Inserting in DB
		Database.Open(TEXT("C:/Users/ChesnokovIvan/Desktop/Unreal Engine/WeatherApp/Source/test.db"), TEXT(""), TEXT(""));
		if (!Database.Execute(TEXT("CREATE TABLE IF NOT EXISTS cities (city text NOT NULL,temp float,temp_feel float, weather_description text, wind_speed float)")))
		{
			UE_LOG(LogTemp, Warning, TEXT("Table was not created"));
		}
		FString SQLInsert = "INSERT INTO cities VALUES ('Perm', " + FString::SanitizeFloat(TemperatureEstimated) + ", " + FString::SanitizeFloat(TemperatureFeelsLike) + ", '" + WeatherDiscription + "', " + FString::SanitizeFloat(WindSpeed) + ")";
		UE_LOG(LogTemp, Warning, TEXT("Inserted: %s"), *SQLInsert);
		if (!Database.Execute(*SQLInsert))
		{
			UE_LOG(LogTemp, Warning, TEXT("Data was not inserted"));
			return;
		}

		FDataBaseRecordSet* RecordSet;
		Database.Execute(TEXT("SELECT DISTINCT city, temp, temp_feel, weather_description, wind_speed FROM cities"), RecordSet);
		FDataBaseRecordSet::TIterator Iter(RecordSet);
		TArray<FDatabaseColumnInfo> ColumnNames = RecordSet->GetColumnNames();
		for (int i = 1; i <= RecordSet->GetRecordCount(); i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("City: %s"), *Iter->GetString(*ColumnNames[0].ColumnName));
			UE_LOG(LogTemp, Warning, TEXT("Temperature Estimated: %f"), Iter->GetFloat(*ColumnNames[1].ColumnName));
			UE_LOG(LogTemp, Warning, TEXT("Temperature Feel: %f"), Iter->GetFloat(*ColumnNames[2].ColumnName));
			UE_LOG(LogTemp, Warning, TEXT("Weather Desription: %s"), *Iter->GetString(*ColumnNames[3].ColumnName));
			UE_LOG(LogTemp, Warning, TEXT("Wind Speed: %f"), Iter->GetFloat(*ColumnNames[4].ColumnName));
			Iter.operator++();
		}
		UE_LOG(LogTemp, Warning, TEXT("Number of Records: %i"), Iter->GetRecordCount());
		delete RecordSet;
	}
}

void AMainGameModeBase::ParseCitiesJson()
{
	FString PathToJson = FPaths::ProjectDir() + "/Source/CityList.json";
	FString JsonStr;

	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*PathToJson))
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no file with all cities here: %s"), *PathToJson);
		return;
	}

	FFileHelper::LoadFileToString(JsonStr, *PathToJson);

	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonValue> JsonValue;

	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonStr);
	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonValue))
	{
		//Get the value of the json object by field name
		TArray<TSharedPtr<FJsonValue>> JsonValueArray = JsonValue->AsArray();
		for (TSharedPtr<FJsonValue> JsonValueJson : JsonValueArray)
		{
			FString Country = JsonValueJson->AsObject()->GetStringField("country");
			FString City = JsonValueJson->AsObject()->GetStringField("name");
			if (Country == "RU")
			{
				Cities.AddUnique(City);
			}
		}
		Cities.Sort();
	}
}

void AMainGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	ParseCitiesJson();
	WeatherHttpCall();
}

void AMainGameModeBase::BeginDestroy()
{
	Super::BeginDestroy();
	Database.Close();
}