#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Slate.h"
#include "TimelineTrackWidget.h"
#include "TimelinePluginComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), DisplayName = "Timeline" )
class TIMELINEPLUGIN_API UTimelinePluginComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTimelinePluginComponent();
	
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsVariableTracked(const FString& VariableName);
	void PopulateAvailableVariables();
	void GetVariablesFromParentBlueprint();
	void OnTypeSelected(TSharedPtr<FString> NewSelection);
	void OnVariableSelected(TSharedPtr<FString> NewSelection);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Timeline") Pour une variable cach�e dans la fen�tre details
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Timeline")
	bool IsInitialized;



	// OwnerActor References
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Timeline")
	AActor* OwnerActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Timeline")
	UClass* OwnerActorClass;

	// Dur�e totale de l'animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	float AnimationDuration;

	// R�f�rence to Animation Widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimelineAnimation")
	STimelineTrackWidget AnimationTimeline;



	// Available TrackedTypes
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Timeline")
	TArray<FString> AvailableTypes;

	// Current TrackedType
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timeline")
	FString SelectedType;


	// Available Variables Of Current TrackedTypes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timeline")
	TArray<FString> AvailableVariables;

	// Available Variables Of Available TrackedTypes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	TArray<FString> AvailableBools;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	TArray<FString> AvailableInts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	TArray<FString> AvailableFloats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	TArray<FString> AvailableDoubles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	TArray<FString> AvailableVectors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	TArray<FString> AvailableRotators;



	// Variables already Tracked
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timeline")
	TArray<FString> TrackedVariables;

	// TrackedVariables Of Available TrackedType
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	TMap<FString, bool> TrackedBools;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	TMap<FString, int> TrackedInts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	TMap<FString, float> TrackedFloats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	TMap<FString, double> TrackedDoubles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	TMap<FString, FVector> TrackedVectors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	TMap<FString, FRotator> TrackedRotators;
};
