#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimelinePluginWidget.h"
#include "AnimationTimeline.h"
#include "TimelinePluginComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), DisplayName = "Timeline" )
class TIMELINEPLUGIN_API UTimelinePluginComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTimelinePluginComponent();
	
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeComponent();

	bool IsAnimationTimelineWidgetValid();
	TArray<FName> GetAvailableFunctions();
	bool IsVariableTracked(const FString& VariableName);

	void PopulateAvailableVariables();
	void GetVariablesFromParentBlueprint();
	void OnTypeSelected(TSharedPtr<FString> NewSelection);
	void OnVariableSelected(TSharedPtr<FString> NewSelection);
	void RemoveTrack(int32 TrackID);
	int32 GenerateUniqueRandomID();

	void ApplyTrackDefaultValueToVariable(int32 TrackIndex);
	void ApplyTrackValueAtTimeToVariable(int32 TrackIndex, float Time);
	void CallSelectedFunction();

	FAnimationTrack* GetTrackOfID(int32 TrackID);
	std::optional<bool> GetTrackBoolValueAtTime(int32 TrackID, float Time);
    std::optional<int32> GetTrackIntValueAtTime(int32 TrackID, float Time);
    float GetTrackFloatValueAtTime(int32 TrackID, float Time, int32 Index);
    double GetTrackDoubleValueAtTime(int32 TrackID, float Time);
	float GetAlphaAtTime(float Start, float End, float Time);
    float InterpolateFloat(float StartValue, float EndValue, float Alpha, EInterpolationType InterpolationType);
    int32 InterpolateInteger(int32 StartValue, int32 EndValue, float Alpha, EInterpolationType InterpolationType);
    double InterpolateDouble(double StartValue, double EndValue, float Alpha, EInterpolationType InterpolationType);
    FVector InterpolateVector(FVector StartValue, FVector EndValue, float Alpha, EInterpolationType InterpolationType);
    FRotator InterpolateRotator(FRotator StartValue, FRotator EndValue, float Alpha, EInterpolationType InterpolationType);

protected:
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

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Timeline")
	class UTimelinePluginWidget* TimelinePluginWidget;

	// AnimationTimeline Structure
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	FAnimationTimeline AnimationTimeline;

	// AnimationTimeline Structure
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	bool IsAnimationLooping = true;

	float CurrentTime = 0.0f;
	FName SelectedFunction = FName("None");

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
	TMap<FString, FString> TrackedVariables;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timeline")
	TArray<int32> TrackedIDs;

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
