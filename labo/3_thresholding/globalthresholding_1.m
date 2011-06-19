function [ output ] = globalthresholding_1( image )
    image=rgb2gray(image);
    dimensie=size(image); % nodig in for lus in while   
    
    %Via imhist de drempelwaarde zelf uithalen:
    %imhist(image);
    %kan T = 88
    %    T = 116
    %    T = 155
    % Dit zijn de grootste verschil in bulten
    T0 = 0.000000001;
    
    %drempelwaarde automatisch via iteratief clusteringalgoritme:
    % Stap 1: initiele waarde T kiezen (midden tussen de min & max 
    % intensiteit van het beeld)
    max_intensiteit=max(max(image, [], 1)); %max(image, [], 1) = maximum per kolom, dan nog eens max om daar het max uit te halen
    min_intensiteit=min(min(image, [], 1)); %idem als hierboven
    T=min_intensiteit + (max_intensiteit - min_intensiteit) / 2;
    vorigeT = T + 2 * T0; %aangezien eerste iteratie altijd moet plaatsvinden zetten we initiele waarde van vorigeT groot geneog
  
    % Stap 5: Herhaal stap 2, 3, 4 tot het verschil in opeenvolgende T's 
    % kleiner is dan T0
    while((vorigeT - T) > T0)
        % Stap 2: Segmenteer het beeld met T, resultaat: 2 verzameling
        % pixels G1 en G2 (intensiteit G1 < T, intensiteit G2 > T)
        behoort_tot_g1=image<T; %Zal een 1 hebben waar de intensiteit < T
        g1 = zeros(sum(sum(behoort_tot_g1)),1); %sum(sum(..)) telt het aantal 1'tjes in behoort_tot_g1
        g1_grootte = 1; %huidige index bijhouden
        
        behoort_tot_g2=image>T;
        g2 = zeros(sum(sum(behoort_tot_g2)),1);
        g2_grootte = 1;
        
        for i=1:dimensie(1)
            for j=1:dimensie(2)
                if (behoort_tot_g1(i,j))
                    g1(g1_grootte) = image(i,j); %intensiteit opslaan in g1
                    g1_grootte = g1_grootte + 1;
                elseif (behoort_tot_g2(i,j))
                    g2(g2_grootte) = image(i,j); %intensiteit opslaan in g2
                    g2_grootte = g2_grootte + 1;
                end
            end
        end
        
        % Stap 3: Bereken de gemiddelde intensiteit m1 & m2 van verzameling
        % G1 & G2
        m1=mean(g1);
        m2=mean(g2);
        
        % Stap 4: Bereken een nieuwe waarde voor T = (m1+m2)/2
        vorigeT = T;
        T=(m1+m2)/2;
    end
    
    output = not(image < T); %not om kleuren weer om te draaien
end

