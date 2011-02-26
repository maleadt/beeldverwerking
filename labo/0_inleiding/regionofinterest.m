function [ region ] = regionofinterest( filename, topleft, downright )
%REGIONOFINTEREST Summary of this function goes here
%   Detailed explanation goes here

file = imread(filename);
region = file(topleft(2):downright(2), topleft(1):downright(1));

end

