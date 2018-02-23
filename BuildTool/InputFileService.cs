using System.IO;
using System.Linq;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace BuildTool
{
    public static class InputFileService
    {
        public static void LoadRespositoryProjects(JObject root, string[] repositoryPaths)
        {
            foreach(var repositoryPath in repositoryPaths)
            {
                foreach(var projectFolder in Directory.EnumerateDirectories(repositoryPath, "*", SearchOption.TopDirectoryOnly))
                {
                    var absoluteProjectFolder = Path.Combine(repositoryPath, projectFolder);
                    var absoluteProjectFile = Path.Combine(absoluteProjectFolder, "project.json");
                    if(File.Exists(absoluteProjectFile))
                    {
                        var projectData = JObject.Parse(File.ReadAllText(absoluteProjectFile)) as JObject;
                        foreach(var projectJson in projectData["Projects"] as JObject)
                        {
                            projectJson.Value["Folder"] = absoluteProjectFolder;
                        }
                        root.Merge(projectData);
                    }
                }
            }
        }

        public static void LoadBase(string baseDir)
        {
            
        }
    }
}