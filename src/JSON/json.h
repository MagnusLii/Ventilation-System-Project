#pragma once

#include <map>
#include <string>

// {
// "auto": false,
// "speed": 18
// }
// // Automatic mode:
// {
// "auto": true,
// "pressure": 25
// }

class CTRLjson {
    public:
        CTRLjson(bool manual, int speed);
        CTRLjson(const char *str);
        void parse(const char *str);

    private:
        bool manual;
        int speed;

};