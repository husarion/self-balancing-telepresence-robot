$(document).ready(function() {

    window.manipulator_height = $('#manipulator').outerHeight();

    var joystick_options = {
        zone: document.getElementById('manipulator'),
        color: 'white',
        size: 120,
        threshold: 0.15,
        fadeTime: 500,
        multitouch: false,
        dataOnly: false,
        position: {top: '147px', left: '137px'},
        mode: 'static',
        restOpacity: 0.5
    };

    var joystick_mobile_options = {
        zone: document.getElementById('manipulator'),
        color: 'white',
        size: 90,
        threshold: 0.15,
        fadeTime: 500,
        multitouch: false,
        dataOnly: false,
        position: {top: '107px', left: '100px'},
        mode: 'static',
        restOpacity: 0.5
    };

    var joystick;

    var last_position_x = 0;
    var last_position_y = 0;
    
    var down_active = false;
    var up_active = false;
    var left_active = false;
    var right_active= false;
    
    function trigger_move(nipple) {
        var angle = nipple.angle.degree;
        var current_position_x = nipple.position.x;
        var current_position_y = nipple.position.y;
        var threshold = 3;

        if(
            ( ( (current_position_x - last_position_x) > threshold ) || ( (current_position_x - last_position_x) < -threshold ) ) ||
            ( ( (current_position_y - last_position_y) > threshold ) || ( (current_position_y - last_position_y) < -threshold ) )
        )
        {
            
            if ( (angle >= 0 && angle < 55) || (angle >= 305 && angle < 360) ) {
                last_direction_x = 'right';
                if(!right_active) {
                    $('#move_right').trigger('mousedown');
                    right_active = true;
                } 
            } else {
                if(right_active) {
                    $('#move_right').trigger('mouseup');
                    right_active = false;
                }
            }

            if (angle >= 125 && angle < 235) {
                last_direction_x = 'left';
                if(!left_active) {
                    $('#move_left').trigger('mousedown');
                    left_active = true;
                } 
            } else {
                if(left_active) {
                    $('#move_left').trigger('mouseup');
                    left_active = false;
                }
            }

            if (angle >= 35 && angle < 145) {
                last_direction_y = 'up';
                if(!up_active) {
                    $('#move_up').trigger('mousedown');
                    up_active = true;
                } 
            } else {
                if(up_active) {
                    $('#move_up').trigger('mouseup');
                    up_active = false;
                }
            }

            if (angle >= 215 && angle < 325) {
                last_direction_y = 'down';
                if(!down_active) {
                    $('#move_down').trigger('mousedown');
                    down_active = true;
                } 
            } else {
                if(down_active) {
                    $('#move_down').trigger('mouseup');
                    down_active = false;
                }
            }

            last_position_x = current_position_x;
            last_position_y = current_position_y;
        } else {
            return;
        }
    }

    function windowIsMobile() {
        if( $('#battery-level-wrapper').css('display') == 'none' ) {
            window.isMobile = true;
            return true;
        }
        return false;
    };

    var tout;

    function init_manipulator(){
        if(joystick !== undefined) {
            joystick.destroy();    
        }
        
        if (windowIsMobile()) {
            joystick = nipplejs.create(joystick_mobile_options);
        } else {
            joystick = nipplejs.create(joystick_options);
        }
    
        joystick.on('move', function (evt, nipple) {
            clearTimeout(tout);
            tout = setTimeout(function() {
                trigger_move(nipple)
            }, 20);
        }).on('start', function(event, nipple) {
            clearTimeout(tout);
        }).on('end', function(event, nipple) {
            clearTimeout(tout);
            if(up_active) {
                $('#move_up').trigger('mouseup');
                up_active = false;
            }
            if(down_active) {
                $('#move_down').trigger('mouseup');
                down_active = false;
            }
            if(left_active) {
                $('#move_left').trigger('mouseup');
                left_active = false;
            }
            if(right_active) {
                $('#move_right').trigger('mouseup');
                right_active = false;
            }
        });
    }

    function update_gearbox() {
        var elem = $('#gear_selector');
        var current_gear = $('#gear_selector').data('current-gear');
            
        if(windowIsMobile()) {
            gear_step_px = 40;
        } else {
            gear_step_px = 56;
        }
        
        $('#gear_selector').css({'top' : gear_step_px * ( gears - current_gear ) });
    }

    var gear_step_px = 56;

    if(windowIsMobile()) {
        gear_step_px = 40;
    } else {
        gear_step_px = 56;
    }

    var gears = 3;
    var current_gear = 2; // start gear

    function updateGearSelector(gear) {
        $('#gear_selector').removeClass('g1 g2 g3');
        $('#gear_selector').addClass('g'+gear);
        $('#gear_selector').data('current-gear', gear);
    }

    $('#gear_selector').draggable({
        axis: "y",
        containment: "#gearbox",
        scroll: false,
        stop: function(event, ui) {
            
            var grid_y = gear_step_px;
            var elem = $( this );
            var top = parseInt(elem.css('top'));
            var cy = (top % grid_y);
        
            var new_top = (Math.abs(cy)+0.5*grid_y >= grid_y) ? (top - cy + (top/Math.abs(top))*grid_y) : (top - cy);
        
            ui.helper.stop(true).animate({
                top: new_top,
                opacity: 1
            }, 200);

        
            var position = new_top;
            
            if (position == 0) {
                current_gear = gears;
            } else {
                current_gear = (gear_step_px * gears - gear_step_px)  / position;
            }
            updateGearSelector(current_gear);
            $('#g'+current_gear).trigger('mousedown').delay(50).trigger('mouseup');
        }
    });

    var max_steps = 14;

    function generate_battery_level(steps) {
        
        var html = '';
        if(steps > max_steps) {
            steps = max_steps;
        }
        for(i=0; i < steps; i++) {
            html=html+'<div class="step">&nbsp</div>';
        }
        $('#battery-level-wrapper .inner').html(html);
    }

    $('#battery-level-wrapper .inner').append(generate_battery_level());
    
    function update_battery_level() {
        var voltage = parseFloat ($('#lb_bat').text() || 0);
        var max_voltage = 11.8;
        var min_voltage = 10;

        var max_level = max_voltage - min_voltage;
        var current_level = max_level - (max_voltage - voltage);
        var one_step = max_steps / max_level;

        var current_steps = current_level * one_step;

        generate_battery_level(Math.round(current_steps));
    }
    
    $('#lb_bat').on('DOMSubtreeModified', function() {
        update_battery_level();
    });
    
    function init_scroller() {
      $('#console_container').mCustomScrollbar({
        theme: "light",
        scrollbarPosition: "outside",
        scrollButtons: {enable:true},
        mouseWheel: true,
        scrollInertia: 0,
        alwaysShowScrollbar: 1,
        advanced: { 
            updateOnContentResize: true
        },
        callbacks: {
            onUpdate: function() {
                $("#console_container").mCustomScrollbar("scrollTo","bottom");
            }
        }
      });
    }
    
    var resize_tout;

    $(window).resize(function() {
        clearTimeout(resize_tout);
        resize_tout = setTimeout(function() {
            init_manipulator();
            update_gearbox();
            }, 100
        );
    });
    
    $(window).load(function () {
        init_scroller();
        init_manipulator();
        update_battery_level();
    });
});