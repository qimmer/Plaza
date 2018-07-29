angular.module('plaza')
.directive('topBar', function(entityService) {

    function link($scope, $element, $attrs) {        
        $scope.connect = function() {
            if($scope.connection) {
                entityService.close($scope);
            } 

            entityService.createConnection($scope);
            
        }

        $scope.getConnectionState = function () {
            return ($scope.connection) ? 0 : ($scope.connection.isConnected ? 2 : 1)
        }
    }

    return {
        restrict: 'AE',
        link: link,
        templateUrl: 'App/Directives/TopBar.html'
    };
});