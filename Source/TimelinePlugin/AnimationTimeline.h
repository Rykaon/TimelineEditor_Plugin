#pragma once

#include "CoreMinimal.h"
#include "AnimationTrack.h"
#include "AnimationTimeline.generated.h"

USTRUCT(BlueprintType)
struct FAnimationTimeline
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FAnimationTrack> Tracks;
};