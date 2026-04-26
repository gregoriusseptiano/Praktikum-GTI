function hasil = scaling_gray(img, sx, sy)

img = double(img);
[m, n] = size(img);

m2 = floor(m * sy);
n2 = floor(n * sx);

hasil = zeros(m2, n2);

for x = 1:m2
    for y = 1:n2

        x_old = floor(x / sy);
        y_old = floor(y / sx);

        if x_old < 1
            x_old = 1;
        end
        if y_old < 1
            y_old = 1;
        end

        if (x_old <= m && y_old <= n)
            hasil(x, y) = img(x_old, y_old);
        end

    end
end

hasil = uint8(hasil);

end
