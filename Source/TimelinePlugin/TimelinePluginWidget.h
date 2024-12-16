#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"


class TIMELINEPLUGIN_API UTimelinePluginWidget : public IDetailCustomization
{
public:
    // Cr�� une instance de cette classe pour la personnalisation
    static TSharedRef<IDetailCustomization> MakeInstance();

    // Impl�mentation de la personnalisation
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
