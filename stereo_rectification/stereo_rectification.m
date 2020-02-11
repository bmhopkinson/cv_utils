function stereo_rectification(infile)
%stereorectification and disparity map generation based on Matlab toolbox functions. 
%use stereocamera calibrations obtained from 'stereoCameraCalibrator' application in matlab (gives very similar calibrations to Caltech matlab camera calibration toolb
%infile contains a tab delimited list of files to be processed with the
%following structure: "Left_image_name  Right_image_name  stereoCameraCalibration_file"
%the stereoCameraCalibration_file  should be in the current directory while
%the input images should be in "./input"; MUST USE SAME CALIBRATION FILE  FOR ALL IMAGES

fid = fopen(infile);
Files = textscan(fid,'%s\t%s\t%s\n');
Lfiles = Files{1};
Rfiles = Files{2};
CalibFiles = Files{3};

minDisp = 0;
maxDisp = 96;
disparityRange = [minDisp maxDisp];  %difference must be divisible by 16

data = load(CalibFiles{1});  %imports stereoParams object (MUST BE NAMED "stereoParams") which contains stereo rig caliabration information
stereoParams = data.stereoParams;  %need to do this so the stereoParams object is visible in the parfar loop
parpool('local',10);
p = gcp; %get current parallel pool
%addAttachedFiles(p,CalibFiles{1});
nbrPairs = length(Lfiles);
parfor i = 1:nbrPairs
    thisLfile = strcat('./input/',Lfiles{i});
    thisRfile = strcat('./input/',Rfiles{i});
    I1 = imread(thisLfile);
    I2 = imread(thisRfile);
    
    
    [J1_valid, J2_valid] = rectifyStereoImages(I1,I2, stereoParams,'OutputView','valid'); %rectify image
    disparityMap = disparity(rgb2gray(J1_valid),rgb2gray(J2_valid),'BlockSize',15, 'DisparityRange',disparityRange);  %produce pixel dispartiy map between rectified images
    
    %clean up output
    J_final = J1_valid(:,(maxDisp+1):(end+minDisp),:);  %trim edges that can't be matched due to search window
    disparityMap = disparityMap(:,(maxDisp+1):(end+minDisp)); %trim edges that can't be matched due to search window
    
    %save rectified image and stereo dispartiy map
    [path, name, ext] = fileparts(Lfiles{i});
    outfile_Image = strcat('./output/',name,'_rect','.tiff');
    outfile_Map = strcat('./output/',name,'_dispmap','.mat');
    imwrite(J_final,outfile_Image,'tiff');  %output as lossless tiff
    savedispMap_parfor(outfile_Map,disparityMap);

end

%shutdown parallel pool

delete(p); %shutdown current parallel pool
end

