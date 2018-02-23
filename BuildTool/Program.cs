using System;
using System.IO;
using System.Linq;
using Newtonsoft.Json.Linq;
using CommandLine;
using System.Collections.Generic;

namespace BuildTool
{
    class Options
    {
        [Option('r', "read", Required = true, HelpText = "Input files to be processed.")]
        public IEnumerable<string> InputFiles { get; set; }

        // Omitting long name, defaults to name of property, ie "--verbose"
        [Option(Default = false, HelpText = "Prints all messages to standard output.")]
        public bool Verbose { get; set; }

        [Option("stdin", Default = false, HelpText = "Read from stdin")]
        public bool stdin { get; set; }

        [Value(0, MetaName = "offset", HelpText = "File offset.")]
        public long? Offset { get; set; }
    }

    public class Program
    {
        static void RunOptionsAndReturnExitCode(Options opts)
        {

        }

        static void HandleParseError(IEnumerable<Error> opts)
        {

        }

        static void Main(string[] args)
        {
            CommandLine.Parser.Default.ParseArguments<Options>(args)
                .WithParsed<Options>(opts => RunOptionsAndReturnExitCode(opts))
                .WithNotParsed<Options>((errs) => HandleParseError(errs));

            var dom = new JObject();
            InputFileService.LoadRespositoryProjects(dom, new string[] { Directory.GetCurrentDirectory() });
        }
    }
}
