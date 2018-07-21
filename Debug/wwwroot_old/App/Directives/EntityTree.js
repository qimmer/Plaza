angular.module('plaza')
.directive('entityTree', function($rootScope) {
    var treeOptions = {
        equality: function(n1, n2) {
            return n1 === n2;
        },
        nodeChildren: "children",
        dirSelectable: true,
        multiSelection: false,
        allowDeselect: false,
        isLeaf: function(node) { return (node.isLeaf === undefined) ? true : node.isLeaf; }
    };

    var propertyIconMap = {
        Components: 'im im-git',
        PropertyOffset: 'im im-pencil',
        Properties: 'im im-plugin',
    }

    var entityMap = {};

    function organize(entity, owner, ownerProperty, name) {
        var organizedObject = {
            original: entity,
            name: (entity.Name === undefined) ? name : entity.Name,
            children: [],
            owner: owner,
            ownerProperty: ownerProperty
        };

        for(var key in entity) {
            if(key == '$components') continue;

            var value = entity[key];

            if(Array.isArray(value)) {
                var arr = {   
                    owner: entity,
                    ownerProperty: key,
                    isFolder: true,
                    name: key,
                    children: [],
                    isLeaf: true
                };

                for(var i = 0; i < value.length; ++i) {
                    arr.isLeaf = false;
                    var element = value[i];
                    var organizedChild = organize(element, entity, i, toString(arr.children.length));
                    organizedChild.isLeaf = false;
                    arr.children.push(organizedChild);
                }

                organizedObject.children.push(arr);
            } else if(value != null && typeof value === 'object') {
                var organizedChild = organize(value, key);
                organizedChild.isLeaf = true;
                organizedChild.name = key;
                organizedObject.children.push(organizedChild);
            }
        }        

        entityMap[entity] = organizedObject;

        return organizedObject;
    }

    function link($scope, $element, $attrs) {
        $scope.treeOptions = treeOptions;
        $scope.organize = organize;
        $scope.expandedNodes = [];
        $scope.selectedNodes = [];
        $scope.filterText = "";
        
        $scope.$watch('entities', function() {
            entityMap = {};

            $scope.organizedEntities = organize($scope.entities, null, null, 'root');
            
            if($scope.expandedNodes.length == 0 && $scope.organizedEntities.children.length > 0) {
                $scope.expandedNodes.push($scope.organizedEntities.children[0]);
                for(var key in $scope.organizedEntities.children[0].children) {
                    $scope.expandedNodes.push($scope.organizedEntities.children[0].children[key]);
                }
            }
        });

        $scope.$watch('selectedEntity', function() {
            $scope.selectedNodes = entityMap[$scope.selectedEntity];
        });

        $scope.toggle = function(node) {
            var index = $scope.expandedNodes.indexOf(node);
            if (index > -1) {
                $scope.expandedNodes.splice(index, 1);
            } else {
                $scope.expandedNodes.push(node);
            }

            if(node.original) {
                $scope.selectedEntity = node.original;

                if($scope.onSelection) {
                    $scope.onSelection({entity: $scope.selectedEntity});
                }
            }
        }

        $scope.getIcon = function(node) {
            if(node.isFolder) {
                if($scope.expandedNodes.includes(node)) {
                    return 'im im-folder-open';
                } else {
                    return 'im im-folder';
                }
            }

            for(var property in node.original) {
                var icon = propertyIconMap[property];
                if(icon !== undefined) {
                    return icon;
                }
            }

            return 'im im-cube';
        }

        $scope.getAngleIcon = function(node) {
            return (node.children && node.children.length > 0) ? ($scope.expandedNodes.includes(node) ? 'im im-angle-down' : 'im im-angle-right') : 'im';
        }

        $scope.filter = function( item  ) {
            if($scope.filterText.length == 0) return true;

            if(item.children && item.children.some($scope.filter)) return true;
            
            if(item.original && item.original['$components']) {
                 return item.original['$components'].some(function(component) { component.toLowerCase().indexOf($scope.filterText.toLowerCase()) != -1 });
            }

            return false;
      };
    }

    return {
        restrict: 'AE',
        scope: {
            entities: '=',
            selectedEntity: '=',
            onSelection: '&'
        },
        link: link,
        templateUrl: 'App/Directives/EntityTree.html'
    };
});