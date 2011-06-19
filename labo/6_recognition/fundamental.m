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

regsize=100;

for i=1:numpts
   lregions(i,:,:)=imcrop(im1, region_around_point(pts1(1,i), pts1(2,i), regsize));
   lsizes(i,:)=size(lregions(i,:,:)),
end

% Find the position of maximum correlation along the epipolar lines in the right image

max_corr=zeros(3,numpts);
for i=1:size(xcoord,2)
    for j=1:numpts
        % Bereken de x,y coordinaten op de epipolaire lijn
        epipolair_x=xcoord(i);
        epipolair_y=-(l1(1,j)*xcoord(i)+l1(3,j))/l1(2,j);
        
        % Haal een deel van de image uit de grootte afbeelding rechts
        rregion=imcrop(im2, region_around_point(epipolair_x, epipolair_y, regsize));
        rsize=size(rregion);
        
        % Als de grootte van de links deelimage overeenkomt met de
        % rechtse wordt de correlatie uitgevoerd
        if(rsize(1) == lsizes(j,2) && rsize(2) == lsizes(j,3))
            corr=corr2(reshape(lregions(j,:,:),lsizes(j,3),lsizes(j,2)),rregion);
            % Indien de nieuwe correlatiecoëff beter is dan vorige
            if(corr > max_corr(3,j))
                % Vervangen we de huidige coördinaten met nieuwe
                max_corr(1,j)=epipolair_x;
                max_corr(2,j)=epipolair_y;
                % en slaan de correlatiecoëff op.
                max_corr(3,j)=corr;
            end
        end
    end
end

% Plot correlation results
figure(h2), imshow(im2), hold on,
for i = 1:numpts,
    plot(xcoord,-(l1(1,i)*xcoord+l1(3,i))/l1(2,i),'g');
    %% Plot the postion of maximum correlation in this figure
end
hold off