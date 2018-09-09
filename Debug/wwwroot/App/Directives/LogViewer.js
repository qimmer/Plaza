angular.module('plaza')
    .directive('logViewer', function ($rootScope, entityService) {
        
        return {
            restrict: 'AE',
            scope: {
                connection: "=",
                selectedEntities: "="
            },
            templateUrl: 'App/Directives/LogViewer.html',
            controller: function ($scope, $element, $attrs) {
                $scope.moment = moment;
                $scope.severityName = [
                    "Info",
                    "Warning",
                    "Error",
                    "Fatal",
                ];

                $scope.severityClass = [
                    "info",
                    "warning",
                    "danger",
                    "danger",
                ];
            },
            link: function ($scope, $element, $attrs) {

            }
        };
    });