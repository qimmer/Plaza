angular.module('plaza')
    .directive('propertyEditor', function ($rootScope, entityService) {

        var componentPriorities = {
            Identification: 0,
            Stream: 1,
        }

        var hiddenComponents = [
            'Ownership'
        ];

        return {
            restrict: 'AE',
            controller: function($scope, $element, $attrs) {
                $scope.collapsedComponents = [];
                $scope.draft = {};
                $scope.isDraftLocked = false;
    
                $scope.updateDraft = function() {
                    if($scope.connection && !$scope.isDraftLocked) {
                        $scope.draft = {};
                        for (var key in $scope.connection.selectedEntity) {
                            if (key[0] == "$" || Array.isArray($scope.connection.selectedEntity[key])) continue;
                            $scope.draft[key] = $scope.connection.selectedEntity[key];
                        }
                    }
                }
    
                $scope.applyDraft = function() {
                    var selectedEntity = $scope.connection.selectedEntity;
                    var draft = $scope.draft;
                    entityService.updateEntity($scope.connection, entityService.calculatePath($scope.connection.selectedEntity), $scope.draft).then(function(){
                        for(var key in draft) {
                            selectedEntity[key] = draft[key];
                        }
                    });
                }
    
                $scope.hasComponent = function (item) {
                    var componentList = $scope.connection.selectedEntity['$components'];
                    return componentList && componentList[name];
                }
                
                $scope.componentComparator = function (c1, c2) {
                    var c1p = componentPriorities[c1] || 9999;
                    var c2p = componentPriorities[c2] || 9999;
                    return (c1p < c2p) ? -1 : 1;
                };
    
                $scope.filterComponents = function () {
                    if($scope.connection) {
                        if (!$scope.connection.selectedEntity || !$scope.connection.components) {
                            $scope.filteredComponents = [];
                            return;
                        }
    
                        var availableComponents = Object.keys($scope.connection.components);
                        var entityComponents = $scope.connection.selectedEntity['$components'];
    
                        $scope.filteredComponents = availableComponents
                            .filter(function (componentName) {
                                return entityComponents.includes(componentName) && !hiddenComponents.includes(componentName);
                            })
                            .sort($scope.componentComparator);
                    }
                    
                }
    
                $scope.$watch('connection.components', $scope.filterComponents);
                $scope.$watch('connection.selectedEntity.$components', $scope.filterComponents, true);
                $scope.$watch('connection.selectedEntity', function() {
                    $scope.updateDraft();
                }, true);
    
            },
            scope: {
                connection: "="  
            },
            templateUrl: 'App/Directives/PropertyEditor.html'
        };
    });