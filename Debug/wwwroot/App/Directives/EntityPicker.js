angular.module('plaza')
    .directive('entityPicker', function ($rootScope, entityService) {
        return {
            restrict: 'AE',
            scope: {
                connection: "=",
                component: "=",
                onPick: "&?"
            },
            templateUrl: 'App/Directives/EntityPicker.html',
            controller: function ($scope, $element, $attrs) {
            },
            link: function ($scope, $element, $attrs) {

            }
        };
    });