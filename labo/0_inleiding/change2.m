function [ verschil ] = change2( bestandsnaam1, bestandsnaam2 )
%CHANGE Bereken het verschil tussen twee afbeeldingen.
%   Deze functie berekent het verschil tussen twee gegeven afbeeldingen,
%   ingeladen via hun bestandsnaam. Retturn matrix bevat niet langer het
%   verschil in intensiteiten, maar een absolute waarde.

% Afbeeldingen inladen
bestand1 = imread(bestandsnaam1);
bestand2 = imread(bestandsnaam2);
if ~isequal(size(bestand1), size(bestand2)),
    error('Bestanden moeten even groot zijn');
end;

% Verschil berekenen (witte achtergrond, is duidelijker)
verschil = 255 - 255 * idivide(abs(bestand1 - bestand2) + 254, 255, 'floor');

end

