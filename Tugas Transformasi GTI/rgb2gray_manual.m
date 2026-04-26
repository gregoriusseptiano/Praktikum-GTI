function hasil = rgb2gray_manual(img)

img = double(img);
[m, n, c] = size(img);

hasil = zeros(m, n);

for x = 1:m
    for y = 1:n
        r = img(x, y, 1);
        g = img(x, y, 2);
        b = img(x, y, 3);

        hasil(x, y) = 0.299*r + 0.587*g + 0.114*b;
    end
end

hasil = uint8(hasil);

end
