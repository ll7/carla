// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

DECLARE_LOG_CATEGORY_EXTERN(FMUSetup, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(FMURuntime, Warning, All);

#include "Containers/Map.h"
#include "FMU2.h"
#include "Engine/EngineTypes.h"
#include "CoreMinimal.h"
#include "Misc/Paths.h"
#include "Components/ActorComponent.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "HAL/FileManagerGeneric.h"
#include "Kismet/GameplayStatics.h"
#include "FmuActorComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FMIKIT_API UFmuActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFmuActorComponent();

	UFUNCTION(BlueprintCallable)
	float getReal(FString Name);
	UFUNCTION(BlueprintCallable)
	void setReal(FString Name, float Value);

	UFUNCTION(BlueprintCallable)
	void setBoolean(FString Name, bool Value);
	UFUNCTION(BlueprintCallable)
	bool getBoolean(FString Name);

	UFUNCTION(BlueprintCallable)
	void setInteger(FString Name, int Value);
	UFUNCTION(BlueprintCallable)
	int getInteger(FString Name);

	UFUNCTION(BlueprintCallable)
	void setString(FString Name, FString Value);
	UFUNCTION(BlueprintCallable)
	FString getString(FString Name);

protected:
	virtual void InitializeComponent() override;
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) override;
#endif

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool staticTicking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (EditCondition = staticTicking))
	float stepSize = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool finiteSimulation = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (EditCondition = finiteSimulation))
	float StopTimeMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool overrideTolerance = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (EditCondition = overrideTolerance))
	float simulationTolerance = 0.001f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFilePath FmuPath;
	UPROPERTY(VisibleAnywhere, Category = "MapsAndSets", DisplayName = "FMU Variables")
	TMap<FString, int> FmuVariables;

private:
	static FString extractFmu(FString sourcePath);
	static bool extract(FString *sourcePath, FString *targetPath);
	void importFmuParameters();
	bool isVariablePresent(FString Name);

	TSharedPtr<fmikit::FMU2Slave, ESPMode::ThreadSafe> mFmu;

	FString mFmuExtractPath;

	std::string mFmuPath, mGuid, mModelIdentifier, mInstanceName;
	fmi2Real mStartTime;
	fmi2Real mStopTime;
	fmi2Real mTolerance;
	fmi2Real mTimeLastTick;
	fmi2Real mTimeNow;
	bool mLoaded = false;
};
