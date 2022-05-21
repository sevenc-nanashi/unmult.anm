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
            float r = pixels[index].r;
            float g = pixels[index].g;
            float b = pixels[index].b;
            float a = pixels[index].a;
            if (a < 255) {
                r = (r * a) / 256;
                g = (g * a) / 256;
                b = (b * a) / 256;
            }
            int max = std::max(std::max(pixels[index].r, pixels[index].g), pixels[index].b);
            if (max > 0) {
                float rate = static_cast<float>(255) / max;
                int tr = static_cast<int>(pixels[index].r * rate);
                int tg = static_cast<int>(pixels[index].g * rate);
                int tb = static_cast<int>(pixels[index].b * rate);
                int a = 0;
                if (tb > 0) {
                    a = (pixels[index].b * 256) / tb;
                }
                else if (tg > 0) {
                    a = (pixels[index].g * 256) / tg;
                }
                else if (tr > 0) {
                    a = (pixels[index].r * 256) / tr;
                }
                else {
                    a = 0;
                }
                pixels[index].r = std::clamp(tr, 0, 255);
                pixels[index].g = std::clamp(tg, 0, 255);
                pixels[index].b = std::clamp(tb, 0, 255);
                pixels[index].a = std::clamp(a, 0, 255);
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
    __declspec(dllexport) int luaopen_unmult_core(lua_State* L) {
        luaL_register(L, "unmult_core", functions);
        return 1;
    }
}