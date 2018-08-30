angular.module('plaza').controller('mainController', function($scope, entityService) {
    var apiUrl = window.location.protocol + "//" + window.location.hostname + (window.location.port ? (":" + window.location.port) : "") +  "/api";
    
    $scope.connection = entityService.createConnection(apiUrl);

    $scope.editor = {
        tabs: [],
        currentTab: null
    };

    $scope.selectedEntities = [];
    
    $scope.closeEntity = function(entity) {
        $scope.editor.tabs.splice($scope.editor.tabs.indexOf(entity), 1);

        $scope.editor.currentTab = $scope.editor.tabs.length ? $scope.editor.tabs[0] : null;
    }

    $scope.openEntity = function(entity) {
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

    document.addEventListener("keydown", function(e) {
    if ($scope.editor.currentTab && e.keyCode == 83 && (navigator.platform.match("Mac") ? e.metaKey : e.ctrlKey)) {
      e.preventDefault();
      
      entityService.save($scope.connection, $scope.editor.currentTab);
    }
  }, false);

});
