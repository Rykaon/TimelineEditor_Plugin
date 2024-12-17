#pragma once

#include "CoreMinimal.h"
#include "AnimationKeyFrame.generated.h"

USTRUCT(BlueprintType)
struct  FAnimationKeyFrame
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Time;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 IntValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool BoolValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FloatValueX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FloatValueY;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FloatValueZ;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName InterpolationType;
};