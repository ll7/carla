// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "ScoomaticBaseAIController.h"

// #include "MapGen/RoadMap.h"
// #include "Traffic/RoutePlanner.h"
#include "Scoomatic/CarlaScoomaticBase.h"

#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
// #include "WheeledVehicleMovementComponent.h"

// =============================================================================
// -- Static local methods -----------------------------------------------------
// =============================================================================
/* TODO
static bool RayCast(const AActor &Actor, const FVector &Start, const FVector &End)
{
  FHitResult OutHit;
  static FName TraceTag = FName(TEXT("VehicleTrace"));
  FCollisionQueryParams CollisionParams(TraceTag, true);
  CollisionParams.AddIgnoredActor(&Actor);

  const bool Success = Actor.GetWorld()->LineTraceSingleByObjectType(
      OutHit,
      Start,
      End,
      FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllDynamicObjects),
      CollisionParams);

  // DrawDebugLine(Actor.GetWorld(), Start, End,
  //     Success ? FColor(255, 0, 0) : FColor(0, 255, 0), false);

  return Success && OutHit.bBlockingHit;
}

static bool IsThereAnObstacleAhead(
    const ACarlaWheeledVehicle &Vehicle,
    const float Speed,
    const FVector &Direction)
{
  const auto ForwardVector = Vehicle.GetVehicleOrientation();
  const auto VehicleBounds = Vehicle.GetVehicleBoundingBoxExtent();

  FVector NormDirection = Direction.GetSafeNormal();

  const float Distance = std::max(50.0f, Speed * Speed); // why?

  const FVector StartCenter = Vehicle.GetActorLocation() +
      (ForwardVector * (250.0f + VehicleBounds.X / 2.0f)) + FVector(0.0f, 0.0f, 50.0f);
  const FVector EndCenter = StartCenter + NormDirection * (Distance + VehicleBounds.X / 2.0f);

  const FVector StartRight = StartCenter +
      (FVector(ForwardVector.Y, -ForwardVector.X, ForwardVector.Z) * 100.0f);
  const FVector EndRight = StartRight + NormDirection * (Distance + VehicleBounds.X / 2.0f);

  const FVector StartLeft = StartCenter +
      (FVector(-ForwardVector.Y, ForwardVector.X, ForwardVector.Z) * 100.0f);
  const FVector EndLeft = StartLeft + NormDirection * (Distance + VehicleBounds.X / 2.0f);

  return
    RayCast(Vehicle, StartCenter, EndCenter) ||
    RayCast(Vehicle, StartRight, EndRight) ||
    RayCast(Vehicle, StartLeft, EndLeft);
}

template <typename T>
static void ClearQueue(std::queue<T> &Queue)
{
  std::queue<T> EmptyQueue;
  Queue.swap(EmptyQueue);
}
*/

// =============================================================================
// -- Constructor and destructor -----------------------------------------------
// =============================================================================

AScoomaticBaseAIController::AScoomaticBaseAIController(const FObjectInitializer &ObjectInitializer)
  : Super(ObjectInitializer)
{
  /* TODO
  RandomEngine = CreateDefaultSubobject<URandomEngine>(TEXT("RandomEngine"));

  RandomEngine->Seed(RandomEngine->GenerateRandomSeed());
  */

  PrimaryActorTick.bCanEverTick = true;
  PrimaryActorTick.TickGroup = TG_PrePhysics;
}

AScoomaticBaseAIController::~AScoomaticBaseAIController() {}

// =============================================================================
// -- AController --------------------------------------------------------------
// =============================================================================

void AScoomaticBaseAIController::OnPossess(APawn *aPawn)
{
  Super::OnPossess(aPawn);

  if (IsPossessingAScoomatic())
  {
    UE_LOG(LogCarla, Error, TEXT("Controller already possessing a scoomatic!"));
    return;
  }
  Scoomatic = Cast<ACarlaScoomaticBase>(aPawn);
  check(Scoomatic != nullptr);
  
  /*
  MaximumSteerAngle = Vehicle->GetMaximumSteerAngle();
  check(MaximumSteerAngle > 0.0f);
  ConfigureAutopilot(bAutopilotEnabled);

  if (RoadMap == nullptr)
  {
    TActorIterator<ACityMapGenerator> It(GetWorld());
    RoadMap = (It ? It->GetRoadMap() : nullptr);
  }
  */
}

void AScoomaticBaseAIController::OnUnPossess()
{
  Super::OnUnPossess();

  Scoomatic = nullptr;
}

void AScoomaticBaseAIController::Tick(const float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (!IsPossessingAScoomatic())
  {
    return;
  }

  if (!bAutopilotEnabled && !bControlIsSticky)
  {
    // TODO Autopilot not implemented
    // Vehicle->ApplyVehicleControl(FVehicleControl{}, EVehicleInputPriority::Relaxation);
  }

  Scoomatic->FlushScoomaticControl();
}

// =============================================================================
// -- Autopilot ----------------------------------------------------------------
// =============================================================================
/*
void AWheeledVehicleAIController::ConfigureAutopilot(const bool Enable, const bool KeepState)
{
  bAutopilotEnabled = Enable;
  if (!KeepState)
  {
    // Reset state.
    Vehicle->SetSteeringInput(0.0f);
    Vehicle->SetThrottleInput(0.0f);
    Vehicle->SetBrakeInput(0.0f);
    Vehicle->SetReverse(false);
    Vehicle->SetHandbrakeInput(false);
    ClearQueue(TargetLocations);
    Vehicle->SetAIVehicleState(
        bAutopilotEnabled ?
        ECarlaWheeledVehicleState::FreeDriving :
        ECarlaWheeledVehicleState::AutopilotOff);
  }

  TrafficLightState = ETrafficLightState::Green;
}
*/

// =============================================================================
// -- Traffic ------------------------------------------------------------------
// =============================================================================
/*
void AWheeledVehicleAIController::SetFixedRoute(
    const TArray<FVector> &Locations,
    const bool bOverwriteCurrent)
{
  if (bOverwriteCurrent)
  {
    ClearQueue(TargetLocations);
  }
  for (auto &Location : Locations)
  {
    TargetLocations.emplace(Location);
  }
}
*/