angular.module('plaza').service('entityService', function ($http, $timeout, eventService) {
    var service = {};

    service.getParentPath = function (path) {
        var lastSlashIndex = path.lastIndexOf("/");
        if (lastSlashIndex < 0) return undefined;

        return path.substring(0, lastSlashIndex);
    }

    function mergeEntity(connection, entity) {
        for(var key in entity) {
            var value = entity[key];
            if(Array.isArray(value)) {
                for(var i = 0; i < value.length; ++i) {
                    if(typeof value[i] === 'object') {
                        mergeEntity(connection, value[i]);
                        value[i] = value[i].$path;
                    }
                }
            } else if (typeof value === 'object' && value !== null) {
                mergeEntity(connection, value);
                entity[key] = entity.$path;
            }
        }

        connection.getEntities()[entity.$path] = entity;

        if(entity.$components.indexOf('Component') != -1) {
            connection.getComponents()[entity.Name] = entity;

            connection.getEntities(connection.getChildren(entity.$path));
        }

        if(entity.$components.indexOf('Module') != -1) {
            connection.getModules()[entity.Name] = entity;

            connection.getEntities(connection.getChildren(entity.$path));
        }

        if(entity.$components.indexOf('Property') != -1) {
            connection.getProperties()[entity.Name] = entity;

            connection.getEntities(connection.getChildren(entity.$path));
        }
    }

    service.error = function(err) {
        throw err;
    }
    
    service.findUniqueName = function(parent, name) {
        var extensionIndex = 1;
        var suggestion = name + "_" + extensionIndex;
        while (true) {
            var found = false;
            for (var i = 0; i < parent.$children.length; ++i) {
                if (parent.$children[i].Name === suggestion) {
                    found = true;
                }
            }
            if (found) {
                extensionIndex++;
                suggestion = name + "_" + extensionIndex;
            } else {
                break;
            }
        }
        return suggestion;
    }

    service.createConnection = function (endpoint) {
        var root = {};
        var entities = {
            "": root
        };
        var modules = {};
        var components = {};
        var properties = {};

        var connection = {
            getRoot: function() { return root; },
            getEntities: function(paths) {
                if(paths === undefined) {
                    return entities; 
                }
                
                return paths.map(function(path) { return connection.getEntity(path); } );
            },
            getModules: function() { return modules; },
            getComponents: function() { return components; },
            getProperties: function() { return properties; },
            getEntity: function(path) { 
                var entity = entities[path];
                if(!entity) {
                    entity = { $loading: true };
                    entities[path] = entity;
                    service.getEntity(connection, path);
                    return entity;
                }
                return entity;
            },
            getChildren: function(path) { 
                var entity = connection.getEntity(path);
                if(!entity) {
                    console.log("Entity not found: " + path);
                    return [];
                }
                var children = [];
                for(var key in entity) {
                    if(key[0] == '$') continue;
                    if(!Array.isArray(entity[key])) continue;
                    children = children.concat(entity[key]);
                }
                
                return children; 
            },

            endpoint: endpoint,
            sessionId: null,
            isConnected: false
        };

        service.getSession(connection).then(function(id) {
            connection.sessionId = id;

            service.getEntity(connection, "").then(function (root) {    
                service.update(connection);
    
                var infiniteUpdate = function () {
                    if (connection.updateTask) {
                        $timeout(function() {
                            connection.updateTask = service.update(connection).then(infiniteUpdate, infiniteUpdate);
                        }, 250);
                    }
                }
    
                connection.updateTask = true;
    
                infiniteUpdate();
            }, service.error);
        });

        return connection;
    };

    service.close = function (connection) {
        if (connection) {
            connection.isConnected = false;
            connection.updateTask = null;
            connection.sessionId = null;
        }
    }

    service.invalidate = function (connection, path) {
        var entity = connection.getEntity(path);

        entity.$invalidated = true;

        if (entity.$parentPath && (!entity.$components || !entity.$components.includes("PersistancePoint"))) {
            service.invalidate(connection, connection.getEntity(entity.$parentPath));
        }
    }

    service.save = function (connection, path) {
        var entity = connection.getEntity(path);

        if(entity.$components && entity.$components.includes("PersistancePoint")) {
            service.updateEntity(connection, path, { 'PersistancePointSaving': true }).then(function (response) {
                entity.$invalidated = false;
            }, service.error);
        }

        if (entity.$children) {
            entity.$children.forEach(service.save);
        }
    }

    service.getSession = function(connection) {
        return $http.get(connection.endpoint + '/session').then(function (response) {
            return response.data;
        });
    }

    service.getChanges = function (connection) {
        return $http.get(connection.endpoint + '/changes/' + connection.sessionId).then(function (response) {
            connection.isConnected = true;

            var changedEntities = response.data.EntityModifications;
            for (var i = 0; i < changedEntities.length; ++i) {
                var changedEntity = changedEntities[i].EntityModificationEntity;

                if(changedEntity) {
                    mergeEntity(connection, changedEntity);
                }
            }
        }, service.error);
    }

    service.update = function (connection) {
        if (connection) {
            return service.getChanges(connection);
        }
    }

    service.getEntity = function (connection, path) {
        if(path === undefined) {
            console.log("Path cannot be undefined");
            return;
        }

        var entity = connection.getEntity(path);
        entity.$loading = true;

        return $http.get(connection.endpoint + '/entity' + path).then(function (response) {
            connection.isConnected = true;

            mergeEntity(connection, response.data);

            return entity;
        }, service.error);
    }

    service.addComponent = function (connection, path, componentName) {
        return $http.get(connection.endpoint + '/addcomponent/' + componentName + path).then(function (response) {
            connection.isConnected = true;
        }, service.error);
    }

    service.removeComponent = function (connection, path, componentName) {
        return $http.get(connection.endpoint + '/removecomponent/' + componentName + path).then(function (response) {
            connection.isConnected = true;
        }, service.error);
    }

    service.updateEntity = function (connection, path, newEntity) {
        return $http.put(connection.endpoint + '/entity' + path, newEntity).then(function (response) {
            connection.isConnected = true;
        }, service.error);
    }

    service.createEntity = function (connection, path) {
        return $http.post(connection.endpoint + '/entity' + path).then(function (response) {
            connection.isConnected = true;
        }, service.error);
    }

    service.deleteEntity = function (connection, path) {
        return $http.delete(connection.endpoint + '/entity' + path).then(function (response) {
            connection.isConnected = true;
        }, service.error);
    }

    return service;
});