angular.module('plaza').service('entityService', function ($http, $timeout, eventService) {
    var service = {};

    service.mergeObject = function (original, newObject) {
        // First, remove all properties that are no longer part of newObject
        if (Array.isArray(newObject)) {
            original.length = newObject.length;
        } else {
            for (var key in original) {
                if (key[0] !== '$' && newObject[key] === undefined) {
                    delete original[key];
                }
            }
        }

        for (var key in newObject) {
            if (typeof newObject[key] == 'object' && newObject[key] != null) {
                if (original[key] === undefined) {
                    original[key] = newObject[key];
                }
                service.mergeObject(original[key], newObject[key]);
            } else {
                original[key] = newObject[key];
            }
        }
    }

    service.getParentPath = function (path) {
        var lastSlashIndex = path.lastIndexOf("/");
        if (lastSlashIndex < 0) return path;

        return path.substring(0, lastSlashIndex);
    }

    function findOrCreateIndex(arr, name) {
        var index = -1;
        for (var i = 0; i < arr.length; ++i) {
            if (arr[i].Name == name) {
                index = i;
                break;
            }
        }

        if (index == -1) {
            index = arr.length;
            arr.push({
                Name: name
            });
        }
        return index;
    }

    function organize(entity, parent) {
        if (entity.$children === undefined) {
            entity.$isLeaf = true;
            entity.$children = [];
        }

        if (parent !== undefined) {
            entity.$parent = parent;
        } else {
            entity.$parent = undefined;
        }

        entity.$type = "element";

        for (var key in entity) {
            if (key[0] == '$') continue;

            var value = entity[key];

            if (Array.isArray(value)) {
                entity.$isLeaf = false;

                var children = entity.$children[findOrCreateIndex(entity.$children, key)];
                children.$children = value;
                children.$isLeaf = true;
                children.$parent = entity;
                children.$type = "folder";

                for (var i = 0; i < value.length; ++i) {
                    organize(value[i], children);
                }
            }
        }
    }

    service.error = function(err) {
        throw err;
    }

    service.createConnection = function (endpoint) {
        var connection = {
            root: {},
            endpoint: endpoint,
            isConnected: false
        };

        service.getEntity(connection, "").then(function (root) {
            connection.modules = {};
            connection.components = {};
            connection.properties = {};

            service.parseComponentModel(
                connection.root,
                connection.modules,
                connection.components,
                connection.properties
            );

            service.update(connection);

            var infiniteUpdate = function () {
                if (connection.updateTask) {
                    $timeout(function() {
                        connection.updateTask = service.update(connection).then(infiniteUpdate, infiniteUpdate);
                    }, 500);
                }
            }

            connection.updateTask = true;

            infiniteUpdate();
        }, service.error);

        return connection;
    };

    service.close = function (connection) {
        if (connection) {
            connection.isConnected = false;
            connection.updateTask = null;
        }
    }

    service.findEntity = function (connection, path) {
        var root = connection.root;
        var pathElements = path.split("/");

        for (var i = 0; i < pathElements.length; ++i) {
            var element = pathElements[i];

            if (element.length == 0) {
                continue;
            }

            if (!Array.isArray(root)) {
                if (!root[element]) return undefined;

                root = root[element];
            } else {
                var index = parseInt(element);
                if (index.toString() === element) {
                    root = root[index];
                } else {
                    for (var j = 0; j < root.length; ++j) {
                        if (root[j].Name == element) {
                            root = root[j];
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        return undefined;
                    }
                }

            }
        }

        return root;
    }

    service.invalidate = function (entity) {
        entity.$invalidated = true;

        if (entity.$parent && (!entity.$components || !entity.$components.includes("PersistancePoint"))) {
            service.invalidate(entity.$parent);
        }
    }

    service.save = function (connection, entity) {
        if(entity.$components && entity.$components.includes("PersistancePoint")) {
            service.updateEntity(connection, service.calculatePath(entity), { 'PersistancePointSaving': true }).then(function (response) {
                entity.$invalidated = false;
            }, service.error);
        }

        if (entity.$children) {
            entity.$children.forEach(service.save);
        }
    }

    service.getChanges = function (connection) {
        return $http.get(connection.endpoint + '/changes').then(function (response) {
            connection.isConnected = true;

            var changedEntities = response.data.EntityModifications;
            for (var i = 0; i < changedEntities.length; ++i) {
                var changedEntity = changedEntities[i].EntityModificationEntity;
                var entity = service.findEntity(connection, changedEntity.$path);

                if (entity !== undefined) {
                    service.mergeObject(entity, changedEntity);

                    service.invalidate(entity);

                    if (changedEntity.$path.length == 0) { // Root entity
                        organize(entity);
                    } else {
                        organize(entity, entity.$parent);
                    }
                }
            }

            return entity;
        }, service.error);
    }

    service.update = function (connection) {
        if (connection) {
            return service.getChanges(connection);
        }
    }

    service.getEntity = function (connection, path) {
        return $http.get(connection.endpoint + '/entity' + path).then(function (response) {
            connection.isConnected = true;

            var entity = service.findEntity(connection, path);

            if (entity === undefined) {
                return response.data; /// Entity retrieved, but parents has not been fetched yet
            }

            service.mergeObject(entity, response.data);

            if (path.length == 0) { // Root entity
                organize(entity);
            } else {
                organize(entity, entity.$parent);
            }

            return entity;
        }, service.error);
    }

    service.calculatePath = function (entity) {
        if (entity.Name === undefined) return "";

        if (entity.$parent !== undefined && entity.$parent !== entity) {
            if (entity.$type === "element") {
                return service.calculatePath(entity.$parent) + "/" + entity.$parent.$children.indexOf(entity);
            } else {
                return service.calculatePath(entity.$parent) + "/" + entity.Name;
            }

        } else {
            return "";
        }
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

    service.parseComponentModel = function (moduleRoot, moduleTable, componentTable, propertyTable) {
        var modules = moduleRoot['Modules'];
        for (var i = 0; i < modules.length; ++i) {
            moduleTable[modules[i].Name] = modules[i];

            var components = modules[i]['Components'];
            if (components) {
                for (var j = 0; j < components.length; ++j) {
                    componentTable[components[j].Name] = components[j];

                    var properties = components[j]["Properties"];
                    for (var k = 0; k < properties.length; ++k) {
                        propertyTable[properties[k].Name] = properties[k];
                    }
                }
            }
        }
    }

    return service;
});