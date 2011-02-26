function [ region ] = regionofinterest_click( filename )
%REGIONOFINTEREST_CLICK Summary of this function goes here
%   Detailed explanation goes here

file = imread(filename);

figure;
imshow(file);

[x1, y1] = ginput(1);
[x2, y2] = ginput(1);

topleft = int32([min(x1, x2), min(y1, y2)]);
downright = int32([max(x1, x2), max(y1, y2)]);

file = imread(filename);
region = file(topleft(2):downright(2), topleft(1):downright(1));

end

