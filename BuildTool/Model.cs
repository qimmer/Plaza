using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace BuildTool
{
    public static class InputFileService
    {
        public static void ParseFlags(JObject root, Options options)
        {
            if(!root.TryGetValue("Flags", out var flagsToken) || !(flagsToken is JObject flags)) throw new ArgumentException("Flags object not found in model");

            foreach(var flag in options.Flags)
            {
                if(!flags.TryGetValue(flag, out var flagToken) || !(flagToken is JObject flagData)) throw new ArgumentException($"Unknown flag: {flag}. Available flags are: \n" + string.Join("\n", flags.Properties().Select(x => x.Name)));
            }

            var removedObjects = flags.Properties().Where(flag => !options.Flags.Select(x => x.ToLower()).Contains(flag.Name.ToLower())).ToArray();
            foreach(var removal in removedObjects)
            {
                flags.Remove(removal.Name);
            }
        }

        public static void RemoveObjectsWithUnmetConditions(JObject root)
        {
            if(!root.TryGetValue("Flags", out var flagsToken) || !(flagsToken is JObject flags)) throw new ArgumentException("Flags object not found in model");

            foreach(var kind in root)
            {
                foreach(var entry in kind.Value)
                {
                    if(entry is JObject obj)
                    {
                        if(obj.TryGetValue("Conditions", out var conditions))
                        {
                            var conditionStrings = conditions.Value<string>().Split(';').Select(x => x.Trim());
                            if(!conditionStrings.All(x => flags.ContainsKey(x)))
                            {
                                obj.Remove();
                            }
                        }
                    }
                }
            }
        }

        public static void LoadRespositoryProjects(JObject root, Options options)
        {
            foreach(var repositoryPath in options.Repositories)
            {
                foreach(var projectFolder in Directory.EnumerateDirectories(repositoryPath, "*", SearchOption.TopDirectoryOnly))
                {
                    var absoluteProjectFolder = Path.Combine(repositoryPath, projectFolder);
                    var absoluteProjectFile = Path.Combine(absoluteProjectFolder, "project.json");
                    if(File.Exists(absoluteProjectFile))
                    {
                        var projectData = JObject.Parse(File.ReadAllText(absoluteProjectFile)) as JObject;
                        var projects = projectData["Projects"] as JObject;

                        foreach(var projectJson in projects)
                        {
                            projectJson.Value["Folder"] = absoluteProjectFolder;
                        }
                        root.Merge(projectData);
                    }
                }
            }
        }

        public static void FindSourceFiles(JObject root, Options options)
        {
            if(!root.TryGetValue("Projects", out var projectsToken)) throw new ArgumentException("Projects object not found in model");
            if(!(projectsToken is JObject projects) || !projects.TryGetValue(options.Project, out var projectToken)) throw new ArgumentException($"Project '{options.Project}' not found");
            if(!(projectToken is JObject project) || !project.TryGetValue("Folder", out var pathToken)) throw new ArgumentException($"Project path for '{options.Project}' not found");
            if(!root.TryGetValue("Rules", out var rulesToken) || !(rulesToken is JObject rules)) throw new ArgumentException($"Rules object not found in model");
            if(!root.TryGetValue("Rules", out var rulesToken) || !(rulesToken is JObject rules)) throw new ArgumentException($"Rules object not found in model");

            var projectPath = pathToken.Value<string>();
            foreach(var rule in rules.OfType<JObject>())
            {
                var fileTypes = rule["FileTypes"].Value<string>().Split(';').Select(x => x.Trim());


            }
        }

        public static void LoadBases(JObject root, Options options)
        {
            foreach(var baseDir in options.Bases)
            {
                var files = Directory.GetFiles(baseDir, "*.json", SearchOption.AllDirectories);
                foreach(var file in files)
                {
                    var projectData = JObject.Parse(File.ReadAllText(Path.Combine(baseDir, file))) as JObject;
                    root.Merge(projectData);
                }
            }
        }
    }
}