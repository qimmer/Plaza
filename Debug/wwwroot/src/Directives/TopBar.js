require('angular');

angular.module('plaza-debug', [])
.directive('topBar', function($rootScope) {

    function link($scope, $element, $attrs) {
    }

    return {
        restrict: 'AE',
        scope: {
        },
        link: link,
        template: require('./TopBar.html')
    };
});