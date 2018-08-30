angular.module('plaza')
    .directive('entityTree', function ($rootScope, entityService) {
        var propertyIconMap = {
            ModuleRoot: 'im im-sitemap icon-red',
            Module: 'im im-sitemap icon-red',
            
            Component: 'im im-cubes icon-cyan',
            Extension: 'im im-plugin icon-cyan',
            Base: 'im im-sitemap icon-cyan',

            Property: 'im im-pencil icon-blue',

            Function: 'im im-gear icon-red',
            Event: 'im im-flash icon-yellow',
            Subscription: 'im im-flag icon-orange',

            Enum: 'im im-star icon-green',
        }

        return {
            restrict: 'AE',
            scope: {
                connection: "=",
                selectedEntities: "=",
                onOpen: "&?"
            },
            templateUrl: 'App/Directives/EntityTree.html',
            controller: function ($scope, $element, $attrs) {
                $scope.expandedNodes = [''];
                $scope.filterText = "";
/*
                $scope.contextMenu = [
                    { // Add (folder)
                        text: function () { return 'Add ' + $scope.connection.selectedNode.Name; },
                        click: function () {
                            entityService.createEntity($scope.connection, $scope.connection.selectedNode.$path + "/" + findUniqueName($scope.connection.selectedNode, $scope.connection.selectedNode.Name));
                            return true;
                        },
                        displayed: isFolder
                    },
                    { // Destroy (element)
                        text: 'Destroy',
                        click: function () {
                            entityService.deleteEntity($scope.connection, $scope.connection.selectedNode.$path);
                        },
                        displayed: isElement
                    },
                    null,
                    { // Add Component
                        text: "Add",
                        displayed: isElement,
                        children: function () {
                            var moduleList = [];
                            for (var moduleName in $scope.connection.modules) {
                                var componentList = [];
                                var components = $scope.connection.modules[moduleName].Components;
                                if (components) {
                                    components.forEach(function (component) {
                                        if (!$scope.connection.selectedEntity.$components.includes(component.Name)) {
                                            componentList.push({
                                                text: component.Name,
                                                click: function () { entityService.addComponent($scope.connection, $scope.connection.selectedNode.$path, component.Name); }
                                            });
                                        }
                                    });
                                }

                                if (componentList.length > 0) {
                                    moduleList.push({
                                        text: moduleName,
                                        children: componentList
                                    });
                                }
                            }
                            return moduleList.sort(function (a, b) { if (a.text < b.text) { return -1; } if (a.text > b.text) { return 1; } return 0; });
                        }
                    },
                    { // Remove Component
                        text: "Remove",
                        displayed: isElement,
                        children: function () {
                            var componentList = [];
                            for (var componentName in $scope.connection.components) {
                                if ($scope.connection.selectedEntity.$components.includes(componentName)) {
                                    componentList.push({
                                        text: componentName,
                                        click: function () { entityService.removeComponent($scope.connection, $scope.connection.selectedNode.$path, componentName); }
                                    });
                                }
                            }
                            return componentList;
                        }
                    }
                ];
*/
                $scope.$watchCollection('selectedEntities', function () {
                    if($scope.selectedEntities.length > 0) {

                        var parent = entityService.getParentPath($scope.selectedEntities[0]);
                        while(parent !== undefined) {
                            if($scope.expandedNodes.indexOf(parent) == -1) {
                                $scope.expandedNodes.push(parent);
                            }

                            parent = entityService.getParentPath(parent);
                        }
                    }
                });

                $scope.getIcon = function (connection, nodePath) {
                    if(connection) {
                        var entity = connection.getEntity(nodePath);
                        if(entity && entity.$components) {
                            for (var i = 0; i < entity.$components.length; ++i) {
                                var icon = propertyIconMap[entity.$components[i]];
                                if (icon !== undefined) {
                                    return icon;
                                }
                            }
                        }
                    }

                    return 'im im-cube';
                }
            },
            link: function ($scope, $element, $attrs) {

            }
        };
    });