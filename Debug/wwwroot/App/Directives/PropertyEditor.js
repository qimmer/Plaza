angular.module('plaza')
    .directive('propertyEditor', function ($rootScope, entityService) {

        var componentPriorities = {
            Identification: 0,
            Stream: 1,
        }

        var hiddenComponents = [
            'Ownership',
            'Replication'
        ];

        return {
            restrict: 'AE',
            controller: function($scope, $element, $attrs) {
                $scope.types = [
                    { value: "", name: "<None>" },
                    { value: "void", name: "void" },
                    { value: "u8", name: "unsigned char" },
                    { value: "u16", name: "unsigned short" },
                    { value: "u32", name: "unsigned int" },
                    { value: "u64", name: "unsigned long long" },
                    { value: "s8", name: "char" },
                    { value: "s16", name: "short" },
                    { value: "s32", name: "int" },
                    { value: "s64", name: "long long" },
                    { value: "float", name: "float" },
                    { value: "double", name: "double" },
                    { value: "bool", name: "bool" },
                    { value: "StringRef", name: "string" },

                    { value: "v2i", name: "int2" },
                    { value: "v3i", name: "int3" },
                    { value: "v4i", name: "int4" },

                    { value: "v2f", name: "float2" },
                    { value: "v3f", name: "float3" },
                    { value: "v4f", name: "float4" },

                    { value: "m3x3f", name: "float3x3" },
                    { value: "m4x4f", name: "float4x4" },

                    { value: "Entity", name: "Entity" },
                    { value: "Type", name: "Type" },

                    { value: "rgba8", name: "rgba8" },
                    { value: "rgb8", name: "rgb8" },

                    { value: "rgba32", name: "rgba32" },
                    { value: "rgb32", name: "rgb32" },

                    { value: "Variant", name: "Variant" }
                ];

                $scope.selectedEntityHistory = [];
                $scope.selectedEntityHistoryCurrentIndex = -1;

                $scope.collapsedComponents = [];
                $scope.draft = {};
                $scope.isDraftLocked = false;
                $scope.expandedArrays = {};

                $scope.pickEntity = function() {

                }

                $scope.addChild = function(propertyName) {

                    entityService.createEntity($scope.connection, $scope.draft.$path + '/' + propertyName + '/' + $scope.selectedEntities[0][propertyName].length).then(function() {
                        $scope.updateDraft();
                    });
                }

                $scope.lockDraft = function() {
                    $scope.isDraftLocked = true;
                }
                
                $scope.updateDraft = function() {
                    if($scope.connection && !$scope.isDraftLocked && $scope.selectedEntities.length > 0) {
                        $scope.draft = angular.copy($scope.connection.getEntity($scope.selectedEntities[0]));
                    }
                }
    
                $scope.isComponentVisible = function(name) {
                    return hiddenComponents.indexOf(name) == -1;
                }
                $scope.applyDraft = function() {
                    var entities = $scope.selectedEntities.map(function(path) { return $scope.connection.getEntity(path); });
                    
                    var draft = angular.copy($scope.draft);
                    for(var key in draft) {
                            if(key[0]=='$') delete draft[key];
                    }
                    
                    $scope.isDraftLocked = false;

                    for(var i = 0; i < entities.length; ++i) {
                        entityService.updateEntity($scope.connection, entities[i].$path, $scope.draft);
                    }
                }

                $scope.keyPress = function(keyCode) {
                    switch(keyCode) {
                        case 13: // Enter
                            $scope.applyDraft();
                            break;
                    }
                }

                $scope.getProperties = function(componentName) {
                    var component = $scope.connection.getComponents()[componentName];
                    if(!component || !component.Properties) return [];
                    
                    return $scope.connection.getEntities(component.Properties);
                }

                $scope.componentComparator = function (c1, c2) {
                    var c1p = componentPriorities[c1] || 9999;
                    var c2p = componentPriorities[c2] || 9999;
                    return (c1p < c2p) ? -1 : 1;
                };
    
                $scope.navigateBack = function() {
                    $scope.selectedEntityHistory.length--;
                    $scope.selectedEntities[0] = $scope.selectedEntityHistory[$scope.selectedEntityHistory.length - 1];
                }

                $scope.navigateUp = function() {
                    var entity = $scope.connection.getEntity($scope.selectedEntities[0]);
                    $scope.selectedEntities[0] = entity.$owner;
                }

                $scope.$watch('selectedEntities[0]', function() {
                    if($scope.selectedEntityHistory[$scope.selectedEntityHistory.length - 1] != $scope.selectedEntities[0]) {
                        $scope.selectedEntityHistory.push($scope.selectedEntities[0]);
                    }
                        
                    $scope.updateDraft();
                });
    
                $scope.$watch(function() {
                    return $scope.connection.getEntity($scope.draft.$path);
                }, $scope.updateDraft);
            },
            scope: {
                connection: "="  ,
                selectedEntities: "="
            },
            templateUrl: 'App/Directives/PropertyEditor.html'
        };
    });