angular.module('plaza')
    .directive('entityTreeNode', function () {
        return {
            restrict: 'AE',
            scope: {
                uuid: "@",
                connection: "=",
                expandedNodes: "=",
                selectedNodes: "=",
                onOpen: "&?",
                icon: "="
            },
            templateUrl: 'App/Directives/EntityTreeNode.html',
            controller: function ($scope, $element, $attrs) {
                $scope.select = function () {
                    $scope.selectedNodes.length = 1;
                    $scope.selectedNodes[0] = $scope.uuid;
                }

                $scope.toggle = function() {
                    var index = $scope.expandedNodes.indexOf($scope.uuid);
                    if(index == -1) {
                        $scope.expandedNodes.push($scope.uuid);
                    } else {
                        $scope.expandedNodes.splice(index, 1);
                    }
                }

                $scope.isArrayElement = function(e) {
                    return e.$index !== undefined;
                }

                $scope.open = function(uuid) {
                    if($scope.onOpen) {
                        $scope.onOpen({uuid: uuid});
                    }
                }
            },
            link: function ($scope, $element, $attrs) {

            }
        };
    });