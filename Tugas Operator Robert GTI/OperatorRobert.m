clc;
clear;
close all;

% =====================================
% Membaca gambar
% =====================================
img = imread('gambar.png');

% =====================================
% Konversi ke grayscale
% =====================================
if size(img,3) == 3
    gray = rgb2gray(img);
else
    gray = img;
end

gray = double(gray);

% =====================================
% Kernel Roberts 2x2
% =====================================
Gx = [1 0;
      0 -1];

Gy = [0 1;
     -1 0];

% =====================================
% Proses konvolusi
% =====================================
Ix = conv2(gray, Gx, 'same');
Iy = conv2(gray, Gy, 'same');

% =====================================
% Menghitung magnitude tepi
% =====================================
edge = abs(Ix) + abs(Iy);

% =====================================
% Threshold sederhana
% =====================================
T = 50;
edge(edge < T) = 0;

% =====================================
% Normalisasi hasil
% =====================================
edge = uint8(255 * edge / max(edge(:)));

% =====================================
% Menampilkan hasil
% =====================================
figure;

subplot(1,3,1);
imshow(uint8(gray));
title('Grayscale');

subplot(1,3,2);
imshow(uint8(Ix), []);
title('Gradient X');

subplot(1,3,3);
imshow(edge);
title('Edge Detection (Roberts)');
