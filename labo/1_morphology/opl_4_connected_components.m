function opl_4_connected_components( )
%LABO_OPL_ Summary of this function goes here
%   Detailed explanation goes here

coinsbw = imread('images/coinsbw.bmp');
fprintf('8 cirkels zichtbaar\n');

%Tellen van de components, wit wordt geteld, dus inversie nodig
[L, num]=bwlabel(not(coinsbw));
fprintf('%d connected components\n', num);

%Cirkels verkleinen, zodat het losstaande componenten worden
se = strel('disk', 8);
coinsbw_aangepast = imerode(not(coinsbw), se);

%Connected components weer tellen (wit wordt geteld, dus geen inversie
%nodig)
[L, num]=bwlabel(coinsbw_aangepast);
fprintf('%d connected components na operaties\n', num);

end

