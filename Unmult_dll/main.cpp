#include <algorithm>
#include <lua.hpp>

// ピクセルのデータを扱いやすくするための構造体
struct Pixel_RGBA {
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
};

// Luaから呼び出す関数
int unmult_color(lua_State* L) {
    // 引数の受け取り
    Pixel_RGBA* pixels = reinterpret_cast<Pixel_RGBA*>(lua_touserdata(L, 1));
    int w = static_cast<int>(lua_tointeger(L, 2));
    int h = static_cast<int>(lua_tointeger(L, 3));

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int index = x + w * y;
            if (pixels[index].a < 255) {
                pixels[index].r *= pixels[index].a / 256;
                pixels[index].g *= pixels[index].a / 256;
                pixels[index].b *= pixels[index].a / 256;
            }
            int rgb[3] = { pixels[index].r, pixels[index].g, pixels[index].b };
            float rate = static_cast<float>(255) / *std::max_element(
                rgb,
                rgb + 3
            );
            if (rate != +INFINITY) {
                int tr = pixels[index].r * rate;
                int tg = pixels[index].g * rate;
                int tb = pixels[index].b * rate;
                int a = 0;
                if (tr > 0) {
                    a = (pixels[index].r * 255) / tr;
                }
                else if (tg > 0) {
                    a = (pixels[index].g * 255) / tg;
                }
                else if (tg > 0) {
                    a = (pixels[index].b * 255) / tb;
                };

                if (tr > 255) {
                    tr = 255;
                };
                if (tg > 255) {
                    tg = 255;
                };
                if (tb > 255) {
                    tb = 255;
                };
                pixels[index].r = tr;
                pixels[index].g = tg;
                pixels[index].b = tb;
                pixels[index].a = a;
            }
            else {
                pixels[index].r = 0;
                pixels[index].g = 0;
                pixels[index].b = 0;
                pixels[index].a = 0;
            }
        }
    }

    return 0;
}


static luaL_Reg functions[] = {
    { "unmult_color", unmult_color },
    { nullptr, nullptr }
};

extern "C" {
    __declspec(dllexport) int luaopen_Unmult_dll(lua_State* L) {
        luaL_register(L, "Unmult_dll", functions);
        return 1;
    }
}