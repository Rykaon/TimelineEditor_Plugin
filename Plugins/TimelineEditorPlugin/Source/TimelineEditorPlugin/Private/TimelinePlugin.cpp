#include "TimelinePlugin.h"
#include "TimelinePluginComponent.h"
#include "TimelinePluginWidget.h"
#include "PropertyEditorModule.h"

IMPLEMENT_MODULE(FTimelinePluginModule, MyEditorPlugin)

void FTimelinePluginModule::StartupModule()
{
    UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : TimelinePlugin has started!"));

    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(
        TEXT("TimelinePluginComponent"),
        FOnGetDetailCustomizationInstance::CreateStatic(&UTimelinePluginWidget::MakeInstance)
    );

    // Enregistrement des catégories si nécessaire
    PropertyModule.NotifyCustomizationModuleChanged();
}

void FTimelinePluginModule::ShutdownModule()
{
    UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : TimelinePlugin has shutdown!"));

    if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
        PropertyModule.UnregisterCustomClassLayout(TEXT("TimelinePluginComponent"));
        PropertyModule.NotifyCustomizationModuleChanged();

        UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : Unregistered Detail Customization!"));
    }
}