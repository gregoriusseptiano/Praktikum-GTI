function hasil = flipping_manual(img, mode)

img = double(img);
[m, n, c] = size(img);
hasil = zeros(m, n, c);

if strcmp(mode, 'horizontal')
    for x = 1:m
        for y = 1:n
            for k = 1:c
                hasil(x, y, k) = img(x, n - y + 1, k);
            end
        end
    end
elseif strcmp(mode, 'vertical')
    for x = 1:m
        for y = 1:n
            for k = 1:c
                hasil(x, y, k) = img(m - x + 1, y, k);
            end
        end
    end
else
    error('Mode harus horizontal atau vertical');
end

hasil = uint8(hasil);

end
