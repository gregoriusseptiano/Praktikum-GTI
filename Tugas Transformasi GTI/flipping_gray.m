function hasil = flipping_gray(img, mode)

img = double(img);
[m, n] = size(img);

hasil = zeros(m, n);

if strcmp(mode, 'horizontal')
    for x = 1:m
        for y = 1:n
            hasil(x, y) = img(x, n - y + 1);
        end
    end
elseif strcmp(mode, 'vertical')
    for x = 1:m
        for y = 1:n
            hasil(x, y) = img(m - x + 1, y);
        end
    end
end

hasil = uint8(hasil);

end
