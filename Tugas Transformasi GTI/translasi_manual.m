function hasil = translasi_manual(img, tx, ty)

img = double(img);
[m, n, c] = size(img);
hasil = zeros(m, n, c);

for x = 1:m
    for y = 1:n
        x_new = x + ty;
        y_new = y + tx;

        if (x_new >= 1 && x_new <= m && y_new >= 1 && y_new <= n)
            for k = 1:c
                hasil(x_new, y_new, k) = img(x, y, k);
            end
        end
    end
end

hasil = uint8(hasil);

end
