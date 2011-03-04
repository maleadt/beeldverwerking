function opl_4_connected_components( )
%LABO_OPL_ Summary of this function goes here
%   Detailed explanation goes here

coinsbw = imread('images/coinsbw.bmp');
fprintf('8 cirkels zichtbaar\n');

[L, num]=bwlabel(coinsbw);
fprintf('%d connected components\n', num);

se = strel('disk', 7);
coinsbw_aangepast = imdilate(not(coinsbw), se);

[L, num]=bwlabel(not(coinsbw_aangepast));
fprintf('%d connected components na operaties\n', num);

end

