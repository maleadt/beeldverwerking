function [ logicimage ] = createlogicimage( bestandsnaam )
%CREATELOGICIMAGE Summary of this function goes here
%   Detailed explanation goes here


bestand = imread(bestandsnaam);
logicimage = rgb2gray(bestand);
grootte = size(logicimage);

for i=1:grootte(1)
    for j=1:grootte(2)
        if (logicimage(i, j) < 150)
            logicimage(i, j) = 0;
        else
            logicimage(i, j) = 255;
        end
    end
end


figure, imshow(logicimage);

end

