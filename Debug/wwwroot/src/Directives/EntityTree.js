require('angular');
require('angular-tree-control');

angular.module('plaza-debug', [])
.directive('entityTree', function($rootScope) {

    var treeOptions = {
        nodeChildren: "children",
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
        },
        isLeaf: function(node) { return (node.isLeaf === undefined) ? true : node.isLeaf; }
    };

    function organize(entity, name) {
        var organizedObject = {
            original: entity,
            name: (entity.Name === undefined) ? name : entity.Name,
            children: []
        };

        for(var key in entity) {
            var value = entity[key];

            if(Array.isArray(value)) {
                var arr = {   
                    name: key,
                    children: [],
                    isLeaf: false
                };

                for(var i = 0; i < value.length; ++i) {
                    var element = value[i];
                    var organizedChild = organize(element, toString(arr.children.length));
                    organizedChild.isLeaf = false;
                    arr.children.push(organizedChild);
                }

                organizedObject.children.push(arr);
            } else if(value != null && typeof value === 'object') {
                var organizedChild = organize(value, key);
                organizedChild.isLeaf = true;
                organizedObject.children.push(organizedChild);
            }
        }        

        return organizedObject;
    }

    function link($scope, $element, $attrs) {
        $scope.treeOptions = treeOptions;
        $scope.organize = organize;
    }

    return {
        restrict: 'AE',
        scope: {
            entities: '='
        },
        link: link,
        template: require('./EntityTree.html')
    };
});