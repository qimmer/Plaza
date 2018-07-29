angular.module('plaza')
    .directive('entityTree', function ($rootScope, entityService) {
        var propertyIconMap = {
            Components: 'im im-git',
            PropertyOffset: 'im im-pencil',
            Properties: 'im im-plugin',
        }

        function findUniqueName(parent, name) {
            var extensionIndex = 1;
            var suggestion = name + "_" + extensionIndex;
            while(true) {
                var found = false;
                for(var i = 0; i < parent.$children.length; ++i) {
                    if(parent.$children[i].Name === suggestion) {
                        found = true;
                    }
                }
                if(found) {
                    extensionIndex++;
                    suggestion = name + "_" + extensionIndex;
                } else {
                    break;
                }
            }
            return suggestion;
        }

        function link($scope, $element, $attrs) {

            $scope.expandedNodes = [];
            $scope.selectedNodes = [];
            $scope.filterText = "";

            $scope.contextMenu = [
                { // Add (folder)
                    text: function() { return 'Add ' + $scope.connection.selectedNode.Name; }, 
                    click: function () {
                        entityService.createEntity($scope, $scope.connection.selectedNode.$path + "/" + findUniqueName($scope.connection.selectedNode, $scope.connection.selectedNode.Name));
                        return true;
                    },
                    displayed: function() { return $scope.connection.selectedNode && $scope.connection.selectedNode.$type === "folder" }
                },
                { // Remove (element)
                    text: 'Remove', 
                    click: function () {
                        entityService.deleteEntity($scope, $scope.connection.selectedNode.$path);
                    },
                    displayed: function() { return $scope.connection.selectedNode && $scope.connection.selectedNode.$type === "element" }
                }
            ];

            $scope.$watch('connection.root', function () {
                if ($scope.expandedNodes.length == 0 && $scope.connection.root.$children && $scope.connection.root.$children.length > 0) {
                    $scope.expandedNodes.push($scope.connection.root.$children[0]);
                    for (var key in $scope.connection.root.$children[0].$children) {
                        $scope.expandedNodes.push($scope.connection.root.$children[0].$children[key]);
                    }
                }
            });

            $scope.toggle = function (node) {
                var index = $scope.expandedNodes.indexOf(node);
                if (index > -1) {
                    $scope.expandedNodes.splice(index, 1);
                } else {
                    $scope.expandedNodes.push(node);
                }

                $scope.connection.selectedNode = node;
                $scope.connection.selectedEntity = ($scope.connection.selectedNode['$components']) === undefined ? $scope.connection.selectedNode.$parent : $scope.connection.selectedNode;
            }

            $scope.getIcon = function (node) {
                if (!node.$components) {
                    if ($scope.expandedNodes.includes(node)) {
                        return 'im im-folder-open';
                    } else {
                        return 'im im-folder';
                    }
                }

                for (var property in node) {
                    var icon = propertyIconMap[property];
                    if (icon !== undefined) {
                        return icon;
                    }
                }

                return 'im im-cube';
            }

            $scope.getAngleIcon = function (node) {
                return (node.$children && node.$children.length > 0) ? ($scope.expandedNodes.includes(node) ? 'im im-angle-down' : 'im im-angle-right') : 'im';
            }

            $scope.filter = function (item) {
                if ($scope.filterText.length == 0) return true;

                if (item.$children && item.$children.some($scope.filter)) return true;

                if (item['$components']) {
                    return item['$components'].some(function (component) { component.toLowerCase().indexOf($scope.filterText.toLowerCase()) != -1 });
                }

                return false;
            };
        }

        return {
            restrict: 'AE',
            link: link,
            templateUrl: 'App/Directives/EntityTree.html'
        };
    });