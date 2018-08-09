angular.module('plaza').controller('mainController', function($scope, entityService) {
    window.rootScope = $scope; 
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
    var apiUrl = window.location.protocol + "//" + window.location.hostname + (window.location.port ? (":" + window.location.port) : "") +  "/api";
    $scope.connection = entityService.createConnection(apiUrl);

    $scope.editor = {
        tabs: [],
        currentTab: null
    };
    
    $scope.closeEntity = function(entity) {
        $scope.editor.tabs.splice($scope.editor.tabs.indexOf(entity), 1);

        $scope.editor.currentTab = $scope.editor.tabs.length ? $scope.editor.tabs[0] : null;
    }

    $scope.openEntity = function(entity) {
        if(!entity.$components) return;

        if(!$scope.editor.tabs.includes(entity)) {
            $scope.editor.tabs.push(entity);
        }

        $scope.editor.currentTab = entity;
    }

    $scope.getEditorTemplate = function(entity) {
        for(var i = 0; i < entity.$components.length; ++i) {
            var template = $scope.editorTemplates[entity.$components[i]];
            if(template) {
                return template;
            }
        }
    }

    $scope.editorTemplates = {
        Module: 'App/Editors/Module.html'
    };
});