spm<4, 4> x = {{0, 0, 1}, {1, 1, 2}, {2, 3, 4}};
spm<4, 4> y = {{1, 1, 3}, {3, 3, 9}};
for (var i = 0; i <= 4; i++) {
    if (i < 3)
        x += y;
    else x -= y;
}
return x;