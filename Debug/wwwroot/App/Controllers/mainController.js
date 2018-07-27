angular.module('plaza').controller('mainController', function($scope, entityService) {
    $scope.entityTree = {};
    $scope.conn = entityService.createConnection($scope, $scope.ip);
    $scope.selectedEntity = null;
    $scope.conn.onEntityReceived.subscribe($scope, function (entities) {
        $scope.entityTree = entities;
        $scope.properties = entityService.parseProperties(entities);
    });

    $scope.setSelection = function(entity) {
        $scope.selectedEntity = entity;
    }
});