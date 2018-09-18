angular.module('plaza')
    .directive('flowGraph', function (entityService, $document) {
        return {
            restrict: 'AE',
            scope: {
                connection: "=",
                rootUuid: "="
            },
            templateUrl: 'App/Directives/FlowGraph.html',
            controller: function ($scope, $element, $attrs) {
                $scope.zoom = 1.0;
                $scope.scroll = {
                    x: 0,
                    y: 0
                };

                $scope.getBgStyle = function () {
                    return {
                        "background-position": "left " + ($scope.scroll.x) + "px top " + ($scope.scroll.y) + "px",
                        "transform": "translate(" + (-(1 / $scope.zoom) * 50 + 50).toString() + "%, " + (-(1 / $scope.zoom) * 50 + 50).toString() + "%)"
                    };
                }

                $scope.getContainerStyle = function () {
                    return {
                        "transform": "scale(" + $scope.zoom + ")",
                        "width": ((1 / $scope.zoom) * 100).toString() + "%",
                        "height": ((1 / $scope.zoom) * 100).toString() + "%",
                    };
                }

                $scope.getNodeStyle = function (uuid) {
                    var entity = $scope.connection.getEntity(uuid);

                    if (!entity.FlowNodeLocation) {
                        entity.FlowNodeLocation = {
                            x: Math.ceil(Math.random() * 1000),
                            y: Math.ceil(Math.random() * 1000)
                        };

                        entityService.updateEntity($scope.connection, entity.Uuid, {
                            FlowNodeLocation: entity.FlowNodeLocation
                        });
                    }

                    return {
                        left: (entity.FlowNodeLocation.x + $scope.scroll.x) + "px",
                        top: (entity.FlowNodeLocation.y + $scope.scroll.y) + "px",
                    };
                }

                $element.bind("DOMMouseScroll mousewheel onmousewheel", function (event) {

                    // cross-browser wheel delta
                    var event = window.event || event; // old IE support
                    var delta = event.wheelDelta || -event.detail;

                    if (Math.abs(delta) > 0) {
                        $scope.$apply(function () {
                            if (delta < 0) {
                                $scope.zoom *= 0.9;
                            }

                            if (delta > 0) {
                                $scope.zoom *= 1.1;
                            }
                        });

                        // for IE
                        event.returnValue = false;
                        // for Chrome and Firefox
                        if (event.preventDefault) {
                            event.preventDefault();
                        }

                    }
                });

                var initialMouseX, initialMouseY, dragUuid, dragScope;

                function mousemove($event) {
                    var dx = $event.clientX - initialMouseX;
                    var dy = $event.clientY - initialMouseY;

                    initialMouseX = $event.clientX;
                    initialMouseY = $event.clientY;

                    dragScope.$apply(function () {
                        if (dragUuid) {
                            var entity = $scope.connection.getEntity(dragUuid);
                            entity.FlowNodeLocation.x += dx / $scope.zoom;
                            entity.FlowNodeLocation.y += dy / $scope.zoom;

                            entityService.updateEntity($scope.connection, dragUuid, {
                                FlowNodeLocation: entity.FlowNodeLocation
                            });
                        } else {
                            $scope.scroll.x += dx / $scope.zoom;
                            $scope.scroll.y += dy / $scope.zoom;
                        }
                    });

                    return false;
                }

                function mouseup() {
                    dragUuid = null;
                    $document.unbind('mousemove', mousemove);
                    $document.unbind('mouseup', mouseup);
                }

                $scope.mouseDown = function ($event, entityUuid) {
                    $scope.connection.selectedEntities = [entityUuid];

                    dragUuid = entityUuid;
                    dragScope = this;
                    initialMouseX = $event.clientX;
                    initialMouseY = $event.clientY;
                    $document.bind('mousemove', mousemove);
                    $document.bind('mouseup', mouseup);

                    return false;
                }

                $scope.dragStart = function ($event) {
                    if (!dragUuid) {
                        dragUuid = null;
                        dragScope = this;
                        initialMouseX = $event.clientX;
                        initialMouseY = $event.clientY;
                        $document.bind('mousemove', mousemove);
                        $document.bind('mouseup', mouseup);
                        return false;
                    }

                }

                $scope.isValue = function (value, index, array) {
                    return typeof value !== "Object";
                }

                $scope.getKeys = function (table) {
                    var keys = [];
                    for (var key in table) {
                        if (key[0] === "$") continue;

                        if (typeof table[key] === "object") continue;

                        keys.push(key);
                    }
                    return keys;
                }
            }
        };
    });