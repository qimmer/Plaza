angular.module('plaza').service('entityService', function ($http, $timeout, $interval, eventService) {
    var service = {};

    function mergeEntity(connection, entity, parent, parentProperty) {
        var existing = connection.getEntities()[entity.Uuid];

        for(var key in entity) {
            if(key[0] == '$') continue;

            var value = entity[key];
            if(Array.isArray(value)) {
                for(var i = 0; i < value.length; ++i) {
                    if(typeof value[i] === 'object') {
                        value[i].$owner = entity.Uuid;
                        value[i].$property = key;
                        value[i].$index = i;

                        mergeEntity(connection, value[i], entity, key);
                        
                        value[i] = value[i].Uuid;
                    }
                }
            } else if (typeof value === 'object' && value !== null && value.$components) {
                mergeEntity(connection, value);
                value.$owner = entity.Uuid;
                value.$property = key;
                entity[key] = entity.Uuid;
            }
        }

        if(existing && existing.$components) {
            for(var i = 0; i < existing.$components.length; ++i) {
                var list = connection.getComponentList(existing.$components[i]);
                list.splice(list.indexOf(existing), 1);
            }
        }

        connection.getEntities()[entity.Uuid] = entity;

        for(var i = 0; i < entity.$components.length; ++i) {
            var list = connection.getComponentList(entity.$components[i]);
            list.push(entity);
        }

        entity.$time = moment().valueOf();
    }

    service.error = function(err) {
        throw err;
    }
    
    service.findUniqueName = function(connection, parent, propertyName, name) {
        var extensionIndex = 0;

        var suggestion = name;
        var childUuids = connection.getEntity(parent)[propertyName] || [];
        var childNames = childUuids.map(function(uuid) { return connection.getEntity(uuid).Name; });
        while (childNames.indexOf(suggestion) != -1) {
            extensionIndex++;
            suggestion = name + "_" + extensionIndex;
        }
        return parent + '/' + propertyName + '/' + suggestion;
    }

    service.createConnection = function (endpoint) {
        var entities = {};
        var componentLists = {};

        var connection = {
            getRoot: function() { return "00000000-0000-0000-0000-000000000000"; },
            getEntities: function(uuids) {
                if(!connection.isConnected) return {};

                if(uuids === undefined) {
                    return entities; 
                }
                
                return uuids.map(function(uuid) { return connection.getEntity(uuid); } );
            },
            getComponentList: function(componentUuid) {
                if(!connection.isConnected) return [];

                if(!componentUuid) return componentLists;

                var list = componentLists[componentUuid];
                if(list === undefined) {
                    componentLists[componentUuid] = [];
                    list = componentLists[componentUuid];
                }
                return list; 
            },
            getComponentUuid: function(componentName) {
                var components = Object.values(entities).filter(function(e) { return e.Name === componentName && e.Properties; });
                if(!components.length) return null;

                return components[0].Uuid;
            },
            getEntity: function(uuid) { 
                if(!connection.isConnected) return null;

                var entity = entities[uuid];
                if(!entity) {
                    entity = { $loading: true };
                    entities[uuid] = entity;
                    service.getEntity(connection, uuid);
                    return entity;
                }
                return entity;
            },
            getChildren: function(uuid) { 
                var entity = connection.getEntity(uuid);
                if(!entity) {
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
            reset: function() {
                entities = {};
                componentLists = {};
            },

            endpoint: endpoint,
            sessionId: null,
            isConnected: false
        };

        return connection;
    };

    service.update = function(connection) {
        var promise = null;
        if (connection.isConnected) {
            promise = service.getChanges(connection).then(function() {
                return new Promise(resolve => $timeout(resolve, 250));
            }, function(err) {throw err;});
        } else {
            promise = service.getSession(connection).then(function(id) {
                connection.isConnected = true;
                connection.sessionId = id;

                return service.getEntity(connection, connection.getRoot()).then(function (root) {    
                }, function(err) { throw err; });
            }, function(err) { throw err; });
        }

        return promise.catch(function(err) {
            connection.isConnected = false;
            connection.sessionId = null;
            connection.reset();

            return new Promise(resolve => $timeout(resolve, 1000));
        });
    }

    service.close = function (connection) {
        if (connection) {
            connection.isConnected = false;
            connection.sessionId = null;
        }
    }

    service.save = function (connection, uuid) {
        var persistancePointUuid = connection.getComponentUuid("PersistancePoint");
        var entity = connection.getEntity(uuid);

        while(entity && entity.$owner && !entity.$components.includes(persistancePointUuid)) {
            entity = connection.getEntity(entity.$owner);
        }

        if(entity && entity.$components && entity.$components.includes(persistancePointUuid)) {
            return $http.put(connection.endpoint + '/entity/' + entity.Uuid, { 'PersistancePointSaving': true }).then(function (response) {
                connection.isConnected = true;
            }, service.error);
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

    service.getEntity = function (connection, entityUuid) {
        if(entityUuid === undefined) {
            console.log("Uuid cannot be undefined");
            return;
        }

        var entity = connection.getEntity(entityUuid);
        entity.$loading = true;

        return $http.get(connection.endpoint + '/entity/' + entityUuid).then(function (response) {
            connection.isConnected = true;

            mergeEntity(connection, response.data);

            return entity;
        }, service.error);
    }

    service.addComponent = function (connection, entityUuid, componentUuid) {
        return $http.get(connection.endpoint + '/addcomponent/' + componentUuid + '/' +  entityUuid).then(function (response) {
            connection.isConnected = true;

            service.save(connection, entityUuid);
        }, service.error);
    }

    service.removeComponent = function (connection, entityUuid, componentUuid) {
        return $http.get(connection.endpoint + '/removecomponent/' + componentUuid + '/' + entityUuid).then(function (response) {
            connection.isConnected = true;

            service.save(connection, entityUuid);
        }, service.error);
    }

    service.updateEntity = function (connection, entityUuid, newEntity) {
        var entity = connection.getEntity(entityUuid);
        var oldName = entity.Name;
        var newName = newEntity.Name || oldName;

        return $http.put(connection.endpoint + '/entity/' + entityUuid, newEntity).then(function (response) {
            connection.isConnected = true;

            service.save(connection, entityUuid);
        }, service.error);
    }

    service.createChild = function (connection, entityUuid, propertyName) {
        return $http.post(connection.endpoint + '/entity/' + entityUuid + '/' + propertyName).then(function (response) {
            connection.isConnected = true;

            service.save(connection, entityUuid);
        }, service.error);
    }

    service.deleteEntity = function (connection, entityUuid) {
        var parent = connection.getEntity(entityUuid).$owner;

        return $http.delete(connection.endpoint + '/entity/' + entityUuid).then(function (response) {
            connection.isConnected = true;

            service.save(connection, parent);
        }, service.error);
    }

    return service;
});