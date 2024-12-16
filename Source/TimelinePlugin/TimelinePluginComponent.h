#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimelinePluginComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TIMELINEPLUGIN_API UTimelinePluginComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTimelinePluginComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	TMap<FString, float> TrackedFloats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	TMap<FString, FVector> TrackedVectors;

	// Durée totale de l'animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	float AnimationDuration;
};
