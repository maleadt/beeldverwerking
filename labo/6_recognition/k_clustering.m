% input_args: afbeelding van toe te kennen afbeelding aan een cluster
% output_args: geeft de cluster weer waarin het onbekende blad zich bevind
function [ output_args ] = k_clustering( input_args )
load data_trafficsigns.mat
% Bepalen van de k-clusters door een functie in matlab er wordt gewerkt met
% drie verschillende sets bladeren dus moeten er zich drie clusters vormen.
[clusters, centeroids] = kmeans(datavalues, 3);
centeroids
%Centeroids bevat de centrums van deze clusters

% Onbekend blad toekennen aan een cluster

% Bepalen van de features
im = imread(input_args);
regionproperties = regionprops(im, 'BoundingBox');
bounding_box = regionproperties.BoundingBox;
regionproperties = regionprops(im, 'Area');
area = regionproperties.Area;
eerste_feature = area / (bounding_box(1,3) * bounding_box(1,4));
regionproperties = regionprops(im, 'MinorAxisLength');
lengte_ka = regionproperties.MinorAxisLength;
regionproperties = regionprops(im, 'MajorAxisLength');
lengte_ga = regionproperties.MajorAxisLength;

tweede_feature = lengte_ka / lengte_ga;
min_afstand = Inf;
% Euclidische afstand bepalen tot de centerpunten
classificatie =  -1;
for centerpunt = 1:length(centeroids),
    afstand = sqrt((centeroids(centerpunt,2) - tweede_feature)^2 + (centeroids(centerpunt,1) - eerste_feature)^2);
    if afstand < min_afstand,
        min_afstand = afstand;
        classificatie = centerpunt;
    end
output_args = classificatie;
end
end

