using System;
using System.Linq;
using Nuke.Common;
using Nuke.Common.CI;
using Nuke.Common.CI.AppVeyor;
using Nuke.Common.Execution;
using Nuke.Common.Git;
using Nuke.Common.IO;
using Nuke.Common.ProjectModel;
using Nuke.Common.Tooling;
using Nuke.Common.Tools.MSBuild;
using Nuke.Common.Utilities.Collections;
using static Nuke.Common.EnvironmentInfo;
using static Nuke.Common.IO.FileSystemTasks;
using static Nuke.Common.IO.PathConstruction;
using static Nuke.Common.Tools.MSBuild.MSBuildTasks;

[CheckBuildProjectConfigurations]
class Build : NukeBuild
{
    /// Support plugins are available for:
    ///   - JetBrains ReSharper        https://nuke.build/resharper
    ///   - JetBrains Rider            https://nuke.build/rider
    ///   - Microsoft VisualStudio     https://nuke.build/visualstudio
    ///   - Microsoft VSCode           https://nuke.build/vscode

    public static int Main () => Execute<Build>(x => x.Compile);

    [Parameter("Configuration to build - Default is 'Debug' (local) or 'Release' (server)")]
    readonly Configuration Configuration = IsLocalBuild ? Configuration.Debug : Configuration.Release;

    [Solution] readonly Solution Solution;
    [GitRepository] readonly GitRepository GitRepository;

    AbsolutePath DmfSolution => RootDirectory / "../DMF/Dmf.sln";

    Target Clean => _ => _
        .Before(Restore)
        .Executes(() =>
        {
        });

    Target Restore => _ => _
        .Executes(() =>
        {
            MSBuild(s => s
                .SetTargetPath(Solution)
                .SetTargets("Restore"));
        });

    Target BuildDmf => _ => _
    .Executes(() =>
    {
        if (IsLocalBuild)
            return;

        Console.WriteLine($"DMF solution path: {DmfSolution}");

        var platform = AppVeyor.Instance.Platform;
        switch (platform)
        {
            "x86" => MSBuildTargetPlatform.Win32,
            "ARM64" => (MSBuildTargetPlatform)"ARM64",
            _ => MSBuildTargetPlatform.x64
        };

        MSBuild(s => s
            .SetTargetPath(DmfSolution)
            .SetTargets("Build")
            .SetConfiguration(Configuration)
            .SetTargetPlatform(platform)
            .SetMaxCpuCount(Environment.ProcessorCount)
            .SetNodeReuse(IsLocalBuild));
    });

    Target Compile => _ => _
        .DependsOn(BuildDmf)
        .Executes(() =>
        {
            MSBuild(s => s
                .SetTargetPath(Solution)
                .SetTargets("Rebuild")
                .SetConfiguration(Configuration)
                .SetMaxCpuCount(Environment.ProcessorCount)
                .SetNodeReuse(IsLocalBuild));
        });

}
