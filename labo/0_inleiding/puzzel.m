function [ puzzel ] = puzzel( bestandsnaam )
%PUZZEL Summary of this function goes here
%   Detailed explanation goes here

bestand = imread(bestandsnaam);

div_x = int32(8);
div_y = int32(8);
bestandsgrootte = size(bestand);
if mod(bestandsgrootte(2), div_x) ~= 0,
    error('Bestand niet in te delen langs x-as');
end;
if mod(bestandsgrootte(1), div_y) ~= 0,
    error('Bestand niet in te delen langs y-as');
end;
step_x = idivide(bestandsgrootte(2), div_x);
step_y = idivide(bestandsgrootte(1), div_y);

puzzel = im2uint8(zeros(bestandsgrootte));

gekozen = zeros(div_x, div_y);
x_orig = 1;
y_orig = 1;
for index = 1:div_x * div_y
    % Random doelvakje kiezen
    x = randi(div_x);
    y = randi(div_y);
    while gekozen(x, y) == 1,
        x = randi(div_x);
        y = randi(div_y);
    end;
    gekozen(x, y) = 1;
    
    % Tekenen
    puzzel(1+(y-1)*step_x:y*step_y, 1+(x-1)*step_x:x*step_x) = bestand(1+(y_orig-1)*step_y:y_orig*step_y, 1+(x_orig-1)*step_x:x_orig*step_x);
    
    % Oorsprong vakje verhogen
    x_orig = x_orig + 1;
    if x_orig > div_x,
        x_orig = 1;
        y_orig = y_orig + 1;
    end;
end;

end

