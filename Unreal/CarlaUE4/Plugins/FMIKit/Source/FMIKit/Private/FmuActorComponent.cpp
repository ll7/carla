// Fill out your copyright notice in the Description page of Project Settings.

#include "FmuActorComponent.h"
#include "GameFramework/Actor.h"
#include "XmlFile.h"
#include "elzip.hpp"

DEFINE_LOG_CATEGORY(FMUSetup);
DEFINE_LOG_CATEGORY(FMURuntime);

// Sets default values for this component's properties
UFmuActorComponent::UFmuActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
	FmuPath.FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() + "../PT2.fmu");
	// ...
}

/*
Checks if the fmuPath was changed and extracts the FMU and imports the parameters.
TODO: model variables do not show up in the blueprint user interface.
*/
#if WITH_EDITOR
void UFmuActorComponent::PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UFmuActorComponent, FmuPath))
	{
		UE_LOG(FMUSetup, Display, TEXT("FmuPath changed: %s"), *(PropertyChangedEvent.MemberProperty->GetNameCPP()));
		mFmuExtractPath = extractFmu(FmuPath.FilePath);
		UE_LOG(FMUSetup, Display, TEXT("Extracted to: %s"), *mFmuExtractPath);
		importFmuParameters();
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UFmuActorComponent::InitializeComponent()
{
	Super::InitializeComponent();
	UE_LOG(FMUSetup, Log, TEXT("InitializeComponenent called for FMU %s"), *FmuPath.FilePath);
	mFmuExtractPath = extractFmu(FmuPath.FilePath);
	importFmuParameters();
}

/*
Initializes the FMU.
TODO: Error checking and handling. The FMU initialization functions do not indicate failure.
*/
void UFmuActorComponent::BeginPlay()
{
	Super::BeginPlay();

	for (auto &Elem : FmuVariables)
	{
		UE_LOG(FMUSetup, Verbose, TEXT("(%s, \"%d\")\n"), *Elem.Key, Elem.Value);
	}
	if (overrideTolerance)
		mTolerance = simulationTolerance;

	UE_LOG(FMUSetup, Verbose, TEXT("FMU Initialization parameters, %s %s %s %s"), *FString(mGuid.c_str()), *FString(mModelIdentifier.c_str()), *mFmuExtractPath, *FString(mInstanceName.c_str()));

	mFmu = MakeShared<fmikit::FMU2Slave, ESPMode::ThreadSafe>(mGuid, mModelIdentifier, std::string(TCHAR_TO_UTF8(*mFmuExtractPath)), mInstanceName);
	mFmu->instantiate(false);
	UE_LOG(FMUSetup, Verbose, TEXT("instantiate complete!"));

	mFmu->setupExperiment(true, mTolerance, mStartTime, finiteSimulation, mStopTime);
	UE_LOG(FMUSetup, Verbose, TEXT("setupExperiment complete!"));

	mFmu->enterInitializationMode();
	UE_LOG(FMUSetup, Verbose, TEXT("enterInitializationMode complete!"));

	mFmu->exitInitializationMode();
	UE_LOG(FMUSetup, Verbose, TEXT("exitInitializationMode complete!"));

	mLoaded = true;
}

/* 
Extracts the FMU at fmuPath to a folder in the same directory.
The Path is {FMUname}.{Changed Unix Timestamp} to prevent stuttering on extraction.
Returns the extraction path.
*/
FString UFmuActorComponent::extractFmu(FString fmuPath)
{
	FString tempFmuPath = FPaths::ConvertRelativePathToFull(*fmuPath);
	FString FmuParentPath = FPaths::GetPath(tempFmuPath);
	IPlatformFile &PlatformFileManager = FPlatformFileManager::Get().GetPlatformFile();

	FDateTime modifiedDate = PlatformFileManager.GetTimeStamp(*tempFmuPath);

	FString extractDir = FPaths::SetExtension(tempFmuPath, FString::Printf(TEXT("%lld"), modifiedDate.ToUnixTimestamp()));

	if (PlatformFileManager.DirectoryExists(*extractDir))
	{
		return extractDir;
	}

	TArray<FString> foundFolders;

	FFileManagerGeneric FileMgr;
	FString wildcard = FString::Printf(TEXT("%s.*"), *FPaths::GetBaseFilename(*tempFmuPath));
	FString search_path(FPaths::Combine(FmuParentPath, *wildcard));
	UE_LOG(FMUSetup, Verbose, TEXT("Old FMU cleanup Search Path: %s"), *search_path);
	FileMgr.FindFiles(foundFolders, *search_path, false, true);

	for (FString folder : foundFolders)
	{
		FString folderPath = FmuParentPath;
		folderPath.PathAppend(*folder, folder.Len());

		UE_LOG(FMUSetup, Warning, TEXT("FMU Cleanup FoundFolder: %s"), *folderPath);
		PlatformFileManager.DeleteDirectoryRecursively(*folderPath);
	}

	extract(&tempFmuPath, &extractDir);
	return extractDir;
}

/* 
Extracts a ZIP compressed File from sourcePath to targetPath.
Contains implementations for Windows and Linux
TODO: Test Windows Implementation
TODO: Do the selection at compile time
 */
bool UFmuActorComponent::extract(FString *sourcePath, FString *targetPath)
{

	if (sourcePath->IsEmpty())
	{
		UE_LOG(FMUSetup, Error, TEXT("Path to FMU %s is empty."), sourcePath);
		return false;
	}
	auto tempFmuPath = FPaths::ConvertRelativePathToFull(*sourcePath);
	auto tempTargetPath = FPaths::ConvertRelativePathToFull(*targetPath);

	IPlatformFile &FileManager = FPlatformFileManager::Get().GetPlatformFile();

	FileManager.DeleteDirectoryRecursively(*tempTargetPath);
	FileManager.CreateDirectory(*tempTargetPath);

	UE_LOG(FMUSetup, Display, TEXT("FMU Path: %s FMU Extraction Dir: %s Platform: %s"), *tempFmuPath, *tempTargetPath, *(UGameplayStatics::GetPlatformName()));

	if (UGameplayStatics::GetPlatformName() == "LINUX")
	{
		FString unzipCommandArgs = FString::Printf(TEXT("-uo %s -d %s"), *tempFmuPath, *tempTargetPath);
		UE_LOG(FMUSetup, Display, TEXT("Extraction command: %s"), *unzipCommandArgs);

		FUnixPlatformProcess::ExecProcess(TEXT("/usr/bin/unzip"), *unzipCommandArgs, 0, 0, 0);
	}
	else
	{
		UE_LOG(FMUSetup, Warning, TEXT("Platform is not LINUX, using elzip to extract FMU"));
		elz::extractZip(std::string(TCHAR_TO_UTF8(*tempFmuPath)), std::string(TCHAR_TO_UTF8(*tempTargetPath)));
	}

	return true;
}

/*
Reads the modelDescription.xml and sets experiment attributes and
populates the modelVariables map.
*/
void UFmuActorComponent::importFmuParameters()
{

	FString fXmlFile = mFmuExtractPath;
	fXmlFile = FPaths::Combine(mFmuExtractPath, TEXT("modelDescription.xml"));

	IPlatformFile &FileManager = FPlatformFileManager::Get().GetPlatformFile();

	if (!FileManager.FileExists(*fXmlFile))
	{
		UE_LOG(FMUSetup, Error, TEXT("No modelDesctription.xml found at %s"), *fXmlFile);
		return;
	}

	FXmlFile model(fXmlFile, EConstructMethod::ConstructFromFile);
	FXmlNode *root = model.GetRootNode();
	FXmlNode *defaultExperiment = root->FindChildNode("DefaultExperiment");
	FXmlNode *modelVariables = root->FindChildNode("ModelVariables");
	TArray<FXmlNode *> nodes = modelVariables->GetChildrenNodes();
	for (FXmlNode *node : nodes)
	{
		FString key = node->GetAttribute("name");
		int value = FCString::Atoi(*node->GetAttribute("valueReference"));
		UE_LOG(FMUSetup, Display, TEXT("Found Model Var: %s : %d"), *key, value);
		FmuVariables.Add(key, value);
	}
	mGuid = TCHAR_TO_UTF8(*root->GetAttribute("guid"));
	mModelIdentifier = TCHAR_TO_UTF8(*root->GetAttribute("modelName"));
	;
	mInstanceName = "instance";
	mStartTime = FCString::Atof(*defaultExperiment->GetAttribute("startTime"));
	mStopTime = FCString::Atof(*defaultExperiment->GetAttribute("stopTime")) * StopTimeMultiplier;
	mTolerance = FCString::Atof(*defaultExperiment->GetAttribute("tolerance"));
	;
	mTimeLastTick = mStartTime;
	mTimeNow = mStartTime;
}

void UFmuActorComponent::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
	mFmu.Reset();
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void UFmuActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!mLoaded)
		return;

	mTimeNow += DeltaTime;
	if (finiteSimulation && mTimeLastTick >= mStopTime)
		return;

	if (staticTicking)
	{
		if (!(mTimeNow > mTimeLastTick + stepSize))
			return;
		mTimeLastTick += stepSize;
		mFmu->doStep(stepSize);
	}
	else
	{
		mTimeLastTick += DeltaTime;
		mFmu->doStep(DeltaTime);
	}
}

float UFmuActorComponent::getReal(FString Name)
{
	return isVariablePresent(Name) ? mFmu->getReal(FmuVariables[Name]) : NAN;
}

void UFmuActorComponent::setReal(FString Name, float Value)
{
	if (isVariablePresent(Name))
		mFmu->setReal(FmuVariables[Name], Value);
}

bool UFmuActorComponent::getBoolean(FString Name)
{
	return isVariablePresent(Name) ? mFmu->getBoolean(FmuVariables[Name]) : false;
}

void UFmuActorComponent::setBoolean(FString Name, bool Value)
{
	if (isVariablePresent(Name))
		mFmu->setBoolean(FmuVariables[Name], Value);
}

int UFmuActorComponent::getInteger(FString Name)
{
	return isVariablePresent(Name) ? mFmu->getInteger(FmuVariables[Name]) : -1;
}

void UFmuActorComponent::setInteger(FString Name, int Value)
{
	if (isVariablePresent(Name))
		mFmu->setInteger(FmuVariables[Name], Value);
}

FString UFmuActorComponent::getString(FString Name)
{
	return isVariablePresent(Name) ? FString(mFmu->getString(FmuVariables[Name]).c_str()) : FString("Invalid String Name");
}

void UFmuActorComponent::setString(FString Name, FString Value)
{
	if (isVariablePresent(Name))
		mFmu->setString(FmuVariables[Name], std::string(TCHAR_TO_UTF8(*Value)));
}

bool UFmuActorComponent::isVariablePresent(FString Name)
{
	if (!FmuVariables.Contains(Name))
	{
		UE_LOG(FMURuntime, Error, TEXT("No variable with name %s found in FMU: %s"), *Name, *FmuPath.FilePath);
		return false;
	}
	return true;
}