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

        return {
            restrict: 'AE',
            scope: {
                connection: "=",
                onOpen: "&?"
            },
            templateUrl: 'App/Directives/EntityTree.html',
            controller: function ($scope, $element, $attrs) {
                $scope.treeOptions = {
                    equality: function (a, b) {
                        if (!a || !b) return false;

                        return a === b;
                    },
                    nodeChildren: "$children",
                    dirSelectable: true,
                    multiSelection: false,
                    allowDeselect: false,
                    isLeaf: function (node) { return (node.$isLeaf === undefined) ? true : node.$isLeaf; }
                };

                $scope.rootNode = $scope.connection.root;

                $scope.expandedNodes = [];
                $scope.selectedNodes = [];
                $scope.filterText = "";

                $scope.open = function(node) {
                    $scope.onOpen({entity: node});
                }

                function isElement() {
                    return $scope.connection.selectedNode && $scope.connection.selectedNode.$type === "element"
                }

                function isFolder() {
                    return $scope.connection.selectedNode && $scope.connection.selectedNode.$type === "folder"
                }

                function getAvailableComponents() {
                    return Object.keys(entityService.properties);
                }

                $scope.contextMenu = [
                    { // Add (folder)
                        text: function () { return 'Add ' + $scope.connection.selectedNode.Name; },
                        click: function () {
                            entityService.createEntity($scope.connection, entityService.calculatePath($scope.connection.selectedNode) + "/" + findUniqueName($scope.connection.selectedNode, $scope.connection.selectedNode.Name));
                            return true;
                        },
                        displayed: isFolder
                    },
                    { // Destroy (element)
                        text: 'Destroy',
                        click: function () {
                            entityService.deleteEntity($scope.connection, entityService.calculatePath($scope.connection.selectedNode));
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
                                                click: function () { entityService.addComponent($scope.connection, entityService.calculatePath($scope.connection.selectedNode), component.Name); }
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
                                        click: function () { entityService.removeComponent($scope.connection, entityService.calculatePath($scope.connection.selectedNode), componentName); }
                                    });
                                }
                            }
                            return componentList;
                        }
                    }
                ];

                $scope.$watch('connection.root', function () {
                    if ($scope.expandedNodes.length == 0 && $scope.connection && $scope.connection.root.$children && $scope.connection.root.$children.length > 0) {
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
                }

                $scope.select = function (node) {
                    $scope.toggle(node);

                    $scope.connection.selectedNode = node;
                    $scope.connection.selectedEntity = ($scope.connection.selectedNode['$components']) === undefined ? $scope.connection.selectedNode.$parent : $scope.connection.selectedNode;
                }

                $scope.rightClick = function (node) {
                    var index = $scope.expandedNodes.indexOf(node);
                    if (index == -1) {
                        $scope.toggle(node);
                    }
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
            },
            link: function ($scope, $element, $attrs) {

            }
        };
    });