angular.module('plaza')
.directive('topBar', function(entityService) {

    function link($scope, $element, $attrs) {
        $scope.connection.ip = $scope.ip;
        
        $scope.connect = function() {
            if($scope.connection.ip) {
                $scope.connection.ip = "";
            } else {
                $scope.connection.ip = $scope.ip;
            }
            
        }

        $scope.getConnectionState = function () {
            return ($scope.connection.ip !== $scope.ip) ? 0 : ($scope.connection.isConnected ? 2 : 1)
        }
    }

    return {
        restrict: 'AE',
        scope: {
            ip: '@',
            connection: '='
        },
        link: link,
        templateUrl: 'App/Directives/TopBar.html'
    };
});