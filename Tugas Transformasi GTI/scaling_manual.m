function hasil = scaling_manual(img, sx, sy)

img = double(img);
[m, n, c] = size(img);

m2 = floor(m * sy);
n2 = floor(n * sx);

hasil = zeros(m2, n2, c);

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
            for k = 1:c
                hasil(x, y, k) = img(x_old, y_old, k);
            end
        end

    end
end

hasil = uint8(hasil);

end
