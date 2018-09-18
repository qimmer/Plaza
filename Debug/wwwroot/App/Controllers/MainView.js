angular.module('plaza').controller('mainController', function($scope, entityService) {
    var apiUrl = window.location.protocol + "//" + window.location.hostname + (window.location.port ? (":" + window.location.port) : "") +  "/api";
    
    $scope.connection = entityService.createConnection(apiUrl);

    $scope.editor = {
        tabs: [],
        currentTab: null
    };

    $scope.selectedEntities = [];
    
    $scope.closeTab = function(uuid) {
        $scope.editor.tabs.splice($scope.editor.tabs.indexOf(uuid), 1);

        $scope.editor.currentTab = $scope.editor.tabs.length ? $scope.editor.tabs[0] : null;
    }

    $scope.openTab = function(uuid) {
        if(!$scope.editor.tabs.includes(uuid)) {
            $scope.editor.tabs.push(uuid);
        }

        $scope.editor.currentTab = uuid;
    }

    $scope.getEditorTemplate = function(uuid) {
        var entity = $scope.connection.getEntity(uuid);
        
        for(var i = 0; i < entity.$components.length; ++i) {
            var template = $scope.editorTemplates[entity.$components[i]];
            if(template) {
                return template;
            }
        }
    }

    $scope.getNumEntities = function() {
        var i = 0;
        for(var key in $scope.connection.getEntities()) {
            i++;
        }
        return i;
    }

    $scope.editorTemplates = {
        Module: 'App/Editors/Module.html'
    };

    Promise.resolve().then(function resolver() {
        return entityService.update($scope.connection)
        .then(resolver);
    });

    document.addEventListener("keydown", function(e) {
    if ($scope.editor.currentTab && e.keyCode == 83 && (navigator.platform.match("Mac") ? e.metaKey : e.ctrlKey)) {
      e.preventDefault();
      
      entityService.save($scope.connection, $scope.editor.currentTab);
    }
  }, false);

});
