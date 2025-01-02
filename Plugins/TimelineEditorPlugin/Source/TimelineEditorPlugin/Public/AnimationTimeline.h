#pragma once

#include "CoreMinimal.h"
#include "AnimationTrack.h"
#include "AnimationTimeline.generated.h"

USTRUCT(BlueprintType)
struct FAnimationTimeline
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float Duration;

    TArray<FAnimationTrack> Tracks;
};