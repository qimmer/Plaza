String.prototype.prettify = function() {
    return this.replace(/([A-Z])/g, ' $1')
    // uppercase the first character
    .replace(/^./, function(str){ return str.toUpperCase(); });
}

angular.module('plaza')
.directive('propertyEditor', function($rootScope, entityService) {

    var componentPriorities = {
        Identification: 0,
        Stream: 1,
    }

    var hiddenComponents = [
        'Ownership'
    ];

    function link($scope, $element, $attrs) {
        $scope.collapsedComponents = [];

       
        $scope.hasComponent = function(item) {
            var componentList = $scope.connection.selectedEntity['$components'];
            return componentList && componentList[name];
        }

        $scope.componentComparator = function(c1, c2) {
            var c1p = componentPriorities[c1] || 9999;
            var c2p = componentPriorities[c2] || 9999;
            return (c1p < c2p) ? -1 : 1;
          };

        $scope.filterComponents = function() {
            if(!$scope.connection.selectedEntity || !$scope.connection.properties) {
                $scope.filteredComponents = [];
                return;
            }
            
            var availableComponents = Object.keys($scope.connection.properties);
            var entityComponents = $scope.connection.selectedEntity['$components'];

            $scope.filteredComponents = availableComponents
            .filter(function(componentName) {
                return entityComponents.includes(componentName) && !hiddenComponents.includes(componentName);
            })            
            .sort($scope.componentComparator);
        }

        $scope.apply = function() {
            var strippedEntity = {};
            for(var key in $scope.connection.selectedEntity) {
                if(key[0] == "$") continue;
                strippedEntity[key] = $scope.connection.selectedEntity[key];
            }

            entityService.updateEntity($scope, $scope.connection.selectedEntity.$path, strippedEntity);
        }

        $scope.$watch('connection.properties', $scope.filterComponents);
        $scope.$watch('connection.selectedEntity', $scope.filterComponents);

    }

    return {
        restrict: 'AE',
        link: link,
        templateUrl: 'App/Directives/PropertyEditor.html'
    };
});