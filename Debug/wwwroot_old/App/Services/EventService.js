angular.module('plaza').service('eventService', function() {
    return {
        event: function() {
            var e = {
                subscriptions: {}
            };
            e.subscribe = function(scope, handler) {
                scope.$on('$destroy', function() {
                    delete e.subscriptions[scope];
                });
        
                e.subscriptions[scope] = handler;
            };

            e.emit = function() {
                for(var key in e.subscriptions) {
                    e.subscriptions[key].apply(null, arguments);
                }
            }
            
            return e;
        }
    };
});