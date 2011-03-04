function opl_5_letter_a( )
%OPL_5-LETTER_A Summary of this function goes here
%   Detailed explanation goes here

im_orig = imread('images/text.bmp');
im_gray = rgb2gray(im_orig); %grayscale maken voor matching
im_bw = im2bw(im_gray);
im_bw_inverted = imcomplement(im_bw); %invert zodat we zwarte achtergrond hebben ipv witte met zwarte letters..

%originele image laten zien om een letter aan te duiden om uit te knippen
figure, imshow(im_orig);
hold on;
input_points = ginput(2);
hold off;

%region of interest berekenen
topleft = int32([min(input_points(1,1), input_points(2,1)), min(input_points(1,2), input_points(2,2))]);
downright = int32([max(input_points(1,1), input_points(2,1)), max(input_points(1,2), input_points(2,2))]);
template = im_bw_inverted(topleft(2):downright(2), topleft(1):downright(1));

%bwhitmiss uitvoeren & weergeven, elk wit puntje stelt een match voor
figure, imshow(bwhitmiss(uint8(im_bw_inverted),uint8(template)));


%normxcorr2 ???
end

