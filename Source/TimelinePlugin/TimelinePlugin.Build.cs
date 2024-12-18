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
            "PropertyEditor",
            "UnrealEd"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {  });

    }
}
