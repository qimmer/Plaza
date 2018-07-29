angular.module('plaza').controller('mainController', function($scope, entityService) {
    $scope.treeOptions = {
        equality: function(a, b) {
            if(!a || !b) return false;

            return a === b;
        },
        nodeChildren: "$children",
        dirSelectable: true,
        multiSelection: false,
        allowDeselect: false,
        isLeaf: function(node) { return (node.$isLeaf === undefined) ? true : node.$isLeaf; }
    };

    entityService.createConnection($scope);
});