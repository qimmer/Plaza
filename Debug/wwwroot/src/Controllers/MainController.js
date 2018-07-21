require('bootstrap');
require('bootstrap/dist/css/bootstrap.css')

require('../Directives/EntityTree.js')
require('../Directives/TopBar.js')

angular.module('plaza-debug').controller('mainController', function($scope, $http) {
    $scope.ip = "127.0.0.1";

    $scope.connect = function() {
        $http.get('http://' + $scope.ip + ':8000').then(function(response) {
            $scope.apply(function() {
                $scope.moduleRoot = response.data;
                $scope.isConnected = true;
            });
        }, $scope.disconnect);
    }

    $scope.disconnect = function() {
        $scope.moduleRoot = {};
        $scope.isConnected = false;
    }

    $scope.disconnect();
});