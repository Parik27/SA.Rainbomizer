#pragma once

#include "CMath.hh"
#include <cmath>
#include <algorithm>

namespace Rainbomizer {

struct CARGB
{
    union
    {
        struct
        {
            unsigned char b;
            unsigned char g;
            unsigned char r;
            unsigned char a;
        };
        int colour;
    };

    CARGB (unsigned char a, unsigned char r, unsigned char g, unsigned char b)
    {
        this->a = a;
        this->r = r;
        this->g = g;
        this->b = b;
    }

    CARGB () = default;

    bool
    operator== (const CARGB &rhs) const
    {
        return rhs.colour == this->colour;
    }
    bool
    operator!= (const CARGB &rhs) const
    {
        return !(rhs == *this);
    }
};

struct ColorFloat
{
    float r;
    float g;
    float b;

    ColorFloat () = default;

    ColorFloat (float r, float g, float b) : r (r), g (g), b (b) {}

    ColorFloat (const CARGB &argb)
    {
        r = argb.r / 255.0f;
        g = argb.g / 255.0f;
        b = argb.b / 255.0f;
    }

    CARGB
    ToARGB ()
    {
        return CARGB{255, static_cast<uint8_t> (r * 255),
                     static_cast<uint8_t> (g * 255),
                     static_cast<uint8_t> (b * 255)};
    }
};

struct HSL
{
    float h;
    float s;
    float l;

    HSL () = default;

    HSL (float h, float s, float l) : h (h), s (s), l (l){};

    HSL (const ColorFloat &input)
    {
        float R = input.r;
        float G = input.g;
        float B = input.b;

        float Xmax = std::max ({R, G, B});
        float Xmin = std::min ({R, G, B});
        float C    = Xmax - Xmin;

        this->l = (Xmax + Xmin) / 2;
        if (C == 0)
            this->h = 0;
        else if (fabs (Xmax - R) < 0.01)
            this->h = 60 * (0 + ((G - B) / C));
        else if (fabs (Xmax - G) < 0.01)
            this->h = 60 * (2 + ((B - R) / C));
        else if (fabs (Xmax - B) < 0.01)
            this->h = 60 * (4 + ((R - G) / C));

        if (this->l == 0 || this->l == 1)
            this->s = 0;
        else
            this->s = C / (1 - (fabs (2 * Xmax - C - 1)));
    }

    ColorFloat
    ToColorFloat () const
    {
        auto f = [this] (float n) {
            float k = fmod ((n + (this->h / 30)), 12);
            float a = this->s * std::min (this->l, 1 - this->l);
            float f
                = this->l
                  - a
                        * std::max (-1.0f,
                                    std::min (k - 3, std::min (9 - k, 1.0f)));
            return f;
        };

        return ColorFloat{f (0), f (8), f (4)};
    }

    CARGB
    ToARGB () const { return ToColorFloat ().ToARGB (); }

    operator CARGB () const { return ToARGB (); }

    operator ColorFloat () const { return ToColorFloat (); }
};
} // namespace Rainbomizer
