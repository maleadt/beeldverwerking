function [ output ] = globalthresholding_2( image )
  figure, imshow(globalthresholding_2_wit(image)), title 'wit gefilterd';
  figure, imshow(globalthresholding_2_zwart(image)), title 'zwart gefilterd';
  figure, imshow(globalthresholding_2_rood(image)), title 'rood gefilterd';
  figure, imshow(globalthresholding_2_blauw(image)), title 'blauw gefilterd';
end
function [ output ] = globalthresholding_2_zwart( image )
    grayscale_image = rgb2gray(image);
    % in grayscale is zwart = 0, echt puur zwart zal in een foto niet veel
    % voorkomen (troebel, ruis..), daarom drempelwaarde van 30 nemen
    output = (grayscale_image < 30);
end
function [ output ] = globalthresholding_2_wit( image )
    grayscale_image = rgb2gray(image);
    % in grayscale is zwart = 0, echt puur wit zal in een foto niet veel
    % voorkomen (troebel, ruis..), daarom drempelwaarde van 230 nemen
    output = (grayscale_image > 230);
end

function [ output ] = globalthresholding_2_rood( image )
    hsv_image = rgb2hsv(image);

    
    %rood wordt voorgesteld door hoek:
    % 0-30 & 330-360
    %bron: http://en.wikipedia.org/wiki/HSL_and_HSV
    % Vastleggen van maximum en minimum voor kleur rood in HSV
    HSV_RED_MAX = 30/360;
    HSV_RED_MIN = 330/360;
   
    %hsv_image(:,:,1) haalt de hue component op
    %hsv_image(:,:,2) haalt de saturatie component op
    output = (hsv_image(:,:,1) < HSV_RED_MAX | hsv_image(:,:,1) > HSV_RED_MIN) & hsv_image(:,:,2) > 0.4;
end

function [ output ] = globalthresholding_2_blauw( image )

    hsv_image = rgb2hsv(image);
    
    %blauw wordt voorgesteld door hoek:
    % 210-270
    hsv_maximum_blauw = 270/360;
    hsv_minimum_blauw = 210/360;
    
    output = hsv_image(:,:,1) < hsv_maximum_blauw & hsv_image(:,:,1) > hsv_minimum_blauw & hsv_image(:,:,2) > 0.3;
end