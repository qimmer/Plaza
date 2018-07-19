angular.module('plaza').controller('entityController', function($scope, $http) {
    $scope.treeOptions = {
        nodeChildren: "$children",
        dirSelectable: true,
        injectClasses: {
            ul: "a1",
            li: "a2",
            liSelected: "a7",
            iExpanded: "a3",
            iCollapsed: "a4",
            iLeaf: "a5",
            label: "a6",
            labelSelected: "a8"
        }
    }

    $scope.ip = "127.0.0.1";

    $scope.connect = function() {
        $http.get('http://' + $scope.ip + ':8000').then(function(response) {
            $scope.moduleRoot = response.data;
            $scope.isConnected = true;
        }, $scope.disconnect);
    }

    $scope.disconnect = function() {
        $scope.moduleRoot = {};
        $scope.isConnected = false;
    }

    $scope.disconnect();
});