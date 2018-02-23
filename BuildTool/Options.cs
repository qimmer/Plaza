using CommandLine;
using System.Collections.Generic;

namespace BuildTool
{
    public class Options
    {
        [Option('p', "project", HelpText = "Name of project to be built.")]
        public string Project { get; set; }

        [Option('r', "repository", HelpText = "Add repository for finding project references")]
        public IEnumerable<string> Repositories { get; set; }

        [Option('b', "base", HelpText = "Add base for plugin json files")]
        public IEnumerable<string> Bases { get; set; }

        [Option('f', "flag", HelpText = "Add flag for specifying platform, configuration etc.")]
        public IEnumerable<string> Flags { get; set; }

        [Option('v', "verbose", HelpText = "Verbose output")]
        public bool Verbose { get; set; }
    }
}
