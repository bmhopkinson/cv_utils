%convert flat file outputs of faceVisibilityToCameras program to sparse matlab arrays

outfile = '0441_simple_faceVisSparse.mat';
baseDir = './output/';
visFC_file = strcat(baseDir,'visibleFC.txt');
imCoord_file = strcat(baseDir,'imCoord.txt');
Fcenters_file = strcat(baseDir,'Fcenters.txt');

%read in flat files
visFCFlat = dlmread(visFC_file);
imCoordFlat = dlmread(imCoord_file);
Fcenters = dlmread(Fcenters_file);

%convert visFC and imCoord to sparse to conserve memory
visibleFC  = sparse(visFCFlat);
imCoord    = sparse(imCoordFlat);

%save output file
save(outfile,'Fcenters','visibleFC','imCoord');
