% input_args: de te representeren afbeelding
% output_args: geeft de kettingcode terug
% sample_rate: rastergrootte
% connectivity: de connectiviteit, ofwel is deze 4 of 8
function [ output_args ] = kettingcode_resampling( input_args, connectivity, sample_rate )
code_count_multiplier = 1;
if( connectivity ~= 4 && connectivity ~= 8 ),
    error('Verkeerde connectiviteit.')
elseif( connectivity == 4 ),
    code_count_multiplier = 2;
end
im = imread(input_args);
% Bepalen van de rand
[B,L] = bwboundaries(1-im2bw(im),'noholes');
B = bwtraceboundary(1-im2bw(im),B{1}(1,:),'N',8,Inf,'clockwise');
fprintf('Opbouw van de kettingmatrix\n')
%B = [ 5, 5; 5, 6; 6,6; 5,5]
KC = [ 3, 2 , 1; 4, 0, 8; 5, 6, 7 ];
KC = rot90(KC,3);
% De ketting initialiseren voor ieder datapunt op nul
begin_punt = B(1,:);
huidig_punt = begin_punt;
ketting = [];
% Alle punten afgaan en de richting bepalen
punt = 1;
index = 1;
while punt < length(B)+1,
    vorig_punt = huidig_punt;
    huidig_punt = B(punt,:);
    dx = huidig_punt(1)-vorig_punt(1);
    dy = huidig_punt(2)-vorig_punt(2);
    dx = dx + 2;
    dy = dy + 2;
    %fprintf('%d\n', KC(dx,dy));
    code_huidig_punt = KC(dx,dy);
    if(connectivity == 4 ),
        if(code_huidig_punt == 1),
            %fprintf('CODE IS RECHTSBOVEN\n')
            ketting(index) = 8;
            ketting(index + 1) = 2;
            index = index + 2;
        elseif(code_huidig_punt == 3)
            %fprintf('CODE IS LINKSBOVEN\n')
            ketting(index) = 4;
            ketting(index + 1) = 2;
            index = index + 2;
        elseif(code_huidig_punt == 5)
            %fprintf('CODE IS LINKSONDER\n')
            ketting(index) = 4;
            ketting(index + 1) = 6;
            index = index + 2;
        elseif(code_huidig_punt == 7)
            %fprintf('CODE IS RECHTSONDER\n')
            ketting(index) = 6;
            ketting(index + 1) = 8;
            index = index + 2;
        else
            %fprintf('CODE IS NIET DIAGONAAL\n')
            ketting(index) = code_huidig_punt;
            index = index + 1;
        end
    else
        ketting(punt) = code_huidig_punt;
    end    
    punt = punt + 1;
end
index = 1;
begin_punt = B(1,:);
huidig_punt = begin_punt;
ketting_gesampled = [];
% Zoek eerst naar een punt die op het rooster ligt
start_index = 1;
for punt = 1:length(B),
    vorig_punt = huidig_punt;
    huidig_punt = B(punt,:);
    x = huidig_punt(1);
    y = huidig_punt(2);
    dx = huidig_punt(1)-vorig_punt(1);
    dy = huidig_punt(2)-vorig_punt(2);
    dx = dx + 2;
    dy = dy + 2;
    if(mod(x,sample_rate)==0 || mod(y,sample_rate)==0) 
        huidig_punt(1) = x;
        huidig_punt(2) = y;
        break
    end
    start_index = index+1;
end
vorig_punt = huidig_punt;
for punt = start_index:length(B),
    huidig_punt = B(punt,:);
    x = huidig_punt(1);
    y = huidig_punt(2);
    if(mod(x,sample_rate)==0 || mod(y,sample_rate)==0),
        huidig_punt;
        vorig_punt;
        %fprintf('Ligt op het raster.')
        dx = huidig_punt(1)-vorig_punt(1);
        dy = huidig_punt(2)-vorig_punt(2);
        vorig_punt = huidig_punt;
        dx = round(dx/sample_rate)+2;
        dy = round(dy/sample_rate)+2;
        code_huidig_punt = KC(dx,dy);
        if(code_huidig_punt ~= 0),
            if(connectivity == 4 ),
                if(code_huidig_punt == 1),
                    %fprintf('CODE IS RECHTSBOVEN\n')
                    ketting_resampled(index) = 8;
                    ketting_resampled(index + 1) = 2;
                    index = index + 2;
                elseif(code_huidig_punt == 3)
                    %fprintf('CODE IS LINKSBOVEN\n')
                    ketting_resampled(index) = 4;
                    ketting_resampled(index + 1) = 2;
                    index = index + 2;
                elseif(code_huidig_punt == 5)
                    %fprintf('CODE IS LINKSONDER\n')
                    ketting_resampled(index) = 4;
                    ketting_resampled(index + 1) = 6;
                    index = index + 2;
                elseif(code_huidig_punt == 7)
                    %fprintf('CODE IS RECHTSONDER\n')
                    ketting_resampled(index) = 6;
                    ketting_resampled(index + 1) = 8;
                    index = index + 2;
                else
                    %fprintf('CODE IS NIET DIAGONAAL\n')
                    ketting_resampled(index) = code_huidig_punt;
                    index = index + 1;
                end
            else
                ketting_resampled(index) = code_huidig_punt;
                index = index + 1;
            end
        end
    end
end
output_args = ketting_resampled;