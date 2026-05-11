% Konversi gambar ke grayscale
if size(img,3) == 3
    gray = rgb2gray(img);
else
    gray = img;
end

% Mengubah tipe data menjadi double
gray = double(gray);
% Ukuran gambar
[m, n] = size(gray);

% Kernel Operator Roberts 2x2
Gx = [1 0;
         0 -1];
Gy = [0 1;
        -1 0];

% Menyiapkan matriks hasil
edge = zeros(m, n);
% Proses konvolusi manual
for i = 1:m-1
    for j = 1:n-1
        % Mengambil matriks piksel 2x2
        region = gray(i:i+1, j:j+1);
        % Inisialisasi gradien
        Ix = 0;
        Iy = 0;
        % Perkalian kernel secara manual
        for k = 1:2
            for l = 1:2
                Ix = Ix + region(k,l) * Gx(k,l);
                Iy = Iy + region(k,l) * Gy(k,l);
            end
        end
        % Menghitung magnitude tepi
        edge(i,j) = abs(Ix) + abs(Iy);
    end
end

% Thresholding untuk mengurangi noise
T = 50;
edge(edge < T) = 0;
% Normalisasi hasil edge detection
if max(edge(:)) > 0
   edge = uint8(255 * edge / max(edge(:)));
else
   edge = uint8(edge);
end


