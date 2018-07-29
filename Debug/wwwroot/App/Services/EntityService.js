angular.module('plaza').service('entityService', function($http, $timeout, $rootScope, eventService) {
    var service = {};

    service.mergeObject = function(original, newObject) {
        // First, remove all properties that are no longer part of newObject
        if(Array.isArray(newObject)) {
            original.length = newObject.length;
        } else {
            for(var key in original) {
                if(key[0] !== '$' && newObject[key] === undefined) {
                    delete original[key];
                }
            }    
        }
        
        for(var key in newObject) {
            if(typeof newObject[key] == 'object') {
                if(original[key] === undefined) {
                    original[key] = newObject[key];
                }
                service.mergeObject(original[key], newObject[key]);
            } else {
                original[key] = newObject[key];
            }
        }
    }

    service.getParentPath = function(path) {
        var lastSlashIndex = path.lastIndexOf("/");
        if(lastSlashIndex < 0) return path;

        return path.substring(0, lastSlashIndex);
    }

    function findOrCreateIndex(arr, name) {
        var index = -1;
        for(var i = 0; i < arr.length; ++i) {
            if(arr[i].Name == name) {
                index = i;
                break;
            }
        }

        if(index == -1) {
            index = arr.length;
            arr.push({
                Name: name
            });
        }
        return index;
    }
    
    function organize(entity, parent) {
        if(entity.$children === undefined) {
            entity.$isLeaf = true;
            entity.$children = [];
        }

        if(parent !== undefined) {
            entity.$parent = parent;
        } else {
            entity.$parent = undefined;
        }
        
        entity.$type = "element";

        for(var key in entity) {
            if(key[0] == '$') continue;

            var value = entity[key];
            
            if(Array.isArray(value)) {
                entity.$isLeaf = false;

                var children = entity.$children[findOrCreateIndex(entity.$children, key)];
                children.$children = value;
                children.$isLeaf = true;
                children.$parent = entity;
                children.$type = "folder";

                for(var i = 0; i < value.length; ++i) {
                    organize(value[i], children);
                }
            }
        }
    }

    service.createConnection = function(scope) {
        if(scope.connection !== undefined) {
            service.close(scope);
        } else {
            scope.connection = {
                root: {}
            };
        }

        window.root = scope.connection.root;

        if(!scope.connection.ip) {
            scope.connection.ip = "localhost";
        }
        
        scope.connection.isConnected = false;

        scope.$on('$destroy', function() {
            service.close(scope);
        });

        scope.$watch('connection.ip', function() {
            service.getEntity(scope, "").then(function(root) {
                scope.connection.properties = service.parseProperties(scope.connection.root);
                service.update(scope);
            });
        });

        var infiniteUpdate = function() {
            if(scope.connection.updateTask) {
                scope.connection.updateTask = service.update(scope).then(infiniteUpdate, infiniteUpdate);
            }
        }
        
        scope.connection.updateTask = true;

        infiniteUpdate();
    };

    service.close = function(scope) {
        if(scope.connection) {      
            scope.connection.isConnected = false;
            scope.connection.updateTask = null;
        }        
    }

    service.findEntity = function(scope, path) {
        var root = scope.connection.root;
        var pathElements = path.split("/");

        for(var i = 0; i < pathElements.length; ++i) {
            var element = pathElements[i];

            if(element.length == 0) {
                continue;
            }

            if(!Array.isArray(root)) {   
                if(!root[element]) return undefined;

                root = root[element];     
            } else {
                var index = parseInt(element);
                if(index.toString() === element) {
                    root = root[index];    
                } else {
                    for(var j = 0; j < root.length; ++j) {
                        if(root[j].Name == element) {
                            root = root[j];
                            found = true;
                            break;
                        }
                    }
                    if(!found) {
                        return undefined;
                    }
                }               
                
            }
        }

        return root;
    }

    service.update = function(scope) {
        if(scope.connection && scope.connection.ip) {
            return service.getEntity(scope, "/Modules/Debug/EntityTracker").then(function(tracker) {
                var modifiedEntities = tracker.EntityModifications;

                var getJobs = [
                ];

                // Update changed entities
                for(var i = 0; i < modifiedEntities.length; ++i) {
                    var path = modifiedEntities[i].EntityModificationEntity;
                    var modificationPath = "/Modules/Debug/EntityTracker/EntityModifications/" + modifiedEntities[i].Name;
                    getJobs.push(service.getEntity(scope, path));
                    getJobs.push(service.deleteEntity(scope, modificationPath));
                }

                if(scope.connection.selectedEntity) {
                    getJobs.push(service.getEntity(scope, service.calculatePath(scope.connection.selectedEntity)));
                }

                return Promise.all(getJobs);                
            });
        }
    }

    service.getEntity = function(scope, path) {
        return $http.get('http://' + scope.connection.ip + ':8080/api' + path).then(function(response) {
            scope.connection.isConnected = true;

            var entity = service.findEntity(scope, path);

            if(entity === undefined) {
                return response.data; /// Entity retrieved, but parents has not been fetched yet
            }

            service.mergeObject(entity, response.data);

            if(path.length == 0) { // Root entity
                organize(entity);
            } else {
                organize(entity, entity.$parent);
            }

            return entity;
        });
    }

    service.calculatePath = function(entity) {
        if(entity.Name === undefined) return "";

        if(entity.$parent !== undefined && entity.$parent !== entity) {
            if(entity.$type === "element") {
                return service.calculatePath(entity.$parent) + "/" + entity.$parent.$children.indexOf(entity);    
            } else {
                return service.calculatePath(entity.$parent) + "/" + entity.Name;
            }
            
        } else {
            return "";
        }        
    }

    service.updateEntity = function(scope, path, newEntity) {
        return $http.put('http://' + scope.connection.ip + ':8080/api' + path, newEntity).then(function(response) {
            scope.connection.isConnected = true;
        });
    }

    service.createEntity = function(scope, path) {
        return $http.post('http://' + scope.connection.ip + ':8080/api' + path).then(function(response) {
            scope.connection.isConnected = true;
        });
    }

    service.deleteEntity = function(scope, path) {
        return $http.delete('http://' + scope.connection.ip + ':8080/api' + path).then(function(response) {
            scope.connection.isConnected = true;
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