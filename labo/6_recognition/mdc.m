function [ output_args ] = mdc()
fprintf('Minimal Distance Classifier\n')
load data_trafficsigns.mat
% Compute prototypes
protot(1,:) = sum(roundred,1)/size(roundred,1);
protot(2,:) = sum(trianglered,1)/size(trianglered,1);

d12 = dataval*(protot(1,:)'-protot(2,:)') - ((protot(1,:)'-protot(2,:)')'*(protot(1,:)'+protot(2,:)'))/2;

colorplot = {'r*','bd'};
figure, hold on,axis equal, axis([0 1 0 1])
for i = 1:size(datavalues,1),
    plot(datavalues(i,1),datavalues(i,2),colorplot{datavalues(i,3)})
end
plot(protot(:,1),protot(:,2),'k*')
plot(dataval(:,1),dataval(:,2),'ys')
hold off
end

