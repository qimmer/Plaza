//
// Created by Kim on 15/07/2018.
//

#include "NetworkingModule.h"
#include <Networking/HttpRequest.h>
#include <Networking/HttpServer.h>
#include <Networking/Server.h>
#include <Networking/TcpStream.h>
#include <Foundation/NativeUtils.h>
#include <Foundation/AppLoop.h>
#include <Json/NativeUtils.h>
#include <Json/JsonPersistance.h>
#include <Json/JsonModule.h>
#include <Foundation/FoundationModule.h>

BeginModule(Networking)
    RegisterDependency(Core)
    RegisterDependency(Foundation)
    RegisterDependency(Json)

	RegisterUnit(Server)
	RegisterUnit(TcpStream)
    RegisterUnit(HttpRequest)
    RegisterUnit(HttpServer)
    RegisterUnit(Networking)

    ModuleData(
        {
            "TcpWaitOnNoWork": false,
            "HttpResponseCodes": [
                { "HttpResponseCodeNumber": 200, "HttpResponseCodeMessage": "OK" },
                { "HttpResponseCodeNumber": 201, "HttpResponseCodeMessage": "Created" },
                { "HttpResponseCodeNumber": 202, "HttpResponseCodeMessage": "Accepted" },
                { "HttpResponseCodeNumber": 204, "HttpResponseCodeMessage": "No Content" },

                { "HttpResponseCodeNumber": 301, "HttpResponseCodeMessage": "Moved Permanently" },
                { "HttpResponseCodeNumber": 302, "HttpResponseCodeMessage": "Found" },
                { "HttpResponseCodeNumber": 303, "HttpResponseCodeMessage": "See Other" },
                { "HttpResponseCodeNumber": 304, "HttpResponseCodeMessage": "Not Modified" },
                { "HttpResponseCodeNumber": 307, "HttpResponseCodeMessage": "Temporary Redirect" },

                { "HttpResponseCodeNumber": 400, "HttpResponseCodeMessage": "Bad Request" },
                { "HttpResponseCodeNumber": 401, "HttpResponseCodeMessage": "Unauthorized" },
                { "HttpResponseCodeNumber": 403, "HttpResponseCodeMessage": "Forbidden" },
                { "HttpResponseCodeNumber": 404, "HttpResponseCodeMessage": "Not Found" },
                { "HttpResponseCodeNumber": 405, "HttpResponseCodeMessage": "Method Not Allowed" },
                { "HttpResponseCodeNumber": 406, "HttpResponseCodeMessage": "Not Acceptable" },
                { "HttpResponseCodeNumber": 412, "HttpResponseCodeMessage": "Precondition Failed" },
                { "HttpResponseCodeNumber": 415, "HttpResponseCodeMessage": "Unsupported Media Type" },

                { "HttpResponseCodeNumber": 500, "HttpResponseCodeMessage": "Internal Server Error" },
                { "HttpResponseCodeNumber": 501, "HttpResponseCodeMessage": "Not Implemented" }
            ],

            "HttpHeaderTypes": [
                { "HttpHeaderTypeIdentifier": "Content-Type" },
                { "HttpHeaderTypeIdentifier": "Content-Length" },
                { "HttpHeaderTypeIdentifier": "Host" },
                { "HttpHeaderTypeIdentifier": "Origin" },
                { "HttpHeaderTypeIdentifier": "Connection" },
                { "HttpHeaderTypeIdentifier": "Cache-Control" },
                { "HttpHeaderTypeIdentifier": "Upgrade-Insecure-Requests" },
                { "HttpHeaderTypeIdentifier": "User-Agent" },
                { "HttpHeaderTypeIdentifier": "Accept" },
                { "HttpHeaderTypeIdentifier": "Accept-Encoding" },
                { "HttpHeaderTypeIdentifier": "Accept-Language" },
                { "HttpHeaderTypeIdentifier": "Pragma" },
                { "HttpHeaderTypeIdentifier": "Referer" },
                { "HttpHeaderTypeIdentifier": "Keep-Alive" },
                { "HttpHeaderTypeIdentifier": "Location" },
                { "HttpHeaderTypeIdentifier": "Server" },
				{ "HttpHeaderTypeIdentifier": "X-Requested-With" }
            ]
        }
    )
EndModule()

BeginUnit(Networking)
    BeginComponent(Networking)
        RegisterArrayProperty(HttpHeaderType, HttpHeaderTypes)
        RegisterArrayProperty(HttpResponseCode, HttpResponseCodes)
        RegisterProperty(bool, TcpWaitOnNoWork)
    EndComponent()
EndUnit()