angular.module('plaza')
    .directive('entityTreeNode', function () {
        return {
            restrict: 'AE',
            scope: {
                nodePath: "@",
                connection: "=",
                expandedNodes: "=",
                selectedNodes: "=",
                icon: "="
            },
            templateUrl: 'App/Directives/EntityTreeNode.html',
            controller: function ($scope, $element, $attrs) {
                $scope.select = function () {
                    $scope.selectedNodes.length = 1;
                    $scope.selectedNodes[0] = $scope.nodePath;
                }

                $scope.toggle = function() {
                    var index = $scope.expandedNodes.indexOf($scope.nodePath);
                    if(index == -1) {
                        $scope.expandedNodes.push($scope.nodePath);
                    } else {
                        $scope.expandedNodes.splice(index, 1);
                    }
                }

                $scope.isArrayElement = function(e) {
                    return e.$index !== undefined;
                }
            },
            link: function ($scope, $element, $attrs) {

            }
        };
    });