function hasil = translasi_gray(img, tx, ty)

img = double(img);
[m, n] = size(img);

hasil = zeros(m, n);

for x = 1:m
    for y = 1:n
        x_new = x + ty;
        y_new = y + tx;

        if (x_new >= 1 && x_new <= m && y_new >= 1 && y_new <= n)
            hasil(x_new, y_new) = img(x, y);
        end
    end
end

hasil = uint8(hasil);

end
