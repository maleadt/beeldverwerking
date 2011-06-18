% input_args: afbeelding waarvan de grote_as (ga) en kleine_as (ka) worden bepaald
% output_args: bevat een matrix van volgende vorm
%     [   lengte_ga,          lengte_ka;
%         start_punt_ga_x,    eind_punt_ga_y;
%         start_punt_ka_x,    eind_punt_ka_y; ]
% hieruit kan dan de eccentriciteit berekent worden alsook de omschrijvende
% rechthoek
function [ output_args ] = randbeschrijving_1( input_args )
im = imread(input_args);
% Bepalen van de rand
[B,L] = bwboundaries(1-im2bw(im),'noholes');
B = bwtraceboundary(1-im2bw(im),B{1}(1,:),'N',8,Inf,'clockwise');
% Bepalen van de punten die het verst uit elkaar liggen
max_afstand = 0;
for i = 1:length(B),
    start_punt = B(i,:);
    for j = 1:length(B),
        eind_punt = B(j,:);
        afstand = sqrt((eind_punt(1)-start_punt(1))^2 + (eind_punt(2)-start_punt(2))^2);
        if(afstand > max_afstand),
            start_punt_max = start_punt;
            eind_punt_max = eind_punt;
            max_afstand = afstand;
        end
    end
end
diameter = max_afstand;
% Richtingscoëfficient bepalen tussen deze twee punten
rico_grote_as = (eind_punt_max(2) - start_punt_max(2)) / (eind_punt_max(1) - start_punt_max(1));
% Loodrechte richting bepalen hierop
rico_kleine_as = -1*(1/rico_grote_as);
% Punten zoeken die een rechte vormen met de richtingscoëfficient van de
% kleine as, en daarvan die nemen met de grootste onderlinge afstand
max_afstand = 0;
for i = 1:length(B),
    start_punt = B(i,:);
    for j = 1:length(B),
        eind_punt = B(j,:);
        afstand = sqrt((eind_punt(1)-start_punt(1))^2 + (eind_punt(2)-start_punt(2))^2);
        rico = (eind_punt(2) - start_punt(2)) / (eind_punt(1) - start_punt(1));
        if rico == rico_kleine_as,
            if(afstand > max_afstand),
                start_punt_kl_max = start_punt;
                eind_punt_kl_max = eind_punt;
                
                max_afstand = afstand;
            end
        end
    end
end
outputargs = [diameter, max_afstand; start_punt_max; eind_punt_max; start_punt_kl_max; eind_punt_kl_max]
