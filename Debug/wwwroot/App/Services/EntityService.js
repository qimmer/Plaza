angular.module('plaza').service('entityService', function($http, $timeout, $rootScope, eventService) {
    var service = {
        connections: {}
    };

    service.createConnection = function(scope, ip) {
        var conn = {
            onEntityReceived: eventService.event(),
            scope: scope,
            ip: ip,
            isConnected: false,
            updateTimeout: $timeout(function() {
                service.update(conn);
            }, 1000)
        };

        if(service.connections[scope] !== undefined) {
            service.close(service.connections[scope]);
        }
        
        service.connections[scope] = conn;

        scope.$on('$destroy', function() {
            service.close(conn);
        });

        return conn;
    };

    service.close = function(conn) {
        conn.updateTimeout.cancel();
        conn.isConnected = false;
        delete conn.onEntityReceived;
        delete service.connections[conn.scope];
    }

    service.update = function(conn) {
        if(conn.ip) {
            service.getEntity(conn, "/").then(function() {
                conn.updateTimeout = $timeout(function() {
                    //service.update(conn);
                }, 1000);
            }, function (err) {
                conn.updateTimeout = $timeout(function() {
                    service.update(conn);
                }, 1000);
            });
        } else {
            conn.updateTimeout = $timeout(function() {
                service.update(conn);
            }, 1000);
        }
    }

    service.getEntity = function(conn, path) {
        return $http.get('http://' + conn.ip + ':8080/api' + path).then(function(response) {
            conn.isConnected = true;
            conn.onEntityReceived.emit(response.data);
            return response.data;
        }, function(err) {
            conn.isConnected = false;
            throw err;
        });
    }

    function parsePropertiesComponent(list, component) {
        var properties = component["Properties"];
        for(var i = 0; i < properties.length; ++i) {
            list.push(properties[i]);
        }
    }

    function parsePropertiesModule(componentTable, module) {
        var components = module['Components'];
        
        for(var i = 0; i < components.length; ++i) {
            var propertyList = []
            parsePropertiesComponent(propertyList, components[i]);
            componentTable[components[i].Name] = propertyList;
        }
        
    }

    function parsePropertiesModuleRoot(componentTable, moduleRoot) {
        var modules = moduleRoot['Modules'];
        for(var i = 0; i < modules.length; ++i) {
            parsePropertiesModule(componentTable, modules[i]);
        }
    }

    service.parseProperties = function(moduleRoot) {
        var componentTable = {};
        parsePropertiesModuleRoot(componentTable, moduleRoot);
        return componentTable;
    }
    
    return service;
});