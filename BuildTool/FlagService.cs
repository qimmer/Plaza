using System;
using System.Collections.Generic;
using System.Linq;
using Newtonsoft.Json.Linq;

namespace BuildTool
{
    public static class FlagService
    {
        public const string ConditionsFieldName = "Conditions";

        public static bool AreConditionsMet(JObject o)
        {
            if(!o.ContainsKey(ConditionsFieldName))
            {
                return true;
            }

            return o[ConditionsFieldName].Value<string>().Split(';').Select(x => x.Trim().ToLower()).All(x => x[0] == '!' ? !GetCurrentFlags().Contains(x.Substring(1).ToLower()) : GetCurrentFlags().Contains(x.Substring(1).ToLower()));
        }

        public static string[] GetCurrentFlags()
        {
            return Environment.GetCommandLineArgs();
        }
    }
}