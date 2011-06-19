function [ region ] = region_around_point( cx, cy, size )
    mid=size/2;
    x=cx-mid;
    y=cy-mid;
    region=[x, y, size-1, size-1];
end
