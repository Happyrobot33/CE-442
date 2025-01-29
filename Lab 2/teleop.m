%define our ship start spot
xCenter = 10/2;
yCenter = 10/2;
radius = 1;

S.ship = square(xCenter, yCenter, radius);
%setup function callback for when keypresses occur
set(gcf(), 'KeyPressFcn', @fig_kpfcn); 
guidata(S.ship,S); 

%This runs every keypress on teh graph
function [] = fig_kpfcn(H,E)
    S = guidata(H);
    stepsize=0.1;
    margin = 0.1;
    switch E.Key
        case 'd'
            if(get(S.ship,'XData')+stepsize < (10-margin))
                set(S.ship,'XData',get(S.ship,'XData')+stepsize);
            end
        case 'a'
            if(get(S.ship,'XData')-stepsize > (0+margin))
                set(S.ship,'XData',get(S.ship,'XData')-stepsize);
            end
        case 'w'
            if(get(S.ship,'YData')+stepsize < (10-margin))
                set(S.ship,'YData',get(S.ship,'YData')+stepsize);
            end
        case 's'
            if(get(S.ship,'YData')-stepsize > (0+margin))
                set(S.ship,'YData',get(S.ship,'YData')-stepsize);
            end
    end
end

%this runs everytime the graph is updated
function h = square(xCenter,yCenter,radius)
    hold on
    xlim([0,10]);
    ylim([0,10]);
    %draw the square at x and y center
    h = plot(xCenter,yCenter, "squarer");
    %set the marker size so the square is the correct size
    h.MarkerSize = radius * 5;
    hold off
end