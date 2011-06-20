%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% A script that performs correlation to obtain corresponding points in a
% stereo pair of images, given the fundamental matrix
% 
% KTeelen, februari 2008
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

im1 = rgb2gray(imread('left1.jpg'));
h1 = figure; imshow(im1)
im2 = rgb2gray(imread('right1.jpg'));
h2 = figure; imshow(im2)

% Load fundamental matrix from .mat file
load fundmat.mat

% Select points in the first image
numpts = 3;
figure(h1),
pts1 = ginput(numpts);
pts1(1,:) = round(pts1(1,:));
pts1(2,:) = round(pts1(2,:));
pts1 = [pts1,ones(numpts,1)]';
figure(h1), imshow(im1), hold on,
plot(pts1(1,:),pts1(2,:),'g*')
hold off
% compute epipolar lines
l1 = F*pts1;
figure(h2), imshow(im2), hold on,
xcoord = 1:size(im2,2);
for i = 1:numpts,
    plot(xcoord,-(l1(1,i)*xcoord+l1(3,i))/l1(2,i),'g');
end
hold off

% Compute the correlation of a template around a selected point in the left image of
% a stereo pair with all possible corresponding regions centerend about points on the 
% epipolar line in the right image 

% First compute regions to correlate with around the selected points in the
% left image with variable regionsize (parameter as regsize)

% Find the position of maximum correlation along the epipolar lines in the right image

% Plot correlation results
figure(h2), imshow(im2), hold on,
for i = 1:numpts,
    plot(xcoord,-(l1(1,i)*xcoord+l1(3,i))/l1(2,i),'g');
    %% Plot the postion of maximum correlation in this figure
end
hold off