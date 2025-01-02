using UnrealBuildTool;

public class TimelinePlugin : ModuleRules
{
	public TimelinePlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "Slate",
            "SlateCore",
            "EditorStyle",
            "Paper2D",
            "PropertyEditor",
            "UnrealEd",
            "UMG",
            "RenderCore",
            "RHI",
        });

        PrivateDependencyModuleNames.AddRange(new string[] {  });

    }
}
