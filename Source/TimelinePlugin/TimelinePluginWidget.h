#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"


class TIMELINEPLUGIN_API UTimelinePluginWidget : public IDetailCustomization
{
public:
    // Créé une instance de cette classe pour la personnalisation
    static TSharedRef<IDetailCustomization> MakeInstance();

    // Implémentation de la personnalisation
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
