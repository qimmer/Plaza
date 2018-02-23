using System;
using System.IO;
using System.Linq;
using Newtonsoft.Json.Linq;
using CommandLine;
using System.Collections.Generic;

namespace BuildTool
{

    public class Program
    {
        static void RunOptionsAndReturnExitCode(Options options)
        {
            var root = new JObject();

            InputFileService.LoadBases(root, options);
            InputFileService.LoadRespositoryProjects(root, options);
            InputFileService.ParseFlags(root, options);
            InputFileService.RemoveObjectsWithUnmetConditions(root);
            InputFileService.FindSourceFiles(root, options);
        }

        static void HandleParseError(IEnumerable<Error> opts)
        {
            foreach( var error in opts)
            {
                System.Console.Write(error.ToString());
            }
        }

        static void Main(string[] args)
        {
            CommandLine.Parser.Default.ParseArguments<Options>(args)
                .WithParsed<Options>(opts => RunOptionsAndReturnExitCode(opts))
                .WithNotParsed<Options>(errs => HandleParseError(errs));
        }
    }
}
