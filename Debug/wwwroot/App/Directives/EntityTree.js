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

                $scope.$watchCollection('selectedEntities', function () {
                    if($scope.selectedEntities.length > 0) {

                        var parent = $scope.connection.getEntity($scope.selectedEntities[0]).$owner;
                        while(parent !== undefined) {
                            if($scope.expandedNodes.indexOf(parent) == -1) {
                                $scope.expandedNodes.push(parent);
                            }

                            var owner = $scope.connection.getEntity($scope.selectedEntities[0]).$owner;
                            if(parent === owner) {
                                break;
                            }
                            parent = $scope.connection.getEntity($scope.selectedEntities[0]).$owner;
                        }
                    }
                });

                $scope.getIcon = function (connection, uuid) {
                    if(connection) {
                        var entity = connection.getEntity(uuid);
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