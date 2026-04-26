function hasil = rotasi_gray(img, sudut)

img = double(img);
theta = sudut * pi / 180;

[m, n] = size(img);
hasil = zeros(m, n);

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
            hasil(x, y) = img(x_old, y_old);
        end

    end
end

hasil = uint8(hasil);

end
