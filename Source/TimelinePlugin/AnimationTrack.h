#pragma once

#include "CoreMinimal.h"
#include "AnimationKeyFrame.h"
#include "AnimationTrack.generated.h"

USTRUCT(BlueprintType)
struct  FAnimationTrack
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName VariableName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FAnimationKeyFrame> Keyframes;
};