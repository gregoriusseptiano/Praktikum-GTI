function hasil = rotasi_manual(img, sudut)

img = double(img);
theta = sudut * pi / 180;

[m, n, c] = size(img);
hasil = zeros(m, n, c);

cx = floor(m/2);
cy = floor(n/2);

for x = 1:m
    for y = 1:n

        x_shift = x - cx;
        y_shift = y - cy;

        x_old = round(x_shift*cos(theta) + y_shift*sin(theta));
        y_old = round(-x_shift*sin(theta) + y_shift*cos(theta));

        x_old = x_old + cx;
        y_old = y_old + cy;

        if (x_old >= 1 && x_old <= m && y_old >= 1 && y_old <= n)
            for k = 1:c
                hasil(x, y, k) = img(x_old, y_old, k);
            end
        end
    end
end

hasil = uint8(hasil);

end
